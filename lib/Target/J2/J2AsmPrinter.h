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

#ifndef LLVM_LIB_TARGET_J2_J2ASMPRINTER_H
#define LLVM_LIB_TARGET_J2_J2ASMPRINTER_H

#include "J2AsmPrinter.h"
#include "llvm/CodeGen/AsmPrinter.h"

namespace llvm {

class J2AsmPrinter : public AsmPrinter {
public:
  J2AsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "J2 Assembly Printer"; }

  void EmitInstruction(const MachineInstr *MI) override;

  void EmitFunctionEntryLabel() override;

  void EmitMachineConstantPoolValue(MachineConstantPoolValue *MCPV) override;

  void EmitConstantPool() override { /* Emitted customly. */ }
};

} // namespace llvm

#endif
