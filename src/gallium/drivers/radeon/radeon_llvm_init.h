/*
 * Copyright 2016 Android-x86 Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors: Chih-Wei Huang <cwhuang@linux.org.tw>
 *
 */

#ifndef RADEON_LLVM_INIT_H
#define RADEON_LLVM_INIT_H

#if HAVE_LLVM < 0x0307

extern void LLVMInitializeR600TargetInfo();
extern void LLVMInitializeR600Target();
extern void LLVMInitializeR600TargetMC();
extern void LLVMInitializeR600AsmPrinter();

#define LLVMInitializeAMDGPUTargetInfo LLVMInitializeR600TargetInfo
#define LLVMInitializeAMDGPUTarget     LLVMInitializeR600Target
#define LLVMInitializeAMDGPUTargetMC   LLVMInitializeR600TargetMC
#define LLVMInitializeAMDGPUAsmPrinter LLVMInitializeR600AsmPrinter

#else

extern void LLVMInitializeAMDGPUTargetInfo();
extern void LLVMInitializeAMDGPUTarget();
extern void LLVMInitializeAMDGPUTargetMC();
extern void LLVMInitializeAMDGPUAsmPrinter();

#endif

#endif /* RADEON_LLVM_INIT_H */
