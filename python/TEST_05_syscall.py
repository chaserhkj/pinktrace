#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set sw=4 ts=4 sts=4 et tw=80 :

import errno, os, sys, unittest

sys.path.insert(0, '.')
from pinktrace import bitness, event, fork, syscall, trace

class TestSyscall_01_Invalid(unittest.TestCase):

    def test_01_name(self):
        self.assertRaises(TypeError, syscall.name)
        self.assertRaises(ValueError, syscall.name, 0, 13)

    def test_02_get_no(self):
        self.assertRaises(TypeError, syscall.get_no)
        self.assertRaises(OSError, syscall.get_no, 0)

    def test_03_set_no(self):
        self.assertRaises(TypeError, syscall.set_no)
        self.assertRaises(TypeError, syscall.set_no, 0)
        self.assertRaises(OSError, syscall.set_no, 0, 1)

    def test_04_get_ret(self):
        self.assertRaises(TypeError, syscall.get_ret)
        self.assertRaises(OSError, syscall.get_ret, 0)

    def test_05_set_ret(self):
        self.assertRaises(TypeError, syscall.set_ret)
        self.assertRaises(TypeError, syscall.set_ret, 0)
        self.assertRaises(OSError, syscall.set_ret, 0, 1)

    def test_06_get_arg(self):
        self.assertRaises(TypeError, syscall.get_arg)
        self.assertRaises(TypeError, syscall.get_arg, 0)
        self.assertRaises(IndexError, syscall.get_arg, 0, syscall.MAX_INDEX)
        self.assertRaises(ValueError, syscall.get_arg, 0, 1, 13)

class TestSyscall_02(unittest.TestCase):

    def test_01_get_no(self):
        pid = fork.fork()
        if pid == 0: # child
            os.kill(os.getpid(), 0)
            os._exit(0)
        else: # parent
            # Loop until we get to the kill() system call as there's no
            # guarantee that other system calls won't be called beforehand.
            ev = -1
            found = False
            while ev != event.EVENT_EXIT_GENUINE:
                trace.syscall(pid)
                pid, status = os.waitpid(pid, 0)

                ev = event.decide(status)
                if ev == event.EVENT_SYSCALL:
                    scno = syscall.get_no(pid)
                    name = syscall.name(scno)
                    if name == 'kill':
                        found = True
                        break

            self.assert_(found)

            try: trace.kill(pid)
            except OSError: pass

    def test_02_set_no(self):
        pid = fork.fork()
        if pid == 0: # child
            os.kill(os.getpid(), 0)
            os._exit(0)
        else: # parent
            # Loop until we get to the kill() system call as there's no
            # guarantee that other system calls won't be called beforehand.
            ev = -1
            while ev != event.EVENT_EXIT_GENUINE:
                trace.syscall(pid)
                pid, status = os.waitpid(pid, 0)

                ev = event.decide(status)
                if ev == event.EVENT_SYSCALL:
                    scno = syscall.get_no(pid)
                    name = syscall.name(scno)
                    if name == 'kill':
                        syscall.set_no(pid, 0xbadca11)
                        scno = syscall.get_no(pid)
                        self.assertEqual(scno, 0xbadca11)
                        break

            try: trace.kill(pid)
            except OSError: pass

    def test_03_get_ret_success(self):
        pid = fork.fork()
        if pid == 0: # child
            os.kill(os.getpid(), 0)
            os._exit(0)
        else: # parent
            # Loop until we get to the kill() system call as there's no
            # guarantee that other system calls won't be called beforehand.
            ev = -1
            insyscall = False
            while ev != event.EVENT_EXIT_GENUINE:
                trace.syscall(pid)
                pid, status = os.waitpid(pid, 0)

                ev = event.decide(status)
                if ev == event.EVENT_SYSCALL:
                    scno = syscall.get_no(pid)
                    name = syscall.name(scno)
                    if insyscall and name == 'kill':
                        ret = syscall.get_ret(pid)
                        self.assertEqual(ret, 0)

                if not insyscall:
                    insyscall = True
                else:
                    insyscall = False

    def test_04_get_ret_fail(self):
        pid = fork.fork()
        if pid == 0: # child
            try:
                open('')
            except IOError:
                pass
            os._exit(0)
        else: # parent
            # Loop until we get to the open() system call as there's no
            # guarantee that other system calls won't be called beforehand.
            ev = -1
            insyscall = False
            while ev != event.EVENT_EXIT_GENUINE:
                trace.syscall(pid)
                pid, status = os.waitpid(pid, 0)

                ev = event.decide(status)
                if ev == event.EVENT_SYSCALL:
                    scno = syscall.get_no(pid)
                    name = syscall.name(scno)
                    if insyscall and name == 'open':
                        ret = syscall.get_ret(pid)
                        self.assertEqual(ret, -errno.ENOENT)

                if not insyscall:
                    insyscall = True
                else:
                    insyscall = False

    def test_05_set_ret_success(self):
        # TODO: Find a smart test case for this one.
        pass

    def test_06_set_ret_fail(self):
        pid = fork.fork()
        if pid == 0: # child
            try:
                os.kill(os.getpid(), 0)
            except OSError:
                os._exit(0)
            else:
                os._exit(1)
        else: # parent
            # Loop until we get to the kill() system call as there's no
            # guarantee that other system calls won't be called beforehand.
            ev = -1
            insyscall = False
            while ev != event.EVENT_EXIT_GENUINE:
                trace.syscall(pid)
                pid, status = os.waitpid(pid, 0)

                ev = event.decide(status)
                if ev == event.EVENT_SYSCALL:
                    scno = syscall.get_no(pid)
                    name = syscall.name(scno)
                    if insyscall and name == 'kill':
                        syscall.set_ret(pid, -errno.EPERM)

                if not insyscall:
                    insyscall = True
                else:
                    insyscall = False

            self.assert_(os.WIFEXITED(status))
            self.assertEqual(os.WEXITSTATUS(status), 0)

    def test_07_get_arg_first(self):
        pass

    def test_08_get_arg_second(self):
        pass

    def test_09_get_arg_third(self):
        pass

    def test_10_get_arg_fourth(self):
        pass

    def test_11_get_arg_fifth(self):
        pass

    def test_12_get_arg_sixth(self):
        pass

if __name__ == '__main__':
    unittest.main()
