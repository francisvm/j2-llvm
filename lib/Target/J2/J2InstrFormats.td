//===-- J2InstrFormats.td - J2 Instruction Formats ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe J2 instructions format - 16bit instructions.
//
//  opcode  - operation code.
//  src     - source register.
//  dst     - destination register.
//  disp    - scaled displacement.
//  imm     - signed immediate.
//  data    - instruction data.
//
//===----------------------------------------------------------------------===//

// Format specifies the encoding used by the instruction.  This is part of the
// ad-hoc solution used to emit machine instruction encodings by our machine
// code emitter.
class Format<bits<4> val> {
  bits<4> Value = val;
}

// Generic J2 Format
class J2Inst<dag outs, dag ins, string asmstr, list<dag> pattern,
             InstrItinClass itin, Format f> : Instruction {
  field bits<16> Inst;
  Format Form = f;

  let Namespace = "J2";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
  let Size = 2;
  let Itinerary = itin;
  field bits<16> SoftFail = 0;
}

//===----------------------------------------------------------------------===//
// Format ZERO instruction class in J2 : <|opcode|>
//===----------------------------------------------------------------------===//

def ZERO : Format<0>;

class FZERO<bits<16> op, dag outs, dag ins, string asmstr, list<dag> pattern,
            InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, ZERO> {

  let Inst{15 - 0} = op;
}

//===----------------------------------------------------------------------===//
// Format N instruction class in J2 : <|opcode|dst|data>
//===----------------------------------------------------------------------===//

def N : Format<1>;

class FN<bits<4> op, bits<8> data, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, N> {
  bits<4> dst;

  let Inst{15 - 12} = op;
  let Inst{11 - 8} = dst;
  let Inst{7 - 0} = data;
}

//===----------------------------------------------------------------------===//
// Format M instruction class in J2 : <|opcode|src|data>
//===----------------------------------------------------------------------===//

def M : Format<2>;

class FM<bits<4> op, bits<8> data, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, M> {
  bits<4> src;

  let Inst{15 - 12} = op;
  let Inst{11 - 8} = src;
  let Inst{7 - 0} = data;
}

//===----------------------------------------------------------------------===//
// Format NM instruction class in J2 : <|opcode|dst|src|data>
//===----------------------------------------------------------------------===//

def NM : Format<3>;

class FNM<bits<4> op, bits<4> data, dag outs, dag ins, string asmstr,
          list<dag> pattern, InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, NM> {
  bits<4> src;
  bits<4> dst;

  let Inst{15 - 12} = op;
  let Inst{11 - 8} = dst;
  let Inst{7 - 4} = src;
  let Inst{3 - 0} = data;
}

//===----------------------------------------------------------------------===//
// Format MD instruction class in J2 : <|opcode|src|disp>
//===----------------------------------------------------------------------===//

def MD : Format<4>;

class FMD<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin> : J2Inst<outs, ins, asmstr, pattern, itin, NM> {
  bits<4> src;
  bits<4> disp;

  let Inst{15 - 8} = op;
  let Inst{7 - 4} = src;
  let Inst{3 - 0} = disp;
  let Defs = [ R0 ];
}

//===----------------------------------------------------------------------===//
// Format ND4 instruction class in J2 : <|opcode|dst|disp>
//===----------------------------------------------------------------------===//

def ND4 : Format<5>;

class FND4<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern,
           InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, ND4> {
  bits<4> dst;
  bits<4> disp;

  let Inst{15 - 8} = op;
  let Inst{7 - 4} = dst;
  let Inst{3 - 0} = disp;
  let Uses = [ R0 ];
}

//===----------------------------------------------------------------------===//
// Format NMD instruction class in J2 : <|opcode|dst|src|disp>
//===----------------------------------------------------------------------===//

def NMD : Format<6>;

class FNMD<bits<4> op, dag outs, dag ins, string asmstr, list<dag> pattern,
           InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, NMD> {
  bits<4> dst;
  bits<4> src;
  bits<4> disp;

  let Inst{15 - 12} = op;
  let Inst{11 - 8} = dst;
  let Inst{7 - 4} = src;
  let Inst{3 - 0} = disp;
}

//===----------------------------------------------------------------------===//
// Format D instruction class in J2 : <|opcode|disp>
//===----------------------------------------------------------------------===//

def D : Format<7>;

class FD<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern,
         InstrItinClass itin> : J2Inst<outs, ins, asmstr, pattern, itin, D> {
  bits<8> disp;

  let Inst{15 - 8} = op;
  let Inst{7 - 0} = disp;
}

//===----------------------------------------------------------------------===//
// Format D12 instruction class in J2 : <|opcode|disp>
//===----------------------------------------------------------------------===//

def D12 : Format<8>;

class FD12<bits<4> op, dag outs, dag ins, string asmstr, list<dag> pattern,
           InstrItinClass itin> : J2Inst<outs, ins, asmstr, pattern, itin, D> {
  bits<12> disp;

  let Inst{15 - 12} = op;
  let Inst{11 - 0} = disp;
}

//===----------------------------------------------------------------------===//
// Format ND8 instruction class in J2 : <|opcode|dst|disp>
//===----------------------------------------------------------------------===//

def ND8 : Format<9>;

class FND8<bits<4> op, dag outs, dag ins, string asmstr, list<dag> pattern,
           InstrItinClass itin>
    : J2Inst<outs, ins, asmstr, pattern, itin, ND8> {
  bits<4> dst;
  bits<8> disp;

  let Inst{15 - 12} = op;
  let Inst{11 - 8} = dst;
  let Inst{7 - 0} = disp;
}

//===----------------------------------------------------------------------===//
// Format I instruction class in J2 : <|opcode|imm>
//===----------------------------------------------------------------------===//

def I : Format<10>;

class FI<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern,
         InstrItinClass itin> : J2Inst<outs, ins, asmstr, pattern, itin, I> {
  bits<8> imm;

  let Inst{15 - 8} = op;
  let Inst{7 - 0} = imm;
}

//===----------------------------------------------------------------------===//
// Format NI instruction class in J2 : <|opcode|dst|imm>
//===----------------------------------------------------------------------===//

def NI : Format<11>;

class FNI<bits<4> op, dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin> : J2Inst<outs, ins, asmstr, pattern, itin, NI> {
  bits<4> dst;
  bits<8> imm;

  let Inst{15 - 12} = op;
  let Inst{11 - 8} = dst;
  let Inst{7 - 0} = imm;
}
