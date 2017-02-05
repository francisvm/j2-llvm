; RUN: llc -O0 -march=J2 < %s -o - | FileCheck %s

define void @foo() nounwind {
entry:
  %tmp = alloca i32
  store i32 12, i32* %tmp
  %tmp1 = load i32, i32* %tmp
  ret void
; CHECK:      mov.l r14, @-r15
; CHECK-NEXT: mov r15, r14
; CHECK-NEXT: add #-4, r15
; CHECK-NEXT: mov r14, [[REG1:r[0-9]+]]
; CHECK-NEXT: mov #12, [[REG2:r[0-9]+]]
; CHECK-NEXT: mov.l [[REG2]], @[[REG1]]
; CHECK-NEXT: mov r14, r15
; CHECK-NEXT: mov.l @r15+, r14
; CHECK-NEXT: rts
; CHECK-NEXT: nop
}

define void @bar() nounwind {
entry:
  %tmp = alloca i32
  store i32 12, i32* %tmp
  %tmp1 = load i32, i32* %tmp
  call void @foo()
  ret void
; CHECK:      mov.l pr, @-r15
; CHECK-NEXT: mov.l r14, @-r15
; CHECK-NEXT: mov r15, r14
; CHECK-NEXT: add #-4, r15
; CHECK-NEXT: mov r14, [[REG1:r[0-9]+]]
; CHECK-NEXT: mov #12, [[REG2:r[0-9]+]]
; CHECK-NEXT: mov.l [[REG2]], @[[REG1]]
; CHECK-NEXT: mov.l @([[CPI1:CPI[0-9]+_[0-9]+]], pc), [[REG3:r[0-9]+]]
; CHECK-NEXT: jsr [[REG3]]
; CHECK-NEXT: nop
; CHECK-NEXT: mov r14, r15
; CHECK-NEXT: mov.l @r15+, r14
; CHECK-NEXT: mov.l @r15+, pr
; CHECK-NEXT: rts
; CHECK-NEXT: nop

; Constant pool island.
; CHECK-NEXT: .p2align 2
; CHECK-NEXT: ; BB#1
; CHECK-NEXT: [[CPI1]]:
; CHECK-NEXT: .long foo
}
