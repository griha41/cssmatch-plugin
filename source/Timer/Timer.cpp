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

#include "Timer.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;
using std::map;
using std::ostringstream;

Timer::Timer()
{
	dateExecution = -1.0f;
	fonction = NULL;
}

Timer::Timer(float retard, void (*fonction)(const string & parametre, const map<string,string> & informations),
			 const string & parametre, const map<string,string> & informations)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	CGlobalVars * gpGlobals = cssmatch->getGlobalVars();

	// On calcule la date d'exécution de la fonction
	dateExecution = gpGlobals->curtime + retard;
	// On récupère les données relatives à la fonction à exécuter (adresse + paramètre)
	this->fonction = fonction;
	this->parametre = parametre;
	this->informations = informations;
}

float Timer::getDateExecution() const
{
	return dateExecution;
}

const string & Timer::getParametre() const
{
	return parametre;
}

const map<string,string> & Timer::getInformations() const
{
	return informations;
}


void Timer::execute()
{
	// Ré-initialisation (à faire avant l'exécution de la fonction)
	// dateExecution = -1.0f;

	// Si l'adresse de la fonction n'est pas initialisée on sort
	if (fonction != NULL)
	{
		// Appel de la fonction
		fonction(parametre,informations);
	}
	dateExecution = -1.0f;
	fonction = NULL;
	parametre = "";
	informations.clear();
}

bool Timer::estValide() const
{
	// Un timer valide est un timer qui n'a pas encore été exécuté
	return dateExecution>-1.0f;
}

const string Timer::toString() const
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	CGlobalVars * gpGlobals = cssmatch->getGlobalVars();

	ostringstream repr;

	repr << "Running : " << dateExecution << "/" << gpGlobals->curtime
		 << " (parameter=\"" << parametre <<"\")";

	return repr.str();
}
