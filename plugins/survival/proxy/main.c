#include <craftd/Plugin.h>
#include <craftd/Server.h>

#include <craftd/protocols/survival.h>

#include "include/SurvivalProxy.h"

#include <craftd/common.h>

#include "callbacks.c"


static
bool
cdsurvivalproxy_ServerStart (CDServer* server)
{
	CDPlugin* self = CD_GetPlugin(server->plugins, "survival.proxy");
	
	CDSurvivalProxyData* proxyData = CD_malloc(sizeof(CDSurvivalProxyData));
	CD_DynamicPut(self, "Proxy.data", (CDPointer)proxyData);
	
	proxyData->port = 25565;
	proxyData->hostname = "127.0.0.1";
	proxyData->dnsBase = evdns_base_new(server->event.base, 1);
	
	DO {
		C_IN(connection, C_ROOT(self->config), "connection") {
			C_SAVE(C_GET(connection, "port"), C_INT, proxyData->port);
			C_SAVE(C_GET(connection, "hostname"), C_STRING, proxyData->hostname);
		}
	}

	return true;
}

static
bool
cdsurvivalproxy_ServerStop (CDServer* server)
{
	return true;
}

extern
bool
CD_PluginInitialize (CDPlugin* self)
{
	self->description = CD_CreateStringFromCString("Survival Proxy");
	
	CD_InitializeSurvivalProtocol(self->server);
	
	CD_EventRegister(self->server, "Server.start!", cdsurvivalproxy_ServerStart);
	CD_EventRegister(self->server, "Server.stop!", cdsurvivalproxy_ServerStop);
	
	CD_EventRegister(self->server, "Client.connect", cdsurvivalproxy_ClientConnect);
	CD_EventRegister(self->server, "Client.process", cdsurvivalproxy_ClientProcess);
	CD_EventRegister(self->server, "Client.disconnect", (CDEventCallbackFunction)cdsurvivalproxy_ClientDisconnect);
	
	return true;
}

extern
bool
CD_PluginFinalize (CDPlugin* self)
{
	CD_EventUnregister(self->server, "Server.start!", cdsurvivalproxy_ServerStart);
	CD_EventUnregister(self->server, "Server.stop!", cdsurvivalproxy_ServerStop);

	CD_EventUnregister(self->server, "Client.connect", cdsurvivalproxy_ClientConnect);
	CD_EventUnregister(self->server, "Client.process", cdsurvivalproxy_ClientProcess);
	CD_EventUnregister(self->server, "Client.disconnect", (CDEventCallbackFunction)cdsurvivalproxy_ClientDisconnect);
	
	return true;
}