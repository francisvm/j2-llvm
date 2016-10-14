; RUN: llc -O0 -march=J2 < %s -o - | FileCheck %s

define void @foo() nounwind {
; CHECK: foo:
entry:
  br label %if.then
; CHECK: bra [[THEN:.*]]

if.then:
; CHECK-NEXT: [[THEN]]:
  br label %return
; CHECK-NEXT: bra [[RETURN:.*]]

return:
; CHECK-NEXT: [[RETURN]]:
  ret void
; CHECK: rts
}
