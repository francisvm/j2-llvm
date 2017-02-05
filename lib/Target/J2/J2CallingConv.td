//===-- J2CallingConv.td - Calling Conventions for J2 ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for J2 architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// J2 Calling Convention
//===----------------------------------------------------------------------===//

def CC_J2 : CallingConv<[
  CCIfType<[ i1, i8, i16 ], CCPromoteToType<i32>>,
  CCIfType<[ i32 ], CCAssignToReg<[ R4, R5, R6, R7 ]>>,
  CCIfType<[ i32 ], CCAssignToStack<4, 4>> // 4 bytes size, 4 bytes alignment.
]>;

//===----------------------------------------------------------------------===//
// Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_J2 : CallingConv<[
  CCIfType<[ i1, i8, i16 ], CCPromoteToType<i32>>,
  CCIfType<[ i32 ], CCAssignToReg<[ R0, R1, R2, R3 ]>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved Registers.
//===----------------------------------------------------------------------===//

def CSR : CalleeSavedRegs<(add R8, R9, R10, R11, R12, R13, R14, PR)>;
