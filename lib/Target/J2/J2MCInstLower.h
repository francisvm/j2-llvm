//===-- J2MCInstLower.h - Lower MachineInstr to MCInst ---------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2MCINSTLOWER_H
#define LLVM_LIB_TARGET_J2_J2MCINSTLOWER_H

#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
class MCContext;
class MCInst;
class MCOperand;
class MachineInstr;
class AsmPrinter;

/// J2MCInstLower - This class is used to lower an MachineInstr into an MCInst.
class LLVM_LIBRARY_VISIBILITY J2MCInstLower {
  MCContext &Ctx;

  AsmPrinter &Printer;

public:
  J2MCInstLower(MCContext &ctx, AsmPrinter &printer)
      : Ctx(ctx), Printer(printer) {}
  using MachineOperandType = MachineOperand::MachineOperandType;

  void Lower(const MachineInstr *MI, MCInst &OutMI) const;
  MCOperand LowerOperand(const MachineOperand &MO) const;

  MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;
  MCSymbol *GetGlobalAddressSymbol(const MachineOperand &MO) const;
  MCSymbol *GetExternalSymbolSymbol(const MachineOperand &MO) const;
};
}

#endif
