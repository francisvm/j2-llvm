//===-- J2AsmBackend.cpp - J2 Asm Backend  --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the J2AsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#include "MCTargetDesc/J2AsmBackend.h"
#include "MCTargetDesc/J2MCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class J2ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  J2ELFObjectWriter(uint8_t OSABI, uint16_t EMachine)
      : MCELFObjectTargetWriter(/*Is64Bit=*/false, OSABI, EMachine,
                                /*HasRelocationAddend=*/true) {}

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override {
    return ELF::R_J2_NONE;
  }
};
}

MCObjectWriter *J2AsmBackend::createObjectWriter(raw_pwrite_stream &OS) const {
  auto *MOTW = new J2ELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType),
                                     ELF::EM_J2);
  return createELFObjectWriter(MOTW, OS, /* isLittle */ true);
}

MCAsmBackend *llvm::createJ2AsmBackend(const Target &T,
                                       const MCRegisterInfo &MRI,
                                       const Triple &TT, StringRef CPU,
                                       const MCTargetOptions &Options) {
  return new J2AsmBackend(T, TT.getOS());
}
