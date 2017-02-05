set(LLVM_TARGET_DEFINITIONS J2.td)

add_public_tablegen_target(J2CommonTableGen)

add_llvm_target(J2CodeGen
  J2TargetMachine.cpp
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
