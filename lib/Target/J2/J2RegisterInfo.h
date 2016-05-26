//===-- J2RegisterInfo.h - J2 Register Information Impl ---------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_J2_J2REGISTERINFO_H
#define LLVM_LIB_TARGET_J2_J2REGISTERINFO_H

#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#define GET_REGINFO_ENUM
#include "J2GenRegisterInfo.inc"

namespace llvm {
struct J2RegisterInfo : public J2GenRegisterInfo {
public:
  J2RegisterInfo();

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
  /// Debug information queries.
  BitVector getReservedRegs(const MachineFunction &MF) const override;

  unsigned getFrameRegister(const MachineFunction &MF) const override {
    return J2::R14;
  }

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;
};
} // end namespace llvm

#endif
