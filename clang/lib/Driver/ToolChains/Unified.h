#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_UNIFIED_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_UNIFIED_H

#include "Gnu.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {

namespace unified {
class LLVM_LIBRARY_VISIBILITY Assembler : public Tool {
public:
  Assembler(const ToolChain &TC) : Tool("unified::Assembler", "assembler", TC) {}

  bool hasIntegratedCPP() const override { return false; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const override;
};

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
  Linker(const ToolChain &TC) : Tool("unified::Linker", "linker", TC) {}

  bool hasIntegratedCPP() const override { return false; }
  bool isLinkJob() const override { return true; }

  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const override;
};
} // namespace unified
} // namespace tools

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Unified : public Generic_ELF {
public:
  Unified(const Driver &D, const llvm::Triple &Triple,
        const llvm::opt::ArgList &Args);

  bool HasNativeLLVMSupport() const override { return true; }

  void AddLibCxxIncludePaths(const llvm::opt::ArgList &DriverArgs,
                             llvm::opt::ArgStringList &CC1Args) const;
  void AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                                 llvm::opt::ArgStringList &CC1Args) const;

  ToolChain::CXXStdlibType GetDefaultCXXStdlibType() const override {
    return ToolChain::CST_Libcxx;
  }
  void AddCXXStdlibLibArgs(const llvm::opt::ArgList &Args,
                           llvm::opt::ArgStringList &CmdArgs) const;

  ToolChain::RuntimeLibType
  GetRuntimeLibType(const llvm::opt::ArgList &Args) const override {
    return ToolChain::RLT_CompilerRT;
  }

protected:
  Tool *buildAssembler() const override;
  Tool *buildLinker() const override;
};

} // namespace toolchains

} // namespace driver
} // namespace clang

#endif