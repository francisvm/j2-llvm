//===-- J2MCTargetDesc.cpp - J2 Target Descriptions -----------------------===//
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

#include "J2MCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

extern "C" void LLVMInitializeJ2TargetMC() {
  Target *T = &TheJ2Target;
}
