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

#include "PlayerProp.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;

void PlayerProp::getClasseOffet() throw(CSSMatchPlayerPropException)
{
	// Récupération d'un pointeur sur la liste des classes
	ServerClass * classes = CSSMatch::getInstance()->getServerGameDll()->GetAllServerClasses();

	// Recherche de la classe
	while (classes != NULL)
	{
		if (classe == classes->GetName())
		{
			// Recherche de l'offset
			int nbrProps = classes->m_pTable->GetNumProps();
			for(int i=0;i<nbrProps;i++)
			{
				SendProp * sProp = classes->m_pTable->GetProp(i);
				if (prop == sProp->GetName())
				{
					// L'offset doit être positif
					offset = sProp->GetOffset();
					if (offset < 0)
						offset *= -1;

					i = nbrProps; // arrêt de la boucle
				}
			}
		}

		classes = classes->m_pNext;
	}

	if (offset == 0)
		throw CSSMatchPlayerPropException(string("Unable to find the offset of the prop ") + classe + "." + prop + " !");
}

PlayerProp::PlayerProp()
{
	offset = 0;
}

PlayerProp::PlayerProp(const string & classe, const string & nom)
{
	offset = 0;
	this->classe = classe;
	prop = nom;

	try
	{
		getClasseOffet();
	}
	catch(const CSSMatchPlayerPropException & e)
	{
		Api::reporteException(e,__FILE__,__LINE__);
	}
}

const string & PlayerProp::getClasse() const
{
	return classe;
}

const string & PlayerProp::getProp() const
{
	return prop;
}
