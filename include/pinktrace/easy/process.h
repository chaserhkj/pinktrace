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

#ifndef _PINK_EASY_PROCESS_H
#define _PINK_EASY_PROCESS_H

/**
 * @file pinktrace/easy/process.h
 * @brief Pink's easy process representation
 * @defgroup pink_easy_process Pink's easy process representation
 * @ingroup pinktrace-easy
 * @{
 **/

#include <stdbool.h>
#include <sys/types.h>
#include <pinktrace/pink.h>
#include <pinktrace/easy/func.h>

PINK_BEGIN_DECL

/**
 * @struct pink_easy_process_t
 * @brief Opaque structure which represents a process entry
 * @note These entries are allocated internally by the tracing context.
 **/
typedef struct pink_easy_process pink_easy_process_t;

/**
 * @struct pink_easy_process_list_t
 * @brief Opaque structure which represents a process list
 * @note This list is maintained internally by the tracing context.
 **/
typedef struct pink_easy_process_list pink_easy_process_list_t;

/**
 * Kill a process
 *
 * @note This function uses @e tgkill(2) or @e tkill(2) if available.
 *
 * @param proc Process entry
 * @param sig Signal to deliver
 * @return Same as @e kill(2)
 **/
int pink_easy_process_kill(const pink_easy_process_t *proc, int sig);

/**
 * Detach from a process as necessary and resume its execution. This function
 * calls pink_trace_detach() if the process is attached and pink_trace_resume() if
 * the process is spawned.
 *
 * @param proc Process entry
 * @param sig Same as pink_trace_cont()
 * \return true on success, false on failure and sets errno accordingly
 **/
bool pink_easy_process_resume(const pink_easy_process_t *proc, int sig);

/**
 * Returns the process ID of the entry
 *
 * @param proc Process entry
 * @return Process ID
 **/
pid_t pink_easy_process_get_pid(const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Returns the process ID of this entry's parent or -1 for the eldest entries.
 *
 * @param proc Process entry
 * @return Parent Process ID or -1
 **/
pid_t pink_easy_process_get_ppid(const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Returns the bitness of the entry
 *
 * @param proc Process entry
 * @return Bitness
 **/
pink_bitness_t pink_easy_process_get_bitness(const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Are we attached to the process?
 *
 * @param proc Process entry
 * @return true if the process is attached, false otherwise
 **/
bool pink_easy_process_is_attached(const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Is this process a clone?
 *
 * @see pink_easy_attach()
 *
 * @param proc Process entry
 * @return true if the process is a clone, false otherwise
 **/
bool pink_easy_process_is_clone(const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Set the user data of the process entry.
 *
 * @note This function accepts a destructor function pointer which may be used
 *       to free the user data. You may pass NULL if you want to handle the
 *       destruction yourself or use the standard @e free(3) function from
 *       stdlib.h for basic destruction.
 *
 * @param proc Process entry
 * @param userdata User data
 * @param userdata_destroy The destructor function of the user data
 **/
void pink_easy_process_set_userdata(pink_easy_process_t *proc, void *userdata,
		pink_easy_free_func_t userdata_destroy)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Get the user data of the process entry, previously set by
 * pink_easy_process_set_data()
 *
 * @param proc Process entry
 * @return User data
 **/
void *pink_easy_process_get_userdata(const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Remove a process from the process list.
 *
 * @note pinktrace doesn't export an insertion function because insertions are
 *       handled internally by this library. You may, however, need to remove
 *       an entry due to problems (e.g. -ESRCH) caused by the process.
 *
 * @param list Process list
 * @param proc Process entry
 **/
void pink_easy_process_list_remove(pink_easy_process_list_t *list,
		const pink_easy_process_t *proc)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Look up the process list for the given process ID.
 *
 * @param list The process list
 * @param pid Process ID
 * @return The process on successful look up, NULL on failure
 **/
pink_easy_process_t *pink_easy_process_list_lookup(const pink_easy_process_list_t *list,
		pid_t pid)
	PINK_GCC_ATTR((nonnull(1)));

/**
 * Walk the process tree.
 *
 * @param list Process list
 * @param func Walk function
 * @param userdata User data to pass to the walk function
 * @return Total number of visited entries
 **/
unsigned pink_easy_process_list_walk(const pink_easy_process_list_t *list,
		pink_easy_walk_func_t func, void *userdata)
	PINK_GCC_ATTR((nonnull(1,2)));

PINK_END_DECL
/** @} */
#endif
