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
#include "../Team/Team.h"
#include "../ConVars/ConVars.h"
#include "../EnregistrementTV/EnregistrementTV.h"
#include "../Match/Match.h"
#include "../Joueur/Joueur.h"
#include "../GestionJoueurs/GestionJoueurs.h"

#ifndef __RAPPORT_H__
#define __RAPPORT_H__

/** Classe virtuelle définissant les fonctionnalitées minimales d'une classe écrivant un rapport */
class Rapport
{
protected:
	/** Nom du rapport */
	std::string nom;

	/** Date de génération du fichier */
	struct tm * dateLocale;

	/** Match concernée par le rapport */
	Match * match;

	/** Index des joueurs de la team terroriste */
	std::vector<int> lignupT;
	/** Index des joueurs de la team anti-terroriste */
	std::vector<int> lignupCT;
	/** Index des joueurs spectateurs */
	std::vector<int> lignupSpec;

	/** Entête du fichier
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	virtual void ecritEntete(std::ofstream & fichier) const = 0;

	/** Corps du fichier
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	virtual void ecritCorps(std::ofstream & fichier) const = 0;

	/** Pied du fichier
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	virtual void ecritPied(std::ofstream & fichier) const = 0;

	/** Initialise les vecteurs contenant les index des joueurs en fonction de leur team */
	void trouveLignups();
public:
	/** Prépare la construction du rapport
	 *
	 * @param match Un pointeur vers le match concerné
	 *
	 */
	Rapport(Match * match);

	virtual ~Rapport();

	/** Ecrit le fichier sur le disque */
	virtual void ecrit() = 0;
};

#endif // __RAPPORT_H__
