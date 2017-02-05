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

def j2MemOperand : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops GPR, i32imm);
}

def j2PCMemOperand : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops PCR, i32imm);
}

// Operand for CONSTPOOL_ENTRY pseudo-instruction.
def j2CPInstOperand : Operand<i32>;

def j2MemPattern8 : ComplexPattern<i32, 2, "SelectAddr<8>", [], []>;
def j2MemPattern16 : ComplexPattern<i32, 2, "SelectAddr<16>", [], []>;
def j2MemPattern32 : ComplexPattern<i32, 2, "SelectAddr<32>", [], []>;

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

def J2Shl : SDNode <"J2ISD::SHL", SDTIntShiftOp>;

def J2Srl : SDNode <"J2ISD::SRL", SDTIntShiftOp>;

//===----------------------------------------------------------------------===//
// Describe instructions
//===----------------------------------------------------------------------===//

// FIXME: Fix itineraries.

def NOP : FZERO<0x9, (outs), (ins), "nop", [], NoItinerary>;

let hasSideEffects = 0, isNotDuplicable = 1 in
def CONSTPOOL_ENTRY : Pseudo<(outs),
                             (ins j2CPInstOperand:$id, j2CPInstOperand:$idx,
                              i32imm:$size),
                             "cpi", [], NoItinerary>;

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

let usesCustomInserter = 1 in {
  def MOV32ir : Pseudo<(outs GPR:$rn), (ins i32imm:$imm),
                       "mov\t$imm, r0", [ (set i32:$rn, imm:$imm) ],
                       NoItinerary>;
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

let Constraints = "$rnd = $rns" in {
let isCommutable = 1, isAdd = 1 in {
def ADDrr : FNM<0x3, 0xC, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
                "add\t$rm, $rnd",
                [ (set i32:$rnd, (add i32:$rm, i32:$rns)) ],
                NoItinerary>;

def ADDri : FNI<0x7, (outs GPR:$rnd), (ins GPR:$rns, i32imm:$imm),
                "add\t$imm, $rnd",
                [ (set i32:$rnd, (add i32:$rns, i32immSExt8:$imm)) ],
                NoItinerary>;
}

def SUBrr : FNM<0x3, 0xB, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
                "sub\t$rm, $rnd",
                [ (set i32:$rnd, (sub i32:$rm, i32:$rns)) ],
                NoItinerary>;

def ANDrr : FNM<0x2, 0x9, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
                "and\t$rm, $rnd",
                [ (set i32:$rnd, (and i32:$rm, i32:$rns)) ],
                NoItinerary>;

def ORrr : FNM<0x2, 0xB, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
               "or\t$rm, $rnd",
               [ (set i32:$rnd, (or i32:$rm, i32:$rns)) ],
               NoItinerary>;

def XORrr : FNM<0x2, 0xA, (outs GPR:$rnd), (ins GPR:$rm, GPR:$rns),
                "xor\t$rm, $rnd",
                [ (set i32:$rnd, (xor i32:$rm, i32:$rns)) ],
                NoItinerary>;

let Defs = [ SR ] in {
def SHAL : FN<0x4, 0x20, (outs GPR:$rnd), (ins GPR:$rns),
              "shal\t$rnd",
              [ (set GPR:$rnd, (J2Shl GPR:$rns, (i32 1))) ],
              NoItinerary>;

def SHAR : FN<0x4, 0x21, (outs GPR:$rnd), (ins GPR:$rns),
              "shar\t$rnd",
              [ (set i32:$rnd, (sra i32:$rns, (i32 1))) ],
              NoItinerary>;

def SHLR : FN<0x4, 0x1, (outs GPR:$rnd), (ins GPR:$rns),
              "shlr\t$rnd",
              [ (set i32:$rnd, (J2Srl i32:$rns, (i32 1))) ],
              NoItinerary>;
}

def SHLL2 : FN<0x4, 0x8, (outs GPR:$rnd), (ins GPR:$rns),
              "shll2\t$rnd",
              [ (set i32:$rnd, (J2Shl i32:$rns, (i32 2))) ],
              NoItinerary>;

def SHLL8 : FN<0x4, 0x18, (outs GPR:$rnd), (ins GPR:$rns),
              "shll8\t$rnd",
              [ (set i32:$rnd, (J2Shl i32:$rns, (i32 8))) ],
              NoItinerary>;

def SHLL16 : FN<0x4, 0x28, (outs GPR:$rnd), (ins GPR:$rns),
               "shll16\t$rnd",
               [ (set i32:$rnd, (J2Shl i32:$rns, (i32 16))) ],
               NoItinerary>;

def SHLR2 : FN<0x4, 0x9, (outs GPR:$rnd), (ins GPR:$rns),
              "shlr2\t$rnd",
              [ (set i32:$rnd, (J2Srl i32:$rns, (i32 2))) ],
              NoItinerary>;

def SHLR8 : FN<0x4, 0x19, (outs GPR:$rnd), (ins GPR:$rns),
              "shlr8\t$rnd",
              [ (set i32:$rnd, (J2Srl i32:$rns, (i32 8))) ],
              NoItinerary>;

def SHLR16 : FN<0x4, 0x29, (outs GPR:$rnd), (ins GPR:$rns),
               "shlr16\t$rnd",
               [ (set i32:$rnd, (J2Srl i32:$rns, (i32 16))) ],
               NoItinerary>;
}

def : Pat<(i32 (J2Srl i32:$rn, (i32 0))),
          (i32 $rn)>;

def : Pat<(i32 (J2Shl i32:$rn, (i32 0))),
          (i32 $rn)>;

let Defs = [ R0 ], Uses = [ R0 ] in {
def ANDri : FN<0xC, 0x8, (outs), (ins i32imm:$imm),
               "and\t$imm, r0",
               [ (set R0, (and R0, i32immSExt8:$imm)) ],
               NoItinerary>;
def ORri : FI<0xCB, (outs), (ins i32imm:$imm),
              "or\t$imm, r0",
              [ (set R0, (or R0, i32immSExt8:$imm)) ],
              NoItinerary>;
def XORri : FN<0xC, 0xA, (outs), (ins i32imm:$imm),
              "xor\t$imm, r0",
              [ (set R0, (xor R0, i32immSExt8:$imm)) ],
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

let mayLoad = 1 in {
let Defs = [ R0 ] in {
def MOV8mr0 : FMD<0x84, (outs), (ins j2MemOperand:$addr),
                  "mov.b\t@$addr, r0",
                  [ (set R0, (sextloadi8 j2MemPattern8:$addr)) ],
                  NoItinerary>;
def MOV16mr0 : FMD<0x85, (outs), (ins j2MemOperand:$addr),
                  "mov.w\t@$addr, r0",
                  [ (set R0, (sextloadi16 j2MemPattern16:$addr)) ],
                  NoItinerary>;
}

def MOV32mrd : FNMD<0x5, (outs GPR:$rn), (ins j2MemOperand:$addr),
                    "mov.l\t@$addr, $rn",
                    [ (set i32:$rn, (load j2MemPattern32:$addr)) ],
                    NoItinerary>;
}

let mayStore = 1 in {
let Uses = [ R0 ] in {
def MOV8r0m : FMD<0x80, (outs), (ins j2MemOperand:$addr),
                   "mov.b\tr0, @$addr",
                   [ (truncstorei8 R0, j2MemPattern8:$addr) ],
                   NoItinerary>;
def MOV16r0m : FMD<0x81, (outs), (ins j2MemOperand:$addr),
                   "mov.w\tr0, @$addr",
                   [ (truncstorei16 R0, j2MemPattern16:$addr) ],
                   NoItinerary>;
}

def MOV32rmd : FNMD<0x1, (outs), (ins GPR:$rm, j2MemOperand:$addr),
                    "mov.l\t$rm, @$addr",
                    [ (store i32:$rm, j2MemPattern32:$addr) ],
                    NoItinerary>;
}

let isCommutable = 1 in {
let Defs = [ MACL ] in {
def MULrr : FNM<0x0, 0x7, (outs), (ins GPR:$rm, GPR:$rn),
                "mul.l\t$rm, $rn",
                [ (set MACL, (mul i32:$rm, i32:$rn)) ],
                NoItinerary>;
}

let Uses = [ R0 ] in {
def MULrr0 : FN<0x4, 0x80, (outs GPR:$rnd), (ins GPR:$rns),
               "mulr\tr0, $rnd",
               [ (set i32:$rnd, (mul R0, i32:$rns)) ],
               NoItinerary>;
}
}

def : Pat<(i32 (mulhu i32:$rm, i32:$rn)),
          (i32 (MULrr i32:$rm, i32:$rn))>;


let Uses = [ PC ] in {
def MOV32PCR : FND8<0xD, (outs GPR:$rn), (ins i32imm:$addr),
                    "mov.l\t@($addr, pc), $rn",
                    [],
                    NoItinerary>;
}

def : Pat<(load (J2Wrapper tconstpool:$src)), (MOV32PCR tconstpool:$src)>;

def : Pat<(J2Wrapper tglobaladdr:$src), (MOV32PCR tconstpool:$src)>;

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

def SWAP16rr : FNM<0x6, 0x8, (outs GPR:$rn), (ins GPR:$rm),
                  "swap.b\t$rm, $rn",
                  [ (set i32:$rn, (rotr (bswap i32:$rm), (i32 16))) ],
                  NoItinerary>;

def SWAP32rr : FNM<0x6, 0x9, (outs GPR:$rn), (ins GPR:$rm),
                  "swap.l\t$rm, $rn",
                  [ (set i32:$rn, (bswap i32:$rm)) ],
                  NoItinerary>;
