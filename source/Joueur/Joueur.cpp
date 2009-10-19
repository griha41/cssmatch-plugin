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

#include "Joueur.h"

Joueur::Joueur()
{
	index = INDEX_INVALIDE;

	kills = 0;
	killsDernierRound = 0;
	killsDerniereManche = 0;
	deaths = 0;
	deathsDernierRound = 0;
	deathsDerniereManche = 0;

	// Initialisation avec un menu bidon
	menu = Menu();
	pageMenu = 0;

	useridCible = USERID_INVALIDE;
}

Joueur::Joueur(int index)
{
	Joueur();
	index = index;

	kills = 0;
	killsDernierRound = 0;
	killsDerniereManche = 0;
	deaths = 0;
	deathsDernierRound = 0;
	deathsDerniereManche = 0;

	// Initialisation avec un menu bidon
	menu = Menu();
	pageMenu = 0;

	useridCible = USERID_INVALIDE;
}

void Joueur::setIndex(int index)
{
	this->index = index;
}

int Joueur::getIndex() const
{
	return index;
}

int Joueur::getKills() const
{
	return kills;
}

int Joueur::incKills()
{
	return ++kills;
}

int Joueur::decKills()
{
	return --kills;
}

void Joueur::setKills(int kills)
{
	this->kills = kills;
}

int Joueur::getDeaths() const
{
	return deaths;
}

int Joueur::incDeaths()
{
	return ++deaths;
}

int Joueur::decDeaths()
{
	return --deaths;
}

void Joueur::setDeaths(int deaths)
{
	this->deaths = deaths;
}

float Joueur::getRatio() const
{
	if (deaths>0)
		return (float)kills/(float)deaths;
	return (float)kills;
}

void Joueur::sauveStatsRound()
{
	killsDernierRound = kills;
	deathsDernierRound = deaths;
}

void Joueur::sauveStatsManche()
{
	killsDerniereManche = kills;
	deathsDerniereManche = deaths;
}

void Joueur::restoreStatsRound()
{
	kills = killsDernierRound;
	deaths = deathsDernierRound;
}

void Joueur::restoreStatsManche()
{
	kills = killsDerniereManche;
	deaths = deathsDerniereManche;
}

Menu * Joueur::getMenu()
{
	return &menu;
}

void Joueur::setMenu(Menu menu)
{
	pageMenu = 1;
	this->menu = menu;
}

int Joueur::getPageMenu() const
{
	return pageMenu;
}

void Joueur::setPageMenu(int p)
{
	pageMenu = p;
}

int Joueur::incPageMenu()
{
	return ++pageMenu;
}

int Joueur::decPageMenu()
{
	return --pageMenu;
}

int Joueur::getUseridCible() const
{
	return useridCible;
}

void Joueur::setUseridCible(int useridCible)
{
	this->useridCible = useridCible;
}
