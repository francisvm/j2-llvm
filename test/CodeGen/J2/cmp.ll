; RUN: llc -O1 -march=J2 < %s -o - | FileCheck %s

define i32 @foo(i32 %i) {
; CHECK: foo:
entry:
; CHECK: mov #3, [[REG0:r[0-9]+]]
  %cmp = icmp sgt i32 %i, 3
; CHECK-NEXT: cmp/gt r4, [[REG0]]
  br i1 %cmp, label %if.then, label %next1
; CHECK-NEXT: bt [[RETBB:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop
; CHECK-NEXT: bra [[BB1:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB1]]:
next1:
; CHECK-NEXT: mov #5, [[REG1:r[0-9]+]]
  %cmp2 = icmp slt i32 %i, 5
; CHECK-NEXT: cmp/ge [[REG1]], r4
  br i1 %cmp2, label %if.then, label %next3
; CHECK-NEXT: bt [[RETBB]]
; CHECK-NEXT: nop
; CHECK-NEXT: bra [[BB2:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB2]]:
next3:
; CHECK-NEXT: mov #2, [[REG2:r[0-9]+]]
  %cmp4 = icmp sge i32 %i, 3
; CHECK-NEXT: cmp/gt r4, [[REG2]]
  br i1 %cmp4, label %if.then, label %next4
; CHECK-NEXT: bt [[RETBB]]
; CHECK-NEXT: nop
; CHECK-NEXT: bra [[BB3:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB3]]:
next4:
  %cmp5 = icmp sgt i32 %i, 5
; CHECK-NEXT: cmp/gt r4, [[REG1]]
  br i1 %cmp5, label %if.then, label %next6
; CHECK-NEXT: bt [[RETBB]]
; CHECK-NEXT: nop
; CHECK-NEXT: bra [[BB4:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB4]]:
next6:
; CHECK-NEXT: mov #11, [[REG4:r[0-9]+]]
  %cmp7 = icmp sle i32 %i, 10
; CHECK-NEXT: cmp/ge [[REG4]], r4
  br i1 %cmp7, label %if.then, label %next8
; CHECK-NEXT: bt [[RETBB]]
; CHECK-NEXT: nop
; CHECK-NEXT: bra [[BB5:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB5]]:
next8:
; CHECK-NEXT: mov r4, [[REG5:r[0-9]+]]
  %cmp9 = icmp eq i32 %i, 30
; CHECK-NEXT: cmp/eq #30, [[REG5]]
  br i1 %cmp9, label %if.then, label %next9
; CHECK-NEXT: bt [[RETBB]]
; CHECK-NEXT: nop
; CHECK-NEXT: bra [[BB6:.LBB[0-9]+_[0-9]+]]
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB6]]:
next9:
; CHECK-NEXT: mov #35, r0
  ret i32 35
; CHECK: rts
; CHECK-NEXT: nop

; CHECK-NEXT: [[BB7:.LBB[0-9]+_[0-9]+]]:
if.then:
; CHECK-NEXT: mov #30, r0
  ret i32 30
; CHECK: rts
; CHECK-NEXT: nop
}
