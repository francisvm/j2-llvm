; RUN: llc -march=J2 < %s -o - | FileCheck %s

declare void @sum(i32 %a, i32 %b)

define void @double_sum(i32 %a, i32 %b) nounwind {
entry:
  call void @sum(i32 %a, i32 %b)
  ret void
; CHECK: double_sum:
; Prologue here.
; CHECK: bsr sum
; CHECK-NEXT: nop
; Epilogue here.
; CHECK: rts
; CHECK-NEXT: nop
}

declare i32 @sumr(i32 %a, i32 %b)

define i32 @double_sumr(i32 %a, i32 %b) nounwind {
entry:
  %call = tail call i32 @sumr(i32 %a, i32 %b)
  %call1 = tail call i32 @sumr(i32 3, i32 %b)
  %add = add nsw i32 %call1, %call
  ret i32 %add
; CHECK: double_sumr:
; Prologue here.
; CHECK: bsr sumr
; CHECK-NEXT: nop
; CHECK-NEXT: mov r0, [[REG1:r[0-9]+]]
; CHECK-NEXT: mov #3, r4
; CHECK-NEXT: bsr sumr
; CHECK-NEXT: nop
; CHECK-NEXT: add [[REG1]], r0
; Epilogue here.
; CHECK: rts
; CHECK-NEXT: nop
}
