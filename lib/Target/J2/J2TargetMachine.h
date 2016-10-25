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

#include "J2Subtarget.h"
#include "llvm/Target/TargetMachine.h"
#include <memory>
#include <sstream>

namespace llvm {

extern llvm::Target TheJ2Target;

class J2TargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  J2Subtarget Subtarget;

public:
  J2TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                  StringRef FS, const TargetOptions &Options,
                  Optional<Reloc::Model> RM, CodeModel::Model CM,
                  CodeGenOpt::Level OL);

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  const J2Subtarget *getSubtargetImpl(const Function &F) const override {
    return &Subtarget;
  }
};

FunctionPass *createJ2DelaySlotFillerPass(const J2TargetMachine & /*tm*/);

} // end namespace llvm

#endif
