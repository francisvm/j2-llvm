//===-- J2ConstantPoolValue.cpp - J2 constant-pool value ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "J2ConstantPoolValue.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

J2ConstantPoolValue::J2ConstantPoolValue(const Constant *value)
    : MachineConstantPoolValue(value->getType()), Value(value) {}

J2ConstantPoolValue *J2ConstantPoolValue::Create(const Constant *Value) {
  return new J2ConstantPoolValue(Value);
}

int J2ConstantPoolValue::getExistingMachineCPValue(MachineConstantPool *CP,
                                                   unsigned Alignment) {
  unsigned AlignMask = Alignment - 1;
  const std::vector<MachineConstantPoolEntry> &Constants = CP->getConstants();
  for (unsigned I = 0, E = Constants.size(); I != E; ++I) {
    if (Constants[I].isMachineConstantPoolEntry() &&
        (Constants[I].getAlignment() & AlignMask) == 0) {
      auto *CPV =
          static_cast<J2ConstantPoolValue *>(Constants[I].Val.MachineCPVal);
      if (CPV->Value == Value)
        return I;
    }
  }
  return -1;
}

void J2ConstantPoolValue::addSelectionDAGCSEId(FoldingSetNodeID &ID) {
  ID.AddPointer(Value);
}

void J2ConstantPoolValue::print(raw_ostream &O) const { O << Value; }
