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
  setStackPointerRegisterToSaveRestore(J2::R15);

  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::SHL, MVT::i32, Custom);
  setOperationAction(ISD::SRL, MVT::i32, Custom);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);

  setOperationAction(ISD::GlobalAddress, MVT::i32, Custom);
}

const char *J2TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((J2ISD::NodeType)Opcode) {
  case J2ISD::FIRST_NUMBER:
    break;
#define CASE(X)                                                                \
  case J2ISD::X:                                                               \
    return #X

    CASE(Ret);
    CASE(Call);
    CASE(Wrapper);

#undef CASE
  }

  return nullptr;
}

SDValue J2TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  default:
    llvm_unreachable("unimplemented operation");
  }
}

SDValue J2TargetLowering::LowerGlobalAddress(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDLoc DL{Op};
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  auto Offset = cast<GlobalAddressSDNode>(Op)->getOffset();
  SDValue GA = DAG.getTargetGlobalAddress(GV, DL, MVT::i32, Offset);

  return DAG.getNode(J2ISD::Wrapper, DL, MVT::i32, GA);
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

SDValue J2TargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                    SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  bool isVarArg = CLI.IsVarArg;

  CLI.IsTailCall = false;

  assert(!isVarArg && "Variable arguments not supported.");

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_J2);

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  // FIXME: Handle more than 4 arguments.

  // But also, we need to glue them in order to avoid inserting other
  // instructions in the middle of the lowering.
  SDValue Glue;
  for (size_t i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = OutVals[i];

    if (VA.isRegLoc()) {
      // Copy the argument to the argument register. Update chain and glue.
      Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Arg, Glue);
      Glue = Chain.getValue(1);
      RegsToPass.emplace_back(VA.getLocReg(), Arg);
    }

    if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL, MVT::i32);
    else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
      Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);
  }

  SmallVector<SDValue, 8> Ops{Chain, Callee};
  std::transform(RegsToPass.begin(), RegsToPass.end(), std::back_inserter(Ops),
                 [&](std::pair<unsigned, SDValue> &elt) {
                   return DAG.getRegister(elt.first, elt.second.getValueType());
                 });

  // FIXME: Caller save registers.

  if (Glue.getNode()) // Push the glue, if it's present.
    Ops.push_back(Glue);

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  Chain = DAG.getNode(J2ISD::Call, DL, NodeTys, Ops);
  Glue = Chain.getValue(1);

  {
    // Return value.
    SmallVector<CCValAssign, 16> RVLocs;
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                   *DAG.getContext());
    CCInfo.AnalyzeCallResult(Ins, RetCC_J2);

    for (auto &Loc : RVLocs) {
      auto RetValue =
          DAG.getCopyFromReg(Chain, DL, Loc.getLocReg(), Loc.getValVT(), Glue);
      Chain = RetValue.getValue(1);
      Glue = RetValue.getValue(2);
      InVals.push_back(Chain.getValue(0));
    }
  }

  return Chain;
}
