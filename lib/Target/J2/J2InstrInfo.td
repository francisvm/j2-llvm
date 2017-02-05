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
// Describe instructions
//===----------------------------------------------------------------------===//

// FIXME: Fix itineraries.

def NOP : FZERO<0x9, (outs), (ins), "nop", [], NoItinerary>;

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
