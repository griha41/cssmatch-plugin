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

#include "RecipientFilter.h"

#include "../misc/common.h"
#include "../plugin/ServerPlugin.h"
#include "../player/ClanMember.h"

#include <list>
#include <algorithm>

using namespace cssmatch;

using std::vector;
using std::out_of_range;
using std::list;
using std::for_each;

bool RecipientFilter::IsInitMessage() const
{
    return false;
}

bool RecipientFilter::IsReliable() const
{
    return false;
}

int RecipientFilter::GetRecipientCount() const
{
    return recipients.size();
}

void RecipientFilter::addRecipient(Player * recipient)
{
    IPlayerInfo * pInfo = recipient->getPlayerInfo();
    if ((pInfo != NULL) && pInfo->IsConnected() && pInfo->IsPlayer())
    { // isValidPlayerInfo excludes SourceTv
        if (! pInfo->IsFakeClient())
            recipients.push_back(recipient->getIdentity()->index);
    }
}

void RecipientFilter::addRecipient(int index)
{
    recipients.push_back(index);
}

void RecipientFilter::addAllPlayers()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();

    list<ClanMember *> * playerlist = plugin->getPlayerlist();
    for_each(playerlist->begin(), playerlist->end(), PlayerToRecipient(this));
}

int RecipientFilter::GetRecipientIndex(int slot) const
{
    int index = CSSMATCH_INVALID_INDEX;

    try
    {
        index = recipients.at(slot);
    }
    catch(const out_of_range & e) { }

    return index;
}

const vector<int> * RecipientFilter::getVector() const
{
    return &recipients;
}
