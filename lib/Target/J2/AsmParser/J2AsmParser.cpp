//===-- J2AsmParser.cpp - Parse J2 assembly to MCInst instructions --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "J2TargetMachine.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include <memory>

using namespace llvm;

#define GET_INSTRINFO_ENUM
#include "J2GenInstrInfo.inc"

// Generated.
static unsigned MatchRegisterName(StringRef Name);

namespace {
struct J2Operand : public MCParsedAsmOperand {

  enum KindTy {
    Token,
    Register,
    Immediate,
  } Kind;

  struct TokOp {
    const char *Data;
    unsigned Length;
  };

  struct RegOp {
    unsigned RegNum;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  SMLoc StartLoc, EndLoc;

  union {
    TokOp Tok;
    RegOp Reg;
    ImmOp Imm;
  };

  J2Operand(KindTy K) : MCParsedAsmOperand{}, Kind{K} {}

  J2Operand(const J2Operand &o) : MCParsedAsmOperand{} {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;

    switch (Kind) {
    case Register:
      Reg = o.Reg;
      break;
    case Immediate:
      Imm = o.Imm;
      break;
    case Token:
      Tok = o.Tok;
      break;
    }
  }

  bool isToken() const override { return Kind == Token; }
  bool isReg() const override { return Kind == Register; }
  bool isImm() const override { return Kind == Immediate; }
  // FIXME: Mem.
  bool isMem() const override { return false; }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const override { return EndLoc; }

  unsigned getReg() const override {
    assert(Kind == Register && "Invalid type access!");
    return Reg.RegNum;
  }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case Register:
      OS << "<register r" << getReg() << ">";
      break;
    case Immediate:
      OS << *Imm.Val;
      break;
    case Token:
      OS << "'" << StringRef{Tok.Data, Tok.Length} << "'";
      break;
    }
  }

  static std::unique_ptr<J2Operand> createToken(StringRef Str, SMLoc S) {
    auto Op = make_unique<J2Operand>(Token);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<J2Operand> createReg(unsigned RegNo, SMLoc S,
                                              SMLoc E) {
    auto Op = make_unique<J2Operand>(Register);
    Op->Reg.RegNum = RegNo;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<J2Operand> createImm(const MCExpr *Val, SMLoc S,
                                              SMLoc E) {
    auto Op = make_unique<J2Operand>(Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    Inst.addOperand(MCOperand::createReg(getReg()));
  }
  void addImmOperands(MCInst &Inst, unsigned N) const {
    Inst.addOperand(MCOperand::createImm(N));
  }

  StringRef getToken() const {
    // FIXME : Implement.
    llvm_unreachable("Not implemented yet");
    return {};
  }
};
}

class J2AsmParser : public MCTargetAsmParser {
  const MCSubtargetInfo &STI;

public:
  J2AsmParser(const MCSubtargetInfo &STIIN, MCAsmParser &P,
              const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STIIN), STI{STIIN} {}

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override {
    MCAsmParser &Parser = getParser();
    RegNo = 0;
    const AsmToken &Tok = Parser.getTok();
    StartLoc = Tok.getLoc();
    EndLoc = Tok.getEndLoc();

    if (Tok.isNot(AsmToken::Identifier))
      return Error(StartLoc, "invalid register name");

    RegNo = MatchRegisterName(Tok.getIdentifier());

    if (RegNo == 0)
      return Error(StartLoc, "invalid register name");

    Parser.Lex();
    return false;
  }

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override {
    // FIXME: Implement.
    return true;
  }

  bool ParseDirective(AsmToken DirectiveID) override {
    // FIXME: Implement.
    return true;
  }

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override {
    MCInst Inst;
    SMLoc ErrorLoc;

    switch (
        MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm)) {
    case Match_Success:
      Inst.setLoc(IDLoc);
      Out.EmitInstruction(Inst, STI);
      return false;
    default:
      // FIXME: Improve error output.
      return Error(IDLoc, "parse error");
    }
  }

#define GET_ASSEMBLER_HEADER
#include "J2GenAsmMatcher.inc"
};

extern "C" void LLVMInitializeJ2AsmParser() {
  RegisterMCAsmParser<J2AsmParser> X(TheJ2Target);
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "J2GenAsmMatcher.inc"
