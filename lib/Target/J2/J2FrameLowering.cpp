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
#include "J2Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

void J2FrameLowering::emitPrologue(MachineFunction &MF,
                                   MachineBasicBlock &MBB) const {
  auto &MFI = MF.getFrameInfo();
  auto StackSize = MFI.getStackSize();
  auto &TTI =
      *static_cast<const J2Subtarget &>(MF.getSubtarget()).getInstrInfo();

  if (StackSize == 0)
    return; // All clear.

  // The first instruction saving callee-saved registers.
  auto it = MBB.begin();

  // Skip the instructions saving callee-saved registers.
  it = std::next(it, MFI.getCalleeSavedInfo().size());

  DebugLoc DL;

  // FIXME: Emit .cfi directives.
  // First, set up the frame pointer.
  if (hasFP(MF)) // mov r14, r15 (fp <- sp)
    BuildMI(MBB, it, DL, TTI.get(J2::MOV32rr), J2::R14).addReg(J2::R15);

  // Then, expand the stack.
  if (isInt<8>(-StackSize)) // 8 bit immediates use ADDri.
    // add #imm, r15
    BuildMI(MBB, it, DL, TTI.get(J2::ADDri), J2::R15)
        .addReg(J2::R15)
        .addImm(-StackSize);
  else {
    // FIXME: Use MOV.L/W with R0.
    llvm_unreachable("Not implemented yet");
  }
}

void J2FrameLowering::emitEpilogue(MachineFunction &MF,
                                   MachineBasicBlock &MBB) const {
  auto &MFI = MF.getFrameInfo();
  auto StackSize = MFI.getStackSize();
  auto &TTI =
      *static_cast<const J2Subtarget &>(MF.getSubtarget()).getInstrInfo();

  if (StackSize == 0)
    return; // All clear.

  // The basic block terminator.
  auto it = MBB.getFirstTerminator();

  auto DL = it == MBB.end() ? DebugLoc{} : it->getDebugLoc();

  // Skip the instructions restoring callee-saved registers.
  it = std::prev(it, MFI.getCalleeSavedInfo().size());

  // FIXME: Emit .cfi directives.
  // First, set up the frame pointer.
  if (hasFP(MF)) // mov r15, r14 (sp <- fp)
    BuildMI(MBB, it, DL, TTI.get(J2::MOV32rr), J2::R15).addReg(J2::R14);
}

void J2FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                           BitVector &SavedRegs,
                                           RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  // Spill FP, so that it can be reused after a function call.
  if (hasFP(MF))
    SavedRegs.set(J2::R14);
}
