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
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#include "../Api/Api.h"
#include "../ConVars/ConVars.h"
#include "../CSSMatch/CSSMatch.h"

MRecipientFilter::MRecipientFilter()
{
}

MRecipientFilter::~MRecipientFilter()
{
}

bool MRecipientFilter::IsInitMessage() const
{
	return false;
}

bool MRecipientFilter::IsReliable() const
{
	return false;
}

int MRecipientFilter::GetRecipientCount() const
{
	return m_Recipients.Size();
}

void MRecipientFilter::addAllPlayers()
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	//m_Recipients.RemoveAll();
	for (int i=1;i<=maxplayers;i++)
		addRecipient(i);
}

void MRecipientFilter::addRecipient(int indexJoueur)
{
	try
	{
		edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(pEntity);

		if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
			if (m_Recipients.Find(indexJoueur) == CUtlVector<int>::InvalidIndex())
				m_Recipients.AddToTail(indexJoueur);
	}
	catch(const CSSMatchApiException & e){}
}

int MRecipientFilter::GetRecipientIndex(int slot) const
{
	if (slot<0 || slot>=GetRecipientCount())
		return INDEX_INVALIDE;
	return m_Recipients[slot];
}
