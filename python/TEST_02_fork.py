#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set sw=4 ts=4 sts=4 et tw=80 :

import os, sys, unittest

sys.path.insert(0, '.')
from pinktrace import fork, trace

class TestFork_01_Invalid(unittest.TestCase):
    def test_01_fork(self):
        self.assertRaises(TypeError, fork.fork, 'pink')

class TestFork_02(unittest.TestCase):
    def test_01_fork(self):
        pid = fork.fork()
        if (pid == 0): # child
            os._exit(13)
        else: # parent
            trace.cont(pid)
            pid, status = os.waitpid(pid, 0)
            self.assert_(os.WIFEXITED(status))
            self.assertEqual(os.WEXITSTATUS(status), 13)

if __name__ == '__main__':
    unittest.main()