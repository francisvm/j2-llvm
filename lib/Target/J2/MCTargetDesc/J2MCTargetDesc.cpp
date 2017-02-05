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
#include "InstPrinter/J2InstPrinter.h"
#include "J2MCAsmInfo.h"
#include "J2TargetStreamer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSchedule.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#include "J2GenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "J2GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "J2GenSubtargetInfo.inc"

StringRef J2_MC::selectJ2CPU(const Triple &TT, StringRef CPU) {
  if (CPU.empty() || CPU == "generic") {
    CPU = "j2";
  }
  return CPU;
}

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

static MCTargetStreamer *createJ2AsmTargetStreamer(MCStreamer &S,
                                                   formatted_raw_ostream &OS,
                                                   MCInstPrinter *InstPrint,
                                                   bool isVerboseAsm) {
  return new J2TargetAsmStreamer(S);
}

static MCTargetStreamer *
createJ2ObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new J2TargetELFStreamer(S);
}

static MCInstPrinter *createJ2MCInstPrinter(const Triple &T,
                                            unsigned SyntaxVariant,
                                            const MCAsmInfo &MAI,
                                            const MCInstrInfo &MII,
                                            const MCRegisterInfo &MRI) {
  return new J2InstPrinter(MAI, MII, MRI);
}

static MCSubtargetInfo *createJ2MCSubtargetInfo(const Triple &TT, StringRef CPU,
                                                StringRef FS) {
  CPU = J2_MC::selectJ2CPU(TT, CPU);
  return createJ2MCSubtargetInfoImpl(TT, CPU, FS);
}

extern "C" void LLVMInitializeJ2TargetMC() {
  Target *T = &TheJ2Target;
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(*T, createJ2MCRegisterInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(*T, createJ2MCInstrInfo);

  // Register the MC asm info.
  RegisterMCAsmInfoFn X(*T, createJ2MCAsmInfo);

  // Register the asm target streamer.
  TargetRegistry::RegisterAsmTargetStreamer(*T, createJ2AsmTargetStreamer);
  // Register the object target streamer.
  TargetRegistry::RegisterObjectTargetStreamer(*T,
                                               createJ2ObjectTargetStreamer);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(*T, createJ2MCInstPrinter);

  // Register the MCSubtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(*T, createJ2MCSubtargetInfo);

  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(*T, createJ2AsmBackend);
}
