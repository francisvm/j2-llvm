//===-- J2InstrInfo.td -------------------------------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

include "J2InstrFormats.td"

//===----------------------------------------------------------------------===//
// Describe constraints
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Describe LLVM IR constraints
//===----------------------------------------------------------------------===//

def i32immSExt8 : ImmLeaf<i32, [{ return isInt<8>(Imm); }]>;

def j2CallTarget : Operand<i32>;

def j2BRATarget : Operand<OtherVT>;

//===----------------------------------------------------------------------===//
// Describe profiles and nodes
//===----------------------------------------------------------------------===//

def J2Ret : SDNode<"J2ISD::Ret", SDTNone,
                   [ SDNPHasChain, SDNPOptInGlue, SDNPVariadic ]>;

def SDT_J2Call : SDTypeProfile<0, -1, [ SDTCisVT<0, iPTR>]>;

def J2Call : SDNode<"J2ISD::Call", SDT_J2Call,
    [ SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic ]>;

def SDT_J2Wrapper : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;

def J2Wrapper : SDNode <"J2ISD::Wrapper", SDT_J2Wrapper>;

//===----------------------------------------------------------------------===//
// Describe instructions
//===----------------------------------------------------------------------===//

// FIXME: Fix itineraries.

def NOP : FZERO<0x9, (outs), (ins), "nop", [], NoItinerary>;

let isReturn = 1, isTerminator = 1, hasDelaySlot = 1, Uses = [ PR ] in {
  def RTS : FZERO<0xB, (outs), (ins), "rts", [ (J2Ret) ], NoItinerary>;
}

let isCall = 1, hasDelaySlot = 1, Defs = [ R0, PR ]  in {
def JSR : FM<0x4, 0xB, (outs), (ins GPR:$rm),
             "jsr\t$rm", [ (J2Call GPR:$rm) ], NoItinerary>;

def BSR : FD12<0xB, (outs), (ins j2CallTarget:$target),
               "bsr\t$target", [ (J2Call tglobaladdr:$target) ], NoItinerary>;
}

let isBranch = 1, isTerminator = 1, hasDelaySlot = 1 in {
def BRA : FD12<0xA, (outs), (ins j2BRATarget:$target),
               "bra\t$target", [ (br bb:$target) ],
               NoItinerary>;

let Uses = [ SR ] in {
def BT : FD<0x89, (outs), (ins j2BRATarget:$target),
            "bt\t$target", [ (brcond SR, bb:$target) ],
            NoItinerary>;
}
}

def MOV8ri : FNI<0xE, (outs GPR:$rn), (ins i32imm:$imm),
                 "mov\t$imm, $rn", [ (set i32:$rn, i32immSExt8:$imm) ],
                 NoItinerary>;

def MOV32rr : FNM<0x6, 0x3, (outs GPR:$rn), (ins GPR:$rm),
                  "mov\t$rm, $rn", [], NoItinerary>;

let mayLoad = 1 in {
def MOV8mr : FNM<0x6, 0x0, (outs GPR:$rn), (ins GPR:$rm),
                 "mov.b\t@$rm, $rn",
                 [ (set i32:$rn, (sextloadi8 i32:$rm)) ], NoItinerary>;
def MOV16mr : FNM<0x6, 0x1, (outs GPR:$rn), (ins GPR:$rm),
                  "mov.w\t@$rm, $rn",
                  [ (set i32:$rn, (sextloadi16 i32:$rm)) ], NoItinerary>;
def MOV32mr : FNM<0x6, 0x2, (outs GPR:$rn), (ins GPR:$rm),
                  "mov.l\t@$rm, $rn",
                  [ (set i32:$rn, (load i32:$rm)) ], NoItinerary>;
}

let mayStore = 1 in {
def MOV8rm : FNM<0x2, 0x0, (outs), (ins GPR:$rm, GPR:$rn),
                 "mov.b\t$rm, @$rn",
                 [ (truncstorei8 i32:$rm, i32:$rn) ], NoItinerary>;
def MOV16rm : FNM<0x2, 0x1, (outs), (ins GPR:$rm, GPR:$rn),
                  "mov.w\t$rm, @$rn",
                  [ (truncstorei16 i32:$rm, i32:$rn) ], NoItinerary>;
def MOV32rm : FNM<0x2, 0x2, (outs), (ins GPR:$rm, GPR:$rn),
                  "mov.l\t$rm, @$rn",
                  [ (store i32:$rm, i32:$rn) ], NoItinerary>;
}

let isCommutable = 1, Constraints = "$rnd = $rns", isAdd = 1 in {
def ADDrr : FNM<0x3, 0xC, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
                "add\t$rm, $rnd",
                [ (set i32:$rnd, (add i32:$rm, i32:$rns)) ],
                NoItinerary>;

def ADDri : FNI<0x7, (outs GPR:$rnd), (ins GPR:$rns, i32imm:$imm),
                "add\t$imm, $rnd",
                [ (set i32:$rnd, (add i32:$rns, i32immSExt8:$imm)) ],
                NoItinerary>;

def SUBrr : FNM<0x3, 0xB, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
                "sub\t$rm, $rnd",
                [ (set i32:$rnd, (sub i32:$rm, i32:$rns)) ],
                NoItinerary>;
}

let Defs = [ MAC ] in {
def CLRMAC : FZERO<0x28, (outs), (ins),
                   "clrmac",
                   [ (set MAC, (i64 0)) ],
                   NoItinerary>;
}

class ST<string type, bits<8> data, string regname, Register reg>
  : FN<0x0, data, (outs GPR:$rn), (ins),
       "st"#type#"\t"#regname#", $rn", [], NoItinerary> {
         let Defs = [ reg ];
}

class LD<string type, bits<8> data, string regname, Register reg>
  : FN<0x4, data, (outs), (ins GPR:$rm),
       "ld"#type#"\t$rm, "#regname, [], NoItinerary> {
         let Uses = [ reg ];
}

def STSMACH : ST<"s", 0xA, "mach", MACH>;
def LDSMACH : LD<"s", 0xA, "mach", MACH>;

def STSMACL : ST<"s", 0x1A, "macl", MACL>;
def LDSMACL : LD<"s", 0x1A, "macl", MACL>;

def STSPR : ST<"s", 0x2A, "pr", PR>;
def LDSPR : LD<"s", 0x2A, "pr", PR>;

def STCGBR : ST<"c", 0x12, "gbr", GBR>;
def LDCGBR : LD<"c", 0x12, "gbr", GBR>;

def STCVBR : ST<"c", 0x22, "vbr", VBR>;
def LDCVBR : LD<"c", 0x22, "vbr", VBR>;

def : Pat<(i32 (J2Wrapper tglobaladdr:$in)), (MOV8ri tglobaladdr:$in)>;

// Calling a target global addr, results in calling the global addr.
def : Pat<(J2Call (J2Wrapper tglobaladdr:$in)), (BSR tglobaladdr:$in)>;

let isCompare = 1, Defs = [ SR ] in { // Sets the T bit
class CMP<bits<4> op, bits<4> data, string asmstr, CondCode CC>
    : FNM<op, data, (outs), (ins GPR:$rm, GPR:$rn), asmstr,
      [ (set SR, (setcc GPR:$rm, GPR:$rn, CC)) ], NoItinerary> {}

def CMPEQrr : CMP<0x3, 0x0, "cmp/eq\t$rm, $rn", SETUEQ>;
def CMPHSrr : CMP<0x3, 0x2, "cmp/hs\t$rm, $rn", SETGE>;
def CMPGErr : CMP<0x3, 0x3, "cmp/ge\t$rm, $rn", SETUGE>;
def CMPHIrr : CMP<0x3, 0x6, "cmp/hi\t$rm, $rn", SETULE>;
def CMPGTrr : CMP<0x3, 0x7, "cmp/gt\t$rm, $rn", SETGT>;

def CMPPLrz : FN<0x4, 0x15, (outs), (ins GPR:$rn),
                 "cmp/pl\t$rn",
                 [ (set SR, (setcc GPR:$rn, (i32 0), SETGT)) ], NoItinerary>;

def CMPPZrz : FN<0x4, 0x11, (outs), (ins GPR:$rn),
                 "cmp/pz\t$rn",
                 [ (set SR, (setcc GPR:$rn, (i32 0), SETGE)) ], NoItinerary>;

let Uses = [ R0 ] in {
def CMPEQRi : FI<0x88, (outs), (ins i32imm:$imm),
                 "cmp/eq\t$imm, r0",
                 [ (set SR, (setcc R0, i32immSExt8:$imm, SETEQ)) ],
                 NoItinerary>;
}
}

// J2 does not support `a < b` cmp. `a < b` <=> `b >= a`
def : Pat<(setcc (i32 GPR:$rm), (i32 GPR:$rn), SETLT),
          (CMPGErr $rn, $rm)>;

// J2 does not support `a u< b` cmp. `a u< b` <=> `b u>= a`
def : Pat<(setcc (i32 GPR:$rm), (i32 GPR:$rn), SETULT),
          (CMPGErr $rn, $rm)>;

// J2 does not support `a u< b` cmp. `a u< b` <=> `b u>= a`
def : Pat<(setcc (i32 GPR:$rm), (i32 GPR:$rn), SETUGT),
          (CMPHIrr $rn, $rm)>;

// J2 does not support `a <= b` cmp. `a <= b` <=> `b > a`
def : Pat<(setcc (i32 GPR:$rm), (i32 GPR:$rn), SETLE),
          (CMPGTrr $rn, $rm)>;
