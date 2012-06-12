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

#include <pinktrace/internal.h> /* TODO: Fix pink_event_decide() */
#include <pinktrace/easy/internal.h>
#include <pinktrace/pink.h>
#include <pinktrace/easy/pink.h>

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <sys/wait.h>
#include <sys/utsname.h>

static void handle_ptrace_error(pink_easy_context_t *ctx,
		pink_easy_process_t *current,
		const char *errctx)
{
	if (errno == ESRCH) {
		if (ctx->callback_table.teardown)
			ctx->callback_table.teardown(ctx, current);
	} else {
		ctx->callback_table.error(ctx, PINK_EASY_ERROR_TRACE, current, errctx);
	}
	PINK_EASY_REMOVE_PROCESS(ctx, current);
}

static bool handle_startup(pink_easy_context_t *ctx, pink_easy_process_t *current)
{
	/* Set up tracing options */
	if (!pink_trace_setup(current->pid, ctx->ptrace_options)) {
		handle_ptrace_error(ctx, current, "setup");
		return false;
	}

	/* Figure out bitness */
	current->bitness = pink_bitness_get(current->pid);
	if (current->bitness == PINK_BITNESS_UNKNOWN) {
		handle_ptrace_error(ctx, current, "bitness");
		return false;
	}

	/* Set up flags */
	if (ctx->ptrace_options & PINK_TRACE_OPTION_FORK
			|| ctx->ptrace_options & PINK_TRACE_OPTION_VFORK
			|| ctx->ptrace_options & PINK_TRACE_OPTION_CLONE)
		current->flags |= PINK_EASY_PROCESS_FOLLOWFORK;

	/* Happy birthday! */
	current->flags &= ~PINK_EASY_PROCESS_STARTUP;
	if (ctx->callback_table.startup) {
		pink_easy_process_t *parent = NULL;
		if (current->ppid != -1)
			parent = pink_easy_process_list_lookup(&(ctx->process_list), current->ppid);
		ctx->callback_table.startup(ctx, current, parent);
	}

	return true;
}

int pink_easy_loop(pink_easy_context_t *ctx)
{
	/* Enter the event loop */
	while (ctx->nprocs != 0) {
		pid_t pid;
		int r, status, sig;
		unsigned event;
		pink_easy_process_t *current;

		pid = waitpid(-1, &status, __WALL);
		if (pid < 0) {
			switch (errno) {
			case EINTR:
				continue;
			case ECHILD:
				return ctx->callback_table.cleanup
					? ctx->callback_table.cleanup(ctx)
					: EXIT_SUCCESS;
			default:
				ctx->fatal = true;
				ctx->error = PINK_EASY_ERROR_WAIT;
				ctx->callback_table.error(ctx);
				return -ctx->error;
			}
		}

		current = pink_easy_process_list_lookup(&(ctx->process_list), pid);
		/* FIXME: pink_event_decide() is broken by design! */
		event = ((unsigned) status >> 16);

		/* Under Linux, execve changes pid to thread leader's pid,
		 * and we see this changed pid on EVENT_EXEC and later,
		 * execve sysexit. Leader "disappears" without exit
		 * notification. Let user know that, drop leader's tcb,
		 * and fix up pid in execve thread's tcb.
		 * Effectively, execve thread's tcb replaces leader's tcb.
		 *
		 * BTW, leader is 'stuck undead' (doesn't report WIFEXITED
		 * on exit syscall) in multithreaded programs exactly
		 * in order to handle this case.
		 *
		 * PTRACE_GETEVENTMSG returns old pid starting from Linux 3.0.
		 * On 2.6 and earlier, it can return garbage.
		 */
		if (event == PTRACE_EVENT_EXEC) {
			pink_bitness_t old_bitness = current->bitness;
			pink_easy_process_t *execve_thread = current;
			long old_pid = 0;

			if (pink_easy_os_release < KERNEL_VERSION(3,0,0))
				goto dont_switch_procs;
			if (!pink_trace_geteventmsg(pid, (unsigned long *)&old_pid))
				goto dont_switch_procs;
			if (old_pid <= 0 || old_pid == pid)
				goto dont_switch_procs;
			execve_thread = pink_easy_process_list_lookup(&(ctx->process_list), old_pid);
			if (!execve_thread)
				goto dont_switch_procs;

			/* Drop leader, switch to the thread, reusing leader's pid */
			PINK_EASY_REMOVE_PROCESS(ctx, current);
			current = execve_thread;
			current->pid = pid;
dont_switch_procs:
			/* Update bitness */
			current->bitness = pink_bitness_get(current->pid);
			if (current->bitness == PINK_BITNESS_UNKNOWN) {
				handle_ptrace_error(ctx, current, "bitness");
				continue;
			}
			if (ctx->callback_table.exec) {
				r = ctx->callback_table.exec(ctx, current, old_bitness);
				if (r & PINK_EASY_CFLAG_ABORT) {
					ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
					return -ctx->error;
				}
				if (r & PINK_EASY_CFLAG_DROP) {
					PINK_EASY_REMOVE_PROCESS(ctx, current);
					continue;
				}
			}
		}

		if (current == NULL) {
			/* We might see the child's initial trap before we see the parent
			 * return from the clone syscall. Leave the child suspended until
			 * the parent returns from its system call. Only then we will have
			 * the association between parent and child.
			 */
			PINK_EASY_INSERT_PROCESS(ctx, current);
			current->pid = pid;
			current->flags = PINK_EASY_PROCESS_STARTUP;
			continue;
		}

		if (WIFSIGNALED(status) || WIFEXITED(status)) {
			PINK_EASY_REMOVE_PROCESS(ctx, current);
			if (ctx->callback_table.exit) {
				r = ctx->callback_table.exit(ctx, pid, status);
				if (r & PINK_EASY_CFLAG_ABORT) {
					ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
					return -ctx->error;
				}
			}
			continue;
		}
		if (!WIFSTOPPED(status)) {
			ctx->callback_table.error(ctx, PINK_EASY_ERROR_PROCESS, current, "WIFSTOPPED");
			PINK_EASY_REMOVE_PROCESS(ctx, current);
			continue;
		}

		/* Is this the very first time we see this tracee stopped? */
		if (current->flags & PINK_EASY_PROCESS_STARTUP && !handle_startup(ctx, current))
			continue;

		if (event == PTRACE_EVENT_FORK || event == PTRACE_EVENT_VFORK || event == PTRACE_EVENT_CLONE) {
			pink_easy_process_t *new_thread;
			long new_pid;
			if (!pink_trace_geteventmsg(current->pid, (unsigned long *)&new_pid)) {
				handle_ptrace_error(ctx, current, "geteventmsg");
				continue;
			}
			new_thread = pink_easy_process_list_lookup(&(ctx->process_list), new_pid);
			if (new_thread == NULL) {
				/* Not attached to the thread yet, nor is it alive... */
				PINK_EASY_INSERT_PROCESS(ctx, new_thread);
				new_thread->pid = pid;
				new_thread->flags = (PINK_EASY_PROCESS_STARTUP | PINK_EASY_PROCESS_IGNORE_ONE_SIGSTOP);
				new_thread->ppid = current->pid;
			} else {
				/* Thread is waiting for Pink to let her go on... */
				new_thread->ppid = current->pid;
				new_thread->bitness = current->bitness;
				new_thread->flags &= ~PINK_EASY_PROCESS_STARTUP;
				/* Happy birthday! */
				if (ctx->callback_table.startup)
					ctx->callback_table.startup(ctx, new_thread, current);
				if (!pink_trace_syscall(new_thread->pid, 0))
					handle_ptrace_error(ctx, current, "syscall");
			}
		} else if (event == PTRACE_EVENT_EXIT && ctx->callback_table.pre_exit) {
			unsigned long status;
			if (!pink_trace_geteventmsg(current->pid, &status)) {
				handle_ptrace_error(ctx, current, "geteventmsg");
				continue;
			}
			r = ctx->callback_table.pre_exit(ctx, current, (int)status);
			if (r & PINK_EASY_CFLAG_ABORT) {
				ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
				return -ctx->error;
			}
			if (r & PINK_EASY_CFLAG_DROP) {
				PINK_EASY_REMOVE_PROCESS(ctx, current);
				continue;
			}
		}

		sig = WSTOPSIG(status);

		/* Is this post-attach SIGSTOP? */
		if (sig == SIGSTOP && (current->flags & PINK_EASY_PROCESS_IGNORE_ONE_SIGSTOP)) {
			current->flags &= ~PINK_EASY_PROCESS_IGNORE_ONE_SIGSTOP;
			goto restart_tracee_with_sig_0;
		}
		if (sig != (SIGTRAP|0x80)) {
			if (ctx->callback_table.signal) {
				r = ctx->callback_table.signal(ctx, current, status);
				if (r & PINK_EASY_CFLAG_ABORT) {
					ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
					return -ctx->error;
				}
				if (r & PINK_EASY_CFLAG_DROP) {
					PINK_EASY_REMOVE_PROCESS(ctx, current);
					continue;
				}
				if (r & PINK_EASY_CFLAG_SIGIGN)
					goto restart_tracee_with_sig_0;
			}
			goto restart_tracee;
		}

		/* System call trap! */
		current->flags ^= PINK_EASY_PROCESS_INSYSCALL;
		if (ctx->callback_table.syscall) {
			bool entering = current->flags & PINK_EASY_PROCESS_INSYSCALL;
			r = ctx->callback_table.syscall(ctx, current, entering);
			if (r & PINK_EASY_CFLAG_ABORT) {
				ctx->error = PINK_EASY_ERROR_CALLBACK_ABORT;
				return -ctx->error;
			}
			if (r & PINK_EASY_CFLAG_DROP) {
				PINK_EASY_REMOVE_PROCESS(ctx, current);
				continue;
			}
		}

restart_tracee_with_sig_0:
		sig = 0;
restart_tracee:
		if (!pink_trace_syscall(current->pid, sig))
			handle_ptrace_error(ctx, current, "syscall");
	}

	return 0;
}
