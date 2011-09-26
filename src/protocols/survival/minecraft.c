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

#define CRAFTD_SURVIVAL_MINECRAFT_IGNORE_EXTERN
#include <craftd/protocols/survival/common.h>
#undef CRAFTD_SURVIVAL_MINECRAFT_IGNORE_EXTERN

const char* SVCharset =
	" #$%&\"()*+,-./:;<=>!?@[\\]^_'{|}~⌂ªº¿®¬½¼¡«»£×ƒ"
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜøØáíóúñÑ";

const char SVCharsetPixel[] = {
	4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 1, 5, 1, 5, 1, 1, 4, 5, 4, 1, 5, 6, 3, 5, 3, 5
};

const size_t SVCharsetLen = 143;
const size_t SVCharsetOffset[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 35, 37, 39, 41, 43, 45, 47,
	49, 51, 53, 55, 57, 59, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
	74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
	93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108,
	109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
	125, 127, 129, 131, 133, 135, 137, 139, 141, 143, 145, 147, 149, 151, 153,
	155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183,
	185, 187, 189, 191
};

const SVEntityId SVMaxEntityId = INT_MAX;

void
SV_DestroyString (SVString self)
{
	CD_DestroyString(self);
}

SVMetadata*
SV_CreateMetadata (void)
{
	SVMetadata* self = CD_malloc(sizeof(SVMetadata));
	
	if (!self) {
		return NULL;
	}
	
	self->length = 0;
	self->item = CD_malloc(0);

	return self;
}

void
SV_DestroyMetadata (SVMetadata* self)
{
	for (size_t i = 0; i < self->length; i++) {
		SV_DestroyData(self->item[i]);
	}

	CD_free(self->item);
	CD_free(self);
}

SVData*
SV_CreateData (void)
{
	SVData* self = CD_malloc(sizeof(SVData));

	if (!self) {
		return NULL;
	}

	return self;
}

void
SV_DestroyData (SVData* self)
{
	// Destroy the bstring, the other types lay on the stack
	if (self->type == SVTypeString) {
		CD_DestroyString(self->data.S);
	}

	CD_free(self);
}

SVMetadata*
SV_ConcatDatas (SVMetadata* metadata, SVData** items, size_t length)
{
	for (size_t i = 0; i < length; i++) {
		SV_AppendData(metadata, items[i]);
	}

	return metadata;
}

SVMetadata*
SV_AppendData (SVMetadata* metadata, SVData* data)
{
	metadata->item = CD_realloc(metadata->item, sizeof(SVData*) * ++metadata->length);

	metadata->item[metadata->length - 1] = data;

	return metadata;
}

bool
SV_StringIsValid (SVString self)
{
	assert(self);

	for (size_t i = 0, ie = CD_StringLength(self); i < ie; i++) {
		bool      has = false;
		CDString* ch  = CD_CharAt(self, i);

		for (size_t h = 0; h < SVCharsetLen; h++) {
			const char* che = &SVCharset[SVCharsetOffset[h]];

			if (strncmp(CD_StringContent(ch), che, CD_StringSize(ch)) == 0) {
				has = true;
				break;
			}
		}

		if (!has && !(strncmp(CD_StringContent(ch), "§", 2) == 0 && i < ie - 2)) {
			CD_DestroyString(ch);
			return false;
		}

		CD_DestroyString(ch);
	}

	return true;
}

SVString
SV_StringSanitize (SVString self)
{
	CDString* result = CD_CreateString();

	assert(self);

	for (size_t i = 0, ie = CD_StringLength(self); i < ie; i++) {
		bool      has = false;
		CDString* ch  = CD_CharAt(self, i);

		for (size_t h = 0; h < SVCharsetLen; h++) {
			const char* che = &SVCharset[SVCharsetOffset[h]];
			
			if (strncmp(CD_StringContent(ch), che, CD_StringSize(ch)) == 0) {
				has = true;
				break;
			}
		}

		if (i == ie - 2 && strncmp(CD_StringContent(ch), "§", 2) == 0){
			CD_DestroyString(ch);
			break;
		}

		if (has || strncmp(CD_StringContent(ch), "§", 2) == 0) {
			CD_AppendString(result, ch);
		}
		else {
			CD_AppendCString(result, "?");
		}

		CD_DestroyString(ch);
	}

	self->length = CD_UTF8_strnlen(CD_StringContent(self), self->raw->slen);

	return result;
}

SVString
SV_StringColorRange (SVString self, SVStringColor color, size_t a, size_t b)
{
	if (self->external) {
		CDString* tmp = self;
		self          = CD_CloneString(tmp);
		CD_DestroyString(tmp);
	}

	CDString* start = CD_CreateStringFromFormat("§%x", color);
	CDString* end   = CD_CreateStringFromCString(SV_COLOR_WHITE);

	assert(self);
	assert(a < b);
	assert(a >= 0 && b <= CD_StringLength(self));

	CD_InsertString(self, end, b);
	CD_InsertString(self, start, a);

	CD_DestroyString(start);
	CD_DestroyString(end);

	return self;
}

SVString
SV_StringColor (SVString self, SVStringColor color)
{
	assert(self);

	if (CD_StringLength(self) > 0) {
		return SV_StringColorRange(self, color, 0, CD_StringLength(self));
	}
	else {
		return self;
	}
}

bool
SV_CompareChunkPosition (CDSet* self, SVChunkPosition* a, SVChunkPosition* b)
{
	assert(self);

	return (a->x == b->x && a->z == b->z);
}

unsigned int
SV_HashChunkPosition (CDSet* self, SVChunkPosition* position)
{
	const int HASHMULTIPLIER = 31;
	const int CHUNKBUCKETS   = 401; // Max chunks to the nearest prime

	assert(self);

	return ((((position->x * HASHMULTIPLIER)) * HASHMULTIPLIER + position->z) * HASHMULTIPLIER) % CHUNKBUCKETS;
}

void
SV_ChunkToByteArray (SVChunk* chunk, uint8_t* array)
{
	size_t offset = 0;

	memcpy(array + offset, chunk->blocks, 32768);
	offset += 32768;
	memcpy(array + offset, chunk->data, 16384);
	offset += 16384;
	memcpy(array + offset, chunk->blockLight, 16384);
	offset += 16384;
	memcpy(array + offset, chunk->skyLight, 16384);
}
