/*
 * Copyright (c) 2012 Ali Polatel <alip@exherbo.org>
 * Based in part upon strace which is:
 *   Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 *   Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 *   Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PINK_ABI_H
#define _PINK_ABI_H

/**
 * @file pinktrace/abi.h
 * @brief Pink's supported system call ABIs
 * @defgroup pink_abi Pink's supported system call ABIs
 * @ingroup pinktrace
 * @{
 **/

#include <stdbool.h>
#include <sys/types.h>
#include <pinktrace/macros.h>
#include <pinktrace/system.h>

typedef int pink_abi_t;

#define PINK_ABIS_SUPPORTED 1
#define PINK_ABI_DEFAULT 0

#if PINK_ARCH_ARM
# undef PINK_ABIS_SUPPORTED
# define PINK_ABIS_SUPPORTED 2
# define PINK_ABI_THUMB PINK_ABI_DEFAULT
# define PINK_ABI_ARM 1
#elif PINK_ARCH_IA64
# define PINK_ABI_IA64 PINK_ABI_DEFAULT
#elif PINK_ARCH_POWERPC
# define PINK_ABI_POWERPC PINK_ABI_DEFAULT
#elif PINK_ARCH_POWERPC64
# undef PINK_ABIS_SUPPORTED
# define PINK_ABIS_SUPPORTED 2
# define PINK_ABI_POWERPC64 PINK_ABI_DEFAULT
# define PINK_ABI_POWERPC 1
#elif PINK_ARCH_X86
# define PINK_ABI_X86 PINK_ABI_DEFAULT
#elif PINK_ARCH_X86_64
# undef PINK_ABIS_SUPPORTED
# define PINK_ABIS_SUPPORTED 3
# define PINK_ABI_X86_64 PINK_ABI_DEFAULT
# define PINK_ABI_X86 1
# define PINK_ABI_X32 2
#elif PINK_ARCH_X32
# undef PINK_ABIS_SUPPORTED
# define PINK_ABIS_SUPPORTED 2
# define PINK_ABI_X32 PINK_ABI_DEFAULT
# define PINK_ABI_X86 1
#endif

PINK_BEGIN_DECL

/**
 * Return the word size of the system call ABI
 *
 * @param abi System call ABI
 * @param wsize Pointer to store the word size, must @b not be @e NULL
 * @return Word size on success, -1 on failure and sets errno accordingly
 **/
bool pink_abi_wordsize(pink_abi_t abi, size_t *wsize)
	PINK_GCC_ATTR((nonnull(2)));

PINK_END_DECL
/** @} */
#endif
