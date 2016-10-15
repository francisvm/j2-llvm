//===-- J2MCCodeEmitter.cpp - Convert J2 Code to Machine Code -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the J2MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#include "J2MCCodeEmitter.h"
#include "J2FixupKinds.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "mccodeemitter"

#define GET_INSTRINFO_ENUM
#define GET_INSTRMAP_INFO
#include "J2GenInstrInfo.inc"

namespace llvm {
MCCodeEmitter *createJ2MCCodeEmitter(const MCInstrInfo &MCII,
                                     const MCRegisterInfo &MRI,
                                     MCContext &Ctx) {
  return new J2MCCodeEmitter(MCII, Ctx);
}
} // End of namespace llvm.

void J2MCCodeEmitter::EmitByte(unsigned char C, raw_ostream &OS) const {
  OS << (char)C;
}

void J2MCCodeEmitter::EmitInstruction(uint64_t Val, unsigned Size,
                                      const MCSubtargetInfo &STI,
                                      raw_ostream &OS) const {
  // Output the instruction encoding in little endian byte order.
  // Little-endian byte ordering:
  //   J2:   2 | 1
  for (unsigned i = 0; i < Size; ++i) {
    unsigned Shift = i * 8;
    EmitByte((Val >> Shift) & 0xff, OS);
  }
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned J2MCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                            const MCOperand &MO,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  // MO must be an Expr.
  assert(MO.isExpr() && MO.getExpr()->getKind() == MCExpr::SymbolRef);

  J2::Fixups FixupKind = J2::fixup_J2_NONE;

  switch (MI.getOpcode()) {
#define CASE(X)                                                                \
  case J2::X:                                                                  \
    FixupKind = J2::fixup_J2_##X;                                              \
    break

    CASE(BSR);
    CASE(BRA);
    CASE(BT);
  default:
    llvm_unreachable("Opcode not handled.");
  }

  Fixups.push_back(
      MCFixup::create(0, MO.getExpr(), MCFixupKind(FixupKind), MI.getLoc()));

  return 0;
}

/// encodeInstruction - Emit the instruction.
/// Size the instruction with Desc.getSize().
void J2MCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  uint32_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  // Get byte count of instruction
  unsigned Size = Desc.getSize();
  if (!Size)
    llvm_unreachable("Desc.getSize() returns 0");

  EmitInstruction(Binary, Size, STI, OS);
}

#include "J2GenMCCodeEmitter.inc"
