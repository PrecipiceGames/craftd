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

#ifndef CRAFTD_SURVIVAL_PACKET_H
#define CRAFTD_SURVIVAL_PACKET_H

#include <craftd/protocols/survival/common.h>

#define CRAFTD_PROTOCOL_VERSION (19)

typedef enum _SVPacketChain {
	SVRequest,
	SVResponse,
	SVPing
} SVPacketChain;

typedef enum _SVPacketType {
    SVKeepAlive               = 0x00,
    SVLogin                   = 0x01,
    SVHandshake               = 0x02,
    SVChat                    = 0x03,
    SVTimeUpdate              = 0x04,
    SVEntityEquipment         = 0x05,
    SVSpawnPosition           = 0x06,
    SVUseEntity               = 0x07,
    SVUpdateHealth            = 0x08,
    SVRespawn                 = 0x09,
    SVOnGround                = 0x0A,
    SVPlayerPosition          = 0x0B,
    SVPlayerLook              = 0x0C,
    SVPlayerMoveLook          = 0x0D,
    SVPlayerDigging           = 0x0E,
    SVPlayerBlockPlacement    = 0x0F,
    SVHoldChange              = 0x10,
    SVUseBed                  = 0x11,
    SVAnimation               = 0x12,
    SVEntityAction            = 0x13,
    SVNamedEntitySpawn        = 0x14,
    SVPickupSpawn             = 0x15,
    SVCollectItem             = 0x16,
    SVSpawnObject             = 0x17,
    SVSpawnMob                = 0x18,
    SVPainting                = 0x19,
    SVExperienceOrb           = 0x1A,
    SVStanceUpdate            = 0x1B,
    SVEntityVelocity          = 0x1C,
    SVEntityDestroy           = 0x1D,
    SVEntityCreate            = 0x1E,
    SVEntityRelativeMove      = 0x1F,
    SVEntityLook              = 0x20,
    SVEntityLookMove          = 0x21,
    SVEntityTeleport          = 0x22,
    SVEntityStatus            = 0x26,
    SVEntityAttach            = 0x27,
    SVEntityMetadata          = 0x28,
    SVEntityEffect            = 0x29,
    SVRemoveEntityEffect      = 0x2A,
    SVExperience              = 0x2B,
    SVPreChunk                = 0x32,
    SVMapChunk                = 0x33,
    SVMultiBlockChange        = 0x34,
    SVBlockChange             = 0x35,
    SVPlayNoteBlock           = 0x36, //TODO: Needs to be changed to SVBlockAction
    SVExplosion               = 0x3C,
    SVSoundEffect             = 0x3D,
    SVState                   = 0x46,
    SVThunderbolt             = 0x47,
    SVOpenWindow              = 0x64,
    SVCloseWindow             = 0x65,
    SVWindowClick             = 0x66,
    SVSetSlot                 = 0x67,
    SVWindowItems             = 0x68,
    SVUpdateProgressBar       = 0x69,
    SVTransaction             = 0x6A,
    SVCreativeInventoryAction = 0x6B,
    SVUpdateSign              = 0x82,
    SVItemData                = 0x83,
    SVIncrementStatistic      = 0xC8,
    SVPlayerListItem          = 0xC9,
    SVListPing                = 0xFE,
    SVDisconnect              = 0xFF
} SVPacketType;

/*
 * Commonly used enums
 */

typedef enum _SVAnimationType {
    SVNoAnimation = 0,
    SVSwingArmAnimation = 1,
    SVDamageAnimation = 2,
    SVLeaveBedAnimation = 3,
    SVUnknownAnimation = 102,
    SVCrouchAnimation = 104,
    SVUncrouchAnimation = 105
} SVAnimationType;

typedef enum _SVActionType {
	SVCrouchAction = 1,
	SVUncrouchAction = 2,
	SVLeaveBedAction = 3,
	SVStartSprintAction = 4,
	SVStopSprintAction = 5
} SVActionType;

typedef enum _SVBlockFace {
	SVFaceNegativeY = 0,
	SVFacePositiveY = 1,
	SVFaceNegativeZ = 2,
	SVFacePositiveZ = 3,
	SVFaceNegativeX = 4,
	SVFacePositiveX = 5
} SVBlockFace;

typedef enum _SVEntityEffect {
	SVIncreaseSpeed = 1,
	SVDecreaseSpeed = 2,
	SVIncreaseDigSpeed = 3,
	SVDecreaseDigSpeed = 4,
	SVDamageBoost = 5,
	SVHeal = 6,
	SVHarm = 7,
	SVJump = 8,
	SVConfusion = 9,
	SVRegeneration = 10,
	SVResistance = 11,
	SVFireResistance = 12,
	SVWaterBreathing = 13,
	SVInvisibility = 14,
	SVBlindness = 15,
	SVNightVision = 16,
	SVFoodPoisoning = 17,
	SVWeakness = 18,
	SVPoisoned = 19
} SVEffect;

typedef enum _SVDiggingStatus {
        SVStartedDigging = 0,
        SVDigging,
        SVStoppedDigging = 2,
        SVBlockBroken,
        SVDropItem = 4,
        SVArrowShot = 5
} SVDiggingStatus;

/*
 * Packet stuff
 */

typedef struct _SVPacket {
	SVPacketChain chain;
	SVPacketType  type;
	CDPointer     data;
} SVPacket;

typedef struct _SVPacketKeepAlive {
	SVInteger keepAliveID;
} SVPacketKeepAlive;

typedef union _SVPacketLogin {
	struct {
		SVInteger version;

		SVString username;
	SVLong u1;
	SVInteger u2;
	SVByte u3;
	SVByte u4;
	SVByte u5;
	SVByte u6;
	} request;

	struct {
		SVInteger id;
		SVString u1;
		SVLong mapSeed;
		SVInteger serverMode;
		SVByte dimension;
		SVByte u2;
		SVUByte worldHeight;
		SVUByte maxPlayers;
	} response;
} SVPacketLogin;

typedef union _SVPacketHandshake {
	struct {
		SVString username;
	} request;

	struct {
		SVString hash;
	} response;
} SVPacketHandshake;

typedef union _SVPacketChat {
	struct {
		SVString message;
	} request;

	struct {
		SVString message;
	} response;
} SVPacketChat;

typedef union _SVPacketTimeUpdate {
	struct {
		SVLong time;
	} response;
} SVPacketTimeUpdate;

typedef union _SVPacketEntityEquipment {
	struct {
		SVEntity entity;

		SVShort   slot;
		SVShort   item;
		SVShort   damage; // Still not sure about it
	} response;
} SVPacketEntityEquipment;

typedef union _SVPacketSpawnPosition {
	struct {
		SVBlockPosition position;
	} response;
} SVPacketSpawnPosition;

typedef union _SVPacketUseEntity {
	struct {
		SVInteger user;
		SVInteger target;
		SVBoolean leftClick;
	} request;
} SVPacketUseEntity;

typedef union _SVPacketUpdateHealth {
    struct {
        SVShort health;
        SVShort food;
        SVFloat foodSaturation;
    } response;
} SVPacketUpdateHealth;

typedef union _SVPacketRespawn {
    struct { //Not entirely sure what is recieved, may be all 0's
    	SVByte world;
    	SVByte u1;
    	SVByte mode;
    	SVShort worldHeight;
    	SVLong mapSeed;
    } request;

    struct {
    	SVByte world;
    	SVByte u1;
    	SVByte mode;
    	SVShort worldHeight;
    	SVLong mapSeed;
    } response;
} SVPacketRespawn;

typedef union _SVPacketOnGround {
	struct {
		SVBoolean onGround;
	} request;
} SVPacketOnGround;

typedef union _SVPacketPlayerPosition {
	struct {
		SVPrecisePosition position;

		SVDouble stance;

		struct {
			SVBoolean onGround;
		} is;
	} request;
} SVPacketPlayerPosition;

typedef union _SVPacketPlayerLook {
	struct {
		SVFloat yaw;
		SVFloat pitch;

		struct {
			SVBoolean onGround;
		} is;
	} request;
} SVPacketPlayerLook;

typedef union _SVPacketPlayerMoveLook {
	struct {
		SVPrecisePosition position;

		SVDouble stance;
		SVFloat yaw;
		SVFloat pitch;

		struct {
			SVBoolean onGround;
		} is;
	} request;

	struct {
		SVPrecisePosition position;

		SVDouble stance;
		SVFloat yaw;
		SVFloat pitch;

		struct {
			SVBoolean onGround;
		} is;
	} response;
} SVPacketPlayerMoveLook;

typedef union _SVPacketPlayerDigging {
    struct {
        SVDiggingStatus status;
        
        SVBlockPosition position;

        SVBlockFace face;
    } request;
} SVPacketPlayerDigging;

typedef union _SVPacketPlayerBlockPlacement {
    struct {
        SVBlockPosition position;

        SVBlockFace direction;
        SVItemStack item;
   } request;
} SVPacketPlayerBlockPlacement;

typedef union _SVPacketHoldChange {
	struct {
		SVShort slot;
	} request;
} SVPacketHoldChange;

typedef union _SVPacketUseBed {
	struct {
		SVEntity entity;

		SVByte inBed;

		SVBlockPosition position;
	} response;
} SVPacketUseBed;

typedef union _SVPacketAnimation {
	struct {
		SVEntity entity;

		SVAnimationType type;
	} request;

	struct {
		SVEntity entity;

		SVAnimationType type;
	} response;
} SVPacketAnimation;

typedef union _SVPacketEntityAction {
    struct {
        SVEntity entity;

        SVActionType type;
    } request;
} SVPacketEntityAction;

typedef union _SVPacketNamedEntitySpawn {
	struct {
		SVEntity entity;
		SVString name;

		SVAbsolutePosition position;

		SVByte rotation;
		SVByte pitch;

		SVShort itemId;
	} response;
} SVPacketNamedEntitySpawn;

typedef union _SVPacketPickupSpawn {
	struct {
		SVEntity           entity;
		SVItemStack        item;
		SVAbsolutePosition position;

		SVByte rotation;
		SVByte pitch;
		SVByte roll;
	} response;
} SVPacketPickupSpawn;

typedef union _SVPacketCollectItem {
	struct {
		SVInteger collected;
		SVInteger collector;
	} response;
} SVPacketCollectItem;

typedef union _SVPacketSpawnObject {
	struct {
		SVEntity entity;

		enum {
			SVBoat = 1,
			SVMinecart = 10,
			SVStorageCart = 11,
			SVPoweredCart = 12,
			SVActivatedTNT = 50,
			SVArrow = 60,
			SVThrownSnowball = 61,
			SVThrownEgg = 62,

			SVFallingSand = 70,
			SVFallingGravel = 71,
			SVFishingFloat = 90
		} type;


        SVAbsolutePosition position;

        SVInteger flag; //Unknown, if greater then 0, the next three shorts are sent
        SVShort u1;
        SVShort u2;
        SVShort u3;
    } response;
} SVPacketSpawnObject;

typedef union _SVPacketSpawnMob {
    struct {
        SVInteger id;

        enum {
            SVCreeper = 50, // metadata: possible values -1, 1
            SVSkeleton = 51,
            SVSpider = 52,
            SVGiantZombie = 53,
            SVZombie = 54,
            SVSlime = 55,
            SVGhast = 56,
            SVZombiePigman = 57,

            SVPig = 90, // metadata: possible values 0, 1
            SVSheep = 91,    // metadata: bit 0x10 indicates shearedness, the lower 4 bits indicate wool color.
            SVCow = 92,
            SVHen = 93,
            SVSquid = 94,
            SVWolf = 95
        } type;

        SVAbsolutePosition position;

        SVByte yaw;
        SVByte pitch;

        SVMetadata* metadata;
    } response;
} SVPacketSpawnMob;

typedef union _SVPacketPainting {
    struct {
        SVEntity entity;
        SVString title;

		SVBlockPosition position;

        SVInteger direction;
    } response;
} SVPacketPainting;

typedef union _SVPacketExperienceOrb {
	struct {
		SVEntity entity;
		SVBlockPosition position;
		SVShort count;
	} response;
} SVPacketExperienceOrb;

typedef union _SVPacketStanceUpdate { //most likely not used
	struct {
		SVFloat u1;
		SVFloat u2;
		SVFloat u3;
		SVFloat u4;
		SVBoolean u5;
		SVBoolean u6;
	} request;

	struct {
		SVFloat u1;
		SVFloat u2;
		SVFloat u3;
		SVFloat u4;
		SVBoolean u5;
		SVBoolean u6;
	} response;
} SVPacketStanceUpdate;

typedef union _SVPacketEntityVelocity {
	struct {
		SVEntity   entity;
		SVVelocity velocity;
	} response;
} SVPacketEntityVelocity;

typedef union _SVPacketEntityDestroy {
	struct {
		SVEntity entity;
	} response;
} SVPacketEntityDestroy;

typedef union _SVPacketEntityCreate {
	struct {
		SVEntity entity;
	} response;
} SVPacketEntityCreate;

typedef union _SVPacketEntityRelativeMove {
	struct {
		SVEntity entity;

		SVRelativePosition position;
	} response;
} SVPacketEntityRelativeMove;

typedef union _SVPacketEntityLook {
	struct {
		SVEntity entity;

		SVByte yaw;
		SVByte pitch;
	} response;
} SVPacketEntityLook;

typedef union _SVPacketEntityLookMove {
	struct {
		SVEntity entity;

		SVRelativePosition position;

		SVByte yaw;
		SVByte pitch;
	} response;
} SVPacketEntityLookMove;

typedef union _SVPacketEntityTeleport {
	struct {
		SVEntity   entity;
		SVAbsolutePosition position;

		SVByte rotation;
		SVByte pitch;
	} response;
} SVPacketEntityTeleport;

typedef union _SVPacketEntityStatus { // Not sure yet
    struct {
        SVEntity entity;

        enum {
            SVDrowning = 2,
            SVDead = 3
        } status;
    } response;
} SVPacketEntityStatus;

typedef union _SVPacketEntityAttach {
	struct {
		SVEntity entity;
		SVEntity vehicle;
	} response;
} SVPacketEntityAttach;

typedef union _SVPacketEntityMetadata {
	struct {
		SVEntity    entity;
		SVMetadata* metadata;
	} request;

	struct {
		SVEntity    entity;
		SVMetadata* metadata;
	} response;
} SVPacketEntityMetadata;

typedef union _SVPacketEntityEffect {
	struct {
		SVEntity entity;

		SVEffect effect;

		SVByte amplifier;
		SVShort duration;
	} request;

	struct {
		SVEntity entity;

		SVEffect effect;

		SVByte amplifier;
		SVShort duration;
	} response;
} SVPacketEntityEffect;

typedef union _SVPacketRemoveEntityEffect {
	struct {
		SVEntity entity;
		SVEffect effect;
	} request;

	struct {
		SVEntity entity;
		SVEffect effect;
	} response;
} SVPacketRemoveEntityEffect;

typedef union _SVPacketExperience {
	struct {
		SVByte currentExperience;
		SVByte level;
		SVShort totalExperience;
	} response;
} SVPacketExperience;

typedef union _SVPacketPreChunk {
	struct {
		SVChunkPosition position;

		SVBoolean mode;
	} response;
} SVPacketPreChunk;

typedef union _SVPacketMapChunk {
	struct {
		SVBlockPosition position;
		SVSize     size;

		SVInteger length;
		SVByte*   item;
	} response;
} SVPacketMapChunk;

typedef union _SVPacketMultiBlockChange {
	struct {
		SVChunkPosition position;

		SVShort length;

		SVShort* coordinate;
		SVByte*  type;
		SVByte*  metadata;
	} response;
} SVPacketMultiBlockChange;

typedef union _SVPacketBlockChange {
	struct {
		SVBlockPosition position;

		SVByte type;
		SVByte metadata;
	} response;
} SVPacketBlockChange;

typedef union _SVPacketPlayNoteBlock { //TODO: need to be renamed to _SVBlockAction
    struct {
        SVBlockPosition position;

        /**
         * Instrument Type (0-4)
         * Piston State (0-1)
         */
        SVByte data1;

        /**
         * Pitch (0-24)
         * Pushing Direction (0-5)
         */
        SVByte data2;
    } response;
} SVPacketPlayNoteBlock;

typedef union _SVPacketExplosion { // Not sure yet
	struct {
		SVPrecisePosition position;

		SVFloat radius; // unsure

		SVInteger           length;
		SVRelativePosition* item;
	} response;
} SVPacketExplosion;

typedef union _SVPacketSoundEffect {
	struct {
		enum {
			SVClick1 = 1001,
			SVClick2 = 1000,
			SVFireBow = 1002,
			SVDoorToggle = 1003,
			SVExtinguish = 1004,
			SVPlayRecord = 1005,
			SVSmoke = 2000,
			SVBlockBreak = 2001
		} effect; //int

		SVBlockPosition position;

		enum {
			SVSouthEast = 0,
			SVSouth = 1,
			SVSouthWest = 2,
			SVEast = 3,
			SVCenter = 4,
			SVWest = 5,
			SVNorthEast = 6,
			SVNorth = 7,
			SVNorthWest = 8
		} data; //int
	} response;
} SVPacketSoundEffect;

typedef union _SVPacketState {
	struct {
		//0-Invalid Bed 1/2-Start/Stop Rain 3-Game Mode
		SVByte reason;

		//0-survival 1-creative
		SVByte gameMode;
	} response;
} SVPacketState;

typedef union _SVPacketThunderbolt {
	struct {
		SVEntity entity;
		SVBoolean u1; //always true
		SVAbsolutePosition position;
	} response;
} SVPacketThunderbolt;

typedef union _SVPacketOpenWindow {
	struct {
		SVByte id;

		enum {
			SVChest,
			SVWorkbench,
			SVFurnace,
			SVDispenser
		} type;

		SVString title;

		SVByte slots;
	} response;
} SVPacketOpenWindow;

typedef union _SVPacketCloseWindow {
	struct {
		SVByte id;
	} request;

	struct {
		SVByte id;
	} response;
} SVPacketCloseWindow;

typedef union _SVPacketWindowClick {
    struct {
        SVByte    id;
        SVShort   slot;
        SVBoolean rightClick;
        SVShort   action;
        SVBoolean shiftPressed;

        SVItemStack item; // if the first of the 3 values is -1 the packet ends there
    } request;
} SVPacketWindowClick;

typedef union _SVPacketSetSlot {
	struct {
		SVByte  windowId;
        SVItemStack item;
	} response;
} SVPacketSetSlot;

typedef union _SVPacketWindowItems {
	struct {
		SVByte id;

		SVShort length;
		SVItemStack* item;
	} response;
} SVPacketWindowItems;

typedef union _SVPacketUpdateProgressBar {
	struct {
		SVByte  id;
		SVShort bar;
		SVShort value;
	} response;
} SVPacketUpdateProgressBar;

typedef union _SVPacketTransaction {
	struct {
		SVByte    id;
		SVShort   action;
		SVBoolean accepted;
	} request;

	struct {
		SVByte    id;
		SVShort   action;
		SVBoolean accepted;
	} response;
} SVPacketTransaction;

typedef union _SVPacketCreativeInventoryAction {
	struct {
		SVShort slot;
		SVShort itemId;
		SVShort quantity;
		SVShort damage;
	} request;

	struct {
		SVShort slot;
		SVShort itemId;
		SVShort quantity;
		SVShort damage;
	} response;
} SVPacketCreativeInventoryAction;

typedef union _SVPacketUpdateSign {
	struct {
		SVBlockPosition position;

		SVString first;
		SVString second;
		SVString third;
		SVString fourth;
	} request;

	struct {
		SVBlockPosition position;

		SVString first;
		SVString second;
		SVString third;
		SVString fourth;
	} response;
} SVPacketUpdateSign;

typedef union _SVPacketItemData {
	struct {
		SVShort itemType;
		SVShort itemId;
		SVByte textLength; //This is unsigned!
		SVByte* text;
	} response;
} SVPacketItemData;

typedef union _SVPacketIncrementStatistic {
	struct {
		SVInteger id;
		SVByte    amount;
	} request;
	struct {
		SVInteger id;
		SVByte    amount;
	} response;
} SVPacketIncrementStatistic;

typedef union _SVPacketPlayerListItem {
	struct {
		SVString playerName;
		SVBoolean online;
		SVShort ping; //in milliseconds
	} response;
} SVPacketPlayerListItem;

typedef struct _SVPacketListPing {
    char empty;
} SVPacketListPing;

typedef union _SVPacketDisconnect {
	struct {
		SVString reason;
	} request;

	struct {
		SVString reason;
	} response;
	
	struct {
		SVString description;
	} ping;
} SVPacketDisconnect;
/**
 * Create a Packet from a Buffers object
 *
 * @param input The Buffer to read from
 *
 * @return The instantiated Packet object
 */
SVPacket* SV_PacketFromBuffers (CDBuffers* buffers, bool isResponse);

/**
 * Destroy a Packet object
 */
void SV_DestroyPacket (SVPacket* self);

/**
 * Destroy Packet data
 */
void SV_DestroyPacketData (SVPacket* self);

/**
 * Generate a SVPacket* object from the given bufferevent and return it.
 *
 * This is used internally by SV_PacketFromEvent but can be used in other situations.
 *
 * @param input The Buffer where the input lays
 *
 * @return The instantiated Object cast to (CDPointer)
 */
CDPointer SV_GetPacketDataFromBuffer (SVPacket* self, CDBuffer* input);

/**
 * Generate a Buffer version of the packet to send through the net
 *
 * @return The raw packet data
 */
CDBuffer* SV_PacketToBuffer (SVPacket* self);

#endif
