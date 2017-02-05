//===-- J2FrameLowering.h - Define frame lowering for J2 --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// J2 frame lowering.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2FRAMELOWERING_H
#define LLVM_LIB_TARGET_J2_J2FRAMELOWERING_H

#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class J2FrameLowering : public TargetFrameLowering {
public:
  explicit J2FrameLowering(unsigned Alignment)
      : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment) {}

  bool hasFP(const MachineFunction &MF) const override { return true; }

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
};

} // End llvm namespace

#endif
