//===-- J2TargetMachine.h - Define TargetMachine for J2 ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the J2 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2TARGETMACHINE_H
#define LLVM_LIB_TARGET_J2_J2TARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {

extern llvm::Target TheJ2Target;

class J2TargetMachine : public LLVMTargetMachine {
public:
  J2TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                  StringRef FS, const TargetOptions &Options,
                  Optional<Reloc::Model> RM, CodeModel::Model CM,
                  CodeGenOpt::Level OL);
};

} // end namespace llvm

#endif
