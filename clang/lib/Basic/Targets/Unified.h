

//===--- Unified.h - Unified OS Target Implementation -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the UnifiedTargetInfo class, for the x86_64-unknown-unified
// target OS, which behaves similarly to Linux on x86_64.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_UNIFIED_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_UNIFIED_H

#include "OSTargets.h"
#include "X86.h"

/// \brief Target info for the x86_64-unknown-unified OS, using ELF.
class UnifiedTargetInfo : public LinuxTargetInfo<X86_64TargetInfo> {
public:
  UnifiedTargetInfo(const llvm::Triple &Triple, const clang::TargetOptions &Opts)
      : LinuxTargetInfo<X86_64TargetInfo>(Triple, Opts) {}

  void getTargetDefines(const clang::LangOptions &Opts,
                        clang::MacroBuilder &Builder) const override {
    // Inherit all Linux defines, then add __unified__.
    LinuxTargetInfo<X86_64TargetInfo>::getTargetDefines(Opts, Builder);
    Builder.defineMacro("__unified__", "1");
  }
};

namespace clang {
namespace targets {


} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_UNIFIED_H