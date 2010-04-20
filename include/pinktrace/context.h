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

#ifndef PINKTRACE_GUARD_CONTEXT_H
#define PINKTRACE_GUARD_CONTEXT_H 1

#include <stdbool.h>

/**
 * This opaque structure represents a tracing context.
 **/
typedef struct pink_context pink_context_t;

/**
 * Allocate a new tracing context.
 *
 * \return A tracing context on success, NULL on failure and sets errno
 * accordingly.
 **/
pink_context_t *
pink_context_new(void);

/**
 * Free the given tracing context.
 *
 * \param ctx The tracing context to be freed.
 **/
void
pink_context_free(pink_context_t *ctx);

/**
 * Set the attach property of the given tracing context. If this property is
 * set, tracing will start by attaching to the eldest child using PTRACE_ATTACH
 * and at the end of the tracing session, PTRACE_DETACH will be called.
 *
 * \param ctx The tracing context whose attach property is to be modified.
 * \param on Boolean that specifies whether the attach property is enabled.
 *
 **/
void
pink_context_set_attach(pink_context_t *ctx, bool on);

/**
 * Accessor function for the attach property.
 *
 * \param ctx The tracing context whose attaching context is to be returned.
 *
 * \return true if attach is enabled, false otherwise.
 **/
bool
pink_context_get_attach(pink_context_t *ctx);

/**
 * Set the options property of the given tracing context.
 * Note: PINK_TRACE_OPTION_SYSGOOD is always set if available.
 *
 * \param ctx The tracing context whose options property is to be modified.
 * \param options Bitwise OR'ed PINK_TRACE_OPTION_* flags.
 **/
void
pink_context_set_options(pink_context_t *ctx, int options);

/**
 * Accessor function for the options property.
 *
 * \param ctx The tracing context whose options property is to be returned.
 *
 * \return Bitwise OR'ed flags PINK_TRACE_OPTION_* flags.
 **/
int
pink_context_get_options(pink_context_t *ctx);

#endif /* !PINKTRACE_GUARD_CONTEXT_H */