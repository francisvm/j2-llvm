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

using namespace llvm;

J2TargetLowering::J2TargetLowering(const J2TargetMachine &TM)
    : TargetLowering(TM) {}
