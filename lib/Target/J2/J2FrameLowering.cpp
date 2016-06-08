//===-- J2FrameLowering.cpp - J2 Frame Information ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the J2 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "J2FrameLowering.h"

using namespace llvm;

void J2FrameLowering::emitPrologue(MachineFunction &MF,
                                   MachineBasicBlock &MBB) const {
  return;
}

void J2FrameLowering::emitEpilogue(MachineFunction &MF,
                                   MachineBasicBlock &MBB) const {
  return;
}
