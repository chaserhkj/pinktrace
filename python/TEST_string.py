#!/usr/bin/env python
# coding: utf-8

import os, signal, sys, unittest

sys.path.insert(0, '.')
from pinktrace import string, syscall

UNAME = os.uname()

class TestString_01_Invalid(unittest.TestCase):

    def test_01_decode(self):
        self.assertRaises(TypeError, string.decode)
        self.assertRaises(TypeError, string.decode, 0)
        self.assertRaises(IndexError, string.decode, 0, syscall.MAX_ARGS)
        self.assertRaises(ValueError, string.decode, 0, 1, -1, 13)

    def test_02_encode(self):
        self.assertRaises(TypeError, string.encode)
        self.assertRaises(TypeError, string.encode, 0)
        self.assertRaises(TypeError, string.encode, 0, 1)
        self.assertRaises(IndexError, string.encode, 0, syscall.MAX_ARGS, 'pink')
        self.assertRaises(ValueError, string.encode, 0, 1, 'pink', 13)

    def test_03_encode_safe(self):
        if UNAME[0] != 'Linux': return

        self.assertRaises(TypeError, string.encode_safe)
        self.assertRaises(TypeError, string.encode_safe, 0)
        self.assertRaises(TypeError, string.encode_safe, 0, 1)
        self.assertRaises(IndexError, string.encode_safe, 0, syscall.MAX_ARGS, 'pink')
        self.assertRaises(ValueError, string.encode_safe, 0, 1, 'pink', 13)

if __name__ == '__main__':
    unittest.main()
