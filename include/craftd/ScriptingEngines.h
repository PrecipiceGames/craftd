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

#ifndef CRAFTD_SCRIPTINGENGINES_H
#define CRAFTD_SCRIPTINGENGINES_H

#include <ltdl.h>

#include <craftd/ScriptingEngine.h>

struct _CDServer;

/**
 * The ScriptingEngines class.
 */
typedef struct _CDScriptingEngines {
	struct _CDServer* server;

	CDHash* items;

	lt_dladvise advise;
} CDScriptingEngines;

/**
 * Create a ScriptingEngines object on the given Server
 *
 * @param server The server the ScriptingEngines will run on
 *
 * @return THe instantiated ScriptingEngines object
 */
CDScriptingEngines* CD_CreateScriptingEngines (struct _CDServer* server);

/**
 * Destroy a ScriptingEngines object
 */
void CD_DestroyScriptingEngines (CDScriptingEngines* self);

/**
 * Load scripting engines referenced in the Config
 */
bool CD_LoadScriptingEngines (CDScriptingEngines* self);

/**
 * Load a ScriptingEngine from the given path and save it as loaded plugin.
 *
 * @param path The path to the plugin
 */
CDScriptingEngine* CD_LoadScriptingEngine (CDScriptingEngines* self, const char* name);

CDScriptingEngine* CD_GetScriptingEngine (CDScriptingEngines* self, const char* name);

void CD_UnloadScriptingEngine (CDScriptingEngines* self, const char* name);

#endif
