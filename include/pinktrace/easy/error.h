/*
 * Copyright (c) 2010, 2011, 2012 Ali Polatel <alip@exherbo.org>
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
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LpIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PINK_EASY_ERROR_H
#define _PINK_EASY_ERROR_H

/**
 * @file pinktrace/easy/error.h
 * @brief Pink's easy error codes
 * @defgroup pink_easy_error Pink's easy error codes
 * @ingroup pinktrace-easy
 * @{
 **/

#include <pinktrace/pink.h>

PINK_BEGIN_DECL

/** Child error codes */
typedef enum {
	/** Success **/
	PINK_EASY_CHILD_ERROR_SUCCESS = 0,
	/** Preparation for tracing failed. (e.g. pink_trace_me()) **/
	PINK_EASY_CHILD_ERROR_SETUP,
	/** @e execve(2) failed. **/
	PINK_EASY_CHILD_ERROR_EXEC,
	/** Maximum error number **/
	PINK_EASY_CHILD_ERROR_MAX,
} pink_easy_child_error_t;

/** Error codes */
typedef enum {
	/** Successful run **/
	PINK_EASY_ERROR_SUCCESS = 0,

	/** Operation aborted by a callback **/
	PINK_EASY_ERROR_CALLBACK_ABORT,

	/** Allocating memory for the eldest child failed **/
	PINK_EASY_ERROR_ALLOC_ELDEST,
	/** Allocating memory for a new child failed **/
	PINK_EASY_ERROR_ALLOC,

	/** Attaching to the child failed **/
	PINK_EASY_ERROR_ATTACH,

	/** @e fork(2) failed **/
	PINK_EASY_ERROR_FORK,
	/** @e vfork(2) failed **/
	PINK_EASY_ERROR_VFORK,

	/** Initial wait(2) failed **/
	PINK_EASY_ERROR_WAIT_ELDEST,
	/** wait(2) failed **/
	PINK_EASY_ERROR_WAIT,

	/** Eldest child didn't stop **/
	PINK_EASY_ERROR_STOP_ELDEST,

	/** Setting up @e ptrace(2) options for the eldest child failed **/
	PINK_EASY_ERROR_SETUP_ELDEST,
	/** Setting up @e ptrace(2) options for the new child failed **/
	PINK_EASY_ERROR_SETUP,

	/** Failed to get the bitness of the eldest child **/
	PINK_EASY_ERROR_BITNESS_ELDEST,
	/** Failed to get the bitness of a child after successful @e execve(2) **/
	PINK_EASY_ERROR_BITNESS,

	/** Initial step (e.g. pink_trace_syscall()) failed **/
	PINK_EASY_ERROR_STEP_INITIAL,
	/** Stepping after event #PINK_EVENT_STOP failed **/
	PINK_EASY_ERROR_STEP_STOP,
	/** Stepping after event #PINK_EVENT_TRAP failed **/
	PINK_EASY_ERROR_STEP_TRAP,
	/** Stepping after event #PINK_EVENT_SYSCALL failed **/
	PINK_EASY_ERROR_STEP_SYSCALL,
	/** Stepping after event #PINK_EVENT_EXIT failed **/
	PINK_EASY_ERROR_STEP_EXIT,
	/** Stepping after @e fork(2), @e vfork(2) or @e clone(2) failed **/
	PINK_EASY_ERROR_STEP_FORK,
	/** Stepping after @e execve(2) failed **/
	PINK_EASY_ERROR_STEP_EXEC,
	/** Stepping after genuine signal failed **/
	PINK_EASY_ERROR_STEP_SIGNAL,

	/** pink_trace_geteventmsg() failed after #PINK_EVENT_FORK **/
	PINK_EASY_ERROR_GETEVENTMSG_FORK,
	/** pink_trace_geteventmsg() failed after #PINK_EVENT_EXIT **/
	PINK_EASY_ERROR_GETEVENTMSG_EXIT,

	/** Received unknown event **/
	PINK_EASY_ERROR_EVENT_UNKNOWN,

	/** Maximum error number **/
	PINK_EASY_ERROR_MAX,
} pink_easy_error_t;

/**
 * Returns a string describing the child error.
 *
 * @param e Error code
 * @return String describing the error
 **/
const char *pink_easy_child_strerror(pink_easy_child_error_t e)
	PINK_GCC_ATTR((pure));

/**
 * Returns a string describing the error.
 *
 * @param e Error code
 * @return String describing the error
 **/
const char *pink_easy_strerror(pink_easy_error_t e)
	PINK_GCC_ATTR((pure));

PINK_END_DECL
/** @} */
#endif
