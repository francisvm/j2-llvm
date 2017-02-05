//===-- J2Disassembler.cpp - Disassembler for J2 --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/J2MCTargetDesc.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "j2-disassembler"

using DecodeStatus = MCDisassembler::DecodeStatus;

DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, unsigned RegNo,
                                    uint64_t Address, const void *Decoder) {
  Inst.addOperand(MCOperand::createReg(9 + RegNo));
  return MCDisassembler::Success;
}

#include "J2GenDisassemblerTables.inc"

namespace {
class J2Disassembler : public MCDisassembler {
public:
  using MCDisassembler::MCDisassembler;

  DecodeStatus getInstruction(MCInst &MI, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &OS, raw_ostream &CS) const override {
    if (Bytes.size() < 2) {
      Size = 0;
      return MCDisassembler::Fail;
    }

    uint16_t Insn = 0;
    DecodeStatus Result = MCDisassembler::Fail;

    Insn = (Bytes[1] << 8) | (Bytes[0] << 0);

    Result = decodeInstruction(DecoderTable16, MI, Insn, Address, this, STI);
    if (Result != MCDisassembler::Fail) {
      Size = 2;
      return Result;
    }

    return MCDisassembler::Fail;
  }
};

static MCDisassembler *createJ2Disassembler(const Target &T,
                                            const MCSubtargetInfo &STI,
                                            MCContext &Ctx) {
  return new J2Disassembler(STI, Ctx);
}
}

extern "C" void LLVMInitializeJ2Disassembler() {
  TargetRegistry::RegisterMCDisassembler(TheJ2Target, createJ2Disassembler);
}
