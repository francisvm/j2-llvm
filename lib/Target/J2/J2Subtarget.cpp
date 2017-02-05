//===-- J2Subtarget.cpp - J2 Subtarget Information ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the J2 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "J2Subtarget.h"

using namespace llvm;

#define DEBUG_TYPE "J2-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "J2GenSubtargetInfo.inc"

static constexpr size_t StackAlignment = 4;

J2Subtarget::J2Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                         const J2TargetMachine &TM)
    : J2GenSubtargetInfo(TT, CPU, FS), TargetTriple{TT}, RegisterInfo{},
      InstrInfo{}, FrameLowering{StackAlignment}, TargetLowering{TM} {}
