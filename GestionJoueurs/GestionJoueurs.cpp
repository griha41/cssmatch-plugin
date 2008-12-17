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

#include "GestionJoueurs.h"

using std::vector;

GestionJoueurs::GestionJoueurs()
{
}

GestionJoueurs::~GestionJoueurs()
{
}

GestionJoueurs * GestionJoueurs::getInstance()
{
	static GestionJoueurs instance;
	return &instance;
}

void GestionJoueurs::setTaillePlayerList(int taille)
{
	joueurs.resize(taille);
}

void GestionJoueurs::connexion(int indexJoueur)
{
	// On créer une nouvelle instance de Joueur sur l'index
	joueurs[indexJoueur] = Joueur(indexJoueur);
}

Joueur * GestionJoueurs::getJoueur(int indexJoueur)
{
	Joueur * joueur = NULL;
	if (Api::isValideIndex(indexJoueur))
		joueur = &joueurs[indexJoueur];
	else
	{
		joueur = &joueurs[0]; // L'index 0 n'est jamais utilisé, il ne correspond à aucun joueur
		Api::debug("Player manager recieved an invalid index !"); 
	}
	return joueur;
}

void GestionJoueurs::sauvStatsRound()
{
	vector<Joueur>::iterator joueur = joueurs.begin();
	vector<Joueur>::const_iterator dernierJoueur = joueurs.end();
	while(joueur != dernierJoueur)
	{
		joueur->sauveStatsRound();
		joueur++;
	}
}

void GestionJoueurs::restaureStatsRound()
{
	vector<Joueur>::iterator joueur = joueurs.begin();
	vector<Joueur>::const_iterator dernierJoueur = joueurs.end();
	while(joueur != dernierJoueur)
	{
		joueur->restoreStatsRound();
		joueur++;
	}
}

void GestionJoueurs::sauvStatsManche()
{
	vector<Joueur>::iterator joueur = joueurs.begin();
	vector<Joueur>::const_iterator dernierJoueur = joueurs.end();
	while(joueur != dernierJoueur)
	{
		joueur->sauveStatsManche();
		joueur++;
	}
}

void GestionJoueurs::restaureStatsManche()
{
	vector<Joueur>::iterator joueur = joueurs.begin();
	vector<Joueur>::const_iterator dernierJoueur = joueurs.end();
	while(joueur != dernierJoueur)
	{
		joueur->restoreStatsManche();
		joueur++;
	}
}

