set(LLVM_TARGET_DEFINITIONS J2.td)

tablegen(LLVM J2GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM J2GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM J2GenRegisterInfo.inc -gen-register-info)

add_public_tablegen_target(J2CommonTableGen)

add_llvm_target(J2CodeGen
  J2AsmPrinter.cpp
  J2InstrInfo.cpp
  J2RegisterInfo.cpp
  J2TargetMachine.cpp
  )

add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
