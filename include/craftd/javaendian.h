/*
 * Copyright (c) 2010-2011 Kevin M. Bowling, <kevin.bowling@kev009.com>, USA
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
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

#ifndef CRAFTD_JAVAENDIAN_H
#define CRAFTD_JAVAENDIAN_H

/* Functions to get floating point numbers into Java's endian order
 * If we're on a big-endian float machine, macro away the conversions
 *
 * Uses FLOAT_WORDS_BIGENDIAN from autoconf extras
 *
 * Also define 64-bit integer routines since they don't always exist
 */

#include <craftd/config.h>

#include <string.h>

#ifdef HAVE_ENDIAN_H
#   include <endian.h>
#endif

#ifdef HAVE_SYS_ENDIAN_H
#   include <sys/endian.h>
#endif

#ifdef HAVE_NETINTET_IN_H
#   include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#   include <arpa/inet.h>
#endif

#ifdef WORDS_BIGENDIAN
#   ifndef htonll
#       define htonll(ll) (ll)
#   endif

#   ifndef ntohll
#       define ntohll(ll) (ll)
#   endif
#else
#   if (HAVE_BE64TOH) && !defined(ntohll)
#       define ntohll(ll) be64toh(ll)

#       if (HAVE_HTOBE64)
#           define htonll(ll) htobe64(ll)
#       else
#           define htonll(ll) be64toh(ll)
#       endif
#   else
#       ifndef ntohll
#           define ntohll(x) ((((uint64_t) ntohl(x)) << 32) + ntohl(x >> 32))
#       endif

#       ifndef htonll
#           define htonll(x) ((((uint64_t) htonl(x)) << 32) + htonl(x >> 32))
#       endif
#   endif
#endif


#ifdef FLOAT_WORDS_BIGENDIAN
/* Floating point types are big-endian, macro out conversion */
#   define ntohd(d) (d)
#   define ntohf(f) (f)

#   define htond(d) (d)
#   define htonf(f) (f)
#else
/* We need to convert native floating-point types */

static inline
double
ntohd (double d)
{
	union {
		uint64_t l;
		double   d;
	} tmp;

	tmp.d = d;
	tmp.l = ntohll(tmp.l);

	return tmp.d;
}

static inline
double
htond (double d)
{
	union {
		uint64_t l;
		double   d;
	} tmp;

	tmp.d = d;
	tmp.l = htonll(tmp.l);

	return tmp.d;
}


static inline
float
ntohf (float f)
{
	union {
		uint32_t i;
		float f;
	} tmp;

	tmp.f = f;
	tmp.i = ntohl(tmp.i);

	return tmp.f;
}

static inline
float
htonf (float f)
{
	union {
		uint32_t i;
		float f;
	} tmp;

	tmp.f = f;
	tmp.i = htonl(tmp.i);

	return tmp.f;
}

#endif

#endif
