//===-- J2FixupKinds.h - J2 Specific Fixup Entries --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_MCTARGETDESC_J2FIXUPKINDS_H
#define LLVM_LIB_TARGET_J2_MCTARGETDESC_J2FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace J2 {
enum Fixups {
  // Branch fixups resulting in R_J2_NONE.
  fixup_J2_NONE = FirstTargetFixupKind,

  // Branch and link
  fixup_J2_BSR,

  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
} // namespace J2
} // namespace llvm

#endif
