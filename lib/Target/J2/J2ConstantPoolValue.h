//===- J2ConstantPoolValue.h - J2 constant-pool value -*- C++ -----------*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2CONSTANTPOOLVALUE_H
#define LLVM_LIB_TARGET_J2_J2CONSTANTPOOLVALUE_H

#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

class GlobalValue;

/// A J2-specific constant pool value.
class J2ConstantPoolValue : public MachineConstantPoolValue {
  const Constant *Value;

protected:
  J2ConstantPoolValue(const Constant *Value);

public:
  static J2ConstantPoolValue *Create(const Constant *Value);

  int getExistingMachineCPValue(MachineConstantPool *CP,
                                unsigned Alignment) override;
  void addSelectionDAGCSEId(FoldingSetNodeID &ID) override;
  void print(raw_ostream &O) const override;

  // Access J2-specific fields.
  const Constant *getValue() const { return Value; }
  const GlobalValue *getGlobalValue() const { return cast<GlobalValue>(Value); }
};

} // end namespace llvm

#endif
