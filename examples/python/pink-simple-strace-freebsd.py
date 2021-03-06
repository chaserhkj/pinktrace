#!/usr/bin/env python
# coding: utf-8

"""
A simple strace like program using pinktrace for FreeBSD.
"""

from __future__ import print_function

import errno, os, signal, socket, sys
import pinktrace.bitness
import pinktrace.string
import pinktrace.strarray
import pinktrace.syscall
import pinktrace.trace

def print_ret(ret):
    """Print return value"""

    if (ret >= 0):
        print("= %ld" % ret, end="")
    else:
        print("= %ld %s (%s)" % (ret, errno.errorcode[-ret], os.strerror(-ret)), end="")

def decode_open(pid, bitness):
    """Decode an open() call."""

    path = pinktrace.string.decode(pid, 0, -1, bitness)
    flags = pinktrace.syscall.get_arg(pid, 1, bitness)

    print("open(\"%s\", %d)" % (path, flags) , end="")

def decode_execve(pid, bitness):
    """Decode an execve() call"""

    path = pinktrace.string.decode(pid, 0, -1, bitness)
    addr = pinktrace.syscall.get_arg(pid, 1, bitness)

    print("execve(\"%s\", [" % path, end="")

    i = 0
    sep = ""
    while True:
        path = pinktrace.strarray.decode(pid, addr, i)
        if path is not None:
            print("%s\"%s\"" % (sep, path), end="")
            i += 1
            sep = ", "
        else:
            print("], envp[]", end="")
            break

def decode_socketcall(pid, bitness, scname):
    """Decode a bind() or connect() call"""

    addr, fd = pinktrace.socket.decode_address_fd(pid, 1, bitness)

    print("%s(%ld, " % (scname, fd), end="")
    if addr.family == -1: # NULL
        print("NULL", end="")
    elif addr.family == socket.AF_UNIX:
        print("{sa_family=AF_UNIX, path=%s}" % addr.path, end="")
    elif addr.family == socket.AF_INET:
        print("{sa_family=AF_INET, sin_port=htons(%d), sin_addr=inet(\"%s\")}" % (addr.port, addr.ip), end="")
    elif pinktrace.HAVE_IPV6 and addr.family == socket.AF_INET6:
        print("{sa_family=AF_INET6, sin6_port=htons(%d), inet_pton(AF_INET6, \"%s\", &sin6_addr)}" % (addr.port, addr.ipv6), end="")
    else: # Unknown/unsupported family
        print("{sa_family=???}", end="")

    print(", %d)" % addr.length, end="")

if len(sys.argv) < 2:
    print("Usage: %s program [argument...]", file=sys.stderr)
    sys.exit(1)

pid = os.fork()
if not pid: # child
    pinktrace.trace.me()
    os.kill(os.getpid(), signal.SIGSTOP)

    try:
        os.execvp(sys.argv[1], sys.argv[1:])
    except OSError:
        os._exit(-1)

pid, status = os.waitpid(pid, 0)
assert os.WIFSTOPPED(status), "%#x" % status
assert os.WSTOPSIG(status) == signal.SIGSTOP, "%#x" % status

# parent
# Figure out the bitness of the child.
bitness = pinktrace.bitness.get(pid)
print("Child %d runs in %s mode" % (pid, pinktrace.bitness.name(bitness)))

inexecve = False
insyscall = False
sig = 0
exit_code = 0
while True:
    # At this point the traced child is stopped and needs to be resumed.
    pinktrace.trace.syscall(pid, sig)
    sig = 0
    pid, status = os.waitpid(pid, 0)

    if os.WIFSTOPPED(status):
        if os.WSTOPSIG(status) == signal.SIGTRAP:
            # We get this event twice, one at entering a
            # system call and one at exiting a system call.
            if insyscall:
                ret = pinktrace.syscall.get_ret(pid)
                if inexecve:
                    inexecve = False
                    if not ret:
                        # Update bitness
                        bitness = pinktrace.bitness.get(pid)
                        continue
                # Exiting the system call, print the return value.
                print("", end=" ")
                print_ret(ret)
                print("")
                insyscall = False
            else:
                # Get the system call number and decode as needed.
                scno = pinktrace.syscall.get_no(pid)
                scname = pinktrace.syscall.name(scno)

                if scname == 'execve':
                    inexecve = True

                if scname is None:
                    print("%ld()" % scno, end="")
                elif scname == 'open':
                    decode_open(pid, bitness)
                elif scname == 'execve':
                    decode_execve(pid, bitness)
                elif scname in ("bind", "connect"):
                    decode_socketcall(pid, bitness, scname)
                else:
                    print("%s()" % scname, end="")
                insyscall = True
        else:
            # Child received a genuine signal, send it.
            sig = os.WSTOPSIG(status)
    elif os.WIFEXITED(status):
        exit_code = os.WEXITSTATUS(status)
        print("Child %d exited normally with return code %d" % (pid, exit_code))
        break
    elif os.WIFSIGNALED(status):
        exit_code = 128 + os.WTERMSIG(status)
        print("Child %d exited with signal %d" % (pid, os.TERMSIG(status)))
        break

sys.exit(exit_code)
