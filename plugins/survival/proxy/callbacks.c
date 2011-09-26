#include <craftd/Logger.h>

#include "include/SurvivalProxy.h"

#include <craftd/common.h>


void
cdsurvivalproxy_ClientSendPacket(CDClient* client, SVPacket* packet) {
	if (!client) return;
	
	CDBuffer* data = SV_PacketToBuffer(packet);
	
	CD_ClientSendBuffer(client, data);
	
	CD_DestroyBuffer(data);
}

void
cdsurvivalproxy_ClientProxyPacket(CDClient* client, SVPacket* packet) {
	assert(client);
	
	CDBuffers* proxyBuffers = (CDBuffers*)CD_DynamicGet(client, "Client.proxyBuffers");
	assert(proxyBuffers);
	
	CDBuffer* data = SV_PacketToBuffer(packet);
	
	CD_BufferAddBuffer(proxyBuffers->output, data);
	CD_BuffersFlush(proxyBuffers);
	
	CD_DestroyBuffer(data);
}

static
bool
cdsurvivalproxy_ClientProcess (CDServer* server, CDClient* client, SVPacket* packet) {
	switch (packet->type) {
		case SVDisconnect: {
			SVPacketDisconnect* data = (SVPacketDisconnect*) packet->data;

			CD_ServerKick(server, client, CD_CloneString(data->request.reason));
		} break;

		case SVListPing: {
			SVPacketDisconnect pkt = {
				.ping = {
				   .description = CD_CreateStringFromCString("Craftd Proxy\u00A70\u00A70")
				}
			};
			SVPacket  packet = { SVPing, SVDisconnect, (CDPointer) &pkt };
			
			cdsurvivalproxy_ClientSendPacket(client, &packet);
		} break;

		default: {
			//send the packet straight to our server
			cdsurvivalproxy_ClientProxyPacket(client, packet);
		}	
	}
	
	//SV_DestroyPacket(packet);
	
	return true;
}

static
bool
cdsurvivalproxy_ProxyProcess (CDServer* server, CDClient* client, SVPacket* packet) {
	switch (packet->type) {
		case SVDisconnect: {
			SVPacketDisconnect* data = (SVPacketDisconnect*) packet->data;

			CD_ServerKick(server, client, CD_CloneString(data->request.reason));
		} break;

		default: {
			//send the packet straight to our client
			cdsurvivalproxy_ClientSendPacket(client, packet);
		}
	}
	
	return true;
}

static
void
cdsurvivalproxy_ProxyReadCallback (struct bufferevent* event, CDClient* client)
{
	assert(client);
	CDServer* server = client->server;
	assert(server);
	
	CDBuffers* proxyBuffers = (CDBuffers*)CD_DynamicGet(client, "Client.proxyBuffers");
	assert(proxyBuffers);
	
	void* packet;
	if (server->protocol->parsable(proxyBuffers)) {
		if ((packet = server->protocol->parse(proxyBuffers, true))) {
			CD_BufferReadIn(proxyBuffers, CDNull, CDNull);
			
			//TODO: worker, or something
			cdsurvivalproxy_ProxyProcess(server, client, packet);
			
			SV_DestroyPacket(packet);
		}
	} else {
		if (errno == EILSEQ) {
			CD_ServerKick(server, client, CD_CreateStringFromCString("bad packet"));
		}
	}
	
}

static
void
cdsurvivalproxy_ProxyErrorCallback (struct bufferevent* event, short error, CDClient* client)
{
	assert(client);
	CDServer* server = client->server;
	assert(server);
	
	if (error & BEV_EVENT_CONNECTED) {
		SLOG(client->server, LOG_INFO, "proxy connected :D");
	}
	
	if (!((error & BEV_EVENT_EOF) || (error & BEV_EVENT_ERROR) || (error & BEV_EVENT_TIMEOUT))) {
		//not an error
		return;
	}
	
	if (error & BEV_EVENT_ERROR) {
		SLOG(client->server, LOG_INFO, "libevent: ip %s - %s", client->ip, evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
	} else if (error & BEV_EVENT_TIMEOUT) {
		SERR(client->server, "A bufferevent timeout?");
	} else if (error & BEV_EVENT_EOF) {
		SLOG(client->server, LOG_INFO, "remote EOF");
	}
	
	CD_ServerKick(client->server, client, CD_CreateStringFromCString("remote connection died"));
}


static
bool
cdsurvivalproxy_ClientConnect (CDServer* server, CDClient* client)
{
	CDPlugin* self = CD_GetPlugin(server->plugins, "survival.proxy");
	
	CDSurvivalProxyData* proxyData = (CDSurvivalProxyData*)CD_DynamicGet(self, "Proxy.data");
	
	CDBuffers* proxyBuffers = CD_WrapBuffers(
		bufferevent_socket_new(server->event.base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE));
	CD_DynamicPut(client, "Client.proxyBuffers", (CDPointer) proxyBuffers);
	
	
	SLOG(server, LOG_INFO, "proxy client connect %s:%d", proxyData->hostname, proxyData->port);
	
	if (bufferevent_socket_connect_hostname(proxyBuffers->raw,
		proxyData->dnsBase,
		AF_UNSPEC,
		proxyData->hostname,
		proxyData->port) < 0) {
		
		CD_DestroyBuffers(proxyBuffers);
		return false;	
	}
	
	bufferevent_setcb(proxyBuffers->raw,
		(bufferevent_data_cb) cdsurvivalproxy_ProxyReadCallback,
		NULL,
		(bufferevent_event_cb) cdsurvivalproxy_ProxyErrorCallback,
		client);
	
	bufferevent_enable(proxyBuffers->raw, EV_READ | EV_WRITE);
	
	return true;
}

//fixme: sometimes this is being called twice O.o
static
bool
cdsurvivalproxy_ClientDisconnect (CDServer* server, CDClient* client, bool status)
{
	assert(server);
	assert(client);
	
	SLOG(server, LOG_INFO, "got disconnect for %x", client);
	
	CDBuffers* proxyBuffers = (CDBuffers*)CD_DynamicGet(client, "Client.proxyBuffers");
	if (proxyBuffers) {
		bufferevent_flush(proxyBuffers->raw, EV_READ | EV_WRITE, BEV_FINISHED);
		//bufferevent_setwatermark(proxyBuffers->raw, EV_WRITE, 0, 0);
		bufferevent_disable(proxyBuffers->raw, EV_READ | EV_WRITE);
		bufferevent_free(proxyBuffers->raw);
		
		CD_DestroyBuffers(proxyBuffers);
	}
	
	return true;
}