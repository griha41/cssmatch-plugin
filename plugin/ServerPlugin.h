/* 
 * Copyright 2008-2010 Nicolas Maingot
 * 
 * This file is part of CSSMatch.
 * 
 * CSSMatch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSSMatch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#ifndef __SERVER_PLUGIN_H__
#define __SERVER_PLUGIN_H__

#include "../messages/RecipientFilter.h"
#include "../misc/BaseSingleton.h"
#include "../convars/ConvarsAccessor.h" // ConvarsAccessorException
#include "../player/ClanMember.h"
#include "../exceptions/BaseException.h"
#include "../commands/ConCommandCallbacks.h"
#include "../commands/ClientCommandCallbacks.h"
#include "../commands/ConCommandHook.h"
#include "../messages/Menu.h"

#include "engine/iserverplugin.h"

class IVEngineServer;
class IFileSystem;
class IGameEventManager2;
class IPlayerInfoManager;
class IServerPluginHelpers;
class CGlobalVars;
class IServerGameDLL;
class IEngineSound;
class IServerTools;
class ConVar;
struct edict_t;
class ConCommand;

#include <string>
#include <list>
#include <map>
#include <algorithm>

namespace cssmatch
{
	class ConvarsAccessor;
	class ClanMember;
	class I18nManager;
	class BaseTimer;
	class MatchManager;
	class UpdateNotifier;

	/** Valve's interface instances */
	struct ValveInterfaces
	{
		IVEngineServer * engine; // helper functions (messaging clients, loading content, making entities, running commands, etc)
		IFileSystem * filesystem; // file I/O 
		IGameEventManager2 * gameeventmanager2; // game events interface
		IPlayerInfoManager * playerinfomanager; // game dll interface to interact with players
		IServerPluginHelpers * helpers; // special 3rd party plugin helpers from the engine
		CGlobalVars * gpGlobals; // global vars
		ConvarsAccessor * convars; // console vars
		IServerGameDLL * serverGameDll; // Access to some DLL infos
		IEngineSound * sounds;
		IServerTools * serverTools;

		ValveInterfaces()
			:	engine(NULL),
				filesystem(NULL),
				gameeventmanager2(NULL),
				playerinfomanager(NULL),
				helpers(NULL),
				gpGlobals(NULL),
				convars(NULL),
				serverGameDll(NULL),
				sounds(NULL),
				serverTools(NULL)
				{}
	};

	/** Client command */
	struct ClientCommandHook
	{
		/** Command callback */
		ClientCmdCallback callback;

		/** Is this command protected against spam? */
		bool nospam;
	};

	/** Source plugin IServerPluginCallbacks implementation */
	class ServerPlugin : public BaseSingleton<ServerPlugin>, public IServerPluginCallbacks
	{
	private:
		// Some data type the admin menu put into the menu lines
		struct UseridMenuLineData : public BaseMenuLineData
		{
			int userid;

			UseridMenuLineData(int playerUserid) : userid(playerUserid){};
		};
		struct PlayerMenuLineData : public BaseMenuLineData
		{
			std::string name;
			int userid;

			PlayerMenuLineData(const std::string & playername, int playerUserid)
				: name(playername), userid(playerUserid){};
		};

		/** Is the plugin already loaded before? */
		bool loaded;

		/** Search-for-update thread */
		UpdateNotifier * updateThread;

		/** Valve's interfaces accessor */
		ValveInterfaces interfaces;

		/** Last client index who used a client command */
		int clientCommandIndex;

		/** Global playerlist */
		std::list<ClanMember *> playerlist;

		/** Referee steamid list */
		std::list<std::string> adminlist;

		/** Admin menu */
		Menu * adminMenu;
		Menu * bantimeMenu;

		/** Match manager */
		MatchManager * match;

		/** Timer list */
		std::list<BaseTimer *> timers;

		/** Plugin console variable list */
		std::map<std::string,ConVar *> pluginConVars;

		/** Plugin console command list */
		std::map<std::string,ConCommand *> pluginConCommands;

		/** Hook console command list */
		std::map<std::string,ConCommandHook *> hookConCommands;

		/** Client command callbacks */
		std::map<std::string,ClientCommandHook> clientCommands;

		/** Internationalization tool */
		I18nManager * i18n;

		friend class BaseSingleton<ServerPlugin>;
		ServerPlugin();
		virtual ~ServerPlugin();

		/** Get a named interface from an interface factory
		 * @param factory The factory provided by the game when the plugin is loaded
		 * @param interfaceVersion The name of the interface to get
		 */
		template<typename T>
		static bool getInterface(	CreateInterfaceFn factory,
									T * & toInitialize,
									const std::string & interfaceVersion)
		{
			bool success = false;

			toInitialize = (T *)factory(interfaceVersion.c_str(),NULL);

			if (toInitialize == NULL)
				Msg(std::string(CSSMATCH_NAME ": Unable to get the \"" + interfaceVersion + "\" interface!\n").c_str());
			else
				success = true;

			return success;
		}
	public:
		/** Valve's interface accessor */
		ValveInterfaces * getInterfaces();

		/** Get the last player index wich used a rcon command */
		int GetCommandClient() const;

		/** Set the last player index wich used a rcon command */
		virtual void SetCommandClient(int index);

		/** Get the global playerlist */
		std::list<ClanMember *> * getPlayerlist();

		/** Get the update notifier thread (maybe NULL) */
		UpdateNotifier * getUpdateThread() const;

		/** Get a player
		 * @param pred Predicat to use
		 * @param out Out var
		 * @return <code>true</code> if the player was found, <code>false</code> otherwise
		 */
		template<class Predicat>
		bool getPlayer(const Predicat & pred, ClanMember * & out)
		{
			bool found = false;

			std::list<ClanMember *>::iterator invalidPlayer = playerlist.end();
			std::list<ClanMember *>::iterator itPlayer = std::find_if(playerlist.begin(),invalidPlayer,pred);
			if (itPlayer != invalidPlayer)
			{
				found = true;
				out = *itPlayer;
			}
			else
				out = NULL;

			return found;
		}

		/** Get the referee steamid list (read and write) */
		std::list<std::string> * getAdminlist();

		/** Show the admin menu (or its sub-menu) to a player */
		void showAdminMenu(Player * player);
		void showChangelevelMenu(Player * player); // maplist.txt content
		void constructPlayerlistMenu(Menu * to); // generates a player list menu
		void showSwapMenu(Player * player); // player list
		void showSpecMenu(Player * player); // player list
		void showKickMenu(Player * player); // player list
		void showBanMenu(Player * player); // player list
		void showBanTimeMenu(Player * player); // ban time choose

		// Menus callbacks
		void adminMenuCallback(Player * player, int choice, MenuLine * selected);
		void changelevelMenuCallback(Player * player, int choice, MenuLine * selected);
		void swapMenuCallback(Player * player, int choice, MenuLine * selected);
		void specMenuCallback(Player * player, int choice, MenuLine * selected);
		void kickMenuCallback(Player * player, int choice, MenuLine * selected);
		void banMenuCallback(Player * player, int choice, MenuLine * selected);
		void bantimeMenuCallback(Player * player, int choice, MenuLine * selected);

		/** Get the match manager */
		MatchManager * getMatch();

		/** Add a plugin console variable */
		void addPluginConVar(ConVar * variable);

		/** Access to a known ConVar
		 * @param name The name of the ConVar
		 * @return A pointer on the ConVar if found
		 */
		ConVar * getConVar(const std::string & name)/* throw(ServerPluginException)*//* but std::exception */;

		/** Add a plugin server command */
		void addPluginConCommand(ConCommand * command);

		/** Get the plugin command list */
		const std::map<std::string,ConCommand *> * getPluginConCommands() const;

		/** Hook a ConCommand (one hook = one callback)
		 * @param commandName The name of the ConCommand to hook
		 * @param callback Callback to invoke when the hooked command is used
		 * @param antispam Does this command need to be protected against spam?
		 */
		void hookConCommand(const std::string & commandName, HookCallback callback, bool antispam);

		/** Add a plugin console command 
		 * @param commandName The name of the ConCommand to hook
		 * @param callback Callback to invoke when the command is used
		 * @param antispam Does this command need to be protected against spam?		
		*/
		void addPluginClientCommand(const std::string & commandName, ClientCmdCallback callback, bool antispam);

		/** Get the internationalization tool */
		I18nManager * getI18nManager();

		/** Add a timer */
		void addTimer(BaseTimer * timer);

		/** Remove all pending timers */
		void removeTimers();

		// IServerPluginCallbacks methods
		virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
		virtual void Unload();
		virtual void Pause();
		virtual void UnPause();
		virtual const char * GetPluginDescription();
		virtual void LevelInit(char const * pMapName);
		virtual void ServerActivate(edict_t * pEdictList, int edictCount, int clientMax);
		virtual void GameFrame(bool simulating);
		virtual void LevelShutdown();
		virtual void ClientActive(edict_t * pEntity);
		virtual void ClientDisconnect(edict_t * pEntity);
		virtual void ClientPutInServer(edict_t * pEntity, char const * playername);
		virtual void ClientSettingsChanged(edict_t * pEdict);
		virtual PLUGIN_RESULT ClientConnect(bool * bAllowConnect, edict_t * pEntity, const char *pszName, const char * pszAddress, char * reject, int maxrejectlen);
		virtual PLUGIN_RESULT ClientCommand(edict_t * pEntity,const CCommand &args);
		virtual PLUGIN_RESULT NetworkIDValidated(const char * pszUserName, const char * pszNetworkID);
        virtual void OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t * pPlayerEntity, EQueryCvarValueStatus eStatus, const char * pCvarName, const char * pCvarValue);		

		// Tools

		/** Print a message to the logs
		 * @param message The message to display
		 */
		void log(const std::string & message) const;

		/** Append a command to the server command queue
		 * @param command The command to append
		 */
		void queueCommand(const std::string & command) const;

		/** Immedialty execute a command into the server console
		 * @param command The command to execute
		 */
		void executeCommand(const std::string & command) const;

		/** Check if SourceTV is connected to the server (ignores tv_enable)
		 * @return <code>true</code> if SourceTV was found, <code>false</code> otherwise
		 */
		bool hltvConnected() const;

		/** Get the current player count (ignores SourceTv)
		 * @param team If specified, only count the player from this team
		 * @return The player count
		 */
		int getPlayerCount(TeamCode team = INVALID_TEAM) const;

		/* Returns the game directory name */
		//std::string getGameDir() const;
	};

/* Loop over all players excluding the 0 index 
#define FOREACH_PLAYER(indexVar) \
	int maxplayers = ServerPlugin::getInstance()->getInterfaces()->gpGlobals->maxClients; \
	for (int indexVar=1;indexVar<=maxplayers;indexVar++)*/

/** Search for a valid player pointer satisfying a predicat */
#define CSSMATCH_VALID_PLAYER(Predicat,criteria,out) \
	if (ServerPlugin::getInstance()->getPlayer<Predicat>(Predicat(criteria),out))
}	

#endif // __SERVER_PLUGIN_H__
