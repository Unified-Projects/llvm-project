; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -instcombine -S | FileCheck %s

; Gracefully handle the alloca that is not in the alloca AS (=5)

target datalayout = "e-p:64:64-p1:64:64-p2:32:32-p3:32:32-p4:64:64-p5:32:32-p6:32:32-i64:64-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024-v2048:2048-n32:64-S32-A5-G1-ni:7"
target triple = "amdgcn-amd-amdhsa"

declare void @use(i8*, i32**)

define weak amdgpu_kernel void @__omp_offloading_802_ea0109_main_l8(i32* %a) {
; CHECK-LABEL: @__omp_offloading_802_ea0109_main_l8(
; CHECK-NEXT:  .master:
; CHECK-NEXT:    [[TMP0:%.*]] = alloca i32*, align 1, addrspace(5)
; CHECK-NEXT:    [[DOTSUB:%.*]] = bitcast i32* addrspace(5)* [[TMP0]] to i8 addrspace(5)*
; CHECK-NEXT:    [[TMP1:%.*]] = addrspacecast i8 addrspace(5)* [[DOTSUB]] to i8*
; CHECK-NEXT:    [[A_ON_STACK:%.*]] = addrspacecast i32* addrspace(5)* [[TMP0]] to i32**
; CHECK-NEXT:    call void @use(i8* [[TMP1]], i32** [[A_ON_STACK]])
; CHECK-NEXT:    ret void
;
.master:
  %0 = alloca i8, i64 8, align 1
  %a_on_stack = bitcast i8* %0 to i32**
  store i32* undef, i32** %a_on_stack, align 8
  call void @use(i8* %0, i32** %a_on_stack)
  ret void
}