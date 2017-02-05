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
#include "InstPrinter/J2InstPrinter.h"
#include "J2MCInstLower.h"
#include "J2TargetMachine.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

void J2AsmPrinter::EmitInstruction(const MachineInstr *MI) {
  J2MCInstLower InstLower{OutContext, *this};

  auto MBB = MI->getParent();
  auto MII = MI->getIterator();
  do {
    MCInst TmpInst;
    InstLower.Lower(&*MII, TmpInst);
    EmitToStreamer(*OutStreamer, TmpInst);
    ++MII;
  } while (MII != MBB->instr_end() && MII->isInsideBundle());
}

void J2AsmPrinter::EmitFunctionEntryLabel() {
  OutStreamer->EmitLabel(CurrentFnSym);
}

extern "C" void LLVMInitializeJ2AsmPrinter() {
  RegisterAsmPrinter<J2AsmPrinter> X(TheJ2Target);
}
