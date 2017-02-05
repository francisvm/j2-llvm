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
