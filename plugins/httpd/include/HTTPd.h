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

#ifndef CRAFTD_HTTP_HTTPD_H
#define CRAFTD_HTTP_HTTPD_H

#include <craftd/Server.h>

#include <event2/http.h>

typedef struct _CDContentType {
  const char* extension;
  const char* mime;
} CDContentType;

static const CDContentType CDContentTypes[] = {
  { "txt",  "text/plain" },
  { "c",    "text/plain" },
  { "h",    "text/plain" },
  { "js",   "text/javascript" },
  { "html", "text/html" },
  { "htm",  "text/html" },
  { "css",  "text/css" },
  { "gif",  "image/gif" },
  { "jpg",  "image/jpeg"},
  { "jpeg", "image/jpeg" },
  { "png",  "image/png" },
  { "pdf",  "application/pdf" },
  { "ps",   "application/postsript" },
  { NULL },
};

typedef struct _CDHTTPd {
	CDServer* server;

	struct {
		struct event_base*          base;
		struct evhttp*              httpd;
		struct evhttp_bound_socket* handle;
	} event;

	struct {
		struct {
			struct {
				const char* ipv4;
				const char* ipv6;
			} bind;

			uint16_t port;
		} connection;

		const char* root;
	} config;

	pthread_t      thread;
	pthread_attr_t attributes;
} CDHTTPd;

CDHTTPd* CD_CreateHTTPd (CDPlugin* plugin);

void CD_DestroyHTTPd (CDHTTPd* self);

void* CD_RunHTTPd (CDHTTPd* self);

bool CD_StopHTTPd (CDHTTPd* self);

#endif
