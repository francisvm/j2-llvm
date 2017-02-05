//===-- J2MachineFuctionInfo.h - J2 machine function info -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares J2-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_J2_MACHINE_FUNCTION_INFO_H
#define LLVM_J2_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// Contains J2-specific information for each MachineFunction.
class J2MachineFunctionInfo : public MachineFunctionInfo {
  /// Indicates if the register containing the frame size to be used.
  unsigned FrameSizeVreg = 0;

public:
  J2MachineFunctionInfo() = default;

  explicit J2MachineFunctionInfo(MachineFunction &MF) {}

  bool getFrameSizeVreg() const { return FrameSizeVreg; }
  void setFrameSizeVreg(unsigned Vreg) { FrameSizeVreg = Vreg; }
};

} // end llvm namespace

#endif // LLVM_J2_MACHINE_FUNCTION_INFO_H
