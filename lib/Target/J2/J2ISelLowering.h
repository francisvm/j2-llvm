//===-- J2ISelLowering.h - J2 DAG Lowering Interface ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that J2 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2ISELLOWERING_H
#define LLVM_LIB_TARGET_J2_J2ISELLOWERING_H

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {
namespace J2ISD {
enum NodeType : unsigned {
  // Start the numbering from where ISD NodeType finishes.
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
};
}

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//

class J2TargetMachine;
class J2Subtarget;

class J2TargetLowering : public TargetLowering {
public:
  explicit J2TargetLowering(const J2TargetMachine &TM);
};
}

#endif
