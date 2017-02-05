//===-- J2Subtarget.h - Define Subtarget for the J2 -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the J2 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2SUBTARGET_H
#define LLVM_LIB_TARGET_J2_J2SUBTARGET_H

#include "J2FrameLowering.h"
#include "J2ISelLowering.h"
#include "J2InstrInfo.h"
#include "J2RegisterInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Target/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "J2GenSubtargetInfo.inc"

namespace llvm {
class J2TargetMachine;

class J2Subtarget : public J2GenSubtargetInfo {
  Triple TargetTriple;

  J2RegisterInfo RegisterInfo;

  J2InstrInfo InstrInfo;

  J2FrameLowering FrameLowering;

  J2TargetLowering TargetLowering;

public:
  /// This constructor initializes the data members to match that
  /// of the specified triple.
  J2Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
              const J2TargetMachine &TM);

  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  const J2RegisterInfo *getRegisterInfo() const override {
    return &RegisterInfo;
  }

  const J2InstrInfo *getInstrInfo() const override { return &InstrInfo; }

  const J2FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }

  const J2TargetLowering *getTargetLowering() const override {
    return &TargetLowering;
  }
};
} // End llvm namespace

#endif
