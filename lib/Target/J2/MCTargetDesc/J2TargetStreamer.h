//===-- J2TargetStreamer.h - J2 Register Information -== --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the J2 implementation of the MCTargetStreamer class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_J2_J2TARGETSTREAMER_H
#define LLVM_LIB_TARGET_J2_J2TARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/FormattedStream.h"

namespace llvm {

class J2TargetStreamer : public MCTargetStreamer {
public:
  J2TargetStreamer(MCStreamer &S);

  virtual ~J2TargetStreamer();

private:
};

class J2TargetAsmStreamer : public J2TargetStreamer {
public:
  J2TargetAsmStreamer(MCStreamer &S);
};

class J2TargetELFStreamer : public J2TargetStreamer {
public:
  J2TargetELFStreamer(MCStreamer &S);
};

} // namespace llvm

#endif /* !LLVM_LIB_TARGET_J2_J2TARGETSTREAMER_H */
