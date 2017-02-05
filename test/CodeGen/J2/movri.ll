; RUN: llc -march=J2 < %s -o - | FileCheck %s

define i32 @foo() nounwind {
entry:
  ret i32 30
; CHECK: foo:
; CHECK: mov #30, r0
; CHECK-NEXT: rts
}
