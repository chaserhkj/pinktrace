/* vim: set cino= fo=croql sw=8 ts=8 sts=0 noet cin fdm=syntax : */

/*
 * Copyright (c) 2010 Ali Polatel <alip@exherbo.org>
 *
 * This file is part of Pink's Tracing Library. pinktrace is free software; you
 * can redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License version 2.1, as published by the Free Software
 * Foundation.
 *
 * pinktrace is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PINKTRACE_GUARD_PINKTRACE_H
#define PINKTRACE_GUARD_PINKTRACE_H 1

/**
 * \file
 * Main include file
 **/

#include <stdbool.h>

/**
 * Indicates that this process is to be traced by its parent. Any signal
 * (except SIGKILL) delivered to this process will cause it to stop and its
 * parent to be notified via wait(2). Also, all subsequent calls to execve(2)
 * by this process will cause a SIGTRAP to be sent to it, giving the parent a
 * chance to gain control before the new program begins execution.
 *
 * Note: This function is used only by the child process; the rest are used
 * only by the parent.
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_me(void);

#endif /* !PINKTRACE_GUARD_PINKTRACE_H */
