/* 
 * tie-asm.h -- compile-time HAL assembler definitions dependent on CORE & TIE
 *
 *  NOTE:  This header file is not meant to be included directly.
 */

/* This header file contains assembly-language definitions (assembly
   macros, etc.) for this specific Xtensa processor's TIE extensions
   and options.  It is customized to this Xtensa processor configuration.

   Copyright (c) 1999-2023 Cadence Design Systems Inc.

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

#include <xtensa/coreasm.h>

/*  Selection parameter values for save-area save/restore macros:  */
/*  Option vs. TIE:  */
#define XTHAL_SAS_TIE	0x0001	/* custom extension or coprocessor */
#define XTHAL_SAS_OPT	0x0002	/* optional (and not a coprocessor) */
#define XTHAL_SAS_ANYOT	0x0003	/* both of the above */
/*  Whether used automatically by compiler:  */
#define XTHAL_SAS_NOCC	0x0004	/* not used by compiler w/o special opts/code */
#define XTHAL_SAS_CC	0x0008	/* used by compiler without special opts/code */
#define XTHAL_SAS_ANYCC	0x000C	/* both of the above */
/*  ABI handling across function calls:  */
#define XTHAL_SAS_CALR	0x0010	/* caller-saved */
#define XTHAL_SAS_CALE	0x0020	/* callee-saved */
#define XTHAL_SAS_GLOB	0x0040	/* global across function calls (in thread) */
#define XTHAL_SAS_ANYABI	0x0070	/* all of the above three */
/*  Misc  */
#define XTHAL_SAS_ALL	0xFFFF	/* include all default NCP contents */
#define XTHAL_SAS3(optie,ccuse,abi)	( ((optie) & XTHAL_SAS_ANYOT)  \
					| ((ccuse) & XTHAL_SAS_ANYCC)  \
					| ((abi)   & XTHAL_SAS_ANYABI) )


    /*
      *  Macro to store all non-coprocessor (extra) custom TIE and optional state
      *  (not including zero-overhead loop registers).
      *  Required parameters:
      *      ptr         Save area pointer address register (clobbered)
      *                  (register must contain a 4 byte aligned address).
      *      at1..at4    Four temporary address registers (first XCHAL_NCP_NUM_ATMPS
      *                  registers are clobbered, the remaining are unused).
      *  Optional parameters:
      *      continue    If macro invoked as part of a larger store sequence, set to 1
      *                  if this is not the first in the sequence.  Defaults to 0.
      *      ofs         Offset from start of larger sequence (from value of first ptr
      *                  in sequence) at which to store.  Defaults to next available space
      *                  (or 0 if <continue> is 0).
      *      select      Select what category(ies) of registers to store, as a bitmask
      *                  (see XTHAL_SAS_xxx constants).  Defaults to all registers.
      *      alloc       Select what category(ies) of registers to allocate; if any
      *                  category is selected here that is not in <select>, space for
      *                  the corresponding registers is skipped without doing any store.
      */
    .macro xchal_ncp_store  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start	\continue, \ofs
	// Optional global registers used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	rur.threadptr	\at1		// threadptr option
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	// Optional caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	rsr.scompare1	\at1		// conditional store option
	s32i	\at1, \ptr, .Lxchal_ofs_+0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 247, 1, 1
	ae_movab8	\at1, vb0
	s8i	\at1, \ptr, .Lxchal_ofs_+0
	ae_movab8	\at1, vb1
	s8i	\at1, \ptr, .Lxchal_ofs_+1
	ae_movab8	\at1, vb2
	s8i	\at1, \ptr, .Lxchal_ofs_+2
	ae_movab8	\at1, vb3
	s8i	\at1, \ptr, .Lxchal_ofs_+3
	ae_movab8	\at1, vb4
	s8i	\at1, \ptr, .Lxchal_ofs_+4
	ae_movab8	\at1, vb5
	s8i	\at1, \ptr, .Lxchal_ofs_+5
	ae_movab8	\at1, vb6
	s8i	\at1, \ptr, .Lxchal_ofs_+6
	ae_movab8	\at1, vb7
	s8i	\at1, \ptr, .Lxchal_ofs_+7
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 247, 1, 1
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
    .endm	// xchal_ncp_store

    /*
      *  Macro to load all non-coprocessor (extra) custom TIE and optional state
      *  (not including zero-overhead loop registers).
      *  Required parameters:
      *      ptr         Save area pointer address register (clobbered)
      *                  (register must contain a 4 byte aligned address).
      *      at1..at4    Four temporary address registers (first XCHAL_NCP_NUM_ATMPS
      *                  registers are clobbered, the remaining are unused).
      *  Optional parameters:
      *      continue    If macro invoked as part of a larger load sequence, set to 1
      *                  if this is not the first in the sequence.  Defaults to 0.
      *      ofs         Offset from start of larger sequence (from value of first ptr
      *                  in sequence) at which to load.  Defaults to next available space
      *                  (or 0 if <continue> is 0).
      *      select      Select what category(ies) of registers to load, as a bitmask
      *                  (see XTHAL_SAS_xxx constants).  Defaults to all registers.
      *      alloc       Select what category(ies) of registers to allocate; if any
      *                  category is selected here that is not in <select>, space for
      *                  the corresponding registers is skipped without doing any load.
      */
    .macro xchal_ncp_load  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start	\continue, \ofs
	// Optional global registers used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wur.threadptr	\at1		// threadptr option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_CC | XTHAL_SAS_GLOB) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	// Optional caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	l32i	\at1, \ptr, .Lxchal_ofs_+0
	wsr.scompare1	\at1		// conditional store option
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.elseif ((XTHAL_SAS_OPT | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 1016, 4, 4
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 4
	.endif
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 247, 1, 1
	l8ui	\at1, \ptr, .Lxchal_ofs_+0
	ae_movba8	vb0, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+1
	ae_movba8	vb1, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+2
	ae_movba8	vb2, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+3
	ae_movba8	vb3, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+4
	ae_movba8	vb4, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+5
	ae_movba8	vb5, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+6
	ae_movba8	vb6, \at1
	l8ui	\at1, \ptr, .Lxchal_ofs_+7
	ae_movba8	vb7, \at1
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 247, 1, 1
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 8
	.endif
    .endm	// xchal_ncp_load


#define XCHAL_NCP_NUM_ATMPS	1

    /* 
     *  Macro to store the state of TIE coprocessor AudioEngineLX.
     *  Required parameters:
     *      ptr         Save area pointer address register (clobbered)
     *                  (register must contain a 8 byte aligned address).
     *      at1..at4    Four temporary address registers (first XCHAL_CP1_NUM_ATMPS
     *                  registers are clobbered, the remaining are unused).
     *  Optional parameters are the same as for xchal_ncp_store.
     */
#define xchal_cp_AudioEngineLX_store	xchal_cp1_store
    .macro	xchal_cp1_store  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start \continue, \ofs
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 984, 8, 8
	ae_s64.i	aed0, \ptr, .Lxchal_ofs_+32
	ae_s64.i	aed1, \ptr, .Lxchal_ofs_+40
	ae_s64.i	aed2, \ptr, .Lxchal_ofs_+48
	ae_s64.i	aed3, \ptr, .Lxchal_ofs_+56
	ae_s64.i	aed4, \ptr, .Lxchal_ofs_+64
	ae_s64.i	aed5, \ptr, .Lxchal_ofs_+72
	ae_s64.i	aed6, \ptr, .Lxchal_ofs_+80
	ae_s64.i	aed7, \ptr, .Lxchal_ofs_+88
	ae_s64.i	aed8, \ptr, .Lxchal_ofs_+96
	ae_s64.i	aed9, \ptr, .Lxchal_ofs_+104
	ae_s64.i	aed10, \ptr, .Lxchal_ofs_+112
	ae_s64.i	aed11, \ptr, .Lxchal_ofs_+120
	ae_s64.i	aed12, \ptr, .Lxchal_ofs_+128
	ae_s64.i	aed13, \ptr, .Lxchal_ofs_+136
	ae_s64.i	aed14, \ptr, .Lxchal_ofs_+144
	ae_s64.i	aed15, \ptr, .Lxchal_ofs_+152
	ae_salign64.i	u0, \ptr, .Lxchal_ofs_+160
	ae_salign64.i	u1, \ptr, .Lxchal_ofs_+168
	ae_salign64.i	u2, \ptr, .Lxchal_ofs_+176
	ae_salign64.i	u3, \ptr, .Lxchal_ofs_+184
	ae_movvfusionmisc	aed0		// ureg FUSIONMISC
	ae_s64.i	aed0, \ptr, .Lxchal_ofs_+0 + 0
	ae_movvcirc	aed0		// ureg CIRC
	ae_s64.i	aed0, \ptr, .Lxchal_ofs_+8 + 0
	ae_movvtablefirstsearchnext	aed0		// ureg TABLEFIRSTSEARCHNEXT
	ae_s64.i	aed0, \ptr, .Lxchal_ofs_+16 + 0
	ae_movvfcrfsr	aed0		// ureg FCR_FSR
	ae_s64.i	aed0, \ptr, .Lxchal_ofs_+24 + 0
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 192
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 984, 8, 8
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 192
	.endif
    .endm	// xchal_cp1_store

    /* 
     *  Macro to load the state of TIE coprocessor AudioEngineLX.
     *  Required parameters:
     *      ptr         Save area pointer address register (clobbered)
     *                  (register must contain a 8 byte aligned address).
     *      at1..at4    Four temporary address registers (first XCHAL_CP1_NUM_ATMPS
     *                  registers are clobbered, the remaining are unused).
     *  Optional parameters are the same as for xchal_ncp_load.
     */
#define xchal_cp_AudioEngineLX_load	xchal_cp1_load
    .macro	xchal_cp1_load  ptr at1 at2 at3 at4  continue=0 ofs=-1 select=XTHAL_SAS_ALL alloc=0
	xchal_sa_start \continue, \ofs
	// Custom caller-saved registers not used by default by the compiler:
	.ifeq (XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\select)
	xchal_sa_align	\ptr, 0, 984, 8, 8
	ae_l64.i	aed0, \ptr, .Lxchal_ofs_+0 + 0		// ureg FUSIONMISC
	ae_movfusionmiscv	aed0
	ae_l64.i	aed0, \ptr, .Lxchal_ofs_+8 + 0		// ureg CIRC
	ae_movcircv	aed0
	ae_l64.i	aed0, \ptr, .Lxchal_ofs_+16 + 0		// ureg TABLEFIRSTSEARCHNEXT
	ae_movtablefirstsearchnextv	aed0
	ae_l64.i	aed0, \ptr, .Lxchal_ofs_+24 + 0		// ureg FCR_FSR
	ae_movfcrfsrv	aed0
	ae_l64.i	aed0, \ptr, .Lxchal_ofs_+32
	ae_l64.i	aed1, \ptr, .Lxchal_ofs_+40
	ae_l64.i	aed2, \ptr, .Lxchal_ofs_+48
	ae_l64.i	aed3, \ptr, .Lxchal_ofs_+56
	ae_l64.i	aed4, \ptr, .Lxchal_ofs_+64
	ae_l64.i	aed5, \ptr, .Lxchal_ofs_+72
	ae_l64.i	aed6, \ptr, .Lxchal_ofs_+80
	ae_l64.i	aed7, \ptr, .Lxchal_ofs_+88
	ae_l64.i	aed8, \ptr, .Lxchal_ofs_+96
	ae_l64.i	aed9, \ptr, .Lxchal_ofs_+104
	ae_l64.i	aed10, \ptr, .Lxchal_ofs_+112
	ae_l64.i	aed11, \ptr, .Lxchal_ofs_+120
	ae_l64.i	aed12, \ptr, .Lxchal_ofs_+128
	ae_l64.i	aed13, \ptr, .Lxchal_ofs_+136
	ae_l64.i	aed14, \ptr, .Lxchal_ofs_+144
	ae_l64.i	aed15, \ptr, .Lxchal_ofs_+152
	ae_lalign64.i	u0, \ptr, .Lxchal_ofs_+160
	ae_lalign64.i	u1, \ptr, .Lxchal_ofs_+168
	ae_lalign64.i	u2, \ptr, .Lxchal_ofs_+176
	ae_lalign64.i	u3, \ptr, .Lxchal_ofs_+184
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 192
	.elseif ((XTHAL_SAS_TIE | XTHAL_SAS_NOCC | XTHAL_SAS_CALR) & ~(\alloc)) == 0
	xchal_sa_align	\ptr, 0, 984, 8, 8
	.set	.Lxchal_ofs_, .Lxchal_ofs_ + 192
	.endif
    .endm	// xchal_cp1_load

#define XCHAL_CP1_NUM_ATMPS	0
#define XCHAL_SA_NUM_ATMPS	1

	/*  Empty macros for unconfigured coprocessors:  */
	.macro xchal_cp0_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp0_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp2_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp2_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp3_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp3_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp4_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp4_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp5_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp5_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp6_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp6_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp7_store	p a b c d continue=0 ofs=-1 select=-1 ; .endm
	.macro xchal_cp7_load	p a b c d continue=0 ofs=-1 select=-1 ; .endm

#endif /*_XTENSA_CORE_TIE_ASM_H*/

