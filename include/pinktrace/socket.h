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

#ifndef _PINK_SOCKET_H
#define _PINK_SOCKET_H

/**
 * @file pinktrace/socket.h
 * @brief Pink's socket related data structures and functions
 * @defgroup pink_socket Pink's socket related data structures and functions
 * @ingroup pinktrace
 * @{
 **/

#include <pinktrace/macros.h>
#include <pinktrace/system.h>

#if PINK_OS_FREEBSD || defined(DOXYGEN)
#include <sys/socket.h>
#endif

#include <netinet/in.h>
#include <sys/un.h>

#if PINK_HAVE_NETLINK || defined(DOXYGEN)
#include <linux/netlink.h>
#endif


/**
 * @brief Structure which defines a decoded socket address
 **/
typedef struct {
	/** Family of the socket address **/
	int family;

	/**
	 * Length of the socket address
	 *
	 * @since 0.0.5
	 **/
	socklen_t length;

	/**
	 * This union contains type-safe pointers to the real socket address.
	 * Check the family before attempting to obtain the real object.
	 **/
	union {
		/** Padding, for internal use only **/
		char _pad[128];

		/** Socket address, for internal use only **/
		struct sockaddr _sa;

		/** Unix socket address, only valid if family is AF_UNIX. **/
		struct sockaddr_un sa_un;

		/** Inet socket address, only valid if family is AF_INET. **/
		struct sockaddr_in sa_in;

#if PINK_HAVE_IPV6 || defined(DOXYGEN)
		/**
		 * Inet6 socket address, only valid if family is AF_INET6.
		 * This member is only available if IPV6 support was enabled at
		 * compile time. Check with #PINK_HAVE_IPV6.
		 **/
		struct sockaddr_in6 sa6;
#endif /* PINK_HAVE_IPV6... */

#if PINK_HAVE_NETLINK || defined(DOXYGEN)
		/**
		 * Netlink socket address, only valid if family is AF_NETLINK.
		 * This member is only available if Netlink support was enabled
		 * at compile time. Check with #PINK_HAVE_NETLINK.
		 *
		 * @since 0.0.5
		 **/
		struct sockaddr_nl nl;
#endif /* PINK_HAVE_NETLINK... */
	} u;
} pink_socket_address_t;

#if PINK_OS_LINUX || defined(DOXYGEN)
/**
 * Decoded socket subcalls
 *
 * @note Availability: Linux
 **/
typedef enum {
	/** socket() subcall **/
	PINK_SOCKET_SUBCALL_SOCKET = 1,
	/** bind() subcall **/
	PINK_SOCKET_SUBCALL_BIND,
	/** connect() subcall **/
	PINK_SOCKET_SUBCALL_CONNECT,
	/** listen() subcall **/
	PINK_SOCKET_SUBCALL_LISTEN,
	/** accept() subcall **/
	PINK_SOCKET_SUBCALL_ACCEPT,
	/** getsockname() subcall **/
	PINK_SOCKET_SUBCALL_GETSOCKNAME,
	/** getpeername() subcall **/
	PINK_SOCKET_SUBCALL_GETPEERNAME,
	/** socketpair() subcall **/
	PINK_SOCKET_SUBCALL_SOCKETPAIR,
	/** send() subcall **/
	PINK_SOCKET_SUBCALL_SEND,
	/** recv() subcall **/
	PINK_SOCKET_SUBCALL_RECV,
	/** sendto() subcall **/
	PINK_SOCKET_SUBCALL_SENDTO,
	/** recvfrom() subcall **/
	PINK_SOCKET_SUBCALL_RECVFROM,
	/** shutdown() subcall **/
	PINK_SOCKET_SUBCALL_SHUTDOWN,
	/** setsockopt() subcall **/
	PINK_SOCKET_SUBCALL_SETSOCKOPT,
	/** getsockopt() subcall **/
	PINK_SOCKET_SUBCALL_GETSOCKOPT,
	/** sendmsg() subcall **/
	PINK_SOCKET_SUBCALL_SENDMSG,
	/** recvmsg() subcall **/
	PINK_SOCKET_SUBCALL_RECVMSG,
	/** accept4() subcall **/
	PINK_SOCKET_SUBCALL_ACCEPT4,
} pink_socket_subcall_t;

PINK_BEGIN_DECL

/**
 * Decide whether the socketcall() system call is defined for the given
 * bitness. On some architectures the socket system calls - socket, connect,
 * bind etc. - are implemented as separate system calls. On others these calls
 * are subcalls of the socketcall() system call.
 *
 * @note Availability: Linux
 *
 * @see pink_socket_subcall_t
 *
 * @param bitness Bitness
 * @return true if socketcall() system call is available, false otherwise
 **/
bool pink_has_socketcall(pink_bitness_t bitness)
	PINK_GCC_ATTR((pure));

/**
 * Name the given socket subcall
 *
 * @note Availability: Linux
 *
 * @param subcall Socket subcall
 * @return Name of the subcall
 **/
const char *pink_name_socket_subcall(pink_socket_subcall_t subcall)
	PINK_GCC_ATTR((pure));

PINK_END_DECL
#endif
/** @} */
#endif
