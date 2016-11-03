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

  // Ret
  Ret,

  // Call
  Call,

  // Wrapper
  Wrapper,

  // Left logical shift,
  SHL,

  // Right logical shift,
  SRL,
};
}

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//

class J2TargetMachine;
class J2Subtarget;

class J2TargetLowering : public TargetLowering {
public:
  explicit J2TargetLowering(const J2TargetMachine &TM, const J2Subtarget &STI);

  const char *getTargetNodeName(unsigned Opcode) const override;

  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *BB) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &dl, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  template <enum J2ISD::NodeType Opcode>
  SDValue LowerShift(SDValue Op, SelectionDAG &DAG) const;

  MachineBasicBlock *EmitMOV32irImm(MachineInstr &MI,
                                    MachineBasicBlock *MBB) const;

  MachineBasicBlock *EmitMOV32irGlobalAddress(MachineInstr &MI,
                                              MachineBasicBlock *MBB) const;
};
}

#endif
