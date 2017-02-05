; RUN: llc -O0 -march=J2 < %s -o - | FileCheck %s

define i32 @foo(i32 %a) nounwind {
; CHECK-LABEL: foo
entry:
; CHECK:       mov.l @([[CPI0:CPI[0-9]+_[0-9]+]], pc), [[REG0:r[0-9]+]]
  %add = add nsw i32 128, %a
; CHECK-NEXT:  add r4, [[REG0]]
  ret i32 %add
; CHECK:       rts
; CHECK-NEXT:  nop
; CHECK-NEXT:  .p2align 2
; CHECK-NEXT:  ; BB#1:
; CHECK-NEXT:  [[CPI0]]:
; CHECK-NEXT:  .long 128
}
