## Toolchain file for cross-compiling LLVM/Clang to x86_64-unknown-unified

# The target system is our custom 'unified' OS
set(CMAKE_SYSTEM_NAME Unified CACHE STRING "System name for target OS")
# Architecture
set(CMAKE_SYSTEM_PROCESSOR x86_64 CACHE STRING "Target architecture")

# Root of the target sysroot
if(NOT DEFINED UNIFIED_SYSROOT)
  message(FATAL_ERROR "Please define UNIFIED_SYSROOT to point at your unified sysroot")
endif()

# Tell CMake where to find headers, libs, and programs
set(CMAKE_SYSROOT ${UNIFIED_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${UNIFIED_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Use GNU ABI compatibility (alias to Linux if desired)
set(CMAKE_C_COMPILER_TARGET x86_64-unknown-unified)
set(CMAKE_CXX_COMPILER_TARGET x86_64-unknown-unified)

# Default triple for LLVM itself
set(LLVM_DEFAULT_TARGET_TRIPLE "x86_64-unknown-unified" CACHE STRING "Default target triple")

# Only build X86 backend (host side should override if needed)
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "Backends to build")

# Expose our custom platform file
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/Platform")

# Minimal projects and runtimes
set(LLVM_ENABLE_PROJECTS "clang;llvm" CACHE STRING "LLVM projects to build")
set(LLVM_INSTALL_TOOLCHAIN_ONLY ON CACHE BOOL "Install only compiler, runtimes, and headers")
set(LLVM_BUILD_TESTS OFF CACHE BOOL "Disable running LLVM tests during build")
set(LLVM_ENABLE_RUNTIMES "compiler-rt;libcxx;libcxxabi;libunwind;openmp" CACHE STRING "LLVM runtimes to build")

set(RUNTIMES_x86_64-unknown-unified_COMPILER_RT_BUILD_SANITIZERS OFF CACHE BOOL "Disable sanitizers")
set(RUNTIMES_x86_64-unknown-unified_COMPILER_RT_BUILD_LIBFUZZER OFF CACHE BOOL "Disable libfuzzer")
set(RUNTIMES_x86_64-unknown-unified_COMPILER_RT_INCLUDE_TESTS OFF CACHE BOOL "Disable compiler-rt tests")

set(CLANG_DEFAULT_CXX_STDLIB libstdc++ CACHE STRING "Default C++ standard library")

# Use libc++ as default C++ stdlib
set(CLANG_DEFAULT_CXX_STDLIB libstdc++ CACHE STRING "Default C++ standard library")

# Enforce 64-bit
set(CMAKE_SIZEOF_VOID_P 8 CACHE STRING "Pointer size on target")
set(RUNTIMES_x86_64-unknown-unified_CMAKE_SIZEOF_VOID_P 8 CACHE STRING "Pointer size for runtimes")

# Release builds
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set(RUNTIMES_x86_64-unknown-unified_CMAKE_BUILD_TYPE Release CACHE STRING "Build type for runtimes")

# Link with lld
set(RUNTIMES_x86_64-unknown-unified_CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld" CACHE STRING "Shared linker flags")
set(RUNTIMES_x86_64-unknown-unified_CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld" CACHE STRING "Executable linker flags")

# Installation prefix inside sysroot
set(CMAKE_INSTALL_PREFIX "/system" CACHE STRING "Installation prefix")
