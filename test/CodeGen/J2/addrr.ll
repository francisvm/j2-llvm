; RUN: llc -march=J2 < %s -o - | FileCheck %s

define i32 @foo(i32 %a, i32 %b) nounwind {
entry:
  %add = add nsw i32 %b, %a
  ret i32 %add
; CHECK: foo:
; CHECK: add [[REG1:r[45]+]], [[REG2:r[45]+]]
; CHECK-NEXT: mov [[REG2]], r0
; CHECK: rts
; CHECK-NEXT: nop
}
