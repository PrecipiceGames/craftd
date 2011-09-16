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

#ifndef CRAFTD_BUFFER_H
#define CRAFTD_BUFFER_H

#ifndef CRAFTD_BUFFERS_H
#define CRAFTD_BUFFERS_H
#include <craftd/common.h>
#undef CRAFTD_BUFFERS_H
#else
#include <craftd/common.h>
#endif

typedef struct evbuffer* CDRawBuffer;

typedef struct _CDBuffer {
	CDRawBuffer raw;

	bool external;
} CDBuffer;

/**
 * Create an empty Buffer object
 *
 * @return The instantiated Buffer object
 */
CDBuffer* CD_CreateBuffer (void);

/**
 * Wrap an existing raw buffer (struct evbuffer*) into a Buffer object
 *
 * @return The instantiated Buffer object
 */
CDBuffer* CD_WrapBuffer (CDRawBuffer buffer);

void CD_DestroyBuffer (CDBuffer* self);

CDPointer CD_BufferContent (CDBuffer* self);

size_t CD_BufferLength (CDBuffer* self);

bool CD_BufferEmpty (CDBuffer* self);

int CD_BufferDrain (CDBuffer* self, size_t length);

void CD_BufferAdd (CDBuffer* self, CDPointer data, size_t length);

void CD_BufferAddBuffer (CDBuffer* self, CDBuffer* data);

CDPointer CD_BufferRemove (CDBuffer* self, size_t length);

CDBuffer* CD_BufferRemoveBuffer (CDBuffer* self);

#endif
