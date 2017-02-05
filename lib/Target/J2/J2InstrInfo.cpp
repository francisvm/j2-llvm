//===-- J2InstrInfo.cpp - J2 Instruction Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the J2 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "J2InstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_CTOR_DTOR
#include "J2GenInstrInfo.inc"

#define GET_REGINFO_ENUM
#define GET_REGINFO_HEADER
#include "J2GenRegisterInfo.inc"

void J2InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI,
                              const DebugLoc &DL, unsigned DestReg,
                              unsigned SrcReg, bool KillSrc) const {
  unsigned Opc = J2::MOV32rr;

  if (J2::GPRRegClass.contains(DestReg) && J2::GPRRegClass.contains(SrcReg)) {
    BuildMI(MBB, MI, DL, get(Opc), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    return;
      }

  if (!J2::GPRRegClass.contains(SrcReg)) {
    Opc = [&]() -> unsigned {
      switch (SrcReg) {
#define CASE(Type, X)                                                          \
  case J2::X:                                                                  \
    return J2::ST##Type##X
        CASE(S, MACL);
        CASE(S, MACH);
        CASE(S, PR);
        CASE(C, GBR);
        CASE(C, VBR);
#undef CASE
      default:
        llvm_unreachable("Special register not handled.");
      }
    }();
    BuildMI(MBB, MI, DL, get(Opc), DestReg);
  } else if (!J2::GPRRegClass.contains(DestReg)) {
    Opc = [&]() -> unsigned {
      switch (SrcReg) {
#define CASE(Type, X)                                                          \
  case J2::X:                                                                  \
    return J2::LD##Type##X
        CASE(S, MACL);
        CASE(S, MACH);
        CASE(S, PR);
        CASE(C, GBR);
        CASE(C, VBR);
#undef CASE
      default:
        llvm_unreachable("Special register not handled.");
      }
    }();
    BuildMI(MBB, MI, DL, get(Opc)).addReg(SrcReg, getKillRegState(KillSrc));
  }

}

void J2InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                      MachineBasicBlock::iterator MI,
                                      unsigned SrcReg, bool isKill,
                                      int FrameIndex,
                                      const TargetRegisterClass *RC,
                                      const TargetRegisterInfo *TRI) const {
  DebugLoc DL = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();

  BuildMI(MBB, MI, DL, get(J2::MOV32rm))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FrameIndex);
}

void J2InstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator MI,
                                       unsigned DstReg, int FrameIndex,
                                       const TargetRegisterClass *RC,
                                       const TargetRegisterInfo *TRI) const {
  DebugLoc DL = MI != MBB.end() ? MI->getDebugLoc() : DebugLoc();

  BuildMI(MBB, MI, DL, get(J2::MOV32mr), DstReg).addFrameIndex(FrameIndex);
}
