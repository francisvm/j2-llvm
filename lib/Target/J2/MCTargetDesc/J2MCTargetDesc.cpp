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
#include "J2MCAsmInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#include "J2GenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "J2GenInstrInfo.inc"

static MCRegisterInfo *createJ2MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitJ2MCRegisterInfo(X, J2::PR);
  return X;
}

static MCInstrInfo *createJ2MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitJ2MCInstrInfo(X);
  return X;
}

static MCAsmInfo *createJ2MCAsmInfo(const MCRegisterInfo &MRI,
                                    const Triple &TT) {
  MCAsmInfo *MAI = new J2MCAsmInfo(TT);
  return MAI;
}

extern "C" void LLVMInitializeJ2TargetMC() {
  Target *T = &TheJ2Target;
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(*T, createJ2MCRegisterInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(*T, createJ2MCInstrInfo);

  // Register the MC asm info.
  RegisterMCAsmInfoFn X(*T, createJ2MCAsmInfo);
}
