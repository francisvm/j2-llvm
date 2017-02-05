//===-- J2InstPrinter.cpp - Convert J2 MCInst to assembly syntax ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an J2 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "J2InstPrinter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetInstrInfo.h"
using namespace llvm;

#define PRINT_ALIAS_INSTR
#include "J2GenAsmWriter.inc"

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

void J2InstPrinter::printInst(const MCInst *MI, raw_ostream &O, StringRef Annot,
                              const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
}

void J2InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                 raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    O << StringRef(getRegisterName(Op.getReg())).lower();
  } else if (Op.isImm()) {
    O << "#" << formatImm((int8_t)Op.getImm());
  } else {
    assert(Op.isExpr() && "unknown operand kind");
    O << *Op.getExpr();
  }
}

void J2InstPrinter::printMemOperand(const MCInst *MI, int OpNo, raw_ostream &O,
                                    const char *Modifier) {
  const MCOperand &RegOp = MI->getOperand(OpNo);
  const MCOperand &OffsetOp = MI->getOperand(OpNo + 1);

  assert(OffsetOp.isImm() && "Expected an immediate");
  assert(RegOp.isReg() && "Register operand not a register");

  O << '(' << formatImm(OffsetOp.getImm()) << ", "
    << getRegisterName(RegOp.getReg()) << ')';
}
