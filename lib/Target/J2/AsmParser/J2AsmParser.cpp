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
    Memory,
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

  struct MemOp {
    unsigned Base;

    enum MemOpType {
      None,   // No extra operand.
      Disp4,  // 4-bit displacement.
      Disp8,  // 8-bit displacement.
      Disp12, // 12-bit displacement.
      Plus,   // No displacement, but increment.
      Minus,  // No displacement, but decrement.
    } OpType;

    union {
      uint8_t disp4;
      uint8_t disp8;
      uint16_t disp12;
    };
  };

  SMLoc StartLoc, EndLoc;

  union {
    TokOp Tok;
    RegOp Reg;
    ImmOp Imm;
    MemOp Mem;
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
    case Memory:
      Mem = o.Mem;
      break;
    }
  }

  bool isToken() const override { return Kind == Token; }
  bool isReg() const override { return Kind == Register; }
  bool isImm() const override { return Kind == Immediate; }
  bool isMem() const override { return Kind == Memory; }

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
    case Memory:
      // FIXME: Print disp.
      OS << "<memory @r" << Mem.Base;
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
  MCAsmParser &Parser;

#define GET_ASSEMBLER_HEADER
#include "J2GenAsmMatcher.inc"

public:
  J2AsmParser(const MCSubtargetInfo &STIIN, MCAsmParser &P,
              const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STIIN), STI{STIIN}, Parser{P} {}

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override {
    MCAsmParser &Parser = getParser();
    RegNo = 0;
    const AsmToken &Tok = Parser.getTok();
    StartLoc = Tok.getLoc();
    EndLoc = Tok.getEndLoc();

    if (Tok.isNot(AsmToken::Identifier))
      return true;

    RegNo = MatchRegisterName(Tok.getIdentifier());

    if (RegNo == 0)
      return true;

    Parser.Lex();
    return false;
  }

  bool tryParseRegisterOperand(OperandVector &Operands) {
    unsigned RegNo = 0;
    SMLoc StartLoc;
    SMLoc EndLoc;
    if (ParseRegister(RegNo, StartLoc, EndLoc))
      return true;

    if (RegNo == J2::NoRegister)
      return true;

    const AsmToken &T = Parser.getTok();
    Operands.push_back(J2Operand::createReg(RegNo, T.getLoc(), T.getEndLoc()));
    Parser.Lex();

    return false;
  }

  bool ParseOperand(OperandVector &Operands, StringRef Mnemonic) {
    OperandMatchResultTy ResTy = MatchOperandParserImpl(Operands, Mnemonic);
    if (ResTy == MatchOperand_Success)
      return false;
    if (ResTy == MatchOperand_ParseFail)
      return true;
    return false;
  }

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override {

    // First operand in MCInst is instruction mnemonic.
    Operands.push_back(J2Operand::createToken(Name, NameLoc));

    while (getLexer().isNot(AsmToken::EndOfStatement)) {
      if (ParseOperand(Operands, Name))
        return true;

      while (getLexer().is(AsmToken::Comma)) {
        Parser.Lex();
        if (ParseOperand(Operands, Name))
          return true;

        if (getLexer().isNot(AsmToken::EndOfStatement))
          return Error(getLexer().getLoc(), "unexpected token in argument list");
      }
    }

    // Lex EndOfStatement.
    Parser.Lex();
    return false;
  }

  OperandMatchResultTy parsePCMemOperand(OperandVector &Operands) {
    return false;
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
};

extern "C" void LLVMInitializeJ2AsmParser() {
  RegisterMCAsmParser<J2AsmParser> X(TheJ2Target);
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "J2GenAsmMatcher.inc"
