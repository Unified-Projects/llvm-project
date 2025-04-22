//===--- Unified.cpp - Unified ToolChain Implementation --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Unified toolchain for the x86_64-unknown-unified target.
//
//===----------------------------------------------------------------------===//

#include "Unified.h"
#include "CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/SanitizerArgs.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace llvm::opt;

Unified::Unified(const Driver &D, const llvm::Triple &Triple, const ArgList &Args)
    : Generic_ELF(D, Triple, Args) {
  // Add library search path under unified sysroot
  getFilePaths().push_back(getDriver().SysRoot + "/system/lib");
}

void Unified::addClangSystemIncludeArgs(const ArgList &DriverArgs,
                                        ArgStringList &CC1Args) const {
  // Include standard system headers
  addSystemInclude(DriverArgs, CC1Args, getDriver().SysRoot + "/system/include");
  addSystemInclude(DriverArgs, CC1Args, getDriver().SysRoot + "/system/include/" + getTripleString());
  addSystemInclude(DriverArgs, CC1Args, getDriver().SysRoot + "/usr/include");
  addSystemInclude(DriverArgs, CC1Args, getDriver().SysRoot + "/usr/include/" + getTripleString());
}

void Unified::addLibCxxIncludePaths(const ArgList &DriverArgs,
                                    ArgStringList &CC1Args) const {
  // Include libc++ headers
  addSystemInclude(DriverArgs, CC1Args, getDriver().SysRoot + "/system/include/c++/v1");
  addSystemInclude(DriverArgs, CC1Args, getDriver().Dir + "/../include/c++/v1");
  addSystemInclude(DriverArgs, CC1Args, getDriver().Dir + "/../include/" + getTripleString() + "/c++/v1");
}

void Unified::AddCXXStdlibLibArgs(const ArgList &Args,
                                  ArgStringList &CmdArgs) const {
  // Link against libc++ by default
  CmdArgs.push_back("-lc++");
}

Tool *Unified::buildAssembler() const {
  return new tools::unified::Assembler(*this);
}

Tool *Unified::buildLinker() const {
  return new tools::unified::Linker(*this);
}