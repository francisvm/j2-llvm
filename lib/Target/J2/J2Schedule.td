//===-- J2Schedule.td - J2 Scheduling Definitions ----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Functional units across J2 chips sets. Based on GCC/J2 backend files.
//===----------------------------------------------------------------------===//

def ALU : FuncUnit;
def IMULDIV : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for J2
//===----------------------------------------------------------------------===//

// FIXME: Implement.
def II_FIXME : InstrItinClass;

//===----------------------------------------------------------------------===//
// J2 Generic instruction itineraries.
//===----------------------------------------------------------------------===//

// FIXME: Handle real pipeline.
def J2GenericItineraries : ProcessorItineraries<[ ALU, IMULDIV ], [], [
  InstrItinData<II_FIXME, [ InstrStage<1, [ ALU ]> ]>
]>;
