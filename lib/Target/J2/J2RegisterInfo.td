//===-- J2RegisterInfo.td ----------------------------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Describe registers
//===----------------------------------------------------------------------===//

// J2 registers.
class J2Reg<bits<16> enc, string n, list<string> altNames = [],
            list<Register> subregs = []> : Register<n, altNames> {
  let HWEncoding = enc;
  let Namespace = "J2";
  let SubRegs = subregs;
  // All bits of J2 registers with sub-registers are covered by sub-registers.
  let CoveredBySubRegs = 1;
}

let Namespace = "J2" in {
  def sub_16bit : SubRegIndex<16>;
  def sub_16bit_hi : SubRegIndex<16, 16>;
}

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//

let Namespace = "J2" in {
  // GPR
  def R0 : J2Reg<0, "r0">, DwarfRegNum<[ 0 ]>;
  def R1 : J2Reg<1, "r1">, DwarfRegNum<[ 1 ]>;
  def R2 : J2Reg<2, "r2">, DwarfRegNum<[ 2 ]>;
  def R3 : J2Reg<3, "r3">, DwarfRegNum<[ 3 ]>;
  def R4 : J2Reg<4, "r4">, DwarfRegNum<[ 4 ]>;
  def R5 : J2Reg<5, "r5">, DwarfRegNum<[ 5 ]>;
  def R6 : J2Reg<6, "r6">, DwarfRegNum<[ 6 ]>;
  def R7 : J2Reg<7, "r7">, DwarfRegNum<[ 7 ]>;
  def R8 : J2Reg<8, "r8">, DwarfRegNum<[ 8 ]>;
  def R9 : J2Reg<9, "r9">, DwarfRegNum<[ 9 ]>;
  def R10 : J2Reg<10, "r10">, DwarfRegNum<[ 10 ]>;
  def R11 : J2Reg<11, "r11">, DwarfRegNum<[ 11 ]>;
  def R12 : J2Reg<12, "r12">, DwarfRegNum<[ 12 ]>;
  def R13 : J2Reg<13, "r13">, DwarfRegNum<[ 13 ]>;
  def R14 : J2Reg<14, "r14", [ "fp" ]>, DwarfRegNum<[ 14 ]>;
  def R15 : J2Reg<15, "r15", [ "sp" ]>, DwarfRegNum<[ 15 ]>;

  // Control registers
  def SR : J2Reg<16, "sr">, DwarfRegNum<[ 22 ]>;
  def GBR : J2Reg<17, "gbr">, DwarfRegNum<[ 18 ]>;
  def VBR : J2Reg<18, "vbr">, DwarfRegNum<[ 19 ]>;

  // System registers
  def MACH : J2Reg<19, "mach">, DwarfRegNum<[ 21 ]>;
  def MACL : J2Reg<20, "macl">, DwarfRegNum<[ 21 ]>;
let SubRegIndices = [ sub_16bit, sub_16bit_hi ] in {
  def MAC : J2Reg<21, "mac", [], [ MACL, MACH ]>;
}
  def PR : J2Reg<22, "pr">, DwarfRegNum<[ 17 ]>;
  def PC : J2Reg<23, "pc">, DwarfRegNum<[ 16 ]>;
}

//===----------------------------------------------------------------------===//
// Register classes
//===----------------------------------------------------------------------===//

// General purpose registers.
def GPR : RegisterClass<"J2", [ i32 ], 32, (sequence "R%u", 0, 15)>;

// Control registers.
def CR : RegisterClass<"J2", [ i32 ], 32, (add SR, GBR, VBR)> {
  let CopyCost = -1;
  let isAllocatable = 0;
}

// System registers.
def SYSR : RegisterClass<"J2", [ i32 ], 32, (add MACH, MACL, PR, PC)> {
  let CopyCost = -1;
  let isAllocatable = 0;
}

// MAC register.
def MACR : RegisterClass<"J2", [ i64 ], 64, (add MAC)> {
  let CopyCost = -1;
  let isAllocatable = 0;
}

def PCR : RegisterClass<"J2", [ i32 ], 32, (add PC)> {
  let CopyCost = -1;
  let isAllocatable = 0;
}
