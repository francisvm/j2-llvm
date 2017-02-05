//===-- J2AsmPrinter.cpp - J2 Register Information -== --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// J2 Assembly printer class.
//
//===----------------------------------------------------------------------===//

#include "J2AsmPrinter.h"
#include "J2ConstantPoolValue.h"
#include "InstPrinter/J2InstPrinter.h"
#include "J2MCInstLower.h"
#include "J2TargetMachine.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

using namespace llvm;

void J2AsmPrinter::EmitInstruction(const MachineInstr *MI) {
  J2MCInstLower InstLower{OutContext, *this};

  const DataLayout &DL = getDataLayout();
  auto MBB = MI->getParent();
  auto MF = MBB->getParent();
  auto MII = MI->getIterator();

  if (MI->getOpcode() == J2::CONSTPOOL_ENTRY) {
    auto LabelID = MI->getOperand(0).getImm();
    auto CPIdx = MI->getOperand(1).getIndex();

    // FIXME: Emit data region
    OutStreamer->EmitLabel(GetCPISymbol(LabelID));

    auto &MCPE = MF->getConstantPool()->getConstants()[CPIdx];
    if (MCPE.isMachineConstantPoolEntry())
      EmitMachineConstantPoolValue(MCPE.Val.MachineCPVal);
    else
      EmitGlobalConstant(DL, MCPE.Val.ConstVal);

    return;
  }

  do {
    MCInst TmpInst;
    InstLower.Lower(&*MII, TmpInst);
    EmitToStreamer(*OutStreamer, TmpInst);
    ++MII;
  } while (MII != MBB->instr_end() && MII->isInsideBundle());
}

void J2AsmPrinter::EmitMachineConstantPoolValue(
    MachineConstantPoolValue *MCPV) {
  const DataLayout &DL = getDataLayout();
  auto Size = DL.getTypeAllocSize(MCPV->getType());
  auto GV = static_cast<J2ConstantPoolValue *>(MCPV)->getGlobalValue();
  auto *Expr = MCSymbolRefExpr::create(getSymbol(GV), OutContext);
  OutStreamer->EmitValue(Expr, Size);
}

void J2AsmPrinter::EmitFunctionEntryLabel() {
  OutStreamer->EmitLabel(CurrentFnSym);
}

extern "C" void LLVMInitializeJ2AsmPrinter() {
  RegisterAsmPrinter<J2AsmPrinter> X(TheJ2Target);
}
