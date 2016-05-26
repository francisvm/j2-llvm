//===-- J2MCAsmInfo.cpp - J2 asm properties -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the J2MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "J2MCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

J2MCAsmInfo::J2MCAsmInfo(const Triple &TheTriple) {
  PointerSize = CalleeSaveStackSlotSize = 4;
  CommentString = ";";
  AlignmentIsInBytes = false;
  SupportsDebugInformation = true;
}
