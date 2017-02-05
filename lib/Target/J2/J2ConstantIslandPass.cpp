//===-- J2ConstantIslandPass.cpp - Emit PC Relative loads -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass lowers the constant pool into pseudo-instructions ad the end of
// the function. This is useful for pc-relative calls.
//
//===----------------------------------------------------------------------===//

#include "J2TargetMachine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

#define DEBUG_TYPE "j2-cp-islands"

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

namespace {
  class J2ConstantIslands : public MachineFunctionPass {
  public:
    static char ID;
    J2ConstantIslands() : MachineFunctionPass{ID} {}

    MachineFunctionProperties getRequiredProperties() const override {
      return MachineFunctionProperties().set(
          MachineFunctionProperties::Property::NoVRegs);
    }

    StringRef getPassName() const override {
      return "J2 constant island placement";
    }

    bool runOnMachineFunction(MachineFunction &MF) override {
      const std::vector<MachineConstantPoolEntry> &CPs =
          MF.getConstantPool()->getConstants();

      if (CPs.empty())
        return false;

      auto *TII = MF.getSubtarget().getInstrInfo();

      auto *BB = MF.CreateMachineBasicBlock();
      BB->setAlignment(2);
      MF.ensureAlignment(2);
      MF.push_back(BB);


      for (size_t i = 0, e = CPs.size(); i != e; ++i) {
        auto Size = MF.getDataLayout().getTypeAllocSize(CPs[i].getType());
        BuildMI(*BB, BB->end(), DebugLoc(), TII->get(J2::CONSTPOOL_ENTRY))
            .addImm(i)
            .addConstantPoolIndex(i)
            .addImm(Size);
      }
      return true;
    }
  };

  char J2ConstantIslands::ID = 0;

} // end of anonymous namespace

FunctionPass *llvm::createJ2ConstantIslandPass() {
  return new J2ConstantIslands();
}
