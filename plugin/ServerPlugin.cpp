/* 
 * Copyright 2008-2011 Nicolas Maingot
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

#include "UpdateNotifier.h" // leave it here so Source SDK can undef/redefine the microsoft's ARRAYSIZE macro
#include "ServerPlugin.h"
#include "BaseTimer.h"
#include "../configuration/ConfigurationFile.h"
#include "../convars/I18nConVar.h"
#include "../commands/I18nConCommand.h"
#include "../convars/ConVarCallbacks.h"
//#include "../entity/EntityProp.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "../match/MatchManager.h"
#include "../match/DisabledMatchState.h"

#include "tier1.h"
#include "tier2/tier2.h"
#include "edict.h"
#include "eiface.h"
#include "iplayerinfo.h"
#include "igameevents.h"
#include "../convars/convar.h"
#include "interface.h"
#include "IEngineSound.h" 
#include "toolframework/itoolentity.h"

using namespace cssmatch;

using std::string;
using std::list;
using std::map;
using std::for_each;
using std::count_if;
using std::find;
using std::find_if;
using std::ostringstream;
using std::endl;

/*CON_COMMAND(cssm_test, "CSSMatch: Internal")
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ClanMember * member = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingUserid,atoi(plugin->getInterfaces()->engine->Cmd_Argv(1)),member)
	{
		try
		{
			float x = atof(plugin->getInterfaces()->engine->Cmd_Argv(2));
			float y = atof(plugin->getInterfaces()->engine->Cmd_Argv(3));
			float z = atof(plugin->getInterfaces()->engine->Cmd_Argv(4));
			member->setVecOrigin(Vector(x,y,z));
		}
		catch(const EntityPropException & e)
		{
			CSSMATCH_PRINT_EXCEPTION(e);
		}
	}
}*/

/** Workaround for CS:S OB:
 * Public ConVars are not really considered as public until 
 * their values are set during the game (after a map change)
 * This class set the value of cssmatch_version after ~5 seconds
 */
class MakePublicTimer : public BaseTimer // TODO: Review me each SRCDS release
{
public:
	MakePublicTimer() : BaseTimer(5.0)
	{
	}

	void execute()
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		ConVar * cssmatch_version = plugin->getConVar("cssmatch_version");
		cssmatch_version->SetValue(cssmatch_version->GetString());
	}
};
 #include "eiface.h"
ServerPlugin::ServerPlugin()
	: instances(0), updateThread(NULL), clientCommandIndex(0), adminMenu(NULL), bantimeMenu(NULL), match(NULL), i18n(NULL)
{
}

ServerPlugin::~ServerPlugin()
{
	removeTimers();

	for_each(playerlist.begin(),playerlist.end(),PlayerToRemove());

	if (adminMenu != NULL)
		delete adminMenu;

	if (bantimeMenu != NULL)
		delete bantimeMenu;

	if (match != NULL)
		delete match;

	if (i18n != NULL)
		delete i18n;

	// Unlink and delete all ConVar/ConCommand when unloading ?
	// https://forums.alliedmods.net/showthread.php?p=319104#post319104 ?
	// Edit : ConVar_Unregister unlink them, and ConVar_Register link them again when loading
}

bool ServerPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	Msg(CSSMATCH_NAME ": loading...\n");

	bool success = true;

	instances++;
	if (instances == 1)
	{
		ConnectTier1Libraries(&interfaceFactory,1);
		ConnectTier2Libraries(&interfaceFactory,1);

		success &= 
			getInterface<IPlayerInfoManager>(gameServerFactory,interfaces.playerinfomanager,"PlayerInfoManager",2) && // INTERFACEVERSION_PLAYERINFOMANAGER
			getInterface<IVEngineServer>(interfaceFactory,interfaces.engine,"VEngineServer",21) && // INTERFACEVERSION_VENGINESERVER
			getInterface<IGameEventManager2>(interfaceFactory,interfaces.gameeventmanager2,"GAMEEVENTSMANAGER",2) && // INTERFACEVERSION_GAMEEVENTSMANAGER2
			getInterface<IServerPluginHelpers>(interfaceFactory,interfaces.helpers,"ISERVERPLUGINHELPERS",1) && // INTERFACEVERSION_ISERVERPLUGINHELPERS
			getInterface<IServerGameDLL>(gameServerFactory,interfaces.serverGameDll,"ServerGameDLL",6) && // INTERFACEVERSION_SERVERGAMEDLL
			getInterface<IEngineSound>(interfaceFactory,interfaces.sounds,"IEngineSoundServer",3) && // IENGINESOUND_SERVER_INTERFACE_VERSION
			getInterface<IServerTools>(gameServerFactory,interfaces.serverTools,"VSERVERTOOLS",1); // VSERVERTOOLS_INTERFACE_VERSION

		if (success)
		{
			interfaces.gpGlobals = interfaces.playerinfomanager->GetGlobalVars();

			MathLib_Init(2.2f,2.2f,0.0f,2);

			// Initialize the admin menus
			adminMenu = new Menu(NULL,"menu_administration",
				new MenuCallback<ServerPlugin>(this,&ServerPlugin::adminMenuCallback));
			adminMenu->addLine(true,"menu_changelevel");
			adminMenu->addLine(true,"menu_swap");
			adminMenu->addLine(true,"menu_spec");
			adminMenu->addLine(true,"menu_kick");
			adminMenu->addLine(true,"menu_ban");
			adminMenu->addLine(true,"menu_back");

			bantimeMenu = new Menu(NULL,"menu_ban_time",
				new MenuCallback<ServerPlugin>(this,&ServerPlugin::bantimeMenuCallback));
			bantimeMenu->addLine(true,"menu_5_min");
			bantimeMenu->addLine(true,"menu_1_h");
			bantimeMenu->addLine(true,"menu_permanent");
			bantimeMenu->addLine(true,"menu_back");

			match = new MatchManager(DisabledMatchState::getInstance());
			
			//	Initialize the translations tools
			i18n = new I18nManager();
			I18nConVar * cssmatch_language = new I18nConVar(i18n,"cssmatch_language","english",FCVAR_NONE,"cssmatch_language");
			addPluginConVar(cssmatch_language);
			i18n->setDefaultLanguage(cssmatch_language);

			// Create the other convars
			addPluginConVar(new I18nConVar(i18n,"cssmatch_version",CSSMATCH_VERSION,FCVAR_NOTIFY|FCVAR_REPLICATED,"cssmatch_version",cssmatch_version));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_advanced","1",FCVAR_NONE,"cssmatch_advanced",true,0.0f,true,1.0f));
			//addPluginConVar(new I18nConVar(i18n,"cssmatch_gather","0",FCVAR_NONE,"cssmatch_gather",true,0.0f,true,1.0f));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_report","1",FCVAR_NONE,"cssmatch_report",true,0.0f,true,1.0f));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound","1",FCVAR_NONE,"cssmatch_kniferound",true,0.0f,true,1.0f));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound_money","0",FCVAR_NONE,"cssmatch_kniferound_money",true,0.0f,true,16000.0f));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_kniferound_allows_c4","1",FCVAR_NONE,"cssmatch_kniferound_allows_c4",true,0.0f,true,1.0f));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_end_kniferound","20",FCVAR_NONE,"cssmatch_end_kniferound",true,0.0f,false,0.0f));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_rounds","12",FCVAR_NONE,"cssmatch_rounds",true,0.0f,false,0.0f));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_sets","2",FCVAR_NONE,"cssmatch_sets",true,0.0f,false,0.0f));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_end_set","10",FCVAR_NONE,"cssmatch_end_set",true,0.0f,false,0.0f));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_sourcetv","1",FCVAR_NONE,"cssmatch_sourcetv",true,0.0f,true,1.0f));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_sourcetv_path","cfg/cssmatch/sourcetv",FCVAR_NONE,"cssmatch_sourcetv_path"));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_warmup_time","5",FCVAR_NONE,"cssmatch_warmup_time",true,0.0f,false,0.0f));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_hostname","",FCVAR_NONE,"cssmatch_hostname")); // Deprecated, use hostname instead
			addPluginConVar(new I18nConVar(i18n,"cssmatch_password","",FCVAR_NONE,"cssmatch_password")); // Deprecated, use sv_password instead
			addPluginConVar(new I18nConVar(i18n,"cssmatch_default_config","server.cfg",FCVAR_NONE,"cssmatch_default_config"));

			addPluginConVar(new I18nConVar(i18n,"cssmatch_usermessages","28",FCVAR_NONE,"cssmatch_usermessages"));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_updatesite","www.cssmatch.com",FCVAR_NONE,"cssmatch_updatesite"));
			addPluginConVar(new I18nConVar(i18n,"cssmatch_weapons",
				"awp g3sg1 sg550 galil ak47 scout sg552 famas m4a1 aug glock usp p228 deagle elite fiveseven m3 xm1014 mac10 tmp mp5navy ump45"
				"p90 m249 flashbang hegrenade smokegrenade"
				,FCVAR_NONE,"cssmatch_weapons"));
			
			// Create the plugin's commands
			addPluginConCommand(new I18nConCommand(i18n,"cssm_help",cssm_help,"cssm_help"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_start",cssm_start,"cssm_start"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_stop",cssm_stop,"cssm_stop"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_retag",cssm_retag,"cssm_retag"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_go",cssm_go,"cssm_go"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_restartmanche",cssm_restartmanche,"cssm_restartmanche")); // backward compatibility
			addPluginConCommand(new I18nConCommand(i18n,"cssm_restarthalf",cssm_restartmanche,"cssm_restartmanche")); // same than cssm_restartmanche
			addPluginConCommand(new I18nConCommand(i18n,"cssm_restartround",cssm_restartround,"cssm_restartround"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_adminlist",cssm_adminlist,"cssm_adminlist"));
#ifdef _DEBUG
			addPluginConCommand(new I18nConCommand(i18n,"cssm_grant",cssm_grant,"cssm_grant"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_revoke",cssm_revoke,"cssm_revoke"));
#endif // _DEBUG
			addPluginConCommand(new I18nConCommand(i18n,"cssm_teamt",cssm_teamt,"cssm_teamt"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_teamct",cssm_teamct,"cssm_teamct"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_swap",cssm_swap,"cssm_swap"));
			addPluginConCommand(new I18nConCommand(i18n,"cssm_spec",cssm_spec,"cssm_spec"));

			// Hook needed commands
			hookConCommand("say",say_hook,true);
			hookConCommand("say_team",say_hook,true);
			//hookConCommand("tv_stoprecord",tv_stoprecord_hook);
			//hookConCommand("tv_stop",tv_stoprecord_hook);

			addPluginClientCommand("jointeam",clientcmd_jointeam,false);
			addPluginClientCommand("menuselect",clientcmd_menuselect,false);
			addPluginClientCommand("cssmatch",clientcmd_cssmatch,true);
			addPluginClientCommand("cssm_rates",clientcmd_rates,true);

			// Initialize the ConCommand/ConVar interface
			if (g_pCVar != NULL)
			{
				interfaces.cvars = g_pCVar;
				ConVar_Register(0);
				
				// Grab some existing ConVars
				ConVar * sv_cheats = interfaces.cvars->FindVar("sv_cheats");
				ConVar * sv_alltalk = interfaces.cvars->FindVar("sv_alltalk");
				ConVar * hostname = interfaces.cvars->FindVar("hostname");
				ConVar * sv_password = interfaces.cvars->FindVar("sv_password");
				ConVar * tv_enable = interfaces.cvars->FindVar("tv_enable");
				ConVar * ip = interfaces.cvars->FindVar("ip");
				ConVar * sv_minrate = interfaces.cvars->FindVar("sv_minrate");
				ConVar * sv_maxrate = interfaces.cvars->FindVar("sv_maxrate");
				ConVar * sv_mincmdrate = interfaces.cvars->FindVar("sv_mincmdrate");
				ConVar * sv_maxcmdrate = interfaces.cvars->FindVar("sv_maxcmdrate");
				ConVar * sv_minupdaterate = interfaces.cvars->FindVar("sv_minupdaterate");
				ConVar * sv_maxupdaterate = interfaces.cvars->FindVar("sv_maxupdaterate");
				if ((sv_cheats == NULL) ||
					(sv_alltalk == NULL) ||
					(hostname == NULL) ||
					(sv_password == NULL) ||
					(tv_enable == NULL) ||
					(ip == NULL) ||
					(sv_minrate == NULL) ||
					(sv_maxrate == NULL) ||
					(sv_mincmdrate == NULL) || 
					(sv_maxcmdrate == NULL) ||
					(sv_minupdaterate == NULL) ||
					(sv_maxupdaterate == NULL))
				{
					success = false;
					CSSMATCH_PRINT("At least one game ConVars was not found");
				}
				else
				{
					addPluginConVar(sv_cheats);
					addPluginConVar(sv_alltalk);
					addPluginConVar(hostname);
					addPluginConVar(sv_password);
					addPluginConVar(tv_enable);
					addPluginConVar(ip);
					addPluginConVar(sv_minrate);
					addPluginConVar(sv_maxrate);
					addPluginConVar(sv_mincmdrate);
					addPluginConVar(sv_maxcmdrate);
					addPluginConVar(sv_minupdaterate);
					addPluginConVar(sv_maxupdaterate);
				}
			}
			else
			{
				success = false;
				Msg(CSSMATCH_NAME ": Failed to connect tier1 libs\n");
			}

			// Initialize the file system interface
			if (g_pFullFileSystem != NULL)
			{
				interfaces.filesystem = g_pFullFileSystem;
			}
			else
			{
				success = false;
				Msg(CSSMATCH_NAME ": Failed to connect tier2 libs\n");
			}
			
			// Start the check for updates thread
			try
			{
				updateThread = new UpdateNotifier();
				updateThread->Start();
			}
			catch(const UpdateNotifierException & e)
			{
				Msg(CSSMATCH_NAME ": %s (%s, l.%i)\n",e.what(),__FILE__,__LINE__);
			}

			Msg(CSSMATCH_NAME ": loaded\n");
		}
	}
	else
	{
		Msg(CSSMATCH_NAME ": Plugin already loaded!\n");
		success = false;
	}

	return success;
}

void ServerPlugin::Unload()
{
	Msg(CSSMATCH_NAME ": unloading...\n");

	instances--;
	if (instances == 0)
	{
		ConVar_Unregister();
		DisconnectTier1Libraries();
		DisconnectTier2Libraries();

		if (updateThread != NULL)
		{
			updateThread->End();
			updateThread->Join();
			delete updateThread;
			updateThread = NULL;
		}
	}
	Msg(CSSMATCH_NAME ": unloaded\n");
}

ValveInterfaces * ServerPlugin::getInterfaces()
{
	return &interfaces;
}

int ServerPlugin::GetCommandClient() const
{
	return clientCommandIndex; 
}

void ServerPlugin::SetCommandClient(int index)
{
	clientCommandIndex = index;
}

list<ClanMember *> * ServerPlugin::getPlayerlist()
{
	return &playerlist;
}

UpdateNotifier * ServerPlugin::getUpdateThread() const
{
	return updateThread;
}

list<string> * ServerPlugin::getAdminlist()
{
	return &adminlist;
}

void ServerPlugin::showAdminMenu(Player * player)
{
	player->sendMenu(adminMenu,1);
}

void ServerPlugin::showChangelevelMenu(Player * player)
{
	Menu * maplist = new Menu(adminMenu,"menu_map",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::changelevelMenuCallback));

	try
	{
		ConfigurationFile maplistfile(CFG_FOLDER_PATH "cssmatch/maplist.txt");

		list<string> maps;
		maplistfile.getLines(maps);

		list<string>::const_iterator itMap;
		for(itMap = maps.begin(); itMap != maps.end(); itMap++)
		{
			maplist->addLine(false,*itMap);
		}
	}
	catch(const ConfigurationFileException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	player->sendMenu(maplist,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::constructPlayerlistMenu(Menu * to)
{
	list<ClanMember *>::const_iterator itPlayer;
	for(itPlayer = playerlist.begin(); itPlayer != playerlist.end(); itPlayer++)
	{
		IPlayerInfo * pInfo = (*itPlayer)->getPlayerInfo();
		if (isValidPlayerInfo(pInfo))
			to->addLine(false,pInfo->GetName(),new UseridMenuLineData(pInfo->GetUserID()));
	}
}

void ServerPlugin::showSwapMenu(Player * player)
{
	Menu * swapMenu = new Menu(adminMenu,"menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::swapMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showSpecMenu(Player * player)
{
	Menu * swapMenu = new Menu(adminMenu,"menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::specMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showKickMenu(Player * player)
{
	Menu * swapMenu = new Menu(adminMenu,"menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::kickMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showBanMenu(Player * player)
{
	Menu * swapMenu = new Menu(adminMenu,"menu_player",
		new MenuCallback<ServerPlugin>(this,&ServerPlugin::banMenuCallback));

	constructPlayerlistMenu(swapMenu);

	player->sendMenu(swapMenu,1,I18nManager::WITHOUT_PARAMETERS,true);
}

void ServerPlugin::showBanTimeMenu(Player * player)
{
	player->sendMenu(bantimeMenu,1);
}

void ServerPlugin::adminMenuCallback(Player * player, int choice, MenuLine * selected)
{
	switch(choice)
	{
	case 1:
		showChangelevelMenu(player);
		break;
	case 2:
		showSwapMenu(player);
		break;
	case 3:
		showSpecMenu(player);
		break;
	case 4:
		showKickMenu(player);
		break;
	case 5:
		showBanMenu(player);
		break;	
	case 6:
		player->cexec("cssmatch\n");
		break;
	default:
		player->quitMenu();
	}
}

void ServerPlugin::changelevelMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		string mapname = selected->text;
		if (interfaces.engine->IsMapValid(mapname.c_str()))
			queueCommand("changelevel " + mapname + "\n");
		else
		{
			RecipientFilter recipients;
			recipients.addRecipient(player);

			map<string,string> parameters;
			parameters["$map"] = mapname;

			i18n->i18nChatSay(recipients,"admin_map_not_found",parameters);
		}
	}
	player->quitMenu();
}

void ServerPlugin::swapMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();
		RecipientFilter recipients;
		map<string,string> parameters;

		parameters["$username"] = selected->text;
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,useridData->userid,target)
		{
			IPlayerInfo * pInfo = player->getPlayerInfo();

			target->swap(/*true*/);

			if (isValidPlayerInfo(pInfo))
			{
				recipients.addAllPlayers();
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_swap",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showSwapMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::specMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();
		RecipientFilter recipients;
		map<string,string> parameters;

		parameters["$username"] = selected->text;
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,useridData->userid,target)
		{
			IPlayerInfo * pInfo = player->getPlayerInfo();

			target->spec();

			if (isValidPlayerInfo(pInfo))
			{
				recipients.addAllPlayers();
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_spec",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showSpecMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::kickMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();
		RecipientFilter recipients;
		map<string,string> parameters;

		parameters["$username"] = selected->text;
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,useridData->userid,target)
		{
			PlayerIdentity * identity = player->getIdentity();
			IPlayerInfo * pInfo = player->getPlayerInfo();

			target->kick("admin_kick");

			if (isValidPlayerInfo(pInfo))
			{
				recipients.addAllPlayers();
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_kick_by",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showKickMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::banMenuCallback(Player * player, int choice, MenuLine * selected)
{ 
	if (choice != 10)
	{
		UseridMenuLineData * useridData = static_cast<UseridMenuLineData *>(selected->data);

		player->storeMenuData(new PlayerMenuLineData(selected->text,useridData->userid));
		showBanTimeMenu(player);
	}
	else
		player->quitMenu();
}

void ServerPlugin::bantimeMenuCallback(Player * player, int choice, MenuLine * selected)
{
	if (choice != 10)
	{
		PlayerIdentity * pIdentity = player->getIdentity();

		RecipientFilter recipients;
		map<string,string> parameters;

		PlayerMenuLineData * const targetData = static_cast<PlayerMenuLineData * const>(player->getMenuData());
		parameters["$username"] = targetData->name;

		ClanMember * target = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,targetData->userid,target)
		{
			PlayerIdentity * identity = player->getIdentity();
			IPlayerInfo * adminInfo = player->getPlayerInfo();

			int time = 0;
			switch(choice)
			{
			case 1:
				time = 5;
				break;
			case 2:
				time = 60;
				break;
			}
			target->ban(time,"admin_ban");

			if (adminInfo != NULL)
			{
				recipients.addAllPlayers();
				parameters["$admin"] = adminInfo->GetName();

				if (time == 0)
					i18n->i18nChatSay(recipients,"admin_permanently_ban",parameters,pIdentity->index);
				else
					i18n->i18nChatSay(recipients,"admin_temporaly_ban",parameters,pIdentity->index);
			}
		}
		else
		{
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"admin_is_not_connected",parameters);
		}
		showBanMenu(player); 
		// FIXME: last banned player still appear because kickid is not yet executed here
		// (engine->ServerExecute logs a "unknown command: kickid" message)
	}
	else
		player->quitMenu();
}

MatchManager * ServerPlugin::getMatch()
{
	return match;
}

void ServerPlugin::addPluginConVar(ConVar * variable)
{
	pluginConVars[variable->GetName()] = variable;
}

ConVar * ServerPlugin::getConVar(const string & name)/* throw(ServerPluginException)*/
{
	/*map<string,ConVar *>::iterator invalidConVar = pluginConVars.end();
	map<string,ConVar *>::iterator itConVar = pluginConVars.find(name);

	if (itConVar == invalidConVar)
		throw ServerPluginException("Attempts to access to an unknown variable name");

	return itConVar->second;*/
	return pluginConVars.find(name)->second;
}

void ServerPlugin::addPluginConCommand(ConCommand * command)
{
	pluginConCommands[command->GetName()] = command;
}

const map<string,ConCommand *> * ServerPlugin::getPluginConCommands() const
{
	return &pluginConCommands;
}

void ServerPlugin::hookConCommand(const std::string & commandName, HookCallback callback, bool antispam)
{
	map<string,ConCommandHook *>::iterator invalidHook = hookConCommands.end();
	map<string,ConCommandHook *>::iterator itHook = hookConCommands.find(commandName);

	if (itHook == invalidHook)
	{
		char * cName = new char [commandName.size() + 1];
		V_strcpy(cName,commandName.c_str());

		hookConCommands[commandName] = new ConCommandHook(cName,callback,antispam);
	}
	else
	{
		CSSMATCH_PRINT(commandName + " is already hooked");
	}
}

void ServerPlugin::addPluginClientCommand(const std::string & commandName, ClientCmdCallback callback, bool antispam)
{
	map<string,ClientCommandHook>::iterator invalidCmd = clientCommands.end();
	map<string,ClientCommandHook>::iterator itCmd = clientCommands.find(commandName);

	if (itCmd == invalidCmd)
	{
		clientCommands[commandName].callback = callback;
		clientCommands[commandName].nospam = antispam;
	}
	else
	{
		CSSMATCH_PRINT(commandName + " already exists");
	}	
}

I18nManager * ServerPlugin::getI18nManager()
{
	return i18n;
}

void ServerPlugin::addTimer(BaseTimer * timer)
{
	timers.push_front(timer); 
	// push front, to allow timer invoking others timers, which could have to be immediately executed
}

void ServerPlugin::removeTimers()
{
	list<BaseTimer *>::iterator itTimer;
	for(itTimer = timers.begin(); itTimer != timers.end(); itTimer++)
	{
		delete *itTimer;
	}
	timers.clear();
}

void ServerPlugin::Pause()
{
}

void ServerPlugin::UnPause()
{
}

const char * ServerPlugin::GetPluginDescription()
{
	return CSSMATCH_PLUGIN_PRINT;
}

void ServerPlugin::LevelInit(char const * pMapName)
{
	// End any match in progress
	if (match->getMatchState() != match->getInitialState())
		match->switchToInitialState();

	// Update the referee steamid list
	adminlist.clear();
	try
	{
		ConfigurationFile adminlistFile(CFG_FOLDER_PATH "cssmatch/adminlist.txt");
		adminlistFile.getLines(adminlist);
	}
	catch(const ConfigurationFileException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	// Delete all pending timers
	removeTimers();

	// Workaround for CS:S OB (see MakePublicTimer)
	addTimer(new MakePublicTimer());
}

void ServerPlugin::ServerActivate(edict_t * pEdictList, int edictCount, int clientMax)
{
}

void ServerPlugin::GameFrame(bool simulating)
{
	// Execute and remove the timers out of date
	timers.remove_if(TimerOutOfDate(interfaces.gpGlobals->curtime));
}

void ServerPlugin::LevelShutdown() // !!!!this can get called multiple times per map change
{
}

void ServerPlugin::ClientActive(edict_t * pEntity)
{
}

void ServerPlugin::ClientDisconnect(edict_t * pEntity)
{
	ClanMember * toRemove = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,toRemove)
	{
		playerlist.remove(toRemove);
		delete toRemove;
	}
}

void ServerPlugin::ClientPutInServer(edict_t * pEntity, char const * playername)
{
	int index = interfaces.engine->IndexOfEdict(pEntity);
    if (isValidPlayerInfoIndex(index,interfaces.gpGlobals->maxClients))
    {
		// First remove the player if he's already in the list
		list<ClanMember *>::iterator invalidPlayer = playerlist.end();
		list<ClanMember *>::iterator itPlayer =
			find_if(playerlist.begin(),invalidPlayer,PlayerHavingIndex(index));
		if (itPlayer != invalidPlayer)
		{
			delete (*itPlayer);
			playerlist.erase(itPlayer);
		}

		// Then add the new player to the player list
        try
        {
            list<string>::iterator invalidSteamid = adminlist.end();
			bool isReferee = find(adminlist.begin(),invalidSteamid,interfaces.engine->GetPlayerNetworkIDString(pEntity)) != invalidSteamid;
			playerlist.push_back(new ClanMember(index,isReferee));
        }
        catch(const PlayerException & e)
        {
            CSSMATCH_PRINT_EXCEPTION(e);
        }
    }
}

void ServerPlugin::ClientSettingsChanged(edict_t * pEdict)
{
}

PLUGIN_RESULT ServerPlugin::ClientConnect(bool * bAllowConnect,
										edict_t * pEntity,
										const char * pszName,
										const char * pszAddress,
										char * reject,
										int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT ServerPlugin::ClientCommand(edict_t * pEntity,const CCommand &args)
{
	PLUGIN_RESULT result = PLUGIN_CONTINUE;

	try
	{
		ClanMember * user = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingPEntity,pEntity,user)
		{
			string command = args.Arg(0);
			
			map<string,ClientCommandHook>::iterator invalidCmd = clientCommands.end();
			map<string,ClientCommandHook>::iterator itCmd = clientCommands.find(command);

			if (itCmd != invalidCmd)
			{
				if (itCmd->second.nospam)
				{
					if (user->isReferee() || user->canUseCommand())
						result = (*itCmd->second.callback)(user,args);
					else
						result = PLUGIN_STOP;
				}
				else
					result = (*itCmd->second.callback)(user,args);
			}
		}
#ifdef _DEBUG
		else
		{
			CSSMATCH_PRINT("Unable to find the user");
		}
#endif // _DEBUG
	}
	catch(const BaseException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return result;
}

PLUGIN_RESULT ServerPlugin::NetworkIDValidated(const char * pszUserName, const char * pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

void ServerPlugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t * pPlayerEntity, EQueryCvarValueStatus eStatus, const char * pCvarName, const char * pCvarValue)
{
}

void ServerPlugin::log(const std::string & message) const
{
	ostringstream buffer;
	buffer << CSSMATCH_NAME << ": " << message << "\n";
	const string & toLog = buffer.str();
	interfaces.engine->LogPrint(toLog.c_str());
}

void ServerPlugin::queueCommand(const string & command) const
{
	interfaces.engine->ServerCommand(command.c_str());
}

void ServerPlugin::executeCommand(const std::string & command) const
{
	//interfaces.engine->InsertServerCommand(command.c_str());
	queueCommand(command);
	interfaces.engine->ServerExecute();
}

bool ServerPlugin::hltvConnected() const
{
    list<ClanMember *>::const_iterator invalidPlayer = playerlist.end();
	return find_if(playerlist.begin(),invalidPlayer,PlayerIsHltv()) != invalidPlayer;
}

int ServerPlugin::getPlayerCount(TeamCode team) const
{
	int playerCount = 0;

	if (team == INVALID_TEAM)
			playerCount = (int)playerlist.size();
	else
			playerCount = count_if(playerlist.begin(),playerlist.end(),PlayerHavingTeam(team));

	return playerCount;
}
/*string ServerPlugin::getGameDir() const
{
	char buffer[30];
	interfaces.engine->GetGameDir(buffer,sizeof(buffer));
	return buffer;
}*/

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(	ServerPlugin, 
									IServerPluginCallbacks,
									INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
									*cssmatch::ServerPlugin::getInstance());
