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

#include "KnifeRoundMatchState.h"

#include "../plugin/ServerPlugin.h"
#include "../misc/common.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "DisabledMatchState.h"
#include "TimeoutMatchState.h"
#include "WarmupMatchState.h"
#include "HalfMatchState.h"
#include "MatchManager.h"

using namespace cssmatch;

#include <iostream>

using std::string;
using std::list;
using std::map;
using std::ostringstream;

KnifeRoundMatchState::KnifeRoundMatchState()
{
	kniferoundMenu = new Menu("menu_kniferound",
		new MenuCallback<KnifeRoundMatchState>(this,&KnifeRoundMatchState::kniferoundMenuCallback));
	kniferoundMenu->addLine(true,"menu_alltalk");
	kniferoundMenu->addLine(true,"menu_restart");
	kniferoundMenu->addLine(true,"menu_stop");
	kniferoundMenu->addLine(true,"menu_retag");

	menuWithAdmin = new Menu("menu_kniferound",
		new MenuCallback<KnifeRoundMatchState>(this,&KnifeRoundMatchState::menuWithAdminCallback));
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_restart");
	menuWithAdmin->addLine(true,"menu_stop");
	menuWithAdmin->addLine(true,"menu_retag");

	eventCallbacks["round_start"] = &KnifeRoundMatchState::round_start;
	eventCallbacks["item_pickup"] = &KnifeRoundMatchState::item_pickup;
	eventCallbacks["player_spawn"] = &KnifeRoundMatchState::player_spawn;
	eventCallbacks["round_end"] = &KnifeRoundMatchState::round_end;
	//eventCallbacks["bomb_beginplant"] = &KnifeRoundMatchState::bomb_beginplant;
}

KnifeRoundMatchState::~KnifeRoundMatchState()
{
	delete kniferoundMenu;
	delete menuWithAdmin;
}

void KnifeRoundMatchState::endKniferound(TeamCode winner)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	MatchInfo * infos = match->getInfos();
	// Save the winner of this round for the report
	infos->kniferoundWinner = match->getClan(winner);
		// note: winner is supposed valid

	// Global recipient list
	RecipientFilter recipients;
	recipients.addAllPlayers();
	
	// Announce the winner
	map<string,string> parameters;
	parameters["$team"] = *infos->kniferoundWinner->getName();
	i18n->i18nChatSay(recipients,"kniferound_winner",parameters);

	// Invite the winners to choice a side
	TeamCode teamLoser = (winner == T_TEAM) ? CT_TEAM : T_TEAM;

	list<ClanMember *> * playerlist = plugin->getPlayerlist();
	list<ClanMember *>::iterator itPlayer;
	for(itPlayer = playerlist->begin(); itPlayer != playerlist->end(); itPlayer++)
	{
		PlayerIdentity * identity = (*itPlayer)->getIdentity();
		IPlayerInfo * pInfo = (*itPlayer)->getPlayerInfo();
		TeamCode playerTeam = (*itPlayer)->getMyTeam();
		if (playerTeam == winner)
		{
			interfaces->engine->ClientCommand(identity->pEntity,"chooseteam");
		}
		else if (playerTeam == teamLoser)
		{
			if (isValidPlayerInfo(pInfo))
			{
				if (pInfo->IsFakeClient())
					(*itPlayer)->kick("CSSMatch: Spec Bot");
				else
					pInfo->ChangeTeam(SPEC_TEAM);
			}
		}
	}

	// Prepare a time-out before starting the next match state,
	BaseMatchState * nextState = NULL;
	if ((plugin->getConVar("cssmatch_warmup_time")->GetInt() > 0) && infos->warmup)
	{
		nextState = WarmupMatchState::getInstance();
	}
	else if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
	{
		nextState = HalfMatchState::getInstance();
	}

	if (nextState != NULL)
	{
		int timeoutDuration = plugin->getConVar("cssmatch_end_kniferound")->GetInt();
		if (timeoutDuration > 0)
		{
			TimeoutMatchState::doTimeout(timeoutDuration,nextState);

			//parameters["$team"] = // already set above
			parameters["$time"] = toString(timeoutDuration);
			i18n->i18nChatSay(recipients,"kniferound_dead_time",parameters);
		}
		else
		{
			match->setMatchState(nextState);
		}
	}
	else
	{
		match->stop();
	}
}

void KnifeRoundMatchState::startState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	// Global recipient list
	RecipientFilter recipients;
	recipients.addAllPlayers();

	// Register to the needed events
	map<string,EventCallback>::iterator itEvent;
	for(itEvent = eventCallbacks.begin(); itEvent != eventCallbacks.end(); itEvent++)
	{
		interfaces->gameeventmanager2->AddListener(this,itEvent->first.c_str(),true);
	}

	i18n->i18nChatSay(recipients,"kniferound_restarts");

	match->getInfos()->roundNumber = -2; // a negative round number causes a game restart (see round_start)

	// First restart
	plugin->queueCommand("mp_restartgame 2\n");
}

void KnifeRoundMatchState::endState()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	interfaces->gameeventmanager2->RemoveListener(this);
}

void KnifeRoundMatchState::restartRound()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	MatchInfo * infos = match->getInfos();

	// Back to the first round (round_start will maybe increment that)
	infos->roundNumber = -2;
	// a negative round number causes game restarts until the round number reaches 1

	plugin->queueCommand("mp_restartgame 2\n");
}

void KnifeRoundMatchState::restartState()
{
	restartRound();
}

void KnifeRoundMatchState::showMenu(Player * recipient)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	string language = interfaces->engine->GetClientConVarValue(recipient->getIdentity()->index,"cl_language");
	bool alltalk = plugin->getConVar("sv_alltalk")->GetBool();

	map<string,string> parameters;
	parameters["$action"] = i18n->getTranslation(language,alltalk ? "menu_disable" : "menu_enable");

	if (plugin->getConVar("cssmatch_advanced")->GetBool())
		recipient->sendMenu(menuWithAdmin,1,parameters);
	else
		recipient->sendMenu(kniferoundMenu,1,parameters);
}

void KnifeRoundMatchState::kniferoundMenuCallback(Player * player, int choice, MenuLine * selected)
{
	// 1. Enable/Disable alltalk
	// 2. Restart round
	// 3. Stop the match
	// 3. Clan name detection

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	switch(choice)
	{
	case 1:
		plugin->queueCommand(string("sv_alltalk ") + (plugin->getConVar("sv_alltalk")->GetBool() ? "0\n" : "1\n"));
		player->cexec("cssmatch\n");
		break;
	case 2:
		{
			RecipientFilter recipients;
			map<string,string> parameters;
			IPlayerInfo * pInfo = player->getPlayerInfo();
			PlayerIdentity * identity = player->getIdentity();

			recipients.addAllPlayers();
			if (isValidPlayerInfo(pInfo))
			{
				parameters["$admin"] = pInfo->GetName();
				i18n->i18nChatSay(recipients,"admin_round_restarted_by",parameters,identity->index);
			}
			else
				i18n->i18nChatSay(recipients,"admin_round_restarted");

			match->restartRound();
		}

		player->quitMenu();
		break;
	case 3:
		match->stop();
		player->quitMenu();
		break;
	case 4:
		{
			MatchLignup * lignup = match->getLignup();
			match->detectClanName(T_TEAM);
			match->detectClanName(CT_TEAM);

			RecipientFilter recipients;
			recipients.addRecipient(player);
			map<string,string> parameters;
			parameters["$team1"] = *lignup->clan1.getName();
			parameters["$team2"] = *lignup->clan2.getName();
			i18n->i18nChatSay(recipients,"match_name",parameters);

			player->quitMenu();
		}
		break;
	default:
		player->quitMenu();
	}	
}

void KnifeRoundMatchState::menuWithAdminCallback(Player * player, int choice, MenuLine * selected)
{
	kniferoundMenuCallback(player,choice-1,selected);

	// Here because the above callback could invoke player->quitMenu()
	if (choice == 1)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		plugin->showAdminMenu(player);
	}
}

void KnifeRoundMatchState::FireGameEvent(IGameEvent * event)
{
	try
	{
		(this->*eventCallbacks[event->GetName()])(event);
	}
	catch(const BaseException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

void KnifeRoundMatchState::round_start(IGameEvent * event)
{
	// Do the needed restarts then announce the begin of the knife round

	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	I18nManager * i18n = plugin->getI18nManager();

	switch(match->getInfos()->roundNumber++)
	{
	case -2:
		plugin->queueCommand("mp_restartgame 1\n");
		break;
	case -1:
		plugin->queueCommand("mp_restartgame 2\n");
		break;
	default:
		// Global recipient list
		RecipientFilter recipients;
		recipients.addAllPlayers();
		
		i18n->i18nChatSay(recipients,"kniferound_announcement");
	}
}

void KnifeRoundMatchState::item_pickup(IGameEvent * event)
{
	// Restrict all the weapons but knife

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	string item = event->GetString("item");

	if (item != "knife")
	{
		ClanMember * player = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,event->GetInt("userid"),player)
		{
			if (item == "c4")
			{
				if (! plugin->getConVar("cssmatch_kniferound_allows_c4")->GetBool())
					plugin->addTimer(new ItemRemoveTimer(player,"weapon_c4",false));
			}
			else if (strstr(plugin->getConVar("cssmatch_weapons")->GetString(),item.c_str()) != NULL)
			{
				plugin->addTimer(new ItemRemoveTimer(player,"weapon_" + item,true));
			}
		}
		else
			CSSMATCH_PRINT("Unable to find the player who pickups an item");
	}
}

void KnifeRoundMatchState::player_spawn(IGameEvent * event)
{
	// Set the money for the knife round

	ServerPlugin * plugin = ServerPlugin::getInstance();

	ClanMember * player = NULL;
	CSSMATCH_VALID_PLAYER(PlayerHavingUserid,event->GetInt("userid"),player)
	{
		player->setAccount(plugin->getConVar("cssmatch_kniferound_money")->GetInt());
	}
	else
		CSSMATCH_PRINT("Unable to find the player who spawns");
}

void KnifeRoundMatchState::round_end(IGameEvent * event)
{
	// If the winner is T or CT, end the knife round

	TeamCode winner = (TeamCode)event->GetInt("winner");
	if (winner > SPEC_TEAM)
	{
		endKniferound(winner);
	}
}

/* cf item_pickup
void KnifeRoundMatchState::bomb_beginplant(IGameEvent * event)
{
	// Prevent the bomb from being planted if needed

	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();

	if (! plugin->getConVar("cssmatch_kniferound_allows_c4")->GetBool())
	{
		ClanMember * player = NULL;
		CSSMATCH_VALID_PLAYER(PlayerHavingUserid,event->GetInt("userid"),player)
		{
			PlayerIdentity * identity = player->getIdentity();

			interfaces->helpers->ClientCommand(identity->pEntity,"use weapon_knife");

			RecipientFilter recipients;
			recipients.addRecipient(player);
			i18n->i18nChatSay(recipients,"kniferound_c4");
		}
		else
			CSSMATCH_PRINT("Unable to find the player who plants the bomb");
	}
}*/

ItemRemoveTimer::ItemRemoveTimer(Player * player, const string & item, bool switchKnife)
	: BaseTimer(0.1f), owner(player), toRemove(item), useKnife(switchKnife)
{
}

void ItemRemoveTimer::execute()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ConVar * sv_cheats = plugin->getConVar("sv_cheats");

	ostringstream command;
	command << "ent_remove_all " << toRemove << "\n";

	sv_cheats->m_nValue = 1;
	plugin->executeCommand(command.str());
	sv_cheats->m_nValue = 0;	

	if (useKnife)
	{
		ValveInterfaces * interfaces = plugin->getInterfaces();
		interfaces->helpers->ClientCommand(owner->getIdentity()->pEntity,"use weapon_knife");
	}
}
