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
#include "J2MachineFunctionInfo.h"
#include "J2Subtarget.h"
#include "J2TargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

void J2FrameLowering::emitPrologue(MachineFunction &MF,
                                   MachineBasicBlock &MBB) const {
  auto &MFI = MF.getFrameInfo();
  auto StackSize = MFI.getStackSize() - MFI.getCalleeSavedInfo().size() * 4;
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
    // FIXME: Fetch vreg from MFI.
    auto Vreg0 = MF.getInfo<J2MachineFunctionInfo>()->getFrameSizeVreg();
    BuildMI(MBB, it, DL, TTI.get(J2::ADDrr), J2::R15)
      .addReg(J2::R15)
      .addReg(Vreg0);
  }
}

void J2FrameLowering::emitEpilogue(MachineFunction &MF,
                                   MachineBasicBlock &MBB) const {
  auto &MFI = MF.getFrameInfo();
  auto StackSize = MFI.getStackSize() - MFI.getCalleeSavedInfo().size() * 4;
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

bool J2FrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    const std::vector<CalleeSavedInfo> &CSI,
    const TargetRegisterInfo *TRI) const {
  if (CSI.empty())
    return false;

  DebugLoc DL;
  if (MI != MBB.end())
    DL = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

  for (unsigned i = CSI.size(); i != 0; --i) {
    unsigned Reg = CSI[i - 1].getReg();
    // Add the callee-saved register as live-in. It's killed at the spill.
    MBB.addLiveIn(Reg);
    BuildMI(MBB, MI, DL, TII.get(J2::MOV32rmpush), J2::R15)
        .addReg(Reg)
        .addReg(Reg, RegState::Kill);
  }
  return true;
}

bool J2FrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    const std::vector<CalleeSavedInfo> &CSI,
    const TargetRegisterInfo *TRI) const {
  if (CSI.empty())
    return false;

  DebugLoc DL;
  if (MI != MBB.end())
    DL = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

  for (unsigned i = 0, e = CSI.size(); i != e; ++i)
    BuildMI(MBB, MI, DL, TII.get(J2::MOV32mrpop), CSI[i].getReg())
        .addReg(J2::R15);

  return true;
}

/// This pass is used to handle big stack frames.
struct J2BigStackFrameExpansion : MachineFunctionPass {
  static char ID;
  J2BigStackFrameExpansion() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) {
    auto &MFI = MF.getFrameInfo();
    auto StackSize = MFI.getStackSize() - MFI.getCalleeSavedInfo().size() * 4;

    // Exit early if the size of the stack frame fits on 8 bits.
    if (isInt<8>(-StackSize))
      return false;

    auto &TII =
        *static_cast<const J2Subtarget &>(MF.getSubtarget()).getInstrInfo();
    auto& EntryMBB = MF.front();
    auto MBBI = EntryMBB.begin();
    auto DL = EntryMBB.findDebugLoc(MBBI);

    auto Vreg0 = MF.getRegInfo().createVirtualRegister(&J2::GPRRegClass);
    BuildMI(EntryMBB, MBBI, DL, TII.get(J2::MOV32ir), Vreg0).addImm(-StackSize);
    auto* JFI = MF.getInfo<J2MachineFunctionInfo>();
    JFI->setFrameSizeVreg(Vreg0);

    return true;
  }

  StringRef getPassName() const { return "J2 big stack frame expansion"; }
};

char J2BigStackFrameExpansion::ID = 0;

/// createJ2BigStackFrameExpansionPass - returns an instance of the big stack
/// frame expansion pass
FunctionPass *llvm::createJ2BigStackFrameExpansionPass() {
  return new J2BigStackFrameExpansion();
}
