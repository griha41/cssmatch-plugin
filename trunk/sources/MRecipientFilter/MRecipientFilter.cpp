/* 
 * Copyright 2007, 2008 Nicolas Maingot
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
 * Portions of this code are also Copyright � 1996-2005 Valve Corporation, All rights reserved
 */

#include "../API/API.h"
// #include "MRecipientFilter.h" // API.h

MRecipientFilter::MRecipientFilter(void)
{
}

MRecipientFilter::~MRecipientFilter(void)
{
}

int MRecipientFilter::GetRecipientCount() const
{
	return m_Recipients.Size();
}

int MRecipientFilter::GetRecipientIndex(int slot) const
{
	if ( slot < 0 || slot >= GetRecipientCount() )
		return -1;

	return m_Recipients[ slot ];
}

bool MRecipientFilter::IsInitMessage() const
{
	return false;
}

bool MRecipientFilter::IsReliable() const
{
	return false;
}

void MRecipientFilter::AddAllPlayers()
{
	m_Recipients.RemoveAll();
	for ( int i = 1; i <= API::maxplayers; i++ )
	{
		edict_t *pPlayer = API::engine->PEntityOfEntIndex(i);
		if ( pPlayer && !pPlayer->IsFree())
			if (API::engine->GetPlayerUserId(pPlayer)!=-1)
				AddRecipient(i);
				//AddRecipient(engine->IndexOfEdict(pPlayer));
		//m_Recipients.AddToTail(i);
	}
}

void MRecipientFilter::AddRecipient( int iPlayer )
{
	// D�j� dans la liste ?
	if (m_Recipients.Find(iPlayer)!=m_Recipients.InvalidIndex())
		return;

	m_Recipients.AddToTail( iPlayer );
}