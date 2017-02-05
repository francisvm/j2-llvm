//===-- J2TargetInfo.cpp - J2 Target Implementation -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "J2TargetMachine.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheJ2Target;

extern "C" void LLVMInitializeJ2TargetInfo() {
  RegisterTarget<Triple::j2,/*HasJIT=*/false> X(TheJ2Target, "J2", "J2");
}
