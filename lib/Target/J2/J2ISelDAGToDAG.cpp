//===-- J2ISelDAGToDAG.cpp - A Dag to Dag Inst Selector for J2 ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the J2 target.
//
//===----------------------------------------------------------------------===//

#include "J2ISelDAGToDAG.h"
using namespace llvm;

#define DEBUG_TYPE "J2-isel"

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// J2DAGToDAGISel - J2 specific code to select J2 machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//

/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
void J2DAGToDAGISel::Select(SDNode *Node) {
  // Dump information about the Node being selected
  DEBUG(errs() << "Selecting: "; Node->dump(CurDAG); errs() << "\n");

  // Select the default instruction
  SelectCode(Node);
}
