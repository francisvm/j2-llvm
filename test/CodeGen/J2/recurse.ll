; RUN: llc -O0 -march=J2 < %s -o - | FileCheck %s

define i32 @recurse(i32 %i) {
entry:
  %cmp = icmp slt i32 %i, 2
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  br label %return

if.else:                                          ; preds = %entry
  %sub = sub nsw i32 %i, 1
  %call = call i32 @recurse(i32 %sub)
  %add = add nsw i32 %i, %call
  br label %return

return:                                           ; preds = %if.else, %if.then
  %retval.0 = phi i32 [ 1, %if.then ], [ %add, %if.else ]
  ret i32 %retval.0
; CHECK: recurse
; CHECK: mov #1, [[REG0:r[0-9]+]]
; CHECK-NEXT: cmp/gt r4, [[REG0]]
; CHECK-NEXT: mov.l r4, @r14
; CHECK-NEXT: bt [[L1:\.LBB0_[0-9]+]]
; CHECK-NEXT: bra [[L2:\.LBB0_[0-9]+]]
}
