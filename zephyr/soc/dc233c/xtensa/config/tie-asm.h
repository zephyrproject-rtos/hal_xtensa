/* 
 * tie-asm.h -- compile-time HAL assembler definitions dependent on CORE & TIE
 *
 *  NOTE:  This header file is not meant to be included directly.
 */

/* This header file contains assembly-language definitions (assembly
   macros, etc.) for this specific Xtensa processor's TIE extensions
   and options.  It is customized to this Xtensa processor configuration.

   Copyright (c) 1999-2010 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef _XTENSA_CORE_TIE_ASM_H
#define _XTENSA_CORE_TIE_ASM_H

/*  Selection parameter values for save-area save/restore macros:  */
/*  Option vs. TIE:  */
#define XTHAL_SAS_TIE	0x0001	/* custom extension or coprocessor */
#define XTHAL_SAS_OPT	0x0002	/* optional (and not a coprocessor) */
/*  Whether used automatically by compiler:  */
#define XTHAL_SAS_NOCC	0x0004	/* not used by compiler w/o special opts/code */
#define XTHAL_SAS_CC	0x0008	/* used by compiler without special opts/code */
/*  ABI handling across function calls:  */
#define XTHAL_SAS_CALR	0x0010	/* caller-saved */
#define XTHAL_SAS_CALE	0x0020	/* callee-saved */
#define XTHAL_SAS_GLOB	0x0040	/* global across function calls (in thread) */
/*  Misc  */
#define XTHAL_SAS_ALL	0xFFFF	/* include all default NCP contents */



/* Macro to save all non-coprocessor (extra) custom TIE and optional state
 * (not including zero-overhead loop registers).
 * Save area ptr (clobbered):  ptr  (4 byte aligned)
 * Scratch regs  (clobbered):  at1..at4  (only first XCHAL_NCP_NUM_ATMPS needed)
 */
	.macro xchal_ncp_store  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL
	xchal_sa_start	\continue, \ofs
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~\select
	xchal_sa_align	\ptr, 0, 1024-8, 4, 4
	rsr	\at1, ACCLO		// MAC16 accumulator
	rsr	\at2, ACCHI
	s32i	\at1, \ptr, .Lxchal_ofs_ + 0
	s32i	\at2, \ptr, .Lxchal_ofs_ + 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~\select
	xchal_sa_align	\ptr, 0, 1024-16, 4, 4
	rsr	\at1, M0		// MAC16 registers
	rsr	\at2, M1
	s32i	\at1, \ptr, .Lxchal_ofs_ + 0
	s32i	\at2, \ptr, .Lxchal_ofs_ + 4
	rsr	\at1, M2
	rsr	\at2, M3
	s32i	\at1, \ptr, .Lxchal_ofs_ + 8
	s32i	\at2, \ptr, .Lxchal_ofs_ + 12
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 16
	.endif
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~\select
	xchal_sa_align	\ptr, 0, 1024-4, 4, 4
	rsr	\at1, SCOMPARE1		// conditional store option
	s32i	\at1, \ptr, .Lxchal_ofs_ + 0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~\select
	xchal_sa_align	\ptr, 0, 1024-4, 4, 4
	rur	\at1, THREADPTR		// threadptr option
	s32i	\at1, \ptr, .Lxchal_ofs_ + 0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	.endm	// xchal_ncp_store

/* Macro to save all non-coprocessor (extra) custom TIE and optional state
 * (not including zero-overhead loop registers).
 * Save area ptr (clobbered):  ptr  (4 byte aligned)
 * Scratch regs  (clobbered):  at1..at4  (only first XCHAL_NCP_NUM_ATMPS needed)
 */
	.macro xchal_ncp_load  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL
	xchal_sa_start	\continue, \ofs
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_CALR) & ~\select
	xchal_sa_align	\ptr, 0, 1024-8, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_ + 0
	l32i	\at2, \ptr, .Lxchal_ofs_ + 4
	wsr	\at1, ACCLO		// MAC16 accumulator
	wsr	\at2, ACCHI
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~\select
	xchal_sa_align	\ptr, 0, 1024-16, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_ + 0
	l32i	\at2, \ptr, .Lxchal_ofs_ + 4
	wsr	\at1, M0		// MAC16 registers
	wsr	\at2, M1
	l32i	\at1, \ptr, .Lxchal_ofs_ + 8
	l32i	\at2, \ptr, .Lxchal_ofs_ + 12
	wsr	\at1, M2
	wsr	\at2, M3
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 16
	.endif
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~\select
	xchal_sa_align	\ptr, 0, 1024-4, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_ + 0
	wsr	\at1, SCOMPARE1		// conditional store option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~\select
	xchal_sa_align	\ptr, 0, 1024-4, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_ + 0
	wur	\at1, THREADPTR		// threadptr option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	.endm	// xchal_ncp_load



#define XCHAL_NCP_NUM_ATMPS	2


#define XCHAL_SA_NUM_ATMPS	2

#endif /*_XTENSA_CORE_TIE_ASM_H*/

