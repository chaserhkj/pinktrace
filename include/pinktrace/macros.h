/*
 * Copyright (c) 2010, 2011 Ali Polatel <alip@exherbo.org>
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

#ifndef _PINK_MACRO_H
#define _PINK_MACRO_H

/**
 * @file pinktrace/macro.h
 * @brief Pink's common macros
 * @defgroup pink_macro Pink's common macros
 * @ingroup pinktrace
 * @{
 **/

#ifdef __cplusplus
#define PINK_BEGIN_DECL  extern "C" {
#define PINK_END_DECL    }
#else
/** Start declarations in C mode */
#define PINK_BEGIN_DECL /* empty */
/** End declarations in C mode */
#define PINK_END_DECL /* empty */
#endif

#if !defined(SPARSE) && defined(__GNUC__) && __GNUC__ >= 3
#define PINK_GCC_ATTR(x)     __attribute__(x)
#define PINK_GCC_LIKELY(x)   __builtin_expect(!!(x), 1)
#define PINK_GCC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
/** Macro for GCC attributes */
#define PINK_GCC_ATTR(x) /* empty */
/** GCC builtin_expect macro */
#define PINK_GCC_LIKELY(x)   (x)
/** GCC builtin_expect macro */
#define PINK_GCC_UNLIKELY(x) (x)
#endif

/** @} */
#endif
