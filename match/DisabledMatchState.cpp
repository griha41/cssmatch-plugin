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

#include "DisabledMatchState.h"

#include "igameevents.h"
#include "filesystem.h"

#include "../messages/Countdown.h"
#include "../player/Player.h"
#include "../plugin/ServerPlugin.h"
#include "../configuration/RunnableConfigurationFile.h"
#include "MatchManager.h"
#include "KnifeRoundMatchState.h"
#include "WarmupMatchState.h"
#include "HalfMatchState.h"

#include <map>
#include <string>

using namespace cssmatch;
using std::map;
using std::string;

DisabledMatchState::DisabledMatchState()
{
    disabledMenu = new Menu(NULL, "menu_no_match",
                            new MenuCallback<DisabledMatchState>(this,
                                                                 &DisabledMatchState::
                                                                    disabledMenuCallback));
    disabledMenu->addLine(true, "menu_alltalk");
    disabledMenu->addLine(true, "menu_start");

    menuWithAdmin = new Menu(NULL, "menu_no_match",
                             new MenuCallback<DisabledMatchState>(this,
                                                                  &DisabledMatchState::
                                                                    menuWithAdminCallback));
    menuWithAdmin->addLine(true, "menu_administration_options");
    menuWithAdmin->addLine(true, "menu_alltalk");
    menuWithAdmin->addLine(true, "menu_start");

    kniferoundQuestion = new Menu(NULL, "menu_play_kniferound",
                                  new MenuCallback<DisabledMatchState>(this,
                                                                       &DisabledMatchState::
                                                                        kniferoundQuestionCallback));
    kniferoundQuestion->addLine(true, "menu_yes");
    kniferoundQuestion->addLine(true, "menu_no");
    kniferoundQuestion->addLine(true, "menu_back");

    warmupQuestion = new Menu(kniferoundQuestion, "menu_play_warmup",
                              new MenuCallback<DisabledMatchState>(this,
                                                                   &DisabledMatchState::
                                                                        warmupQuestionCallback));
    warmupQuestion->addLine(true, "menu_yes");
    warmupQuestion->addLine(true, "menu_no");
}

DisabledMatchState::~DisabledMatchState()
{
    delete disabledMenu;
    delete menuWithAdmin;
    delete kniferoundQuestion;
    delete warmupQuestion;
}

void DisabledMatchState::startState()
{
}

void DisabledMatchState::endState()
{
}

void DisabledMatchState::restartRound()
{
    ServerPlugin::getInstance()->queueCommand("mp_restartgame 2\n");
}

void DisabledMatchState::restartState()
{
    restartRound();
}

void DisabledMatchState::showMenu(Player * recipient)
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
        recipient->sendMenu(disabledMenu, 1, parameters);
}

void DisabledMatchState::disabledMenuCallback(Player * player, int choice, MenuLine * selected)
{
    // 1. Enable/Disable Alltalk
    // 2. Start a match

    ServerPlugin * plugin = ServerPlugin::getInstance();
    switch(choice)
    {
    case 1:
        plugin->queueCommand(string("sv_alltalk ") +
                             (plugin->getConVar("sv_alltalk")->GetBool() ? "0\n" : "1\n"));
        player->cexec("cssmatch\n");
        break;
    case 2:
        showKniferoundQuestion(player);
        break;
    default:
        player->quitMenu();
    }
}

void DisabledMatchState::menuWithAdminCallback(Player * player, int choice, MenuLine * selected)
{
    disabledMenuCallback(player, choice-1, selected);

    // Here because the above callback could invoke player->quitMenu()
    if (choice == 1)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        plugin->showAdminMenu(player);
    }
}

void DisabledMatchState::showKniferoundQuestion(Player * recipient)
{
    recipient->sendMenu(kniferoundQuestion, 1);
}

void DisabledMatchState::kniferoundQuestionCallback(Player * player, int choice,
                                                    MenuLine * selected)
{
    // Kniferound?
    // 1. Yes
    // 2. No
    // 3. Back

    switch(choice)
    {
    case 1:
        player->storeMenuData(new MatchMenuLineData(KnifeRoundMatchState::getInstance(), true));
        showWarmupQuestion(player);
        break;
    case 2:
        player->storeMenuData(new MatchMenuLineData(NULL, true));
        showWarmupQuestion(player);
        break;
    case 3:
        player->cexec("cssmatch\n");
        break;
    default:
        player->quitMenu();
    }
}

void DisabledMatchState::showWarmupQuestion(Player * recipient)
{
    recipient->sendMenu(warmupQuestion, 1);
}

void DisabledMatchState::warmupQuestionCallback(Player * player, int choice, MenuLine * selected)
{
    // Warmup?
    // 1. Yes
    // 2. No
    // 3. Back

    MatchMenuLineData * const matchSettings =
        static_cast<MatchMenuLineData * const>(player->getMenuData());
    switch(choice)
    {
    case 1:
        matchSettings->warmup = true;
        if (matchSettings->state == NULL)
            matchSettings->state = WarmupMatchState::getInstance();
        showConfigQuestion(player);
        break;
    case 2:
        matchSettings->warmup = false;
        if (matchSettings->state == NULL)
            matchSettings->state = HalfMatchState::getInstance();
        showConfigQuestion(player);
        break;
    default:
        player->quitMenu();
    }
}

void DisabledMatchState::showConfigQuestion(Player * recipient)
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();

    Menu * configQuestion = new Menu(warmupQuestion, "menu_config",
                                     new MenuCallback<DisabledMatchState>(this,
                                                                          &DisabledMatchState::
                                                                          configQuestionCallback));

    // Search for all .cfg files into cfg/cssmatch/configurations
    FileFindHandle_t fh;
    const char * cfg = interfaces->filesystem->FindFirstEx(
        "cfg/" MATCH_CONFIGURATIONS_PATH "/*.cfg", "MOD", &fh);
    while(cfg != NULL)
    {
        string filename = cfg;
        string copy = filename;
        if (normalizeFileName(filename))
        {
            interfaces->filesystem->RenameFile(
                ("cfg/" MATCH_CONFIGURATIONS_PATH "/" + copy).c_str(),
                ("cfg/" MATCH_CONFIGURATIONS_PATH "/" + filename).c_str());
            plugin->log(copy + " has been renamed to " + filename);
        }

        configQuestion->addLine(false, filename);

        cfg = interfaces->filesystem->FindNext(fh);
    }

    recipient->sendMenu(configQuestion, 1, I18nManager::WITHOUT_PARAMETERS, true);
}

void DisabledMatchState::configQuestionCallback(Player * player, int choice, MenuLine * selected)
{
    if (choice != 10)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        MatchManager * match = plugin->getMatch();
        MatchMenuLineData * const matchSettings =
            static_cast<MatchMenuLineData * const>(player->getMenuData());

        try
        {
            RunnableConfigurationFile config(
                CFG_FOLDER_PATH MATCH_CONFIGURATIONS_PATH + selected->text);

            /*RecipientFilter recipients;
            recipients.addAllPlayers();

            PlayerIdentity * identity = player->getIdentity();
            IPlayerInfo * pInfo = player->getPlayerInfo();
            if (isValidPlayerInfo(pInfo))
            {
                parameters["$admin"] = pInfo->GetName();
                i18n->i18nChatSay(recipients,"match_started_by",parameters,identity->index);
            }
            else
                i18n->i18nChatSay(recipients,"match_started");*/

            match->start(config, matchSettings->warmup, matchSettings->state);
        }
        catch(const ConfigurationFileException & e)
        {
            I18nManager * i18n = plugin->getI18nManager();
            map<string, string> parameters;
            parameters["$file"] = selected->text;

            i18n->i18nMsg("error_file_not_found", parameters);
        }
        //catch(const MatchManagerException & e)
        //{
        // match in progress
        //}
    }

    player->quitMenu();
}
