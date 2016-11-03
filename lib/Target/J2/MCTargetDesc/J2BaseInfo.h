//===-- J2BaseInfo.h - Top level definitions for J2 ---------- --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the J2 target useful for the compiler back-end and the MC libraries.
// As such, it deliberately does not include references to LLVM core
// code gen types, passes, etc..
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ARM_MCTARGETDESC_ARMBASEINFO_H
#define LLVM_LIB_TARGET_ARM_MCTARGETDESC_ARMBASEINFO_H

namespace llvm {

namespace J2II {
enum GlobalAddressSplit {
  GA = 1,
  GA00,
  GA0000,
  GA000000,
};
}

} // namespace llvm

#endif
