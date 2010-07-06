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

#include "MatchClan.h"

#include "../plugin/ServerPlugin.h"
#include "../match/MatchManager.h"
#include "ClanMember.h"

#include <sstream>

using namespace cssmatch;

using std::string;
using std::list;
using std::ostringstream;

MatchClan::MatchClan() : name("Nobody"), allowAutoDetect(true), ready(false)
{
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
		for(itMembers = playerlist->begin(); itMembers != playerlist->end(); itMembers++)
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
	ready = false;
	allowAutoDetect = true;
}

void MatchClan::detectClanName()
{
	if (allowAutoDetect)
	{
		// 1. Get two members
		// 2. Get their names
		// 3. Accumulate consecutive/common characters until we find a clan name than satisfies isValidClanName

		list<ClanMember *> members;
		getMembers(&members);

		list<ClanMember *>::const_iterator itMembers = members.begin();
		list<ClanMember *>::const_iterator lastMembers = members.end();

		// 1.
		if (itMembers != lastMembers)
		{
			// Get the fist member
			ClanMember * member1 = *itMembers;

			// Try to get a second member
			itMembers++;
			if (itMembers != lastMembers)
			{
				// Get the second member
				ClanMember * member2 = *itMembers;


				// 2.
				IPlayerInfo * pInfo1 = member1->getPlayerInfo();
				IPlayerInfo * pInfo2 = member2->getPlayerInfo();
				if (isValidPlayerInfo(pInfo1) && isValidPlayerInfo(pInfo2))
				{
					string memberName1 = pInfo1->GetName();
					string memberName2 = pInfo2->GetName();


					// 3.
					string newName; // new clan name
					bool foundNewName = false; // true if a new clan name was found
					string::const_iterator itMemberName1 = memberName1.begin();
					string::const_iterator endMemberName1 = memberName1.end();
					string::const_iterator itMemberName2 = memberName2.begin();
					string::const_iterator endMemberName2 = memberName2.end();

					// For each character of member1
					//	Is a character common to a character of member2?
					//		Try to accumulate all consecutive/common characters
					//		Is the clan name coherent?
					//			Found a new clan name!
					//	Otherwise continue
					while(itMemberName1 != endMemberName1)
					{
						while((itMemberName1 != endMemberName1) && (itMemberName2 != endMemberName2))
						{
							// (itMemberName1 may changes here)

							if ((*itMemberName1) == (*itMemberName2))
							{ // Found a common character
								newName += *itMemberName1;

								itMemberName1++;
								//itMemberName2++; // see below
							}
							else if (MatchClan::isValidClanName(newName))
							{ // Found a coherent clan name
								setName(newName);
								foundNewName = true;

								// Halt to all the loops !
								itMemberName1 = endMemberName1-1;
								itMemberName2 = endMemberName2-1;
							}
							else
								newName = "";

							itMemberName2++;
						}

						itMemberName1++;
					}

					// If no coherent name was found, we set a neutral name
					if (! foundNewName)
					{
						ostringstream buffer;
						buffer << memberName1 << "'s clan";
						setName(buffer.str());
					}
				}
				else // Error :-(
				{
					CSSMATCH_PRINT("An error occured detecting a clan name !");
					setName("Nobody");
				}
			}
			else // One member in this clan => clan's name = member's name
			{
				IPlayerInfo * pInfo = member1->getPlayerInfo();
				if (isValidPlayerInfo(pInfo))
				{
					name = pInfo->GetName();
				}
				else // Error :-(
				{
					CSSMATCH_PRINT("An error occured detecting a clan name !");
					setName("Nobody");
				}
			}
		}
		else // No member in this clan
		{
			setName("Nobody");
		}
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
