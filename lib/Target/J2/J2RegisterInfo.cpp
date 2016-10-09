//===-- J2RegisterInfo.cpp - J2 Register Information -== ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the J2 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "J2RegisterInfo.h"
#include "J2FrameLowering.h"
#include "J2Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetSubtargetInfo.h"

using namespace llvm;

#define DEBUG_TYPE "J2-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "J2GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

// PR is the register containing the return address.
J2RegisterInfo::J2RegisterInfo() : J2GenRegisterInfo(J2::PR) {}

const MCPhysReg *
J2RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

BitVector J2RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  // FIXME: Use TableGen.
  static constexpr MCPhysReg ReservedRegs[] = {
      J2::R15, J2::SR, J2::GBR, J2::VBR, J2::MACH, J2::MACL, J2::PR, J2::PC};

  BitVector Reserved(getNumRegs());

  for (auto Reg : ReservedRegs)
    Reserved.set(Reg);

  return Reserved;
}

bool J2RegisterInfo::isCalleeSavedIndex(int FrameIdx,
                                        MachineFrameInfo &MFI) const {
  auto &CSI = MFI.getCalleeSavedInfo();

  if (CSI.empty())
    return false;

  return FrameIdx >= CSI.front().getFrameIdx() &&
         FrameIdx <= CSI.back().getFrameIdx();
}

// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
void J2RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                         int SPAdj, unsigned FIOperandNum,
                                         RegScavenger *RS) const {
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  DebugLoc DL = MI.getDebugLoc();
  int FrameIdx = MI.getOperand(FIOperandNum).getIndex();
  auto isCalleeSaved = isCalleeSavedIndex(FrameIdx, MF.getFrameInfo());

  // Use SP for callee-saved registers.
  unsigned BaseReg = isCalleeSaved ? J2::R15 : getFrameRegister(MF);

  MI.getOperand(FIOperandNum).ChangeToRegister(BaseReg, false);
  // FIXME: Callee saved registers.
}
