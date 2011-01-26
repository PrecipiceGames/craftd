/*
 * Copyright (c) 2010 Kevin M. Bowling, <kevin.bowling@kev009.com>, USA
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

#include <config.h>

#include <stdlib.h>
#include <stdio.h>

#include "network.h"
#include "network-private.h"
#include "packets.h"
#include "proxy.h"
#include "craftd.h"

/**
 * This internal method proxy packets from a pointer to a struct and
 * a packet type
 * 
 * @remarks Scope: private
 * 
 * @param bev buffer event
 * @param pkttype type of packet
 * @param packet Pointer to a packet struct
 */
void process_proxypacket(struct PL_entry *player, uint8_t pkttype, void * packet)
{
  switch(pkttype)
  {
    case PID_LOGIN:
    {
        // TODO: Future, async check of minecraft.net for user validity
	// TODO: Future, check against local ACL
	struct packet_login* lpacket = (struct packet_login*) packet;
	
	
	/* Check if the client version is compatible with the craftd version */
	if (lpacket->version != PROTOCOL_VERSION)
	{
	  bstring dconmsg;
	  dconmsg = bfromcstr("Client version is incompatible with this server.");
	  send_kick(player, dconmsg);
	  bstrFree(dconmsg);
	  return;
	}
	
	/* Otherwise, finish populating their Player List entry */
	pthread_rwlock_wrlock(&player->rwlock);
	player->username = bstrcpy(lpacket->username);
	
	
	Server *server = NULL;
	for(int i = 0; Config.proxy_servers[i] != NULL;i++)
	{
	  if(strcmp(Config.proxy_servers[i]->name,Config.proxy_default_server)==0)
	    server = Config.proxy_servers[i];
	}
	
	if(server != NULL)
	  player->sev = create_servercon(player,server);
	
	
	player->loginpacket = Malloc(sizeof(struct packet_login));
	
	memcpy(player->loginpacket, lpacket, sizeof(struct packet_login));
	
	pthread_rwlock_unlock(&player->rwlock);
	send_loginresp(player);
	//send_prechunk(player, 0, 0, true); // TODO: pull spwan position from file
	//send_chunk(player, 0, 0, 0, 16, 128, 16);  // TODO: pull spawn position
	
	/* Login message */
 	bstring loginmsg = bformat("Player %s has joined the proxy server!", 
	    player->username->data);
	send_syschat(loginmsg);
	bstrFree(loginmsg);

	/* Send player MOTD */
	for(int i=0; i < Config_motdsz; ++i)
	{
	  send_directchat(player, Config_motd[i]);
	}
	return;
    }
  }
  return;
}

bool process_isproxypassthrough(uint8_t pkttype)
{
  switch(pkttype)
  {
    case PID_LOGIN:
    case PID_HANDSHAKE:
      return 1;
  }
  return 0;
}