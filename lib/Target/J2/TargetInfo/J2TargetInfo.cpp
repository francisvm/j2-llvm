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

namespace llvm {
Target &getTheJ2Target() {
  static Target TheJ2Target;
  return TheJ2Target;
}
} // namespace llvm

extern "C" void LLVMInitializeJ2TargetInfo() {
  TargetRegistry::RegisterTarget(getTheJ2Target(), "j2", "J-Core 2", "J2",
                                 [](Triple::ArchType) { return false; }, false);
  RegisterTarget<Triple::j2, /*HasJIT=*/false> X(getTheJ2Target(), "j2",
                                                 "J-Core 2", "J2");
}
