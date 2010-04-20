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

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <check.h>

#include <pinktrace/trace.h>

#include "check_pinktrace.h"

START_TEST(test_pink_trace_me_basic)
{
	int status;
	pid_t pid;

	if ((pid = fork()) < 0)
		fail("fork failed: %s", strerror(errno));
	else if (!pid) { /* child */
		if (!pink_trace_me()) {
			perror("pink_trace_me");
			_exit(EXIT_FAILURE);
		}

		_exit(EXIT_SUCCESS);
	}
	else { /* parent */
		waitpid(pid, &status, 0);

		fail_unless(WEXITSTATUS(status) == EXIT_SUCCESS, "pink_trace_me() failed");
	}
}
END_TEST

START_TEST(test_pink_trace_me_signal)
{
	int status;
	pid_t pid;

	if ((pid = fork()) < 0)
		fail("fork failed: %s", strerror(errno));
	else if (!pid) { /* child */
		if (!pink_trace_me()) {
			perror("pink_trace_me");
			_exit(EXIT_FAILURE);
		}
		kill(getpid(), SIGTTIN);
		abort();
	}
	else { /* parent */
		waitpid(pid, &status, 0);

		fail_unless(WIFSTOPPED(status), "Child hasn't been stopped after genuine signal");

		kill(pid, SIGKILL);
	}
}
END_TEST

START_TEST(test_pink_trace_me_execve)
{
	int status;
	pid_t pid;
	char *const myargv[] = { "/bin/true", NULL };

	if ((pid = fork()) < 0)
		fail("fork failed: %s", strerror(errno));
	else if (!pid) { /* child */
		if (!pink_trace_me()) {
			perror("pink_trace_me");
			_exit(EXIT_FAILURE);
		}

		execvp("/bin/true", myargv);
		_exit(EXIT_FAILURE);
	}
	else { /* parent */
		waitpid(pid, &status, 0);

		fail_unless(WIFSTOPPED(status), "Child hasn't been stopped after execve()");
		fail_unless(WSTOPSIG(status) == SIGTRAP, "Wrong signal, got: %d expected: %d",
				WSTOPSIG(status), SIGTRAP);

		kill(pid, SIGKILL);
	}
}
END_TEST

START_TEST(test_pink_trace_cont_basic)
{
	int status;
	pid_t pid;

	if ((pid = fork()) < 0)
		fail("fork failed: %s", strerror(errno));
	else if (!pid) { /* child */
		if (!pink_trace_me()) {
			perror("pink_trace_me");
			_exit(EXIT_FAILURE);
		}
		kill(getpid(), SIGSTOP);
		_exit(13);
	}
	else {
		waitpid(pid, &status, 0);

		fail_unless(WIFSTOPPED(status), "Child hasn't stopped");
		fail_unless(WSTOPSIG(status) == SIGSTOP, "Wrong stop signal, got: %d expected: %d",
				WSTOPSIG(status), SIGSTOP);

		fail_unless(pink_trace_cont(pid, 0), "pink_trace_cont() failed: %s", strerror(errno));
		waitpid(pid, &status, 0);

		fail_unless(WIFEXITED(status), "Child hasn't exited");
		fail_unless(WEXITSTATUS(status) == 13, "Wrong exit code, got: %d expected: %d",
				WEXITSTATUS(status), 13);
	}
}
END_TEST

START_TEST(test_pink_trace_cont_signal)
{
	int status;
	pid_t pid;

	if ((pid = fork()) < 0)
		fail("fork failed: %s", strerror(errno));
	else if (!pid) { /* child */
		if (!pink_trace_me()) {
			perror("pink_trace_me");
			_exit(EXIT_FAILURE);
		}
		kill(getpid(), SIGSTOP);
		_exit(13);
	}
	else { /* parent */
		waitpid(pid, &status, 0);

		fail_unless(WIFSTOPPED(status), "Child hasn't stopped");
		fail_unless(WSTOPSIG(status) == SIGSTOP, "Wrong stop signal, got: %d expected: %d",
				WSTOPSIG(status), SIGSTOP);

		fail_unless(pink_trace_cont(pid, SIGTERM), "pink_trace_cont() failed: %s", strerror(errno));
		waitpid(pid, &status, 0);

		fail_unless(WIFSIGNALED(status), "Child hasn't been signaled");
		fail_unless(WTERMSIG(status) == SIGTERM, "Wrong signal, got: %d expected: %d",
				WTERMSIG(status), SIGTERM);
	}
}
END_TEST

START_TEST(test_pink_trace_kill)
{
	int status;
	pid_t pid;

	if ((pid = fork()) < 0)
		fail("fork failed: %s", strerror(errno));
	else if (!pid) { /* child */
		if (!pink_trace_me()) {
			perror("pink_trace_me");
			_exit(EXIT_FAILURE);
		}
		kill(getpid(), SIGSTOP);
		_exit(13);
	}
	else { /* parent */
		waitpid(pid, &status, 0);

		fail_unless(WIFSTOPPED(status), "Child hasn't stopped");
		fail_unless(WSTOPSIG(status) == SIGSTOP, "Wrong stop signal, got: %d expected: %d",
				WSTOPSIG(status), SIGSTOP);

		fail_unless(pink_trace_kill(pid), "pink_trace_kill() failed: %s", strerror(errno));
		waitpid(pid, &status, 0);

		fail_unless(WIFSIGNALED(status), "Child hasn't been signaled");
		fail_unless(WTERMSIG(status) == SIGKILL, "Wrong signal, got: %d expected: %d",
				WTERMSIG(status), SIGKILL);
	}
}
END_TEST

START_TEST(test_pink_trace_singlestep_basic)
{
}
END_TEST

START_TEST(test_pink_trace_singlestep_signal)
{
}
END_TEST

START_TEST(test_pink_trace_syscall_basic)
{
}
END_TEST

START_TEST(test_pink_trace_syscall_signal)
{
}
END_TEST

START_TEST(test_pink_trace_setup_sysgood)
{
}
END_TEST

START_TEST(test_pink_trace_setup_fork)
{
}
END_TEST

START_TEST(test_pink_trace_setup_vfork)
{
}
END_TEST

START_TEST(test_pink_trace_setup_clone)
{
}
END_TEST

START_TEST(test_pink_trace_setup_vforkdone)
{
}
END_TEST

START_TEST(test_pink_trace_setup_exit)
{
}
END_TEST

Suite *
trace_suite_create(void)
{
	Suite *s = suite_create("trace");

	/* pink_trace_me() */
	TCase *tc_pink_trace_me = tcase_create("pink_trace_me");

	tcase_add_test(tc_pink_trace_me, test_pink_trace_me_basic);
	tcase_add_test(tc_pink_trace_me, test_pink_trace_me_signal);
	tcase_add_test(tc_pink_trace_me, test_pink_trace_me_execve);

	suite_add_tcase(s, tc_pink_trace_me);

	/* pink_trace_cont() */
	TCase *tc_pink_trace_cont = tcase_create("pink_trace_cont");

	tcase_add_test(tc_pink_trace_cont, test_pink_trace_cont_basic);
	tcase_add_test(tc_pink_trace_cont, test_pink_trace_cont_signal);

	suite_add_tcase(s, tc_pink_trace_cont);

	/* pink_trace_kill() */
	TCase *tc_pink_trace_kill = tcase_create("pink_trace_kill");

	tcase_add_test(tc_pink_trace_kill, test_pink_trace_kill);

	suite_add_tcase(s, tc_pink_trace_kill);

	/* pink_trace_singlestep() */
	TCase *tc_pink_trace_singlestep = tcase_create("pink_trace_singlestep");

	tcase_add_test(tc_pink_trace_singlestep, test_pink_trace_singlestep_basic);
	tcase_add_test(tc_pink_trace_singlestep, test_pink_trace_singlestep_signal);

	suite_add_tcase(s, tc_pink_trace_singlestep);

	/* pink_trace_syscall() */
	TCase *tc_pink_trace_syscall = tcase_create("pink_trace_syscall");

	tcase_add_test(tc_pink_trace_syscall, test_pink_trace_syscall_basic);
	tcase_add_test(tc_pink_trace_syscall, test_pink_trace_syscall_signal);

	suite_add_tcase(s, tc_pink_trace_syscall);

	/* pink_trace_setup() and pink_trace_geteventmsg() */
	TCase *tc_pink_trace_setup = tcase_create("pink_trace_setup");

	tcase_add_test(tc_pink_trace_setup, test_pink_trace_setup_sysgood);
	tcase_add_test(tc_pink_trace_setup, test_pink_trace_setup_fork);
	tcase_add_test(tc_pink_trace_setup, test_pink_trace_setup_vfork);
	tcase_add_test(tc_pink_trace_setup, test_pink_trace_setup_clone);
	tcase_add_test(tc_pink_trace_setup, test_pink_trace_setup_vforkdone);
	tcase_add_test(tc_pink_trace_setup, test_pink_trace_setup_exit);

	suite_add_tcase(s, tc_pink_trace_setup);

	return s;
}