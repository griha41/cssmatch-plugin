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

#include "WarmupMatchState.h"

#include "../plugin/ServerPlugin.h"
#include "../player/ClanMember.h"
#include "../messages/I18nManager.h"
#include "MatchManager.h"
#include "DisabledMatchState.h"
#include "HalfMatchState.h"

using namespace cssmatch;

using std::string;
using std::list;
using std::map;

WarmupMatchState::WarmupMatchState() : finished(false)

{
    warmupMenu = new Menu(NULL, "menu_warmup",
                          new MenuCallback<WarmupMatchState>(this,
                                                             &WarmupMatchState::warmupMenuCallback));
    warmupMenu->addLine(true, "menu_alltalk");
    warmupMenu->addLine(true, "menu_restart");
    warmupMenu->addLine(true, "menu_stop");
    warmupMenu->addLine(true, "menu_retag");
    warmupMenu->addLine(true, "menu_go");

    menuWithAdmin = new Menu(NULL, "menu_warmup",
                             new MenuCallback<WarmupMatchState>(this,
                                                                &WarmupMatchState::
                                                                    menuWithAdminCallback));
    menuWithAdmin->addLine(true, "menu_administration_options");
    menuWithAdmin->addLine(true, "menu_alltalk");
    menuWithAdmin->addLine(true, "menu_restart");
    menuWithAdmin->addLine(true, "menu_stop");
    menuWithAdmin->addLine(true, "menu_retag");
    menuWithAdmin->addLine(true, "menu_go");

    eventCallbacks["player_spawn"] = &WarmupMatchState::player_spawn;
    eventCallbacks["round_start"] = &WarmupMatchState::round_start;
    eventCallbacks["item_pickup"] = &WarmupMatchState::item_pickup;
    //eventCallbacks["bomb_beginplant"] = &WarmupMatchState::bomb_beginplant;
}

WarmupMatchState::~WarmupMatchState()
{
    delete warmupMenu;
    delete menuWithAdmin;
}

void WarmupMatchState::endWarmup()
{
    if (! finished) // cssm_go after the warmup finished?
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        MatchManager * match = plugin->getMatch();
        /*I18nManager * i18n = plugin->getI18nManager();

        RecipientFilter recipients;
        recipients.addAllPlayers();

        i18n->i18nChatSay(recipients,"warmup_end");*/

        finished = true;

        if (plugin->getConVar("cssmatch_sets")->GetInt() > 0)
        {
            match->setMatchState(HalfMatchState::getInstance());
        }
        else
        {
            match->stop();
        }
    }
}

void WarmupMatchState::removeC4()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ConVar * sv_cheats = plugin->getConVar("sv_cheats");

    sv_cheats->m_nValue = 1;
    plugin->executeCommand("ent_remove weapon_c4\n");
    sv_cheats->m_nValue = 0;
}

void WarmupMatchState::doGo(Player * player)
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    MatchManager * match = plugin->getMatch();
    I18nManager * i18n = plugin->getI18nManager();

    RecipientFilter recipients;
    recipients.addAllPlayers();
    map<string, string> parameters;

    try
    {
        TeamCode team = player->getMyTeam();
        MatchClan * clan = match->getClan(team);

        MatchLignup * lignup = match->getLignup();
        MatchClan * otherClan = (&lignup->clan1 != clan) ? &lignup->clan1 : &lignup->clan2;

        parameters["$team"] = *clan->getName();
        if (clan->isReady())
        {
            // Clan already "ready"
            i18n->i18nChatSay(recipients, "warmup_already_ready", parameters,
                              player->getIdentity()->index);
        }
        else
        {
            clan->setReady(true);

            // If both clan1 and clan2 are ready, end the warmup
            if (otherClan->isReady())
            {
                i18n->i18nChatSay(recipients, "warmup_all_ready");
                endWarmup();
            }
            // Ohterwise just announce that this clan is ready
            else
            {
                i18n->i18nChatSay(recipients, "warmup_ready", parameters,
                                  player->getIdentity()->index);
            }
        }
    }
    catch(const MatchManagerException & e)
    {
        CSSMATCH_PRINT_EXCEPTION(e);
    }
}

void WarmupMatchState::startState()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();
    MatchManager * match = plugin->getMatch();

    MatchLignup * lignup = match->getLignup();
    lignup->clan1.setReady(false); // override any previous warmup settings
    lignup->clan2.setReady(false);

    finished = false; // (Warmup not finished yet)

    plugin->queueCommand("mp_restartgame 2\n");

    // Subscribe to the needed game events
    map<string, EventCallback>::iterator itEvent;
    for(itEvent = eventCallbacks.begin(); itEvent != eventCallbacks.end(); ++itEvent)
    {
        interfaces->gameeventmanager2->AddListener(this, itEvent->first.c_str(), true);
    }

    match->getInfos()->roundNumber = 1;
}

void WarmupMatchState::endState()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();

    interfaces->gameeventmanager2->RemoveListener(this);

    countdown.stop();
}

void WarmupMatchState::restartRound()
{
    ServerPlugin::getInstance()->queueCommand("mp_restartgame 2\n");
}

void WarmupMatchState::restartState()
{
    restartRound();
}

void WarmupMatchState::showMenu(Player * recipient)
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();
    I18nManager * i18n = plugin->getI18nManager();
    string language = interfaces->engine->GetClientConVarValue(
        recipient->getIdentity()->index, "cl_language");
    bool alltalk = plugin->getConVar("sv_alltalk")->GetBool();

    map<string, string> parameters;
    parameters["$action"] = i18n->getTranslation(language, alltalk ? "menu_disable" : "menu_enable");

    if (plugin->getConVar("cssmatch_advanced")->GetBool())
        recipient->sendMenu(menuWithAdmin, 1, parameters);
    else
        recipient->sendMenu(warmupMenu, 1, parameters);
}

void WarmupMatchState::warmupMenuCallback(Player * player, int choice, MenuLine * selected)
{
    // 1. Enable/Disable alltalk
    // 2. Round restart
    // 3. Clan names detection
    // 4. Force !go

    ServerPlugin * plugin = ServerPlugin::getInstance();
    MatchManager * match = plugin->getMatch();
    I18nManager * i18n = plugin->getI18nManager();

    switch(choice)
    {
    case 1:
        plugin->queueCommand(string("sv_alltalk ") +
                             (plugin->getConVar("sv_alltalk")->GetBool() ? "0\n" : "1\n"));
        player->cexec("cssmatch\n");
        break;
    case 2:
    {
        RecipientFilter recipients;
        map<string, string> parameters;
        IPlayerInfo * pInfo = player->getPlayerInfo();
        PlayerIdentity * identity = player->getIdentity();

        recipients.addAllPlayers();
        if (isValidPlayerInfo(pInfo))
        {
            parameters["$admin"] = pInfo->GetName();
            i18n->i18nChatSay(recipients, "admin_round_restarted_by", parameters, identity->index);
        }
        else
            i18n->i18nChatSay(recipients, "admin_round_restarted");

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
        match->detectClanName(T_TEAM, true);
        match->detectClanName(CT_TEAM, true);

        RecipientFilter recipients;
        recipients.addRecipient(player);
        map<string, string> parameters;
        parameters["$team1"] = *lignup->clan1.getName();
        parameters["$team2"] = *lignup->clan2.getName();
        i18n->i18nChatSay(recipients, "match_name", parameters);

        player->quitMenu();
    }
    break;
    case 5:
    {
        RecipientFilter recipients;
        map<string, string> parameters;
        IPlayerInfo * pInfo = player->getPlayerInfo();
        PlayerIdentity * identity = player->getIdentity();

        recipients.addAllPlayers();
        if (isValidPlayerInfo(pInfo))
        {
            parameters["$admin"] = pInfo->GetName();
            i18n->i18nChatSay(recipients, "admin_all_teams_say_ready_by", parameters,
                              identity->index);
        }
        else
            i18n->i18nChatSay(recipients, "admin_all_teams_say_ready");

        endWarmup();
    }
        player->quitMenu();
        break;
    default:
        player->quitMenu();
    }
}

void WarmupMatchState::menuWithAdminCallback(Player * player, int choice, MenuLine * selected)
{
    warmupMenuCallback(player, choice-1, selected);

    // Here because the above callback could invoke player->quitMenu()
    if (choice == 1)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        plugin->showAdminMenu(player);
    }
}

void WarmupMatchState::FireGameEvent(IGameEvent * event)
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

void WarmupMatchState::player_spawn(IGameEvent * event)
{
    // Make each player impervious to bullets

    ClanMember * player = NULL;
    CSSMATCH_VALID_PLAYER(PlayerHavingUserid, event->GetInt("userid"), player)
    {
        player->setLifeState(0);
    }
}

void WarmupMatchState::round_start(IGameEvent * event)
{
    // Do the restarts and announce the begin of each warmup round

    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();
    MatchManager * match = plugin->getMatch();
    MatchInfo * infos = match->getInfos();
    I18nManager * i18n = plugin->getI18nManager();

    RecipientFilter recipients;

/*	switch(infos->roundNumber++)
    {
    case -2:
        plugin->queueCommand("mp_restartgame 1\n");
        break;
    case -1:
        plugin->queueCommand("mp_restartgame 2\n");
        break;
    case 0:
        {*/
    if (infos->roundNumber == 1)
    {
        int duration = plugin->getConVar("cssmatch_warmup_time")->GetInt()*60;

        countdown.fire(duration);

        // Increment the round number so a restart will not re-lauch the countdown
        ++infos->roundNumber;
    }
    /*	}
        break;
    default:*/
    recipients.addAllPlayers();
    i18n->i18nChatSay(recipients, "warmup_announcement");
    /*	break;
    }*/
}

void WarmupMatchState::item_pickup(IGameEvent * event)
{
    // Restrict C4

    string item = event->GetString("item");

    if (item == "c4")
    {
        removeC4();
    }
}

/* cf : item_pickup
void WarmupMatchState::bomb_beginplant(IGameEvent * event)
{
    // Prevent the bomb from being planted if needed

    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();
    I18nManager * i18n = plugin->getI18nManager();

    ClanMember * player = NULL;
    CSSMATCH_VALID_PLAYER(PlayerHavingUserid,event->GetInt("userid"),player)
    {
        PlayerIdentity * identity = player->getIdentity();

        interfaces->helpers->ClientCommand(identity->pEntity,"use weapon_knife");

        RecipientFilter recipients;
        recipients.addRecipient(player);
        i18n->i18nChatSay(recipients,"warmup_c4");
    }
    else
        CSSMATCH_PRINT("Unable to find the player who plants the bomb");

}*/
