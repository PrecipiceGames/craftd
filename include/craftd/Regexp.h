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

#ifndef CRAFTD_REGEXP_H
#define CRAFTD_REGEXP_H

#include <pcre.h>

typedef enum _CDRegexpOption {
	CDRegexpNone            = 0,
	CDRegexpCaseInsensitive = PCRE_CASELESS,
	CDRegexpExtended        = PCRE_EXTENDED,
	CDRegexpMultiline       = PCRE_MULTILINE,
	CDRegexpDotAll          = PCRE_DOTALL
} CDRegexpOption;

#define CD_REGEXP_I PCRE_CASELESS
#define CD_REGEXP_X PCRE_EXTENDED
#define CD_REGEXP_M PCRE_MULTILINE
#define CD_REGEXP_S PCRE_DOTALL

#include <craftd/String.h>

typedef struct _CDRegexp {
	char* string;
	int   options;

	pcre*       pattern;
	pcre_extra* study;
} CDRegexp;

typedef struct _CDRegexpMatches {
	size_t matched;

	size_t     length;
	CDString** item;
} CDRegexpMatches;

CDRegexp* CD_CreateRegexp (char* regexp, int options);

void CD_DestroyRegexp (CDRegexp* self);

void CD_DestroyRegexpKeepString (CDRegexp* self);

CDRegexpMatches* CD_CreateRegexpMatches (size_t length);

void CD_DestroyRegexpMatches (CDRegexpMatches* self);

CDRegexpMatches* CD_RegexpMatch (CDRegexp* self, CDString* string);

CDRegexpMatches* CD_RegexpMatchString (char* regexp, int options, CDString* string);

CDRegexpMatches* CD_RegexpMatchCString (char* regexp, int options, char* string);

bool CD_RegexpTest (CDRegexp* self, CDString* string);

#endif
