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

  SDLoc DL(Node);

  switch (Node->getOpcode()) {
  case ISD::FrameIndex:
    assert(Node->getValueType(0) == MVT::i32);

    int FI = cast<FrameIndexSDNode>(Node)->getIndex();
    SDValue TFI = CurDAG->getTargetFrameIndex(FI, MVT::i32);
    const auto Opc = J2::MOV32rr;
    CurDAG->SelectNodeTo(Node, Opc, MVT::i32, TFI);
    return;
  }

  // Select the default instruction
  SelectCode(Node);
}

namespace {
#define j2log2(N) j2log2<(N)>::value
template <uint64_t A> struct j2log2 {
  static const uint8_t value = 1 + j2log2(A / 2);
};

template <> struct j2log2<1> { static const uint8_t value = 0; };
}

// SelectAddr for loads/stores of N bits.
template <size_t N>
bool J2DAGToDAGISel::SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset) {
  if (Addr->use_size() != 1)
    return false;

  auto *Use = *Addr->use_begin();
  auto Opcode = Use->getOpcode();
  if (!(Opcode == ISD::LOAD || Opcode == ISD::STORE))
    return false;

  SDLoc DL{Addr};

  // Combine constant offsets.
  if (CurDAG->isBaseWithConstantOffset(Addr)) {
    auto *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
    if (isUInt<j2log2(N)>(CN->getZExtValue())) {
      if (auto *FIN = dyn_cast<FrameIndexSDNode>(Addr.getOperand(0)))
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
      else
        Base = Addr.getOperand(0);

      // 16 bit displacement is * 2
      // 32 bit displacement is * 4
      Offset = CurDAG->getTargetConstant(CN->getZExtValue() >> (j2log2(N) - 3),
                                         DL, MVT::i32);
      return true;
    }
  }
  return false;
}
