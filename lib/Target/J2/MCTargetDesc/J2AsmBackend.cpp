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
#include "llvm/Support/Endian.h"
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
      FIXUP(PC2_12);
#undef FIXUP
    default:
      llvm_unreachable("Unkown fixup!");
    }
  }
};
}

std::unique_ptr<MCObjectWriter>
J2AsmBackend::createObjectWriter(raw_pwrite_stream &OS) const {
  auto MOTW = llvm::make_unique<J2ELFObjectWriter>(
      MCELFObjectTargetWriter::getOSABI(OSType), ELF::EM_J2);
  return createELFObjectWriter(std::move(MOTW), OS, /* isLittle */ true);
}

const MCFixupKindInfo &J2AsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  static const MCFixupKindInfo Infos[J2::NumTargetFixupKinds] = {
      // Name               offset size flags
      {"fixup_J2_NONE",     0,     0,   0},
      {"fixup_J2_PC2_12",   4,     12,  MCFixupKindInfo::FKF_IsPCRel},
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind");

  return Infos[Kind - FirstTargetFixupKind];
}

namespace {
template <size_t Size, size_t Multiply>
void applyDisplacement(int64_t Value, MutableArrayRef<char> Data) {
  assert(isInt<Size>(Value) && "Value too big for relocation.");

  char *RawData = Data.data();

  Value /= Multiply;
  Value &= ~(uint64_t(~0) << Size);
  support::endian::write64le(RawData,
                             support::endian::read64le(RawData) | Value);
}
}

void J2AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                              const MCValue &Target, MutableArrayRef<char> Data,
                              uint64_t Value, bool IsResolved) const {
  auto Offset = Fixup.getOffset();
  switch ((unsigned)Fixup.getKind()) {
  case J2::fixup_J2_PC2_12:
    applyDisplacement<12 /* Size */, 2 /* Multiply */>(Value,
                                                       Data.slice(Offset));
    break;
  default:
    llvm_unreachable("Unkown fixup!");
  }
}

MCAsmBackend *llvm::createJ2AsmBackend(const Target &T,
                                       const MCRegisterInfo &MRI,
                                       const Triple &TT, StringRef CPU,
                                       const MCTargetOptions &Options) {
  return new J2AsmBackend(T, TT.getOS());
}
