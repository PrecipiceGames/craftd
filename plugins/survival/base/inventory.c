
/**
 * Event callback functions relating to player inventory management.
 */

static
bool
cdsurvival_PlayerHoldChange (CDServer* server, SVPlayer* player, SVShort slot)
{

    SDEBUG(server, "%s was holding slot %d, now: %d", CD_StringContent(player->username), player->currentSlot, slot); 

    player->currentSlot = slot;

    SVItemStack* current = (SVItemStack*)CD_MapGet(player->inventory, player->currentSlot);

    SVPacketEntityEquipment pkt = {
        .response = {
            .entity = player->entity,
            .slot   = 0, // 0=held
            .item   = (current ? current->id : -1),
            .damage = (current ? current->damage : 0)
        }
    };

    SVPacket response = { SVResponse, SVEntityEquipment, (CDPointer) &pkt };

    cdsurvival_SendPacketToAllInRegion(player, &response);

    return true;
}

static
bool
cdsurvival_PlayerInventoryCreative (CDServer* server, SVPlayer* player, SVItemStack stack)
{
    SDEBUG(server, "%s creative inventory itemId: %d, slot: %d, count: %d, damage: %d",
                CD_StringContent(player->username), stack.id, stack.slot, stack.count, stack.damage);

    //The player has aquired a new stack of stuff
    if (stack.slot > -1) {
        SVItemStack* copy = CD_malloc(sizeof(SVItemStack));
        memcpy(copy, &stack, sizeof(SVItemStack));
        CD_MapPut(player->inventory, stack.slot, (CDPointer)copy);

    }

    return true;
}

