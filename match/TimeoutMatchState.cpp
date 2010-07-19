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

#include "TimeoutMatchState.h"

#include <string>
#include <map>

using namespace cssmatch;

using std::string;
using std::map;

TimeoutMatchState::TimeoutMatchState() : duration(0), nextState(NULL)
{
	timeoutMenu = new Menu(NULL,"menu_time-out",
		new MenuCallback<TimeoutMatchState>(this,&TimeoutMatchState::timeoutMenuCallback));
	timeoutMenu->addLine(true,"menu_alltalk");
	timeoutMenu->addLine(true,"menu_stop");
	timeoutMenu->addLine(true,"menu_retag");
	
	menuWithAdmin = new Menu(NULL,"menu_time-out",
		new MenuCallback<TimeoutMatchState>(this,&TimeoutMatchState::menuWithAdminCallback));
	menuWithAdmin->addLine(true,"menu_administration_options");
	menuWithAdmin->addLine(true,"menu_alltalk");
	menuWithAdmin->addLine(true,"menu_stop");
	menuWithAdmin->addLine(true,"menu_retag");
}

TimeoutMatchState::~TimeoutMatchState()
{
	delete timeoutMenu;
	delete menuWithAdmin;
}

void TimeoutMatchState::doTimeout(int timeoutDuration, BaseMatchState * state) // I'm static
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	TimeoutMatchState * timeoutState = TimeoutMatchState::getInstance();

	timeoutState->duration = timeoutDuration;
	timeoutState->nextState = state;

	match->setMatchState(timeoutState);
}

void TimeoutMatchState::startState()
{
	countdown.fire(duration,nextState);
}

void TimeoutMatchState::endState()
{
	countdown.stop();
}

void TimeoutMatchState::restartRound()
{
	ServerPlugin::getInstance()->queueCommand("mp_restartgame 2\n");
}

void TimeoutMatchState::restartState()
{
	restartRound();
}

void TimeoutMatchState::showMenu(Player * recipient)
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
		recipient->sendMenu(timeoutMenu,1,parameters);
}

void TimeoutMatchState::timeoutMenuCallback(Player * player, int choice, MenuLine * selected)
{
	// 1. Enable/Disable alltalk
	// 2. Stop the match
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
		match->stop();
		player->quitMenu();
		break;
	case 3:
		{
			MatchLignup * lignup = match->getLignup();
			match->detectClanName(T_TEAM,true);
			match->detectClanName(CT_TEAM,true);

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

void TimeoutMatchState::menuWithAdminCallback(Player * player, int choice, MenuLine * selected)
{
	timeoutMenuCallback(player,choice-1,selected);

	// Here because the above callback could invoke player->quitMenu()
	if (choice == 1)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();
		plugin->showAdminMenu(player);
	}
}
