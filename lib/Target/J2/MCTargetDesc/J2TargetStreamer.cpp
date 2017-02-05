//===-- J2TargetStreamer.cpp - J2 Register Information -== ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the J2 implementation of the MCTargetStreamer class.
//
//===----------------------------------------------------------------------===//

#include "J2TargetStreamer.h"

using namespace llvm;

J2TargetStreamer::J2TargetStreamer(MCStreamer &S) : MCTargetStreamer{S} {}

J2TargetStreamer::~J2TargetStreamer() {}

J2TargetAsmStreamer::J2TargetAsmStreamer(MCStreamer &S) : J2TargetStreamer{S} {}

J2TargetELFStreamer::J2TargetELFStreamer(MCStreamer &S) : J2TargetStreamer{S} {}
