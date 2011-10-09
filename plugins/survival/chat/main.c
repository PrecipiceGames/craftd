#include <craftd/Plugin.h>
#include <craftd/Server.h>

#include <craftd/protocols/survival.h>

/**
 * Plugin that handles player chat.
 *
 * Provides the Chat.output event to those that would like to modify
 * the output before it is sent. This plugin also fires the
 * Chat.command event for those plugins interested in handling
 * chat commands.
 *
 */

static struct {
	/// The token used to trigger a command (default '/')
	CDString* commandToken;

	/// The command to use when no command token is given (default 'say')
	CDString* defaultCommand;

} _config;


static CDList*      _ChatOutputParams;
static CDList*      _ChatCommandParams;
static CDRegexp*    _ChatCommandRegex;


// Callback definitions

/**
 * Sends a message to all connected to the server.
 * The given message string is destroyed.
 */
static void svchat_SendMessage (CDServer* server, CDString* message);
static bool svchat_PlayerChat  (CDServer* server, SVPlayer* player, CDString* message);
static bool svchat_ChatCommand (CDServer* server, SVPlayer* player, CDString* command, CDString* args);
static bool svchat_PlayerPreLogin (CDServer* server, SVPlayer* player);
static bool svchat_PlayerLogout (CDServer* server, SVPlayer* player);


extern
bool
CD_PluginInitialize (CDPlugin* self)
{
	self->description = CD_CreateStringFromCString("Minecraft Chat Handler");

	DO { // Initiailize config cache
		//char* tmpc = NULL;
		CDString* tmp = NULL;

		_config.commandToken = CD_CreateStringFromCString("/");
		_config.defaultCommand = CD_CreateStringFromCString("");

		tmp = CD_CreateStringFromCString(C_TO_STRING(C_PATH(self->config, "commandToken")));
		if (!CD_StringEmpty(tmp)) {
			CD_ReplaceString(_config.commandToken, tmp);
			SLOG(self->server, LOG_NOTICE, "chat> using alternate command token: %s", CD_StringContent(_config.commandToken));
		} else {
			CD_DestroyString(tmp);
		}


		tmp = CD_CreateStringFromCString(C_TO_STRING(C_PATH(self->config, "defaultCommand")));
		if (!CD_StringEmpty(tmp)) {
			CD_ReplaceString(_config.defaultCommand, tmp);
			SLOG(self->server, LOG_NOTICE, "chat> using alternate default command: %s", CD_StringContent(_config.defaultCommand));
		} else {
			CD_DestroyString(tmp);
		}
	}

	_ChatCommandRegex = CD_CreateRegexp("^(\\w+)(?:\\s+(.*?))?$", CDRegexpNone);

	// Chat.command(player, cmd, args)
	CD_EventProvides(self->server, "Chat.command",
		_ChatCommandParams = CD_CreateEventParameters("SVPlayer", "CDString", "CDString", NULL));

	// Chat.output(player, username, message)
	CD_EventProvides(self->server, "Chat.output",
		_ChatOutputParams = CD_CreateEventParameters("SVPlayer", "CDString", "CDString", NULL));

    CD_EventRegister(self->server, "Player.chat",   svchat_PlayerChat);
    CD_EventRegister(self->server, "Player.prelogin",  svchat_PlayerPreLogin);
    CD_EventRegister(self->server, "Player.logout", svchat_PlayerLogout);
    CD_EventRegister(self->server, "Chat.command",  svchat_ChatCommand);


	return true;
}

extern
bool
CD_PluginFinalize (CDPlugin* self)
{

    CD_EventUnregister(self->server, "Chat.command",   svchat_ChatCommand);
    CD_EventUnregister(self->server, "Player.logout",  svchat_PlayerLogout);
    CD_EventUnregister(self->server, "Player.prelogin",   svchat_PlayerPreLogin);
    CD_EventUnregister(self->server, "Player.chat",    svchat_PlayerChat);

	CD_DestroyEventParameters(_ChatOutputParams);
	CD_DestroyEventParameters(_ChatCommandParams);

	CD_DestroyRegexpKeepString(_ChatCommandRegex);

	CD_DestroyString(_config.commandToken);
	CD_DestroyString(_config.defaultCommand);

	_ChatOutputParams       = NULL;
	_ChatCommandParams      = NULL;
	_ChatCommandRegex       = NULL;
	 _config.commandToken   = NULL;
	_config.defaultCommand  = NULL;

	return true;
}


/**
 * Sends a message to all connected to the server.
 * The given message string is destroyed.
 */
static
void
svchat_SendMessage(CDServer* server, CDString* message)
{
	assert(server);
	CDList* worlds = (CDList*) CD_DynamicGet(server, "World.list");

	CD_LIST_FOREACH(worlds, it) {
		SVWorld* world = (SVWorld*) CD_ListIteratorValue(it);

		SV_WorldBroadcastMessage(world, CD_CloneString(message));
	}
	CD_DestroyString(message);
}

static
bool
svchat_PlayerChat (CDServer* server, SVPlayer* player, CDString* message)
{
	assert(server);
	assert(player);


	if (!CD_StringStartWith(message, CD_StringContent(_config.commandToken))) {
		// This chat message does not begin with our command token,
		// so pass it as an argument to the default chat command.

		CD_EventDispatch(server, "Chat.command", player, _config.defaultCommand, message);
	} else {
		//This chat message has a command, so fire off a Chat.command
		//event.
		CDString* commandString = CD_CreateStringFromOffset(message, CD_StringLength(_config.commandToken), 0);
		CDRegexpMatches* matches = CD_RegexpMatch(_ChatCommandRegex, commandString);

		if (matches) {
			CD_EventDispatch(server, "Chat.command", player, matches->item[1], matches->item[2]);
			CD_DestroyRegexpMatches(matches);
		}

		CD_DestroyString(commandString);
	}

	// We have consumed this event.
	return false;
}

static
bool
svchat_ChatCommand (CDServer* server, SVPlayer* player, CDString* command, CDString* args)
{

	assert(server);
	assert(player);

	SLOG(server, LOG_NOTICE, "%s> %s: %s",
		CD_StringContent(player->username), CD_StringContent(command), CD_StringContent(args));

	if (CD_StringIsEqual(command, "")) {
		svchat_SendMessage(server,
			CD_CreateStringFromFormat("<%s> %s",
				CD_StringContent(player->username),
				CD_StringContent(args)));
	}
	else if (CD_StringIsEqual(command, "me")) {
		svchat_SendMessage(server,
			CD_CreateStringFromFormat("* %s %s",
				CD_StringContent(player->username),
				CD_StringContent(args)));
	}
	else if (CD_StringIsEqual(command, "tell")) {
		//Lookup player
		CD_HASH_FOREACH(player->world->players, it) {
			SVPlayer* otherPlayer = (SVPlayer *) CD_HashIteratorValue(it);
			if (CD_StringIsEqual(otherPlayer->username, CD_StringContent(args))) {
				//send the private message

				SV_PlayerSendMessage(otherPlayer, args);
			}
		}
	}
	else if (CD_StringIsEqual(command, "kill")) {
		SV_PlayerSendMessage(player,
			SV_StringColor(
				CD_CreateStringFromFormat("%s: command not yet implemented", CD_StringContent(command)),
				SVColorYellow));
	}

	return false;

}


static
bool
svchat_PlayerPreLogin (CDServer* server, SVPlayer* player)
{
    SV_WorldBroadcastMessage(player->world, 
        SV_StringColor(CD_CreateStringFromFormat("%s has joined the game",
            CD_StringContent(player->username)), SVColorYellow));

    return true;
}


static
bool
svchat_PlayerLogout (CDServer* server, SVPlayer* player)
{
    SV_WorldBroadcastMessage(player->world, 
        SV_StringColor(CD_CreateStringFromFormat("%s has left the game",
            CD_StringContent(player->username)), SVColorYellow));

    return true;
}

