//===-- J2ISelLowering.cpp - J2 DAG Lowering Implementation ---------------===//
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

#include "J2ISelLowering.h"
#include "J2TargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"

using namespace llvm;

#include "J2GenCallingConv.inc"

J2TargetLowering::J2TargetLowering(const J2TargetMachine &TM,
                                   const J2Subtarget &STI)
    : TargetLowering(TM) {
  // Add GPR class as i32 registers.
  addRegisterClass(MVT::i32, &J2::GPRRegClass);
  computeRegisterProperties(STI.getRegisterInfo());
}

const char *J2TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((J2ISD::NodeType)Opcode) {
  case J2ISD::FIRST_NUMBER:
    break;
#define CASE(X)                                                                \
  case J2ISD::X:                                                               \
    return #X

    CASE(Ret);

#undef CASE
  }

  return nullptr;
}

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.

SDValue J2TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  assert(!IsVarArg && "Variable arguments not supported.");

  auto &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_J2);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    MVT RegVT = VA.getLocVT();
    const TargetRegisterClass *RC = getRegClassFor(RegVT);
    unsigned Reg = MF.addLiveIn(VA.getLocReg(), RC);
    SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);
    InVals.push_back(ArgValue);
  }

  return Chain;
}

SDValue
J2TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                              bool isVarArg,
                              const SmallVectorImpl<ISD::OutputArg> &Outs,
                              const SmallVectorImpl<SDValue> &OutVals,
                              const SDLoc &DL, SelectionDAG &DAG) const {
  assert(!isVarArg && "Variable arguments not supported.");

  auto &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 4> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_J2);

  // We need to chain instructions together.
  SmallVector<SDValue, 4> RetOps{Chain};
  // But also, we need to glue them in order to avoid inserting other
  // instructions in the middle of the lowering.
  SDValue Glue;

  for (size_t i = 0; i < RVLocs.size(); ++i) {
    // Copy the result values into the output registers.
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Return in memory is not supported!");
    SDValue ValToCopy = OutVals[i];

    // Copy return value to the return register. Update chain and glue.
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), ValToCopy, Glue);
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // Update the initial chain.
  RetOps[0] = Chain;
  if (Glue.getNode())
    RetOps.push_back(Glue);

  return DAG.getNode(J2ISD::Ret, DL, MVT::Other, RetOps);
}
