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

#include "Decompte.h"

using std::string;
using std::map;
using std::ostringstream;

void termineDecompte(const string & parametre)
{
	Decompte::getInstance()->finDecompte();
}

void afficheDecompte(const string & parametre, const map<string,string> & parametres)
{
	// Calcul du temps en minutes et secondes
	Decompte * decompte = Decompte::getInstance();
	int tempsRestant = decompte->getTempsRestant();
	int secondes = tempsRestant;
	int minutes = (int)(secondes/60);
	secondes -= minutes*60;

	// On affiche le nombre de minutes et de secondes restantes
	ostringstream msg;
	if (secondes >= 10)
	{
		if (minutes >= 10)
			msg << minutes << " : " << secondes;
		else
			msg << "0" <<minutes << " : " << secondes;
	}
	else
	{
		if (minutes >= 10)
			msg << minutes << " : " << "0" << secondes;
		else
			msg << "0" << minutes << " : " << "0" << secondes;
	}
	Messages::hintMsg(msg.str());

	// S'il reste du temps à décompter, lancement d'un nouveau timer
	if (decompte->decTempsRestant()>=0)
	{
		decompte->getMoteur()->addTimer(Timer(1.0f,afficheDecompte,""));
	}
	else
		// Sinon, le décompte est stoppé
		decompte->finDecompte();
}

Decompte::Decompte()
{
	tempsRestant = 0;

	moteur = GestionTimers(2);

	fonction = NULL;
}

Decompte::~Decompte()
{
}

int Decompte::decTempsRestant()
{
	return --tempsRestant;
}

Decompte * Decompte::getInstance()
{
	static Decompte instance;
	return &instance;
}

void Decompte::lanceDecompte(int duree, void (*fonction)(const std::string &,
	const std::map<std::string,std::string> &))
{
	// Aucun décompte ne doit être déjà actif
	finDecompte();

	tempsRestant = duree;
	this->fonction = fonction;

	// Lancement du décompte
	afficheDecompte("",map<string,string>());
}

void Decompte::finDecompte()
{
	// Supression des timers encore en attente
	//	(au cas où un nouveau décompte serait lancé avant la fin du précédent)
	moteur.purge();

	tempsRestant = 0;

	if (fonction != NULL)
	{
		fonction("",map<string,string>());
		// Au cas où finDecompte serait rappelée, réinitialisation de l'adresse de la fonction
		fonction = NULL;
	}
}

int Decompte::getTempsRestant()
{
	return tempsRestant;
}

GestionTimers * Decompte::getMoteur()
{
	return &moteur;
}
