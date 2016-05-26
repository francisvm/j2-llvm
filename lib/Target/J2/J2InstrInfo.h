//===-- J2InstrInfo.h - J2 Instruction Information --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the J2 implementation of the TargetInstrInfo class.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2INSTRINFO_H
#define LLVM_LIB_TARGET_J2_J2INSTRINFO_H

#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "J2GenInstrInfo.inc"

namespace llvm {
class J2InstrInfo : public J2GenInstrInfo {
};
}

#endif
