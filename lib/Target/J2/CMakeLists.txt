set(LLVM_TARGET_DEFINITIONS J2.td)

tablegen(LLVM J2GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM J2GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM J2GenCallingConv.inc -gen-callingconv)
tablegen(LLVM J2GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM J2GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM J2GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM J2GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM J2GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM J2GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(J2CommonTableGen)

add_llvm_target(J2CodeGen
  J2AsmPrinter.cpp
  J2FrameLowering.cpp
  J2ISelDAGToDAG.cpp
  J2ISelLowering.cpp
  J2InstrInfo.cpp
  J2MCInstLower.cpp
  J2RegisterInfo.cpp
  J2Subtarget.cpp
  J2TargetMachine.cpp
  J2TargetObjectFile.cpp
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
