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

#include "Rapport.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;

Rapport::Rapport(Match * match)
{
	CGlobalVars * gpGlobals = CSSMatch::getInstance()->getGlobalVars();

	// Détermination de la date
	dateLocale = Outils::getDateLocale();

	char datebuf[96];
	strftime(datebuf,sizeof(datebuf),"%Y-%m-%d_%Hh%M",dateLocale);

	// Mémorisation de l'adresse du match concerné
	this->match = match;

	// Construction du nom du fichier
	//string nomFichier(match->getTeam1()->getTag() + "_vs_" + match->getTeam2()->getTag() + "_" + Api::gpGlobals->mapname.ToCStr());
	//Api::valideNomDeFichier(nomFichier);
	string nomFichier(string(datebuf) + "_" + gpGlobals->mapname.ToCStr());

	// Construction du nom final du fichier avec son arborescence (sans l'extention)
	nom = DOSSIER_RAPPORTS_MATCH "/" + nomFichier;

	trouveLignups();
}

Rapport::~Rapport()
{
}

void Rapport::trouveLignups()
{
	// Optimisation des vecteurs
	int maxplayers = CSSMatch::getInstance()->getMaxPlayers();
	int appMaxParTeam = maxplayers/2;
	lignupT.reserve(appMaxParTeam);
	lignupCT.reserve(appMaxParTeam);

	// Remplissage des vecteurs
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromIndex(i);
			IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);
			switch(player->GetTeamIndex())
			{
			case TEAM_T:
				lignupT.push_back(i);
				break;
			case TEAM_CT:
				lignupCT.push_back(i);
				break;
			default:
				lignupSpec.push_back(i);
			}
		}
		catch(const CSSMatchApiException & e){}
	}
}
