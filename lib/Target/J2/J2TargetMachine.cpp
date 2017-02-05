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
#include "J2ISelDAGToDAG.h"
#include "J2TargetObjectFile.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeJ2Target() {
  // Register the target.
  RegisterTargetMachine<J2TargetMachine> X(TheJ2Target);
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

static std::string computeDataLayout(const Triple &TT) { return "e-p:32:32"; }

J2TargetMachine::J2TargetMachine(const Target &T, const Triple &TT,
                                 StringRef CPU, StringRef FS,
                                 const TargetOptions &Options,
                                 Optional<Reloc::Model> RM, CodeModel::Model CM,
                                 CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM), CM, OL),
      TLOF(make_unique<J2TargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

TargetPassConfig *J2TargetMachine::createPassConfig(PassManagerBase &PM) {
  struct J2PassConfig : public TargetPassConfig {
    J2PassConfig(J2TargetMachine *TM, PassManagerBase &PM)
        : TargetPassConfig(TM, PM) {}

    J2TargetMachine &getJ2TargetMachine() const {
      return getTM<J2TargetMachine>();
    }

    bool addInstSelector() override {
      addPass(new J2DAGToDAGISel(getJ2TargetMachine()));
      return false;
    }

    void addPreEmitPass() override {
      addPass(createJ2DelaySlotFillerPass(getJ2TargetMachine()));
      addPass(createJ2ConstantIslandPass());
    }
  };

  return new J2PassConfig(this, PM);
}
