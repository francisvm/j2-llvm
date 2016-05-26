//===-- J2MCAsmInfo.h - J2 asm properties ----------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the J2MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_MCTARGETDESC_J2MCASMINFO_H
#define LLVM_LIB_TARGET_J2_MCTARGETDESC_J2MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class J2MCAsmInfo : public MCAsmInfoELF {
public:
  explicit J2MCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
