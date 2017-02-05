; RUN: llc -march=J2 < %s -o - | FileCheck %s

declare void @sum(i32 %a, i32 %b)

define void @double_sum(i32 %a, i32 %b) nounwind {
entry:
  call void @sum(i32 %a, i32 %b)
  ret void
; CHECK: double_sum:
; Prologue here.
; CHECK: mov.l @([[CPI0:CPI[0-9]+_[0-9]+]], pc), [[REG0:r[0-9]+]]
; CHECK-NEXT: jsr [[REG0]]
; CHECK-NEXT: nop
; Epilogue here.
; CHECK: rts
; CHECK-NEXT: nop
; Constant pool island.
; CHECK-NEXT: .p2align 2
; CHECK-NEXT: ; BB#1
; CHECK-NEXT: [[CPI0]]:
; CHECK-NEXT: .long sum
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
; CHECK: mov.l @([[CPI1:CPI[0-9]+_[0-9]+]], pc), [[REG1:r[0-9]+]]
; CHECK-NEXT: jsr [[REG1]]
; CHECK-NEXT: nop
; CHECK-NEXT: mov r0, [[REG2:r[0-9]+]]
; CHECK-NEXT: mov #3, r4
; CHECK-NEXT: jsr [[REG1]]
; CHECK-NEXT: nop
; CHECK-NEXT: add [[REG2]], r0
; Epilogue here.
; CHECK: rts
; CHECK-NEXT: nop

; Constant pool island.
; CHECK-NEXT: .p2align 2
; CHECK-NEXT: ; BB#1
; CHECK-NEXT: [[CPI1]]:
; CHECK-NEXT: .long sumr

}
