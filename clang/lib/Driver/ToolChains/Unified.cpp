//===--- Unified.cpp - Unified ToolChain Implementation --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Unified toolchain for the x86_64-unknown-unified
// target.
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

Unified::Unified(const Driver &D, const llvm::Triple &Triple,
                 const ArgList &Args)
    : Generic_ELF(D, Triple, Args) {
  // Add library search path under unified sysroot
  getFilePaths().push_back(getDriver().SysRoot + "/lib");
  getFilePaths().push_back(getDriver().SysRoot + "/usr/lib");
}

void Unified::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                        ArgStringList &CC1Args) const {
  // Include standard system headers
  addSystemInclude(DriverArgs, CC1Args,
                   getDriver().SysRoot + "/usr/include");
  addSystemInclude(DriverArgs, CC1Args,
                   getDriver().SysRoot + "/usr/include/" +
                       getTripleString());
}

void Unified::AddLibCxxIncludePaths(const ArgList &DriverArgs,
                                    ArgStringList &CC1Args) const {
  // Include libc++ headers
  addSystemInclude(DriverArgs, CC1Args,
                   getDriver().SysRoot + "/system/include/c++/v1");
  addSystemInclude(DriverArgs, CC1Args, getDriver().Dir + "/../include/c++/v1");
  addSystemInclude(DriverArgs, CC1Args,
                   getDriver().Dir + "/../include/" + getTripleString() +
                       "/c++/v1");
}

void Unified::AddCXXStdlibLibArgs(const ArgList &Args,
                                  ArgStringList &CmdArgs) const {
  // Link against libc++ by default
  CXXStdlibType Type = GetCXXStdlibType(Args);

  switch (Type) {
  case ToolChain::CST_Libcxx:
    CmdArgs.push_back("-lc++");
    break;
  }
}

Tool *Unified::buildAssembler() const {
  return new tools::unified::Assembler(*this);
}

Tool *Unified::buildLinker() const { return new tools::unified::Linker(*this); }

// Definitions for the unified assembler and linker tools:
namespace clang {
namespace driver {
namespace tools {
namespace unified {

void Assembler::ConstructJob(Compilation &C, const JobAction &JA,
                             const InputInfo &Output,
                             const InputInfoList &Inputs,
                             const llvm::opt::ArgList &Args,
                             const char *LinkingOutput) const {
  ArgStringList CmdArgs;
  const Driver &D = getToolChain().getDriver();

  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());

  for (const auto &II : Inputs)
    CmdArgs.push_back(II.getFilename());

  Args.AddAllArgValues(CmdArgs, options::OPT_Wa_COMMA, options::OPT_Xassembler);

  const char *Exec = Args.MakeArgString(getToolChain().GetProgramPath("as"));
  C.addCommand(std::make_unique<Command>(
      JA, *this, ResponseFileSupport::AtFileCurCP(), Exec, CmdArgs, Inputs));
}

void Linker::ConstructJob(Compilation &C, const JobAction &JA,
                          const InputInfo &Output, const InputInfoList &Inputs,
                          const llvm::opt::ArgList &Args,
                          const char *LinkingOutput) const {
  const toolchains::Unified &TC =
      static_cast<const toolchains::Unified &>(getToolChain());
  ArgStringList CmdArgs;

  const Driver &D = TC.getDriver();

  // Silence warning for "clang -g foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_g_Group);
  // and "clang -emit-llvm foo.o -o foo"
  Args.ClaimAllArgs(options::OPT_emit_llvm);
  // and for "clang -w foo.o -o foo". Other warning options are already
  // handled somewhere else.
  Args.ClaimAllArgs(options::OPT_w);

  // Static vs dynamic
  if (Args.hasArg(options::OPT_static)) {
    CmdArgs.push_back("-Bstatic");
  } else {
    if (Args.hasArg(options::OPT_rdynamic))
      CmdArgs.push_back("-export-dynamic");
    if (Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back("-shared");
    }
  }

  // rpath and sysroot
  if (!D.SysRoot.empty())
    CmdArgs.push_back(Args.MakeArgString("--sysroot=" + D.SysRoot));
  // Disable lazy binding (resolve all symbols at link time)
  CmdArgs.push_back(Args.MakeArgString("-z"));
  CmdArgs.push_back(Args.MakeArgString("now"));

  // PIE
  llvm::Reloc::Model RelocationModel;
  unsigned PICLevel;
  bool IsPIE;
  std::tie(RelocationModel, PICLevel, IsPIE) =
      ParsePICArgs(getToolChain(), Args);

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles)) {
    if (!Args.hasArg(options::OPT_shared)) {
      CmdArgs.push_back(Args.MakeArgString(TC.GetFilePath("crt0.o")));
    }
    std::string crtbegin =
        TC.getCompilerRT(Args, "crtbegin", ToolChain::FT_Object);
    if (TC.getVFS().exists(crtbegin)) {
      CmdArgs.push_back(
          Args.MakeArgString(TC.GetFilePath(crtbegin.c_str())));
    }
  }

  Args.AddAllArgs(CmdArgs, options::OPT_L);
  TC.AddFilePathLibArgs(Args, CmdArgs);
  Args.AddAllArgs(CmdArgs, options::OPT_T_Group);
  Args.AddAllArgs(CmdArgs, options::OPT_e);
  Args.AddAllArgs(CmdArgs, options::OPT_s);
  Args.AddAllArgs(CmdArgs, options::OPT_t);
  Args.AddAllArgs(CmdArgs, options::OPT_Z_Flag);
  Args.AddAllArgs(CmdArgs, options::OPT_r);

  if (D.isUsingLTO()) {
    assert(!Inputs.empty() && "Must have at least one input.");
    addLTOOptions(TC, Args, CmdArgs, Output, Inputs[0],
                  D.getLTOMode() == LTOK_Thin);
  }

  AddLinkerInputs(TC, Inputs, Args, CmdArgs, JA);

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs)) {
    AddRunTimeLibs(TC, D, CmdArgs, Args);

    if (D.CCCIsCXX()) {
      if (TC.ShouldLinkCXXStdlib(Args))
        TC.AddCXXStdlibLibArgs(Args, CmdArgs);
    }

    if (Args.hasArg(options::OPT_pthread)) {
      CmdArgs.push_back("-lpthread");
    }

    CmdArgs.push_back("-lc");
  }

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles)) {
    std::string crtend =
        TC.getCompilerRT(Args, "crtend", ToolChain::FT_Object);
    if (TC.getVFS().exists(crtend)) {
      CmdArgs.push_back(
          Args.MakeArgString(TC.GetFilePath(crtend.c_str())));
    }
  }

  const char *Exec = Args.MakeArgString(getToolChain().GetLinkerPath());
  C.addCommand(std::make_unique<Command>(
      JA, *this, ResponseFileSupport::AtFileCurCP(), Exec, CmdArgs, Inputs));
}

} // namespace unified
} // namespace tools
} // namespace driver
} // namespace clang