//===-- J2AsmBackend.h - J2 Asm Backend  ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the J2AsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_J2_MCTARGETDESC_J2ASMBACKEND_H
#define LLVM_LIB_TARGET_J2_MCTARGETDESC_J2ASMBACKEND_H

#include "MCTargetDesc/J2FixupKinds.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCObjectWriter.h"

namespace llvm {

class MCAssembler;
struct MCFixupKindInfo;
class Target;
class MCObjectWriter;

class J2AsmBackend : public MCAsmBackend {
  Triple::OSType OSType;

public:
  J2AsmBackend(const Target &T, Triple::OSType OSType)
      : MCAsmBackend(), OSType(OSType) {}

  MCObjectWriter *createObjectWriter(raw_pwrite_stream &OS) const override;

  void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
                  uint64_t Value, bool IsPCRel) const override;

  unsigned getNumFixupKinds() const override { return J2::NumTargetFixupKinds; }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;

  /// @name Target Relaxation Interfaces
  /// @{

  /// MayNeedRelaxation - Check whether the given instruction may need
  /// relaxation.
  ///
  /// \param Inst - The instruction to test.
  bool mayNeedRelaxation(const MCInst &Inst) const override { return false; }

  /// fixupNeedsRelaxation - Target specific predicate for whether a given
  /// fixup requires the associated instruction to be relaxed.
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    // FIXME.
    llvm_unreachable("RelaxInstruction() unimplemented");
    return false;
  }

  /// RelaxInstruction - Relax the instruction in the given fragment
  /// to the next wider instruction.
  ///
  /// \param Inst - The instruction to relax, which may be the same
  /// as the output.
  /// \param [out] Res On return, the relaxed instruction.
  void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI,
                        MCInst &Res) const override {
    // FIXME.
    llvm_unreachable("RelaxInstruction() unimplemented");
  }

  /// @}

  bool writeNopData(uint64_t Count, MCObjectWriter *OW) const override {
    if ((Count % 2) != 0)
      return false;

    for (uint64_t i = 0; i < Count; i += 2)
      OW->write16(0x9);

    return true;
  }
}; // class J2AsmBackend

} // namespace

#endif
