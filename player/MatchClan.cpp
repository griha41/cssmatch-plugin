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

#include "MatchClan.h"

#include "../plugin/ServerPlugin.h"
#include "../match/MatchManager.h"
#include "../configuration/ConfigurationFile.h" // ConfigurationFile.trim
#include "ClanMember.h"

#include <sstream>
#include <map>

using namespace cssmatch;

using std::string;
using std::list;
using std::ostringstream;
using std::map;

MatchClan::MatchClan() : name("?"), allowAutoDetect(true), ready(false)
{}

// I'm static
bool MatchClan::isValidClanName(const string & newName, const list<ClanMember *> & memberlist)
{
    bool valid = false;

    if (newName.size() >= 3)
    {
        int majority = memberlist.size() / 2;
        int occurences = 0;

        list<ClanMember *>::const_iterator itMember = memberlist.begin();
        while(itMember != memberlist.end())
        {
            IPlayerInfo * pInfo = (*itMember)->getPlayerInfo();
            if (isValidPlayerInfo(pInfo))
            {
                string name = pInfo->GetName();
                if (name.find(newName) != string::npos)
                {
                    ++occurences;
                    if (occurences > majority)
                    {
                        valid = true;
                        break;
                    }
                }
            }
            ++itMember;
        }
    }

    return valid;
}

const string * MatchClan::getName() const
{
    return &name;
}

void MatchClan::setName(const string & newName, bool forbidAutoDetect)
{
    name = newName;
    allowAutoDetect = (! forbidAutoDetect);
}

void MatchClan::getMembers(list<ClanMember *> * members)
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    MatchManager * match = plugin->getMatch();

    try
    {
        TeamCode team = (match->getClan(CT_TEAM) == this) ? CT_TEAM : T_TEAM;

        list<ClanMember *> * playerlist = plugin->getPlayerlist();
        list<ClanMember *>::iterator itMembers;
        for(itMembers = playerlist->begin(); itMembers != playerlist->end(); ++itMembers)
        {
            //Msg("%i team%i\n",(*itMembers)->getIdentity()->userid,(*itMembers)->getMyTeam());
            if ((*itMembers)->getMyTeam() == team)
            {
                members->push_back((*itMembers));
                //Msg("=>> added\n");
            }
        }
    }
    catch(const MatchManagerException & e)
    {
        CSSMATCH_PRINT_EXCEPTION(e);
    }
}

ClanStats * MatchClan::getLastHalfState()
{
    return &lastHalfStats;
}

ClanStats * MatchClan::getStats()
{
    return &stats;
}

void MatchClan::reset()
{
    lastHalfStats.scoreT = 0;
    lastHalfStats.scoreCT = 0;
    stats.scoreT = 0;
    stats.scoreCT = 0;
    lastHalfStats.scoreT = 0;
    lastHalfStats.scoreCT = 0;
    ready = false;
    setAllowDetection(true);
    name = "Nobody";
}

void MatchClan::setAllowDetection(bool allow)
{
    allowAutoDetect = allow;
}

void MatchClan::detectClanName(bool force)
{
    // This function looks complex
    // Actually, the clan tag should be found very fast in most cases

    if (allowAutoDetect || force)
    {
        list<ClanMember *> memberlist;
        getMembers(&memberlist);

        list<ClanMember *>::const_iterator itMember = memberlist.begin();
        int majority = memberlist.size()  / 2;

/*		// First, try elect the most used steam clan tag

        map<string,int> tagStats; // clan tag => occurrences
        string mostUsed; // most
        while(itMember != memberlist.end())
        {
            const char * community = "";

            IPlayerInfo * pInfo = (*itMember)->getPlayerInfo();
            if (isValidPlayerInfo(pInfo) && (! pInfo->IsFakeClient()))
                community = interfaces->engine->GetClientConVarValue((*itMember)->getIdentity()->index,"cl_clantag");

            int occurences = ++tagStats[community];
            if (tagStats[mostUsed] < occurences)
                mostUsed = community;

            if (occurences > majority)
                break;

            ++itMember;
        }

        if (! mostUsed.empty())
            setName(mostUsed);
        else
        {
            // Without steam clan tag, try to guess a clan name*/

        switch(memberlist.size())
        {
        case 0:     // no player
            setName("Nobody");
            break;
        case 1:     // 1 player
        {
            IPlayerInfo * pInfo = memberlist.front()->getPlayerInfo();
            if (isValidPlayerInfo(pInfo))
            {
                setName(pInfo->GetName());
            }
            else
                setName("Nobody");
            break;
        }
        default:     // at least 2 players
        {
            // 1. Get two members
            // 2. Get their names
            // 3. Accumulate consecutive/common characters until we find a clan name that satisfies
            // isValidClanName
            // 4. If no valid name was found, continue with two other members

            bool foundName = false;         // true if a new clan name was found
            int iMember = 0;

            // 1.
            itMember = memberlist.begin();
            while((iMember <= majority) && (! foundName))
            {
                list<ClanMember *>::const_iterator itMember2 = itMember;
                ++itMember2;
                while(itMember2 != memberlist.end() && (! foundName))
                {
                    ClanMember * member1 = *itMember;
                    ClanMember * member2 = *itMember2;

                    // 2.
                    IPlayerInfo * pInfo1 = member1->getPlayerInfo();
                    IPlayerInfo * pInfo2 = member2->getPlayerInfo();
                    if (isValidPlayerInfo(pInfo1) && isValidPlayerInfo(pInfo2))
                    {
                        string memberName1 = pInfo1->GetName();
                        string memberName2 = pInfo2->GetName();

                        // 3.
                        string newName;         // new clan name
                        string::const_iterator itMemberName1 = memberName1.begin();
                        string::const_iterator endMemberName1 = memberName1.end();
                        string::const_iterator endMemberName2 = memberName2.end();

                        // For each character of member1
                        //	Is a character common to a character of member2?
                        //		Try to accumulate all consecutive/common characters
                        //		Is the clan name coherent?
                        //			Found a new clan name!
                        //	Otherwise continue
                        while(itMemberName1 != endMemberName1)
                        {
                            string::const_iterator itMemberName2 = memberName2.begin();
                            while((itMemberName1 != endMemberName1) &&
                                  (itMemberName2 != endMemberName2))
                            {
                                // (itMemberName1 may changes here)

                                if ((*itMemberName1) == (*itMemberName2))
                                {         // Found a common character
                                    newName += *itMemberName1;

                                    ++itMemberName1;
                                    //++itMemberName2; // see below
                                }
                                else if (MatchClan::isValidClanName(newName, memberlist))
                                {         // Found a coherent clan name
                                    ConfigurationFile::trim(newName);
                                    setName(newName);
                                    foundName = true;

                                    // Break the two last while
                                    itMemberName1 = endMemberName1 - 1;
                                    itMemberName2 = endMemberName2 - 1;
                                }
                                else
                                    newName = "";

                                ++itMemberName2;
                            }

                            if (itMemberName1 != endMemberName1)
                                ++itMemberName1;
                        }
                    }

                    ++itMember2;
                }
                ++itMember;
                ++iMember;
            }

            // If no coherent name was found, we set a neutral name
            if (! foundName)
            {
                IPlayerInfo * pInfo = memberlist.front()->getPlayerInfo();
                if (isValidPlayerInfo(pInfo))
                {
                    setName(pInfo->GetName());
                }
                else
                {
                    setName("?");
                    CSSMATCH_PRINT("Failed to find a clan name")
                }
            }
        }
        }
        /*}*/
    }
}

bool MatchClan::isReady() const
{
    return ready;
}

void MatchClan::setReady(bool clanReady)
{
    ready = clanReady;
}
