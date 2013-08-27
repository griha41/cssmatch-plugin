/*
 * Copyright 2008-2013 Nicolas Maingot
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

#include "MatchManager.h"

#include "BaseMatchState.h"
#include "../configuration/RunnableConfigurationFile.h"
#include "../plugin/ServerPlugin.h"
#include "../misc/common.h"
#include "../player/Player.h"
#include "../player/ClanMember.h"
#include "../sourcetv/TvRecord.h"
#include "../report/XmlReport.h"

#include <algorithm>
#include <sstream>

#include "icommandline.h"

using namespace cssmatch;

using std::string;
using std::list;
using std::for_each;
using std::map;
using std::endl;
using std::ostringstream;

void MatchManager::updateHostname()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ConVar * hostname = plugin->getConVar("hostname");

    string newHostname = hostnameTemplate;

    // Replace %s by the clan names
    size_t clanNameSlot = newHostname.find("%s");
    if (clanNameSlot != string::npos)
    {
        const string * clan1Name = lignup.clan1.getName();
        newHostname.replace(clanNameSlot, 2, *clan1Name, 0, clan1Name->size());
    }

    clanNameSlot = newHostname.find("%s");
    if (clanNameSlot != string::npos)
    {
        const std::string * clan2Name = lignup.clan2.getName();
        newHostname.replace(clanNameSlot, 2, *clan2Name, 0, clan2Name->size());
    }

    // Set the new hostname
    hostname->SetValue(newHostname.c_str());
}

void MatchManager::propagateClanNameChanges(MatchClan * clan)
{
    // Update hostname
    updateHostname();
}

MatchManager::MatchManager(BaseMatchState * iniState) throw(MatchManagerException)
    : initialState(iniState), currentState(NULL)
{
    if (iniState == NULL)
        throw MatchManagerException("Initial match state can't be NULL");

    currentState = iniState;
    currentState->startState();

    eventCallbacks["player_activate"] = &MatchManager::player_activate;
    eventCallbacks["player_disconnect"] = &MatchManager::player_disconnect;
    eventCallbacks["player_team"] = &MatchManager::player_team;
    eventCallbacks["player_changename"] = &MatchManager::player_changename;
}


MatchManager::~MatchManager()
{
    endCountdown.stop();
}

MatchLignup * MatchManager::getLignup()
{
    return &lignup;
}

MatchInfo * MatchManager::getInfos()
{
    return &infos;
}

std::list<TvRecord *> * MatchManager::getRecords()
{
    return &records;
}

BaseMatchState * MatchManager::getInitialState() const
{
    return initialState;
}

MatchClan * MatchManager::getClan(TeamCode code) throw(MatchManagerException)
{
    if (currentState != initialState)
    {
        MatchClan * clan = NULL;

        switch(code)
        {
        case T_TEAM:
            if (infos.halfNumber & 1)
                clan = &lignup.clan1;
            else
                clan = &lignup.clan2;
            break;
        case CT_TEAM:
            if (infos.halfNumber & 1)
                clan = &lignup.clan2;
            else
                clan = &lignup.clan1;
            break;
        default:
            throw MatchManagerException(
                "The plugin tried to grab a clan from an invalid team index");
        }

        return clan;
    }
    else
        throw MatchManagerException("No match in progress");
}

void MatchManager::FireGameEvent(IGameEvent * event)
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

void MatchManager::player_activate(IGameEvent * event)
{
    // Announce any connection

    ClanMember * player = NULL;
    CSSMATCH_VALID_PLAYER(PlayerHavingUserid, event->GetInt("userid"), player)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        I18nManager * i18n = plugin->getI18nManager();
        IPlayerInfo * pInfo = player->getPlayerInfo();
        if (isValidPlayerInfo(pInfo))
        {
            RecipientFilter recipients;
            recipients.addAllPlayers();
            map<string, string> parameters;
            parameters["$username"] = pInfo->GetName();

            i18n->i18nChatSay(recipients, "player_join_game", parameters);
        }
    }
}

void MatchManager::player_disconnect(IGameEvent * event)
{
    // Announce any disconnection

    ServerPlugin * plugin = ServerPlugin::getInstance();
    I18nManager * i18n = plugin->getI18nManager();

    RecipientFilter recipients;
    recipients.addAllPlayers();
    /*map<string, string> parameters;
    parameters["$username"] = event->GetString("name");
    parameters["$reason"] = event->GetString("reason");

    i18n->i18nChatSay(recipients,"player_leave_game",parameters);*/

    // Announce the password too
    map<string, string> passParameters;
    passParameters["$password"] = plugin->getConVar("sv_password")->GetString();
    plugin->addTimer(new TimerI18nChatSay(2.0f, recipients, "match_password_remember",
                                          passParameters));

    // If all the players have disconnected, stop the match (and thus the SourceTv record)
    if ((plugin->getPlayerCount(T_TEAM) + plugin->getPlayerCount(CT_TEAM) - 1) <= 0)
        stop();
}

void MatchManager::player_team(IGameEvent * event)
{
    // Search for any change in the team which requires a new clan name detection
    // i.e. n player to less than 2 players, 0 player to 1 player, 1 player to 2 players

    ServerPlugin * plugin = ServerPlugin::getInstance();
    I18nManager * i18n = plugin->getI18nManager();

    TeamCode newSide = (TeamCode)event->GetInt("team");
    TeamCode oldSide = (TeamCode)event->GetInt("oldteam");

    int playercount = 0;
    TeamCode toReDetect = INVALID_TEAM;
    switch(newSide)
    {
    case T_TEAM:
        toReDetect = T_TEAM;
        playercount = plugin->getPlayerCount(T_TEAM) - 1;
        break;
    case CT_TEAM:
        toReDetect = CT_TEAM;
        playercount = plugin->getPlayerCount(CT_TEAM) - 1;
        break;
    }
    if ((toReDetect != INVALID_TEAM) && (playercount < 2))
    {
        // "< 2" because the game has not update the player's team got via IPlayerInfo yet
        // And that's why we use a timer to redetect the clan's name
        plugin->addTimer(new ClanNameDetectionTimer(1.0f, toReDetect));
    }

    toReDetect = INVALID_TEAM;
    switch(oldSide)
    {
    case T_TEAM:
        toReDetect = T_TEAM;
        playercount = plugin->getPlayerCount(T_TEAM);
        break;
    case CT_TEAM:
        toReDetect = CT_TEAM;
        playercount = plugin->getPlayerCount(CT_TEAM);
        break;
    }
    if ((toReDetect != INVALID_TEAM) && (playercount == 2)) // "== 2" see above
    {
        plugin->addTimer(new ClanNameDetectionTimer(1.0f, toReDetect));
    }

    // http://code.google.com/p/cssmatch-plugin/issues/detail?id=75
}

void MatchManager::player_changename(IGameEvent * event)
{
    // Detect the corresponding clan name if needed
    // i.e. if the player is alone in his team

    ServerPlugin * plugin = ServerPlugin::getInstance();
    I18nManager * i18n = plugin->getI18nManager();

    ClanMember * player = NULL;
    CSSMATCH_VALID_PLAYER(PlayerHavingUserid, event->GetInt("userid"), player)
    {
        TeamCode playerteam = player->getMyTeam();
        if ((playerteam > SPEC_TEAM) && (plugin->getPlayerCount(playerteam) == 1))
            plugin->addTimer(new ClanNameDetectionTimer(1.0f, playerteam));
    }
}

void MatchManager::detectClanName(TeamCode code, bool force) throw(MatchManagerException)
{
    if (currentState != initialState)
    {
        try
        {
            MatchClan * clan = getClan(code);
            clan->detectClanName(force);        

            // Propagate the changes to the hostname, etc.
            propagateClanNameChanges(clan);
        }
        catch(const MatchManagerException & e)
        {
            CSSMATCH_PRINT_EXCEPTION(e);
        }
    }
    else
        throw MatchManagerException("No match in progress");
}

void MatchManager::setClanName(TeamCode code, const string & newName)
{
    // Update the clan's name
    MatchClan * clan = getClan(code);
    clan->setName(newName, true);

    // Propagate the changes to the hostname, etc.
    propagateClanNameChanges(clan);
}

void MatchManager::setMatchState(BaseMatchState * newState)
{
    if (newState == NULL)
        throw MatchManagerException("The news state can't be NULL");

    currentState->endState();
    currentState = newState;
    currentState->startState();
}

BaseMatchState * MatchManager::getMatchState() const
{
    return currentState;
}

void MatchManager::start(RunnableConfigurationFile & config, bool warmup, BaseMatchState * state)
throw(MatchManagerException)
{
    // Make sure there isn't already a match in progress
    if (currentState == initialState)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        ValveInterfaces * interfaces = plugin->getInterfaces();

        // Global recipient list
        RecipientFilter recipients;
        recipients.addAllPlayers();

        // Update match infos
        infos.halfNumber = 1;
        infos.roundNumber = 1;
        infos.kniferoundWinner = NULL;

        // Reset all clan related info
        lignup.clan1.reset();
        lignup.clan2.reset();

        // Reset all player stats
        list<ClanMember *> * playerlist = plugin->getPlayerlist();
        for_each(playerlist->begin(), playerlist->end(), ResetClanMember());

        // Cancel any timers in progress
        plugin->removeTimers();

        // Execute the configuration file
        ConVar * hostname = plugin->getConVar("hostname");
        ConVar * sv_password = plugin->getConVar("sv_password");
        ConVar * cssmatch_hostname = plugin->getConVar("cssmatch_hostname");
        cssmatch_hostname->Revert();
        ConVar * cssmatch_password = plugin->getConVar("cssmatch_password");
        cssmatch_password->Revert();
        string oldPassword = sv_password->GetString();
        config.execute();

        if (strcmp(cssmatch_hostname->GetString(), "") == 0)
        {
            // cssmatch_hostname not used (because deprecated)
            hostnameTemplate = hostname->GetString();
        }
        else
        {
            // cssmatch_hostname used: old config file, backward compatibility
            hostnameTemplate = cssmatch_hostname->GetString();
        }

        // Print the plugin list to the server log
        plugin->queueCommand("plugin_print\n");

        // Save the current date
        infos.startTime = *getLocalTime();

        // Start to listen some events
        map<string, EventCallback>::iterator itEvent;
        for(itEvent = eventCallbacks.begin(); itEvent != eventCallbacks.end(); ++itEvent)
        {
            interfaces->gameeventmanager2->AddListener(this, itEvent->first.c_str(), true);
        }

        // Monitor some variable
        plugin->addTimer(new ConVarMonitorTimer(1.0f, plugin->getConVar("sv_alltalk"), "0",
                                                "sv_alltalk"));
        plugin->addTimer(new ConVarMonitorTimer(1.0f, plugin->getConVar("sv_cheats"), "0",
                                                "sv_cheats"));

        // Set the new server password
        string password;
        if ((strcmp(cssmatch_password->GetString(),
                    "") != 0) && (sv_password->GetString() == oldPassword))
        {
            // cssmatch_password used or sv_password not used: old config file? backward
            // compatibility
            password = cssmatch_password->GetString();
            sv_password->SetValue(password.c_str());
        }
        else
        {
            // cssmatch_password not used (because deprecated) or sv_password used, so ignore
            // cssmatch_password
            password = sv_password->GetString();
        }

        map<string, string> parameters;
        parameters["$password"] = password;
        plugin->addTimer(new TimerI18nPopupSay(5.0f, recipients, "match_password_popup", 5,
                                               parameters));

        // Maybe no warmup is needed
        infos.warmup = warmup;

        // Start the first state
        setMatchState(state);

        // Try to find the clan names
        detectClanName(T_TEAM, false);
        detectClanName(CT_TEAM, false);
    }
    else
        throw MatchManagerException("There already is a match in progress");
}

void MatchManager::stop() throw (MatchManagerException)
{
    // Make sure there is a match in progress
    if (currentState != initialState)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        I18nManager * i18n = plugin->getI18nManager();

        // Send all the announcements
        RecipientFilter recipients;
        recipients.addAllPlayers();

        const string * tagClan1 = lignup.clan1.getName();
        ClanStats * clan1Stats = lignup.clan1.getStats();
        int clan1Score = clan1Stats->scoreCT + clan1Stats->scoreT;
        const string * tagClan2 = lignup.clan2.getName();
        ClanStats * clan2Stats = lignup.clan2.getStats();
        int clan2Score = clan2Stats->scoreCT + clan2Stats->scoreT;

        map<string, string> parameters;
        parameters["$team1"] = *tagClan1;
        parameters["$score1"] = toString(clan1Score);
        parameters["$team2"] = *tagClan2;
        parameters["$score2"] = toString(clan2Score);
        i18n->i18nPopupSay(recipients, "match_end_popup", 6, parameters);
        //i18n->i18nConsoleSay(recipients,"match_end_popup",parameters);

        map<string, string> parametersWinner;
        if (clan1Score > clan2Score)
        {
            parametersWinner["$team"] = *tagClan1;
            i18n->i18nChatSay(recipients, "match_winner", parametersWinner);
        }
        else if (clan1Score < clan2Score)
        {
            parametersWinner["$team"] = *tagClan2;
            i18n->i18nChatSay(recipients, "match_winner", parametersWinner);
        }
        else
        {
            i18n->i18nChatSay(recipients, "match_no_winner");
        }

        // Write the report
        if (plugin->getConVar("cssmatch_report")->GetBool())
        {
            XmlReport report(this);
            report.write();
        }

        // Return to the initial state / context
        switchToInitialState();

        // Do a time-out before returning to the initial config
        int timeoutDuration = plugin->getConVar("cssmatch_end_set")->GetInt();
        if (timeoutDuration > 0)
        {
            if (plugin->getPlayerCount() - 1 > 0) // if the server is empty, we can't add timers
                                                  // because GameFrame is no more executed
            {                                     // - 1 : if a player quit the server the player is
                                                  // still count a while
                map<string, string> timeoutParameters;
                timeoutParameters["$time"] = toString(timeoutDuration);
                plugin->addTimer(new TimerI18nChatSay(2.0f, recipients, "match_dead_time",
                                                      timeoutParameters));
                endCountdown.fire(timeoutDuration);
            }
            else
            {
                endCountdown.finish();
            }
        }
    }
    else
        throw MatchManagerException("No match in progress");
}

void MatchManager::switchToInitialState()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();

    // Stop all event listeners
    interfaces->gameeventmanager2->RemoveListener(this);

    // Stop all pending timers
    plugin->removeTimers();

    // Return to the initial state
    setMatchState(initialState);

    // Remove any old tv record
    for_each(records.begin(), records.end(), TvRecordToRemove());
    records.clear();
}

void MatchManager::showMenu(Player * player)
{
    player->cexec("playgamesound UI/buttonrollover.wav\n");
    currentState->showMenu(player);
}

void MatchManager::restartRound() throw (MatchManagerException)
{
    if (currentState != initialState)
    {
        currentState->restartRound();
    }
    else
        throw MatchManagerException("No match in progress");
}

void MatchManager::restartHalf() throw (MatchManagerException)
{
    if (currentState != initialState)
    {
        currentState->restartState();
    }
    else
        throw MatchManagerException("No match in progress");
}

void MatchManager::sendStatus(RecipientFilter & recipients) const
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    I18nManager * i18n = plugin->getI18nManager();

    // Send a status-like to all players (and SourceTv)
    ostringstream status;
    status << endl << "CSSMatch Status" << endl
           << "  Server IP: " << plugin->getConVar("ip")->GetString() << endl
           << "  VAC: " << ((CommandLine()->FindParm("-insecure") == 0) ? "on" : "off") << endl
           << "  Players:" << endl;
    i18n->consoleSay(recipients, status.str());

    list<ClanMember *> * playerlist = plugin->getPlayerlist();
    list<ClanMember *>::const_iterator itPlayer;
    for (itPlayer = playerlist->begin(); itPlayer != playerlist->end(); ++itPlayer)
    {
        status.str("");
        IPlayerInfo * pInfo = (*itPlayer)->getPlayerInfo();
        if (isValidPlayerInfo(pInfo))
        {
            status << "  - " << pInfo->GetName() << " (" << pInfo->GetNetworkIDString() << ")" <<
            endl;
        }
        i18n->consoleSay(recipients, status.str());
    }
}

void MatchManager::EndOfMatchCountdown::finish()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    string configPatch = DEFAULT_CONFIGURATION_FILE;
    try
    {
        configPatch = plugin->getConVar("cssmatch_default_config")->GetString();

        RunnableConfigurationFile config(string(CFG_FOLDER_PATH) + configPatch);
        config.execute();
    }
    catch(const ConfigurationFileException & e)
    {
        I18nManager * i18n = plugin->getI18nManager();

        RecipientFilter recipients;
        recipients.addAllPlayers();

        map<string, string> parameters;
        parameters["$file"] = configPatch;
        i18n->i18nChatWarning(recipients, "error_file_not_found", parameters);
        i18n->i18nChatWarning(recipients, "match_please_changelevel");
    }
}


ClanNameDetectionTimer::ClanNameDetectionTimer(float delay, TeamCode teamCode)
    : BaseTimer(delay), team(teamCode)
{
}

void ClanNameDetectionTimer::execute()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    MatchManager * manager = plugin->getMatch();
    manager->detectClanName(team, false);
}

ConVarMonitorTimer::ConVarMonitorTimer( float delay,
                                        ConVar * varToWatch,
                                        const string & expectedValue,
                                        const string & warningMessage)
    : BaseTimer(delay), toWatch(varToWatch), value(expectedValue), message(warningMessage)
{
}

void ConVarMonitorTimer::execute()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();

    if (toWatch->GetString() != value)
    {
        I18nManager * i18n = plugin->getI18nManager();

        RecipientFilter recipients;
        recipients.addAllPlayers();

        i18n->i18nCenterSay(recipients, message);
    }

    plugin->addTimer(new ConVarMonitorTimer(1.0f, toWatch, value, message));
}
