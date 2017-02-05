//===-- J2MCTargetDesc.h - J2 Target Descriptions ---------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides J2 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_MCTARGETDESC_J2MCTARGETDESC_H
#define LLVM_LIB_TARGET_J2_MCTARGETDESC_J2MCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

extern Target TheJ2Target;

namespace J2_MC {
StringRef selectJ2CPU(const Triple &TT, StringRef CPU);
}

} // End llvm namespace

#define GET_REGINFO_ENUM
#include "J2GenRegisterInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "J2GenSubtargetInfo.inc"

#endif
