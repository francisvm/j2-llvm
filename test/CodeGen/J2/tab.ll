; RUN: llc -O1 -march=J2 < %s -o - | FileCheck %s

define void @st8(i8* nocapture %a) {
; CHECK: st8
entry:
  %arrayidx = getelementptr inbounds i8, i8* %a, i32 5
  store i8 10, i8* %arrayidx, align 1
; CHECK: mov #10, [[REG1:r[0-9]+]]
; CHECK-NEXT: mov.b [[REG1]], @(5, r4)
  ret void
}

define void @st16(i16* nocapture %a) {
; CHECK: st16
entry:
  %arrayidx = getelementptr inbounds i16, i16* %a, i32 5
  store i16 10, i16* %arrayidx, align 2
; CHECK: mov #10, [[REG2:r[0-9]+]]
; CHECK-NEXT: mov.w [[REG2]], @(5, r4)
  ret void
}

define void @st32(i32* nocapture %a) {
; CHECK: st32
entry:
  %arrayidx = getelementptr inbounds i32, i32* %a, i32 5
  store i32 10, i32* %arrayidx, align 4
; CHECK: mov #10, [[REG3:r[0-9]+]]
; CHECK-NEXT: mov.l [[REG3]], @(5, r4)
  ret void
}

define signext i8 @ld8(i8* nocapture readonly %a) {
; CHECK: ld8
entry:
  %arrayidx = getelementptr inbounds i8, i8* %a, i32 5
  %0 = load i8, i8* %arrayidx, align 1
  ret i8 %0
; CHECK: mov.b @(5, r4), r0
}

define signext i16 @ld16(i16* nocapture readonly %a) {
; CHECK: ld16
entry:
  %arrayidx = getelementptr inbounds i16, i16* %a, i32 5
  %0 = load i16, i16* %arrayidx, align 2
  ret i16 %0
; CHECK: mov.w @(5, r4), r0
}

define i32 @ld32(i32* nocapture readonly %a) {
; CHECK: ld32
entry:
  %arrayidx = getelementptr inbounds i32, i32* %a, i32 5
  %0 = load i32, i32* %arrayidx, align 4
  ret i32 %0
; CHECK: mov.l @(5, r4), r0
}
