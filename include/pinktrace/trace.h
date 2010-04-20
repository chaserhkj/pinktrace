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

#ifndef PINKTRACE_GUARD_TRACE_H
#define PINKTRACE_GUARD_TRACE_H 1

/**
 * \file
 * Low level wrapper around ptrace internals
 **/

#include <stdbool.h>
#include <sys/types.h>

/**
 * This define represents the trace option SYSGOOD.
 * If this flag is set in the options argument of pink_trace_setup(), when
 * delivering syscall traps, bit 7 is set in signal number (i.e., deliver
 * (SIGTRAP | 0x80) This makes it easy for the tracer to tell the difference
 * between normal traps and those caused by a syscall. This option may not work
 * on all architectures.
 **/
#define PINK_TRACE_OPTION_SYSGOOD   (1 << 0)
/**
 * This define represents the trace option FORK.
 * If this flag is set in the options argument of pink_trace_setup(), stop the
 * child at the next fork(2) call with (SIGTRAP | PTRACE_EVENT_FORK << 8) and
 * automatically start tracing the newly forked process, which will start with
 * a SIGSTOP. The PID for the new process can be retrieved with
 * pink_trace_geteventmsg().
 **/
#define PINK_TRACE_OPTION_FORK      (1 << 1)
/**
 * This define represents the trace option VFORK.
 * If this flag is set in the options argument of pink_trace_setup(), stop the
 * child at the next vfork(2) call with (SIGTRAP | PTRACE_EVENT_VFORK << 8) and
 * automatically start tracing the newly vforked process, which will start with
 * a SIGSTOP. The PID for the new process can be retrieved with
 * pink_trace_geteventmsg().
 **/
#define PINK_TRACE_OPTION_VFORK     (1 << 2)
/**
 * This define represents the trace option CLONE.
 * If this flag is set in the options argument of pink_trace_setup(), stop the
 * child at the next clone(2) call with (SIGTRAP | PTRACE_EVENT_CLONE << 8) and
 * automatically start tracing the newly cloned process, which will start with
 * a SIGSTOP. The PID for the new process can be retrieved with
 * pink_trace_geteventmsg().
 **/
#define PINK_TRACE_OPTION_CLONE     (1 << 3)
/**
 * This define represents the trace option EXEC.
 * If this flag is set in the options argument of pink_trace_setup(), stop the
 * child at the next execve(2) call with (SIGTRAP | PTRACE_EVENT_EXEC << 8)
 **/
#define PINK_TRACE_OPTION_EXEC      (1 << 4)
/**
 * This define represents the trace option VFORKDONE.
 * If this flag is set in the options argument of pink_trace_setup(), stop the
 * child at the completion of the next vfork(2) call with
 * (SIGTRAP | PTRACE_EVENT_VFORK_DONE << 8)
 **/
#define PINK_TRACE_OPTION_VFORKDONE (1 << 5)
/**
 * This define represents the trace option EXIT.
 * If this flag is set in the options argument of pink_trace_setup(), stop the
 * child at exit with (SIGTRAP | PTRACE_EVENT_EXIT << 8). This child's exit
 * status can be retrieved with pink_trace_geteventmsg(). This stop will be
 * done early during process exit when registers are still available, allowing
 * the tracer to see where the exit occured, whereas the normal exit
 * notification is done after the process is finished exiting. Even though
 * context is available, the tracer cannot prevent the exit from happening at
 * this point.
 **/
#define PINK_TRACE_OPTION_EXIT      (1 << 6)

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

/**
 * Restarts the stopped child process.
 *
 * \param pid Process ID of the child to be restarted.
 * \param sig If this is non-zero and not SIGSTOP, it is interpreted as the
 * signal to be delivered to the child; otherwise, no signal is delivered.
 * Thus, for example, the parent can control whether a signal sent to the child
 * is delivered or not.
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_cont(pid_t pid, int sig);

/**
 * Kills the traced child process with SIGKILL.
 *
 * \param pid Process ID of the child to be killed.
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_kill(pid_t pid);

/**
 * Restarts the stopped child process and arranges it to be stopped after
 * execution of a single instruction.
 *
 * \param pid Process ID of the child to be restarted.
 * \param sig Treated the same was as the signal argument of pink_trace_cont().
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_singlestep(pid_t pid, int sig);

/**
 * Restarts the stopped child process and arranges it to be stopped after
 * the entry or exit of the next system call.
 *
 * \param pid Process ID of the child to be restarted.
 * \param sig Treated the same was as the signal argument of pink_trace_cont().
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_syscall(pid_t pid, int sig);

/**
 * Retrieve a message (as an unsigned long) about the trace event that just
 * happened, placing it in the location given by the second argument. For
 * EXIT event this is the child's exit status. For FORK, VFORK, CLONE and
 * VFORKDONE events this is the process ID of the new process.
 *
 * \param pid Process ID of the child whose event is to be reported.
 * \param data Pointer to an unsigned long into which the message will be
 * stored.
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_geteventmsg(pid_t pid, unsigned long *data);

/**
 * Sets the tracing options.
 *
 * \param pid Process ID of the child to be setup.
 * \param options Bitwise OR'ed PINK_TRACE_OPTION_* flags
 *
 * \return true on success, false on failure and sets errno accordingly.
 **/
bool
pink_trace_setup(pid_t pid, int options);

#endif /* !PINKTRACE_GUARD_TRACE_H */