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

#include <craftd/Logger.h>

#include <craftd/protocols/survival/Packet.h>

SVPacket*
SV_PacketFromBuffers (CDBuffers* buffers, bool isResponse)
{
	SVPacket* self = CD_malloc(sizeof(SVPacket));

	assert(self);
	
	if (isResponse) {
		self->chain = SVResponse;
	} else {
		self->chain = SVRequest;
	}
	self->type  = (uint32_t) (uint8_t) SV_BufferRemoveByte(buffers->input);
	self->data  = SV_GetPacketDataFromBuffer(self, buffers->input);

	if (!self->data) {
		ERR("unparsable packet 0x%.2X", self->type);

		SV_DestroyPacket(self);

		errno = EILSEQ;

		return NULL;
	}

	return self;
}

void
SV_DestroyPacket (SVPacket* self)
{
	assert(self);

	SV_DestroyPacketData(self);

	CD_free((void*) self->data);
	CD_free(self);
}

void
SV_DestroyPacketData (SVPacket* self)
{
	if (!self->data) {
		return;
	}

	switch (self->chain) {
		case SVRequest: {
			switch (self->type) {
				case SVLogin: {
					SVPacketLogin* packet = (SVPacketLogin*) self->data;

					SV_DestroyString(packet->request.username);
				} break;

				case SVHandshake: {
					SVPacketHandshake* packet = (SVPacketHandshake*) self->data;

					SV_DestroyString(packet->request.username);
				} break;

				case SVChat: {
					SVPacketChat* packet = (SVPacketChat*) self->data;

					SV_DestroyString(packet->request.message);
				} break;

				case SVEntityMetadata: {
					SVPacketEntityMetadata* packet = (SVPacketEntityMetadata*) self->data;

					SV_DestroyMetadata(packet->request.metadata);
				} break;

				case SVUpdateSign: {
					SVPacketUpdateSign* packet = (SVPacketUpdateSign*) self->data;

					SV_DestroyString(packet->request.first);
					SV_DestroyString(packet->request.second);
					SV_DestroyString(packet->request.third);
					SV_DestroyString(packet->request.fourth);
				} break;

				case SVDisconnect: {
					SVPacketDisconnect* packet = (SVPacketDisconnect*) self->data;

					SV_DestroyString(packet->request.reason);
				}

				default: break;
			}
		} break;
		
		case SVPing: {
			switch(self->type) {
				case SVDisconnect: {
						SVPacketDisconnect* packet = (SVPacketDisconnect*) self->data;

						SV_DestroyString(packet->ping.description);
				} break;
				default: break;
			}
		}break;

		case SVResponse: {
			switch (self->type) {
				case SVLogin: {
					SVPacketLogin* packet = (SVPacketLogin*) self->data;

					SV_DestroyString(packet->response.u1);
				} break;

				case SVHandshake: {
					SVPacketHandshake* packet = (SVPacketHandshake*) self->data;

					SV_DestroyString(packet->response.hash);
				} break;

				case SVChat: {
					SVPacketChat* packet = (SVPacketChat*) self->data;

					SV_DestroyString(packet->request.message);
				} break;

				case SVNamedEntitySpawn: {
					SVPacketNamedEntitySpawn* packet = (SVPacketNamedEntitySpawn*) self->data;

					SV_DestroyString(packet->response.name);
				} break;

				case SVSpawnMob: {
					SVPacketSpawnMob* packet = (SVPacketSpawnMob*) self->data;

					SV_DestroyMetadata(packet->response.metadata);
				} break;

				case SVPainting: {
					SVPacketPainting* packet = (SVPacketPainting*) self->data;

					SV_DestroyString(packet->response.title);
				} break;

				case SVEntityMetadata: {
					SVPacketEntityMetadata* packet = (SVPacketEntityMetadata*) self->data;

					SV_DestroyMetadata(packet->response.metadata);
				} break;

				case SVMapChunk: {
					SVPacketMapChunk* packet = (SVPacketMapChunk*) self->data;

					CD_free(packet->response.item);
				} break;

				case SVMultiBlockChange: {
					SVPacketMultiBlockChange* packet = (SVPacketMultiBlockChange*) self->data;

					CD_free(packet->response.coordinate);
					CD_free(packet->response.type);
					CD_free(packet->response.metadata);
				} break;

				case SVExplosion: {
					SVPacketExplosion* packet = (SVPacketExplosion*) self->data;

					CD_free(packet->response.item);
				} break;

				case SVOpenWindow: {
					SVPacketOpenWindow* packet = (SVPacketOpenWindow*) self->data;

					SV_DestroyString(packet->response.title);
				} break;

				case SVWindowItems: {
					SVPacketWindowItems* packet = (SVPacketWindowItems*) self->data;

					CD_free(packet->response.item);
				} break;
				
                case SVItemData: {
                	SVPacketItemData* packet = (SVPacketItemData*) self->data;

                	CD_free(packet->response.text); //This value is a byte array, not a string. Blame notch and his names!
                }

                case SVDisconnect: {
                    SVPacketDisconnect* packet = (SVPacketDisconnect*) self->data;

					SV_DestroyString(packet->response.reason);
				} break;

				default: break;
			}
		} break;
	}
}

CDPointer
SV_GetPacketDataFromBuffer (SVPacket* self, CDBuffer* input)
{
    assert(self);
    assert(input);
    DEBUG("Recieved packet type %x",self->type);
    
	switch (self->chain) {
		case SVRequest: {
			switch (self->type) {
				case SVKeepAlive: {
		        	SVPacketKeepAlive* packet = (SVPacketKeepAlive*) CD_malloc(sizeof(SVPacketKeepAlive));

		        	packet->keepAliveID = SV_BufferRemoveInteger(input);

		        	return (CDPointer) packet;
		        }
	
				case SVLogin: {
					SVPacketLogin* packet = (SVPacketLogin*) CD_malloc(sizeof(SVPacketLogin));

		            SV_BufferRemoveFormat(input, "iUlibbbb",
						&packet->request.version,
						&packet->request.username,
						&packet->request.u1,
						&packet->request.u2,
						&packet->request.u3,
						&packet->request.u4,
						&packet->request.u5,
						&packet->request.u6
					);

					return (CDPointer) packet;
				}
			
				case SVHandshake: {
					SVPacketHandshake* packet = (SVPacketHandshake*) CD_malloc(sizeof(SVPacketHandshake));

					packet->request.username = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}
			
				case SVChat: {
					SVPacketChat* packet = (SVPacketChat*) CD_malloc(sizeof(SVPacketChat));

					packet->request.message = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}
			
				case SVUseEntity: {
					SVPacketUseEntity* packet = (SVPacketUseEntity*) CD_malloc(sizeof(SVPacketUseEntity));

					SV_BufferRemoveFormat(input, "iib",
						&packet->request.user,
						&packet->request.target,
						&packet->request.leftClick
					);

					return (CDPointer) packet;
				}
			
				case SVRespawn: {
					SVPacketRespawn* packet = (SVPacketRespawn*) CD_malloc(sizeof(SVPacketRespawn));

					SV_BufferRemoveFormat(input, "bbbsl",
						&packet->request.world,
						&packet->request.u1,
						&packet->request.mode,
						&packet->request.worldHeight,
						&packet->request.mapSeed
					);
					return (CDPointer) packet;
		        }
	
				case SVOnGround: {
					SVPacketOnGround* packet = (SVPacketOnGround*) CD_malloc(sizeof(SVPacketOnGround));

					packet->request.onGround = SV_BufferRemoveBoolean(input);

					return (CDPointer) packet;
				}
			
				case SVPlayerPosition: {
					SVPacketPlayerPosition* packet = (SVPacketPlayerPosition*) CD_malloc(sizeof(SVPacketPlayerPosition));

					SV_BufferRemoveFormat(input, "ddddb",
						&packet->request.position.x,
						&packet->request.position.y,
						&packet->request.stance,
						&packet->request.position.z,
						&packet->request.is.onGround
					);

					return (CDPointer) packet;
				}
			
				case SVPlayerLook: {
					SVPacketPlayerLook* packet = (SVPacketPlayerLook*) CD_malloc(sizeof(SVPacketPlayerLook));

					SV_BufferRemoveFormat(input, "ffb",
						&packet->request.yaw,
						&packet->request.pitch,
						&packet->request.is.onGround
					);

					return (CDPointer) packet;
				}
			
				case SVPlayerMoveLook: {
					SVPacketPlayerMoveLook* packet = (SVPacketPlayerMoveLook*) CD_malloc(sizeof(SVPacketPlayerMoveLook));

					SV_BufferRemoveFormat(input, "ddddffb",
						&packet->request.position.x,
						&packet->request.stance,
						&packet->request.position.y,
						&packet->request.position.z,
						&packet->request.yaw,
						&packet->request.pitch,
						&packet->request.is.onGround
					);

					return (CDPointer) packet;
				}
			
				case SVPlayerDigging: {
					SVPacketPlayerDigging* packet = (SVPacketPlayerDigging*) CD_malloc(sizeof(SVPacketPlayerDigging));

					packet->request.status = SV_BufferRemoveByte(input);

					SV_BufferRemoveFormat(input, "ibi",
						&packet->request.position.x,
						&packet->request.position.y,
						&packet->request.position.z
					);

					packet->request.face = SV_BufferRemoveByte(input);

					return (CDPointer) packet;
				}
			
				case SVPlayerBlockPlacement: {
					SVPacketPlayerBlockPlacement* packet = (SVPacketPlayerBlockPlacement*) CD_malloc(sizeof(SVPacketPlayerBlockPlacement));

					memset(&packet->request.item, 0, sizeof(SVItemStack));

					SV_BufferRemoveFormat(input, "ibibs",
						&packet->request.position.x,
						&packet->request.position.y,
						&packet->request.position.z,

						&packet->request.direction,
						&packet->request.item.id
					);

					if (packet->request.item.id != -1) {
						SV_BufferRemoveFormat(input, "bs",
							&packet->request.item.count,
							&packet->request.item.damage
						);
					}

					return (CDPointer) packet;
				}
			
				case SVHoldChange: {
					SVPacketHoldChange* packet = (SVPacketHoldChange*) CD_malloc(sizeof(SVPacketHoldChange));

					packet->request.slot = SV_BufferRemoveShort(input);

					return (CDPointer) packet;
				}
			
				case SVAnimation: {
					SVPacketAnimation* packet = (SVPacketAnimation*) CD_malloc(sizeof(SVPacketAnimation));

					SV_BufferRemoveFormat(input, "ib",
						&packet->request.entity.id,
						&packet->request.type
					);

					return (CDPointer) packet;
				}
			
				case SVEntityAction: {
					SVPacketEntityAction* packet = (SVPacketEntityAction*) CD_malloc(sizeof(SVPacketEntityAction));

					SV_BufferRemoveFormat(input, "ib",
						&packet->request.entity.id,
						&packet->request.type
					);
					return (CDPointer) packet;
				}
			
				case SVStanceUpdate: { //This is most likely a packet that isn't used, but it might be in the future
					SVPacketStanceUpdate* packet = (SVPacketStanceUpdate*) CD_malloc(sizeof(SVPacketStanceUpdate));

					SV_BufferRemoveFormat(input, "ffffBB",
						&packet->request.u1,
						&packet->request.u2,
						&packet->request.u3,
						&packet->request.u4,
						&packet->request.u5,
						&packet->request.u6
					);

					return (CDPointer) packet;
				}
			
				case SVEntityMetadata: {
					SVPacketEntityMetadata* packet = (SVPacketEntityMetadata*) CD_malloc(sizeof(SVPacketEntityMetadata));

					SV_BufferRemoveFormat(input, "iM",
						&packet->request.entity.id,
						&packet->request.metadata
					);

					return (CDPointer) packet;
				}
			
				case SVEntityEffect: {
					SVPacketEntityEffect* packet = (SVPacketEntityEffect*) CD_malloc(sizeof(SVPacketEntityEffect));

					SV_BufferRemoveFormat(input, "ibbs",
						&packet->request.entity.id,
						&packet->request.effect,
						&packet->request.amplifier,
						&packet->request.duration
					);

					return (CDPointer) packet;
				}
			
				case SVRemoveEntityEffect: {
					SVPacketRemoveEntityEffect* packet = (SVPacketRemoveEntityEffect*) CD_malloc(sizeof(SVPacketRemoveEntityEffect));

					SV_BufferRemoveFormat(input, "ib",
						&packet->request.entity.id,
						&packet->request.effect
					);

					return (CDPointer) packet;
				}
			
				case SVCloseWindow: {
					SVPacketCloseWindow* packet = (SVPacketCloseWindow*) CD_malloc(sizeof(SVPacketCloseWindow));

					packet->request.id = SV_BufferRemoveByte(input);

					return (CDPointer) packet;
				}
			
				case SVWindowClick: {
					SVPacketWindowClick* packet = (SVPacketWindowClick*) CD_malloc(sizeof(SVPacketWindowClick));

					memset(&packet->request.item, 0, sizeof(SVItemStack));

					SV_BufferRemoveFormat(input, "bsBsBs",
						&packet->request.id,
						&packet->request.slot,
						&packet->request.rightClick,
						&packet->request.action,
						&packet->request.shiftPressed,
						&packet->request.item.id
					);

					if (packet->request.item.id != -1) {
						SV_BufferRemoveFormat(input, "bs",
							&packet->request.item.count,
							&packet->request.item.damage
						);
					}

					return (CDPointer) packet;
				}
			
				case SVTransaction: {
					SVPacketTransaction* packet = (SVPacketTransaction*) CD_malloc(sizeof(SVPacketTransaction));

					SV_BufferRemoveFormat(input, "bsB",
						&packet->request.id,
						&packet->request.action,
						&packet->request.accepted
					);

					return (CDPointer) packet;
				}
			
				case SVCreativeInventoryAction: {
					SVPacketCreativeInventoryAction* packet = (SVPacketCreativeInventoryAction*) CD_malloc(sizeof(SVPacketCreativeInventoryAction));

					SV_BufferRemoveFormat(input, "ssss",
						&packet->request.slot,
						&packet->request.itemId,
						&packet->request.quantity,
						&packet->request.damage
					);

					return (CDPointer) packet;
				}

				case SVUpdateSign: {
					SVPacketUpdateSign* packet = (SVPacketUpdateSign*) CD_malloc(sizeof(SVPacketUpdateSign));

					SV_BufferRemoveFormat(input, "isiUUUU",
						&packet->request.position.x,
						&packet->request.position.y,
						&packet->request.position.z,

						&packet->request.first,
						&packet->request.second,
						&packet->request.third,
						&packet->request.fourth
					);

					return (CDPointer) packet;
				}

				case SVIncrementStatistic: {
					SVPacketIncrementStatistic* packet = (SVPacketIncrementStatistic*) CD_malloc(sizeof(SVPacketIncrementStatistic));

					SV_BufferRemoveFormat(input, "ib",
						&packet->request.id,
						&packet->request.amount
					);

					return (CDPointer) packet;
				}
			
				case SVListPing: {
					return (CDPointer) CD_malloc(sizeof(SVPacketListPing));
				}
			
				case SVDisconnect: {
					SVPacketDisconnect* packet = (SVPacketDisconnect*) CD_malloc(sizeof(SVPacketDisconnect));

					packet->request.reason = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}

				default: {
					return (CDPointer) NULL;
				}
			}
		} break;
		
		case SVPing: {
			switch(self->type) {
				case SVDisconnect: {
					SVPacketDisconnect* packet = (SVPacketDisconnect*) CD_malloc(sizeof(SVPacketDisconnect));

					packet->request.reason = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}

				default: {
					return (CDPointer) NULL;
				}
			}
		} break;
		
		case SVResponse: {
			switch (self->type) {
				case SVKeepAlive: {
		        	SVPacketKeepAlive* packet = (SVPacketKeepAlive*) CD_malloc(sizeof(SVPacketKeepAlive));

		        	packet->keepAliveID = SV_BufferRemoveInteger(input);

		        	return (CDPointer) packet;
		        }
	
				case SVLogin: {
					SVPacketLogin* packet = (SVPacketLogin*) CD_malloc(sizeof(SVPacketLogin));

		            SV_BufferRemoveFormat(input, "iUlibbbb",
						&packet->response.id,
						&packet->response.u1,
						&packet->response.mapSeed,
						&packet->response.serverMode,
						&packet->response.dimension,
						&packet->response.u2,
						&packet->response.worldHeight,
						&packet->response.maxPlayers
					);

					return (CDPointer) packet;
				}
			
				case SVHandshake: {
					SVPacketHandshake* packet = (SVPacketHandshake*) CD_malloc(sizeof(SVPacketHandshake));

					packet->response.hash = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}
			
				case SVChat: {
					SVPacketChat* packet = (SVPacketChat*) CD_malloc(sizeof(SVPacketChat));

					packet->response.message = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}
			
				case SVTimeUpdate: {
					SVPacketTimeUpdate* packet = (SVPacketTimeUpdate*) CD_malloc(sizeof(SVPacketTimeUpdate));

					packet->response.time = SV_BufferRemoveLong(input);

					return (CDPointer) packet;
				}
			
				case SVEntityEquipment: {
					SVPacketEntityEquipment* packet = (SVPacketEntityEquipment*) CD_malloc(sizeof(SVPacketEntityEquipment));

					SV_BufferRemoveFormat(input, "isss",
						&packet->response.entity,
						&packet->response.slot,
						&packet->response.item,
						&packet->response.damage
					);

					return (CDPointer) packet;
				}
			
				case SVSpawnPosition: {
					SVPacketSpawnPosition* packet = (SVPacketSpawnPosition*) CD_malloc(sizeof(SVPacketSpawnPosition));

					SVInteger y;
					SV_BufferRemoveFormat(input, "iii",
						&packet->response.position.x,
						&y,
						&packet->response.position.z
					);
					packet->response.position.y = y;

					return (CDPointer) packet;
				}
			
				case SVUpdateHealth: {
					SVPacketUpdateHealth* packet = (SVPacketUpdateHealth*) CD_malloc(sizeof(SVPacketUpdateHealth));

					SV_BufferRemoveFormat(input, "ssf",
						&packet->response.health,
						&packet->response.food,
						&packet->response.foodSaturation
					);

					return (CDPointer) packet;
				}
			
				case SVRespawn: {
					SVPacketRespawn* packet = (SVPacketRespawn*) CD_malloc(sizeof(SVPacketRespawn));

					SV_BufferRemoveFormat(input, "bbbsl",
						&packet->response.world,
						&packet->response.u1,
						&packet->response.mode,
						&packet->response.worldHeight,
						&packet->response.mapSeed
					);
					return (CDPointer) packet;
		        }
	
				case SVPlayerMoveLook: {
					SVPacketPlayerMoveLook* packet = (SVPacketPlayerMoveLook*) CD_malloc(sizeof(SVPacketPlayerMoveLook));

					SV_BufferRemoveFormat(input, "ddddffb",
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.stance,
						&packet->response.position.z,
						&packet->response.yaw,
						&packet->response.pitch,
						&packet->response.is.onGround
					);

					return (CDPointer) packet;
				}
			
				case SVUseBed: {
					SVPacketUseBed* packet = (SVPacketUseBed*) CD_malloc(sizeof(SVPacketUseBed));

					SV_BufferRemoveFormat(input, "ibibi",
						&packet->response.entity.id,
						&packet->response.inBed,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z
					);

					return (CDPointer) packet;
				}
			
				case SVAnimation: {
					SVPacketAnimation* packet = (SVPacketAnimation*) CD_malloc(sizeof(SVPacketAnimation));

					SV_BufferRemoveFormat(input, "ib",
						&packet->response.entity.id,
						&packet->response.type
					);

					return (CDPointer) packet;
				}
			
				case SVNamedEntitySpawn: {
					SVPacketNamedEntitySpawn* packet = (SVPacketNamedEntitySpawn*) CD_malloc(sizeof(SVPacketNamedEntitySpawn));

					SV_BufferRemoveFormat(input, "iUiiibbs",
						&packet->response.entity.id,
						&packet->response.name,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.rotation,
						&packet->response.pitch,
						&packet->response.itemId
					);

					return (CDPointer) packet;
				}
			
				case SVPickupSpawn: {
					SVPacketPickupSpawn* packet = (SVPacketPickupSpawn*) CD_malloc(sizeof(SVPacketPickupSpawn));

					SV_BufferRemoveFormat(input, "isbsiiibbb",
						&packet->response.entity.id,
						&packet->response.item.id,
						&packet->response.item.count,
						&packet->response.item.damage,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.rotation,
						&packet->response.pitch,
						&packet->response.roll
					);

					return (CDPointer) packet;
				}
			
				case SVCollectItem: {
					SVPacketCollectItem* packet = (SVPacketCollectItem*) CD_malloc(sizeof(SVPacketCollectItem));

					SV_BufferRemoveFormat(input, "ii",
						&packet->response.collected,
						&packet->response.collector
					);

					return (CDPointer) packet;
				}
			
				case SVSpawnObject: {
					SVPacketSpawnObject* packet = (SVPacketSpawnObject*) CD_malloc(sizeof(SVPacketSpawnObject));

					SV_BufferRemoveFormat(input, "ibiiii",
						&packet->response.entity.id,
						&packet->response.type,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.flag
					);
					
					if (packet->response.flag > 0) {
						SV_BufferRemoveFormat(input, "sss",
							&packet->response.u1,
							&packet->response.u2,
							&packet->response.u3
						);
					}

					return (CDPointer) packet;
				}
			
				case SVSpawnMob: {
					SVPacketSpawnMob* packet = (SVPacketSpawnMob*) CD_malloc(sizeof(SVPacketSpawnMob));

					SV_BufferRemoveFormat(input, "ibiiibbM",
						&packet->response.id,
						&packet->response.type,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.yaw,
						&packet->response.pitch,
						&packet->response.metadata
					);

					return (CDPointer) packet;
				}
			
				case SVPainting: {
					SVPacketPainting* packet = (SVPacketPainting*) CD_malloc(sizeof(SVPacketPainting));
				
					SVInteger y;
					SV_BufferRemoveFormat(input, "iUiiii",
						&packet->response.entity.id,
						&packet->response.title,
						&packet->response.position.x,
						&y,
						&packet->response.position.z,
						&packet->response.direction
					);
					packet->response.position.y = y;
				
					return (CDPointer) packet;
				}
			
				case SVExperienceOrb: {
					SVPacketExperienceOrb* packet = (SVPacketExperienceOrb*) CD_malloc(sizeof(SVPacketExperienceOrb));

					SVInteger y;
					SV_BufferRemoveFormat(input, "iiiis",
						&packet->response.entity.id,
						&packet->response.position.x,
						&y,
						&packet->response.position.z,
						&packet->response.count
					);
					packet->response.position.y = y;

					return (CDPointer) packet;
				}
			
				case SVStanceUpdate: { //This is most likely a packet that isn't used, but it might be in the future
					SVPacketStanceUpdate* packet = (SVPacketStanceUpdate*) CD_malloc(sizeof(SVPacketStanceUpdate));

					SV_BufferRemoveFormat(input, "ffffBB",
						&packet->response.u1,
						&packet->response.u2,
						&packet->response.u3,
						&packet->response.u4,
						&packet->response.u5,
						&packet->response.u6
					);

					return (CDPointer) packet;
				}
			
				case SVEntityVelocity: {
					SVPacketEntityVelocity* packet = (SVPacketEntityVelocity*) CD_malloc(sizeof(SVPacketEntityVelocity));

					SV_BufferRemoveFormat(input, "isss",
						&packet->response.entity.id,
						&packet->response.velocity.x,
						&packet->response.velocity.y,
						&packet->response.velocity.z
					);

					return (CDPointer) packet;
				}
			
				case SVEntityDestroy: {
					SVPacketEntityDestroy* packet = (SVPacketEntityDestroy*) CD_malloc(sizeof(SVPacketEntityDestroy));

					packet->response.entity.id = SV_BufferRemoveInteger(input);

					return (CDPointer) packet;
				}
			
				case SVEntityCreate: {
					SVPacketEntityCreate* packet = (SVPacketEntityCreate*) CD_malloc(sizeof(SVPacketEntityCreate));
				
					packet->response.entity.id = SV_BufferRemoveInteger(input);
				
					return (CDPointer) packet;
				}
			
				case SVEntityRelativeMove: {
					SVPacketEntityRelativeMove* packet = (SVPacketEntityRelativeMove*) CD_malloc(sizeof(SVPacketEntityRelativeMove));

					SV_BufferRemoveFormat(input, "ibbb",
						&packet->response.entity.id,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z
					);

					return (CDPointer) packet;
				}
			
				case SVEntityLook: {
					SVPacketEntityLook* packet = (SVPacketEntityLook*) CD_malloc(sizeof(SVPacketEntityLook));

					SV_BufferRemoveFormat(input, "ibb",
						&packet->response.entity.id,
						&packet->response.yaw,
						&packet->response.pitch
					);

					return (CDPointer) packet;
				}
			
				case SVEntityLookMove: {
					SVPacketEntityLookMove* packet = (SVPacketEntityLookMove*) CD_malloc(sizeof(SVPacketEntityLookMove));

					SV_BufferRemoveFormat(input, "ibbbbb",
						&packet->response.entity.id,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.yaw,
						&packet->response.pitch
					);

					return (CDPointer) packet;
				}
			
				case SVEntityTeleport: {
					SVPacketEntityTeleport* packet = (SVPacketEntityTeleport*) CD_malloc(sizeof(SVPacketEntityTeleport));

					SV_BufferRemoveFormat(input, "iiiibb",
						&packet->response.entity.id,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.rotation,
						&packet->response.pitch
					);

					return (CDPointer) packet;
				}
			
				case SVEntityStatus: {
					SVPacketEntityStatus* packet = (SVPacketEntityStatus*) CD_malloc(sizeof(SVPacketEntityStatus));

					SV_BufferRemoveFormat(input, "ib",
						&packet->response.entity.id,
						&packet->response.status
					);

					return (CDPointer) packet;
				}
			
				case SVEntityAttach: {
					SVPacketEntityAttach* packet = (SVPacketEntityAttach*) CD_malloc(sizeof(SVPacketEntityAttach));

					SV_BufferRemoveFormat(input, "ii",
						&packet->response.entity.id,
						&packet->response.vehicle.id
					);

					return (CDPointer) packet;
				}
			
				case SVEntityMetadata: {
					SVPacketEntityMetadata* packet = (SVPacketEntityMetadata*) CD_malloc(sizeof(SVPacketEntityMetadata));

					SV_BufferRemoveFormat(input, "iM",
						&packet->response.entity.id,
						&packet->response.metadata
					);

					return (CDPointer) packet;
				}
			
				case SVEntityEffect: {
					SVPacketEntityEffect* packet = (SVPacketEntityEffect*) CD_malloc(sizeof(SVPacketEntityEffect));

					SV_BufferRemoveFormat(input, "ibbs",
						&packet->response.entity.id,
						&packet->response.effect,
						&packet->response.amplifier,
						&packet->response.duration
					);

					return (CDPointer) packet;
				}
			
				case SVRemoveEntityEffect: {
					SVPacketRemoveEntityEffect* packet = (SVPacketRemoveEntityEffect*) CD_malloc(sizeof(SVPacketRemoveEntityEffect));

					SV_BufferRemoveFormat(input, "ib",
						&packet->response.entity.id,
						&packet->response.effect
					);

					return (CDPointer) packet;
				}
			
				case SVExperience: {
					SVPacketExperience* packet = (SVPacketExperience*) CD_malloc(sizeof(SVPacketExperience));

					SV_BufferRemoveFormat(input, "bbs",
						&packet->response.currentExperience,
						&packet->response.level,
						&packet->response.totalExperience
					);

					return (CDPointer) packet;
				}
			
				case SVPreChunk: {
					SVPacketPreChunk* packet = (SVPacketPreChunk*) CD_malloc(sizeof(SVPacketPreChunk));

					SV_BufferRemoveFormat(input, "iib",
						&packet->response.position.x,
						&packet->response.position.z,
						&packet->response.mode
					);

					return (CDPointer) packet;
				}

				case SVMapChunk: {
					SVPacketMapChunk* packet = (SVPacketMapChunk*) CD_malloc(sizeof(SVPacketMapChunk));

					SVShort y;
					SV_BufferRemoveFormat(input, "isibbbi",
						&packet->response.position.x,
						&y,
						&packet->response.position.z,
						&packet->response.size.x,
						&packet->response.size.y,
						&packet->response.size.z,
						&packet->response.length
					);
					packet->response.size.x++;
					packet->response.size.y++;
					packet->response.size.z++;
					packet->response.position.y = y;

					packet->response.item = (SVByte*)CD_BufferRemove(input, packet->response.length * SVByteSize);

					return (CDPointer) packet;
				}

				case SVMultiBlockChange: {
					SVPacketMultiBlockChange* packet = (SVPacketMultiBlockChange*) CD_malloc(sizeof(SVPacketMultiBlockChange));

					SV_BufferRemoveFormat(input, "iis",
						&packet->response.position.x,
						&packet->response.position.z,
						&packet->response.length
					);

					packet->response.coordinate = (SVShort*)CD_BufferRemove(input, packet->response.length * SVShortSize);
					packet->response.type = (SVByte*)CD_BufferRemove(input, packet->response.length * SVByteSize);
					packet->response.metadata = (SVByte*)CD_BufferRemove(input, packet->response.length * SVByteSize);

					return (CDPointer) packet;
				}

				case SVBlockChange: {
					SVPacketBlockChange* packet = (SVPacketBlockChange*) CD_malloc(sizeof(SVPacketBlockChange));

					SV_BufferRemoveFormat(input, "ibibb",
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.type,
						&packet->response.metadata
					);

					return (CDPointer) packet;
				}
			
				case SVPlayNoteBlock: {
					SVPacketPlayNoteBlock* packet = (SVPacketPlayNoteBlock*) CD_malloc(sizeof(SVPacketPlayNoteBlock));
				
					SVShort y;
					SV_BufferRemoveFormat(input, "isibb",
						&packet->response.position.x,
						&y,
						&packet->response.position.z,
						&packet->response.data1,
						&packet->response.data2
					);
					packet->response.position.y = y;

					return (CDPointer) packet;
				}
			
				case SVExplosion: {
					SVPacketExplosion* packet = (SVPacketExplosion*) CD_malloc(sizeof(SVPacketExplosion));

					SV_BufferRemoveFormat(input, "dddfi",
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.radius,
						&packet->response.length
					);

					packet->response.item = (SVRelativePosition*)CD_BufferRemove(input, packet->response.length * sizeof(SVRelativePosition));

					return (CDPointer) packet;
				}
			
				case SVSoundEffect: {
					SVPacketSoundEffect* packet = (SVPacketSoundEffect*) CD_malloc(sizeof(SVPacketSoundEffect));

					SV_BufferRemoveFormat(input, "iibii",
						&packet->response.effect,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,
						&packet->response.data
					);

					return (CDPointer) packet;
				}
			
				case SVState: {
					SVPacketState* packet = (SVPacketState*) CD_malloc(sizeof(SVPacketState));

					SV_BufferRemoveFormat(input, "bb",
						&packet->response.reason,
						&packet->response.gameMode
					);

					return (CDPointer) packet;
				}
			
				case SVThunderbolt: {
					SVPacketThunderbolt* packet = (SVPacketThunderbolt*) CD_malloc(sizeof(SVPacketThunderbolt));

					SV_BufferRemoveFormat(input, "iBiii",
						&packet->response.entity,
						&packet->response.u1,
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z
					);

					return (CDPointer) packet;
				}
			
				case SVOpenWindow: {
					SVPacketOpenWindow* packet = (SVPacketOpenWindow*) CD_malloc(sizeof(SVPacketOpenWindow));

					SV_BufferRemoveFormat(input, "bbUb",
						&packet->response.id,
						&packet->response.type,
						&packet->response.title,
						&packet->response.slots
					);

					return (CDPointer) packet;
				}
			
				case SVCloseWindow: {
					SVPacketCloseWindow* packet = (SVPacketCloseWindow*) CD_malloc(sizeof(SVPacketCloseWindow));

					packet->response.id = SV_BufferRemoveByte(input);

					return (CDPointer) packet;
				}
			
				case SVSetSlot: {
					SVPacketSetSlot* packet = (SVPacketSetSlot*) CD_malloc(sizeof(SVPacketSetSlot));

					SV_BufferRemoveFormat(input, "bss",
						&packet->response.windowId,
						&packet->response.item.slot,
						&packet->response.item.id
					);
					if (packet->response.item.id != -1) {
						packet->response.item.count = SV_BufferRemoveByte(input);
						packet->response.item.damage = SV_BufferRemoveShort(input);
					}

					return (CDPointer) packet;
				}

				case SVWindowItems: {
					SVPacketWindowItems* packet = (SVPacketWindowItems*) CD_malloc(sizeof(SVPacketWindowItems));

					SV_BufferRemoveFormat(input, "bs",
						&packet->response.id,
						&packet->response.length
					);

					packet->response.item = (SVItemStack*) CD_malloc(sizeof(SVItemStack) * packet->response.length);

					int i;
					for (i=0; i<packet->response.length; i++) {
						SVShort itemId = SV_BufferRemoveShort(input);
						if (itemId == -1) continue;
						packet->response.item[i].id = itemId;
						packet->response.item[i].count = SV_BufferRemoveByte(input);
						packet->response.item[i].damage = SV_BufferRemoveShort(input);
					}

					return (CDPointer) packet;
				}
			
				case SVUpdateProgressBar: {
					SVPacketUpdateProgressBar* packet = (SVPacketUpdateProgressBar*) CD_malloc(sizeof(SVPacketUpdateProgressBar));
				
					SV_BufferRemoveFormat(input, "bss",
						&packet->response.id,
						&packet->response.bar,
						&packet->response.value
					);
				}
			
				case SVTransaction: {
					SVPacketTransaction* packet = (SVPacketTransaction*) CD_malloc(sizeof(SVPacketTransaction));

					SV_BufferRemoveFormat(input, "bsB",
						&packet->response.id,
						&packet->response.action,
						&packet->response.accepted
					);

					return (CDPointer) packet;
				}
			
				case SVCreativeInventoryAction: {
					SVPacketCreativeInventoryAction* packet = (SVPacketCreativeInventoryAction*) CD_malloc(sizeof(SVPacketCreativeInventoryAction));

					SV_BufferRemoveFormat(input, "ssss",
						&packet->response.slot,
						&packet->response.itemId,
						&packet->response.quantity,
						&packet->response.damage
					);

					return (CDPointer) packet;
				}

				case SVUpdateSign: {
					SVPacketUpdateSign* packet = (SVPacketUpdateSign*) CD_malloc(sizeof(SVPacketUpdateSign));

					SV_BufferRemoveFormat(input, "isiUUUU",
						&packet->response.position.x,
						&packet->response.position.y,
						&packet->response.position.z,

						&packet->response.first,
						&packet->response.second,
						&packet->response.third,
						&packet->response.fourth
					);

					return (CDPointer) packet;
				}
			
				case SVItemData: {
					SVPacketItemData* packet = (SVPacketItemData*) CD_malloc(sizeof(SVPacketItemData));

					SV_BufferRemoveFormat(input, "ssb",
						&packet->response.itemType,
						&packet->response.itemId,
						&packet->response.textLength
					);
				
					packet->response.text = (SVByte*)CD_BufferRemove(input, (uint8_t)packet->response.textLength * SVByteSize);

					return (CDPointer) packet;
				}
			
				case SVPlayerListItem: {
					SVPacketPlayerListItem* packet = (SVPacketPlayerListItem*) CD_malloc(sizeof(SVPacketPlayerListItem));

					SV_BufferRemoveFormat(input, "UBs",
						&packet->response.playerName,
						&packet->response.online,
						&packet->response.ping
					);

					return (CDPointer) packet;
				}
			
				case SVDisconnect: {
					SVPacketDisconnect* packet = (SVPacketDisconnect*) CD_malloc(sizeof(SVPacketDisconnect));

					packet->response.reason = SV_BufferRemoveString16(input);

					return (CDPointer) packet;
				}

				default: {
					return (CDPointer) NULL;
				}
			}
		} break;
	}
	
	return (CDPointer) NULL;
}

CDBuffer*
SV_PacketToBuffer (SVPacket* self)
	{
	CDBuffer* data = CD_CreateBuffer();

	assert(self);

	SV_BufferAddByte(data, self->type);

	switch (self->chain) {
		case SVRequest: {
			switch (self->type) {
				case SVKeepAlive: {
					SVPacketKeepAlive* packet = (SVPacketKeepAlive*) self->data;

					SV_BufferAddInteger(data, packet->keepAliveID);
				} break;

				case SVLogin: {
					SVPacketLogin* packet = (SVPacketLogin*) self->data;

				    SV_BufferAddFormat(data, "iUlibbbb",
						packet->request.version,
						packet->request.username,
						packet->request.u1,
						packet->request.u2,
						packet->request.u3,
						packet->request.u4,
						packet->request.u5,
						packet->request.u6
					);
				} break;

				case SVHandshake: {
					SVPacketHandshake* packet = (SVPacketHandshake*) self->data;

					SV_BufferAddString16(data, packet->request.username);
				} break;

				case SVChat: {
					SVPacketChat* packet = (SVPacketChat*) self->data;

					SV_BufferAddString16(data, packet->request.message);
				} break;

				case SVUseEntity: {
					SVPacketUseEntity* packet = (SVPacketUseEntity*) self->data;

					SV_BufferAddFormat(data, "iib",
						packet->request.user,
						packet->request.target,
						packet->request.leftClick
					);
				} break;

				case SVRespawn: {
					SVPacketRespawn* packet = (SVPacketRespawn*) self->data;

					SV_BufferAddFormat(data, "bbbsl",
						packet->request.world,
						packet->request.u1,
						packet->request.mode,
						packet->request.worldHeight,
						packet->request.mapSeed
					);
				} break;

				case SVOnGround: {
					SVPacketOnGround* packet = (SVPacketOnGround*) self->data;

					SV_BufferAddBoolean(data, packet->request.onGround);
				} break;

				case SVPlayerPosition: {
					SVPacketPlayerPosition* packet = (SVPacketPlayerPosition*) self->data;

					SV_BufferAddFormat(data, "ddddb",
						packet->request.position.x,
						packet->request.position.y,
						packet->request.stance,
						packet->request.position.z,
						packet->request.is.onGround
					);
				} break;

				case SVPlayerLook: {
					SVPacketPlayerLook* packet = (SVPacketPlayerLook*) self->data;

					SV_BufferAddFormat(data, "ffb",
						packet->request.yaw,
						packet->request.pitch,
						packet->request.is.onGround
					);
				} break;

				case SVPlayerMoveLook: {
					SVPacketPlayerMoveLook* packet = (SVPacketPlayerMoveLook*) self->data;

					SV_BufferAddFormat(data, "ddddffb",
						packet->request.position.x,
						packet->request.stance,
						packet->request.position.y,
						packet->request.position.z,
						packet->request.yaw,
						packet->request.pitch,
						packet->request.is.onGround
					);
				} break;

				case SVPlayerDigging: {
					SVPacketPlayerDigging* packet = (SVPacketPlayerDigging*) self->data;

					SV_BufferAddByte(data, packet->request.status);

					SV_BufferAddFormat(data, "ibi",
						packet->request.position.x,
						packet->request.position.y,
						packet->request.position.z
					);

					SV_BufferAddByte(data, packet->request.face);
				} break;

				case SVPlayerBlockPlacement: {
					SVPacketPlayerBlockPlacement* packet = (SVPacketPlayerBlockPlacement*) self->data;

					SV_BufferAddFormat(data, "ibibs",
						packet->request.position.x,
						packet->request.position.y,
						packet->request.position.z,

						packet->request.direction,
						packet->request.item.id
					);

					if (packet->request.item.id != -1) {
						SV_BufferAddFormat(data, "bs",
							packet->request.item.count,
							packet->request.item.damage
						);
					}
				} break;

				case SVHoldChange: {
					SVPacketHoldChange* packet = (SVPacketHoldChange*) self->data;

					SV_BufferAddShort(data, packet->request.slot);
				} break;

				case SVAnimation: {
					SVPacketAnimation* packet = (SVPacketAnimation*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->request.entity.id,
						packet->request.type
					);
				} break;

				case SVEntityAction: {
					SVPacketEntityAction* packet = (SVPacketEntityAction*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->request.entity.id,
						packet->request.type
					);
				} break;

				case SVStanceUpdate: { //This is most likely a packet that isn't used, but it might be in the future
					SVPacketStanceUpdate* packet = (SVPacketStanceUpdate*) self->data;

					SV_BufferAddFormat(data, "ffffBB",
						packet->request.u1,
						packet->request.u2,
						packet->request.u3,
						packet->request.u4,
						packet->request.u5,
						packet->request.u6
					);
				} break;

				case SVEntityMetadata: {
					SVPacketEntityMetadata* packet = (SVPacketEntityMetadata*) self->data;

					SV_BufferAddFormat(data, "iM",
						packet->request.entity.id,
						packet->request.metadata
					);
				} break;

				case SVEntityEffect: {
					SVPacketEntityEffect* packet = (SVPacketEntityEffect*) self->data;

					SV_BufferAddFormat(data, "ibbs",
						packet->request.entity.id,
						packet->request.effect,
						packet->request.amplifier,
						packet->request.duration
					);
				} break;

				case SVRemoveEntityEffect: {
					SVPacketRemoveEntityEffect* packet = (SVPacketRemoveEntityEffect*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->request.entity.id,
						packet->request.effect
					);
				} break;

				case SVCloseWindow: {
					SVPacketCloseWindow* packet = (SVPacketCloseWindow*) self->data;

					SV_BufferAddByte(data, packet->request.id);
				} break;

				case SVWindowClick: {
					SVPacketWindowClick* packet = (SVPacketWindowClick*) self->data;

					SV_BufferAddFormat(data, "bsBsBs",
						packet->request.id,
						packet->request.slot,
						packet->request.rightClick,
						packet->request.action,
						packet->request.shiftPressed,
						packet->request.item.id
					);

					if (packet->request.item.id != -1) {
						SV_BufferAddFormat(data, "bs",
							packet->request.item.count,
							packet->request.item.damage
						);
					}
				} break;

				case SVTransaction: {
					SVPacketTransaction* packet = (SVPacketTransaction*) self->data;

					SV_BufferAddFormat(data, "bsB",
						packet->request.id,
						packet->request.action,
						packet->request.accepted
					);
				} break;

				case SVCreativeInventoryAction: {
					SVPacketCreativeInventoryAction* packet = (SVPacketCreativeInventoryAction*) self->data;

					SV_BufferAddFormat(data, "ssss",
						packet->request.slot,
						packet->request.itemId,
						packet->request.quantity,
						packet->request.damage
					);
				} break;

				case SVUpdateSign: {
					SVPacketUpdateSign* packet = (SVPacketUpdateSign*) self->data;

					SV_BufferAddFormat(data, "isiUUUU",
						packet->request.position.x,
						packet->request.position.y,
						packet->request.position.z,

						packet->request.first,
						packet->request.second,
						packet->request.third,
						packet->request.fourth
					);
				} break;

				case SVListPing: {
					
				} break;

				case SVDisconnect: {
					SVPacketDisconnect* packet = (SVPacketDisconnect*) self->data;

					SV_BufferAddString16(data, packet->request.reason);
				} break;
				
				default: {
					return NULL;
				};
			}
		} break;
		
		case SVPing: {
			switch(self->type) {
				case SVDisconnect:
				{
					SVPacketDisconnect* packet = (SVPacketDisconnect*) self->data;

					SV_BufferAddString16(data, packet->ping.description);
				} break;
				
				default: {
					return NULL;
				};
			}
		} break;

		case SVResponse: {
			switch (self->type) {
				case SVKeepAlive: {
					SVPacketKeepAlive* packet = (SVPacketKeepAlive*) self->data;

					SV_BufferAddInteger(data, packet->keepAliveID);
				} break;

				case SVLogin: {
					SVPacketLogin* packet = (SVPacketLogin*) self->data;

					SV_BufferAddFormat(data, "iUlibbbb",
						packet->response.id,
						packet->response.u1,
						packet->response.mapSeed,
						packet->response.serverMode,
						packet->response.dimension,
						packet->response.u2,
						packet->response.worldHeight,
						packet->response.maxPlayers
					);
				} break;

				case SVHandshake: {
					SVPacketHandshake* packet = (SVPacketHandshake*) self->data;

					SV_BufferAddString16(data, packet->response.hash);
				} break;

				case SVChat: {
					SVPacketChat* packet = (SVPacketChat*) self->data;

					SV_BufferAddString16(data, packet->response.message);
				} break;

				case SVTimeUpdate: {
					SVPacketTimeUpdate* packet = (SVPacketTimeUpdate*) self->data;

					SV_BufferAddLong(data, packet->response.time);
				} break;

				case SVEntityEquipment: {
					SVPacketEntityEquipment* packet = (SVPacketEntityEquipment*) self->data;

					SV_BufferAddFormat(data, "isss",
						packet->response.entity.id,
						packet->response.slot,
						packet->response.item,
						packet->response.damage
					);
				} break;

				case SVSpawnPosition: {
					SVPacketSpawnPosition* packet = (SVPacketSpawnPosition*) self->data;

					SV_BufferAddFormat(data, "iii",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z
					);
				} break;

				case SVUpdateHealth: {
					SVPacketUpdateHealth* packet = (SVPacketUpdateHealth*) self->data;

					SV_BufferAddFormat(data, "ssf",
						packet->response.health,
						packet->response.food,
						packet->response.foodSaturation
					);
				} break;
				
				case SVRespawn: {
					SVPacketRespawn* packet = (SVPacketRespawn*) self->data;
					
					SV_BufferAddFormat(data, "bbbsl",
						packet->response.world,
						packet->response.u1,
						packet->response.mode,
						packet->response.worldHeight,
						packet->response.mapSeed
					);
				} break;

				case SVPlayerMoveLook: {
					SVPacketPlayerMoveLook* packet = (SVPacketPlayerMoveLook*) self->data;

					SV_BufferAddFormat(data, "ddddffB",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.stance,
						packet->response.position.z,
						packet->response.yaw,
						packet->response.pitch,
						packet->response.is.onGround
					);
				} break;

				case SVUseBed: {
					SVPacketUseBed* packet = (SVPacketUseBed*) self->data;

					SV_BufferAddFormat(data, "ibibi",
						packet->response.entity.id,
						packet->response.inBed,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z
					);
				} break;

				case SVAnimation: {
					SVPacketAnimation* packet = (SVPacketAnimation*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->response.entity.id,
						packet->response.type
					);
				} break;

				case SVNamedEntitySpawn: {
					SVPacketNamedEntitySpawn* packet = (SVPacketNamedEntitySpawn*) self->data;

					SV_BufferAddFormat(data, "iUiiibbs",
						packet->response.entity.id,
						packet->response.name,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.rotation,
						packet->response.pitch,
						packet->response.itemId
					);
				} break;

				case SVPickupSpawn: {
					SVPacketPickupSpawn* packet = (SVPacketPickupSpawn*) self->data;

					SV_BufferAddFormat(data, "isbsiiibbb",
						packet->response.entity.id,
						packet->response.item.id,
						packet->response.item.count,
						packet->response.item.damage,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.rotation,
						packet->response.pitch,
						packet->response.roll
					);
				} break;

				case SVCollectItem: {
					SVPacketCollectItem* packet = (SVPacketCollectItem*) self->data;

					SV_BufferAddFormat(data, "ii",
						packet->response.collected,
						packet->response.collector
					);
				} break;

				case SVSpawnObject: {
					SVPacketSpawnObject* packet = (SVPacketSpawnObject*) self->data;

					SV_BufferAddFormat(data, "ibiiii",
						packet->response.entity.id,
						packet->response.type,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.flag
					);
					if(packet->response.flag > 0) { //These fields are sent under this condition
						SV_BufferAddShort(data, packet->response.u1);
						SV_BufferAddShort(data, packet->response.u2);
						SV_BufferAddShort(data, packet->response.u3);
					}
				} break;

				case SVSpawnMob: {
					SVPacketSpawnMob* packet = (SVPacketSpawnMob*) self->data;

					SV_BufferAddFormat(data, "ibiiibbM",
						packet->response.id,
						packet->response.type,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.yaw,
						packet->response.pitch,
						packet->response.metadata
					);
				} break;

				case SVPainting: {
					SVPacketPainting* packet = (SVPacketPainting*) self->data;

					SV_BufferAddFormat(data, "iUiiii",
						packet->response.entity.id,
						packet->response.title,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.direction
					);
				} break;

				case SVExperienceOrb: {
					SVPacketExperienceOrb* packet = (SVPacketExperienceOrb*) self->data;

					SV_BufferAddFormat(data, "iiiis",
						packet->response.entity.id,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.count
					);
				} break;

				case SVStanceUpdate: {
					SVPacketStanceUpdate* packet = (SVPacketStanceUpdate*) self->data;

					SV_BufferAddFormat(data, "ffffBB",
						packet->response.u1,
						packet->response.u2,
						packet->response.u3,
						packet->response.u4,
						packet->response.u5,
						packet->response.u6
					);
				} break;

				case SVEntityVelocity: {
					SVPacketEntityVelocity* packet = (SVPacketEntityVelocity*) self->data;

					SV_BufferAddFormat(data, "isss",
						packet->response.entity.id,
						packet->response.velocity.x,
						packet->response.velocity.y,
						packet->response.velocity.z
					);
				} break;

				case SVEntityDestroy: {
					SVPacketEntityDestroy* packet = (SVPacketEntityDestroy*) self->data;

					SV_BufferAddInteger(data, packet->response.entity.id);
				} break;

				case SVEntityCreate: {
					SVPacketEntityCreate* packet = (SVPacketEntityCreate*) self->data;

					SV_BufferAddInteger(data, packet->response.entity.id);
				} break;

				case SVEntityRelativeMove: {
					SVPacketEntityRelativeMove* packet = (SVPacketEntityRelativeMove*) self->data;

					SV_BufferAddFormat(data, "ibbb",
						packet->response.entity.id,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z
					);
				} break;

				case SVEntityLook: {
					SVPacketEntityLook* packet = (SVPacketEntityLook*) self->data;

					SV_BufferAddFormat(data, "ibb",
						packet->response.entity.id,
						packet->response.yaw,
						packet->response.pitch
					);
				} break;

				case SVEntityLookMove: {
					SVPacketEntityLookMove* packet = (SVPacketEntityLookMove*) self->data;

					SV_BufferAddFormat(data, "ibbbbb",
						packet->response.entity.id,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.yaw,
						packet->response.pitch
					);
				} break;

				case SVEntityTeleport: {
					SVPacketEntityTeleport* packet = (SVPacketEntityTeleport*) self->data;

					SV_BufferAddFormat(data, "iiiibb",
						packet->response.entity.id,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.rotation,
						packet->response.pitch
					);
				} break;

				case SVEntityStatus: {
					SVPacketEntityStatus* packet = (SVPacketEntityStatus*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->response.entity.id,
						packet->response.status
					);
				} break;

				case SVEntityAttach: {
					SVPacketEntityAttach* packet = (SVPacketEntityAttach*) self->data;

					SV_BufferAddFormat(data, "ii",
						packet->response.entity.id,
						packet->response.vehicle.id
					);
				} break;

				case SVEntityMetadata: {
					SVPacketEntityMetadata* packet = (SVPacketEntityMetadata*) self->data;

					SV_BufferAddFormat(data, "iM",
						packet->response.entity.id,
						packet->response.metadata
					);
				} break;

				case SVEntityEffect: {
					SVPacketEntityEffect* packet = (SVPacketEntityEffect*) self->data;

					SV_BufferAddFormat(data, "ibbs",
						packet->response.entity.id,
						packet->response.effect,
						packet->response.amplifier,
						packet->response.duration
					);
				} break;

				case SVRemoveEntityEffect: {
					SVPacketRemoveEntityEffect* packet = (SVPacketRemoveEntityEffect*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->response.entity.id,
						packet->response.effect
					);
				} break;

				case SVExperience: {
					SVPacketExperience* packet = (SVPacketExperience*) self->data;

					SV_BufferAddFormat(data, "bbs",
						packet->response.currentExperience,
						packet->response.level,
						packet->response.totalExperience
					);
				} break;

				case SVPreChunk: {
					SVPacketPreChunk* packet = (SVPacketPreChunk*) self->data;

					SV_BufferAddFormat(data, "iiB",
						packet->response.position.x,
						packet->response.position.z,
						packet->response.mode
					);
				} break;

				case SVMapChunk: {
					SVPacketMapChunk* packet = (SVPacketMapChunk*) self->data;

					SV_BufferAddFormat(data, "isibbb",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,

						packet->response.size.x - 1,
						packet->response.size.y - 1,
						packet->response.size.z - 1
					);

					SV_BufferAddInteger(data, packet->response.length);

					CD_BufferAdd(data, (CDPointer) packet->response.item, packet->response.length * SVByteSize);
				} break;

				case SVMultiBlockChange: {
					SVPacketMultiBlockChange* packet = (SVPacketMultiBlockChange*) self->data;

					SV_BufferAddFormat(data, "ii",
						packet->response.position.x,
						packet->response.position.z
					);

					SV_BufferAddShort(data, packet->response.length);

					CD_BufferAdd(data, (CDPointer) packet->response.coordinate, packet->response.length * SVShortSize);
					CD_BufferAdd(data, (CDPointer) packet->response.type,       packet->response.length * SVByteSize);
					CD_BufferAdd(data, (CDPointer) packet->response.metadata,   packet->response.length * SVByteSize);
				} break;

				case SVBlockChange: {
					SVPacketBlockChange* packet = (SVPacketBlockChange*) self->data;

					SV_BufferAddFormat(data, "ibibb",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,

						packet->response.type,
						packet->response.metadata
					);
				} break;

				case SVPlayNoteBlock: { //TODO: Rename to SVBlockAction
					SVPacketPlayNoteBlock* packet = (SVPacketPlayNoteBlock*) self->data;

					SV_BufferAddFormat(data, "isibb",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,

						packet->response.data1,
						packet->response.data2
					);
				} break;

				case SVExplosion: {
					SVPacketExplosion* packet = (SVPacketExplosion*) self->data;

					SV_BufferAddFormat(data, "dddf",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,

						packet->response.radius
					);

					SV_BufferAddInteger(data, packet->response.length);

					CD_BufferAdd(data, (CDPointer) packet->response.item, packet->response.length * 3 * SVByteSize);
				} break;

				case SVSoundEffect: {
					SVPacketSoundEffect* packet = (SVPacketSoundEffect*) self->data;

					SV_BufferAddFormat(data, "iibii",
						packet->response.effect,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,
						packet->response.data
					);
				} break;

				case SVState: {
					SVPacketState* packet = (SVPacketState*) self->data;

					SV_BufferAddFormat(data, "bb",
						packet->response.reason,
						packet->response.gameMode
					);
				} break;

				case SVThunderbolt: {
					SVPacketThunderbolt* packet = (SVPacketThunderbolt*) self->data;

					SV_BufferAddFormat(data, "iBiii",
						packet->response.entity.id,
						packet->response.u1,
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z
					);
				}

				case SVOpenWindow: {
					SVPacketOpenWindow* packet = (SVPacketOpenWindow*) self->data;

					SV_BufferAddFormat(data, "bbSb",
						packet->response.id,
						packet->response.type,
						packet->response.title,
						packet->response.slots
					);
				} break;

				case SVCloseWindow: {
					SVPacketCloseWindow* packet = (SVPacketCloseWindow*) self->data;

					SV_BufferAddByte(data, packet->response.id);
				} break;

				case SVSetSlot: {
					SVPacketSetSlot* packet = (SVPacketSetSlot*) self->data;

					SV_BufferAddFormat(data, "bss",
						packet->response.windowId,
						packet->response.item.slot,
						packet->response.item.id
					);

					if (packet->response.item.id != -1) {
						SV_BufferAddFormat(data, "bs",
							packet->response.item.count,
							packet->response.item.damage
						);
					}
				} break;

				case SVWindowItems: {
					SVPacketWindowItems* packet = (SVPacketWindowItems*) self->data;

					SV_BufferAddByte(data, packet->response.id);
					SV_BufferAddShort(data, packet->response.length);

					for (size_t i = 0; i < packet->response.length; i++) {
						if (packet->response.item[i].id == -1) {
							SV_BufferAddShort(data, -1);
						}
						else {
							SV_BufferAddFormat(data, "sbs",
								packet->response.item[i].id,
								packet->response.item[i].count,
								packet->response.item[i].damage
							);
						}
					}
				} break;

				case SVUpdateProgressBar: {
					SVPacketUpdateProgressBar* packet = (SVPacketUpdateProgressBar*) self->data;

					SV_BufferAddFormat(data, "bss",
						packet->response.id,
						packet->response.bar,
						packet->response.value
					);
				} break;

				case SVTransaction: {
					SVPacketTransaction* packet = (SVPacketTransaction*) self->data;

					SV_BufferAddFormat(data, "bsB",
						packet->response.id,
						packet->response.action,
						packet->response.accepted
					);
				} break;

				case SVCreativeInventoryAction: {
					SVPacketCreativeInventoryAction* packet = (SVPacketCreativeInventoryAction*) self->data;

					SV_BufferAddFormat(data, "ssss",
						packet->response.slot,
						packet->response.itemId,
						packet->response.quantity,
						packet->response.damage
					);
				} break;

				case SVUpdateSign: {
					SVPacketUpdateSign* packet = (SVPacketUpdateSign*) self->data;

					SV_BufferAddFormat(data, "isiUUUU",
						packet->response.position.x,
						packet->response.position.y,
						packet->response.position.z,

						packet->response.first,
						packet->response.second,
						packet->response.third,
						packet->response.fourth
					);
				} break;

				case SVItemData: {
					SVPacketItemData* packet = (SVPacketItemData*) self->data;
					
					SV_BufferAddFormat(data, "isiUUUU",
						packet->response.itemType,
						packet->response.itemId,
						packet->response.textLength
					);
					
					CD_BufferAdd(data, (CDPointer) packet->response.text, packet->response.textLength * SVByteSize);
				} break;
				
				case SVIncrementStatistic: {
					SVPacketIncrementStatistic* packet = (SVPacketIncrementStatistic*) self->data;

					SV_BufferAddFormat(data, "ib",
						packet->response.id,
						packet->response.amount
					);
				} break;

				case SVPlayerListItem: {
					SVPacketPlayerListItem* packet = (SVPacketPlayerListItem*) self->data;

					SV_BufferAddFormat(data, "Ubs",
						packet->response.playerName,
						packet->response.online,
						packet->response.ping
					);
				} break;

				case SVDisconnect: {
					SVPacketDisconnect* packet = (SVPacketDisconnect*) self->data;

					SV_BufferAddString16(data, packet->response.reason);
				} break;

				default: {
					return NULL;
				};
			}
		} break;
	}

	return data;
}
