; RUN: llc -O0 -march=J2 < %s -o - | FileCheck %s

define void @foo() nounwind {
entry:
  %tmp = alloca i32
  store i32 12, i32* %tmp
  %tmp1 = load i32, i32* %tmp
  ret void
; CHECK: mov.l r14, @r15
; CHECK-NEXT: mov r15, r14
; CHECK-NEXT: add #-8, r15
; CHECK-NEXT: mov r14, [[REG1:r[0-9]+]]
; CHECK-NEXT: mov #12, [[REG2:r[0-9]+]]
; CHECK-NEXT: mov.l [[REG2]], @[[REG1]]
; CHECK-NEXT: mov r14, r15
; CHECK-NEXT: mov.l @r15, r14
; CHECK-NEXT: rts
; CHECK-NEXT: nop
}
