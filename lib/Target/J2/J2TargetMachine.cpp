//===-- J2TargetMachine.cpp - Define TargetMachine for J2 00---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "J2TargetMachine.h"
#include "MCTargetDesc/J2MCTargetDesc.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeJ2Target() {
  // Register the target.
  RegisterTargetMachine<J2TargetMachine> X(getTheJ2Target());
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

static CodeModel::Model getEffectiveCodeModel(Optional<CodeModel::Model> CM) {
  if (CM)
    return *CM;
  return CodeModel::Small;
}

static std::string computeDataLayout(const Triple &TT) { return "e-p:32:32"; }

J2TargetMachine::J2TargetMachine(const Target &T, const Triple &TT,
                                 StringRef CPU, StringRef FS,
                                 const TargetOptions &Options,
                                 Optional<Reloc::Model> RM,
                                 Optional<CodeModel::Model> CM,
                                 CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM), getEffectiveCodeModel(CM),
                        OL) {}
