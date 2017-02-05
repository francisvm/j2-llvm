//===-- llvm/Target/J2TargetObjectFile.h - J2 Object Info -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_J2_J2TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class J2TargetObjectFile : public TargetLoweringObjectFileELF {};
} // end namespace llvm

#endif
