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
#include "llvm/MC/MCFixupKindInfo.h"
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
#define FIXUP(X)                                                               \
  case J2::fixup_J2_##X:                                                       \
    return ELF::R_J2_##X

    switch ((unsigned)Fixup.getKind()) {
      FIXUP(NONE);
      FIXUP(BSR);
      FIXUP(BRA);
      FIXUP(BT);
      FIXUP(GA);
      FIXUP(GA00);
      FIXUP(GA0000);
      FIXUP(GA000000);
    default:
      llvm_unreachable("Unkown fixup!");
    }
  }
};
}

MCObjectWriter *J2AsmBackend::createObjectWriter(raw_pwrite_stream &OS) const {
  auto *MOTW = new J2ELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType),
                                     ELF::EM_J2);
  return createELFObjectWriter(MOTW, OS, /* isLittle */ true);
}

const MCFixupKindInfo &J2AsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  static const MCFixupKindInfo Infos[J2::NumTargetFixupKinds] = {
      // Name         offset size flags
      {"fixup_J2_NONE", 0, 0, 0},
      {"fixup_J2_BSR", 4, 12, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_J2_BRA", 4, 12, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_J2_BT", 8, 8, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_J2_GA", 0, 8, 0},
      {"fixup_J2_GA00", 0, 8, 0},
      {"fixup_J2_GA0000", 0, 8, 0},
      {"fixup_J2_GA000000", 0, 8, 0},
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind");

  return Infos[Kind - FirstTargetFixupKind];
}

void J2AsmBackend::applyFixup(const MCFixup &Fixup, char *Data,
                              unsigned DataSize, uint64_t Value,
                              bool IsPCRel) const {
  auto Offset = Fixup.getOffset();
  Offset = 0;
  switch ((unsigned)Fixup.getKind()) {
  case J2::fixup_J2_BSR:
  case J2::fixup_J2_BRA:
  {
    // Displacement is by Value * 2;
    int64_t PcRelOffset = Value;
    PcRelOffset >>= 2;
    PcRelOffset &= 0xFFF;
    *(uint16_t *)(Data + Offset) &= (uint16_t)(~0) << 12;
    *(uint16_t *)(Data + Offset) |= PcRelOffset;
    break;
  }
  case J2::fixup_J2_BT:
  {
    // Displacement is by Value * 2;
    int64_t PcRelOffset = Value;
    PcRelOffset >>= 2;
    PcRelOffset &= 0xFF;
    *(uint16_t *)(Data + Offset) &= (uint16_t)(~0) << 8;
    *(uint16_t *)(Data + Offset) |= PcRelOffset;
    break;
  }
  case J2::fixup_J2_GA:
  {
    llvm::errs() << "Off: " << Offset << "\n";
    llvm::errs() << "Value: " << Value << "\n";
    *(uint16_t *)(Data + Offset) &= 0xFF00;
    *(uint16_t *)(Data + Offset) |= (Value >> 24) & 0xFF;
    break;
  }
  case J2::fixup_J2_GA00:
  {
    llvm::errs() << "Off: " << Offset << "\n";
    *(uint16_t *)(Data + Offset) &= 0xFF00;
    *(uint16_t *)(Data + Offset) |= (Value >> 16) & 0xFF00;
    break;
  }
  case J2::fixup_J2_GA0000:
  {
    llvm::errs() << "Off: " << Offset << "\n";
    *(uint16_t *)(Data + Offset) &= 0xFF00;
    *(uint16_t *)(Data + Offset) |= (Value >> 8) & 0xFF0000;
    break;
  }
  case J2::fixup_J2_GA000000:
  {
    llvm::errs() << "Off: " << Offset << "\n";
    *(uint16_t *)(Data + Offset) &= 0xFF00;
    *(uint16_t *)(Data + Offset) |= (Value >> 24) & 0xFF000000;
    break;
  }
  }
}

MCAsmBackend *llvm::createJ2AsmBackend(const Target &T,
                                       const MCRegisterInfo &MRI,
                                       const Triple &TT, StringRef CPU,
                                       const MCTargetOptions &Options) {
  return new J2AsmBackend(T, TT.getOS());
}
