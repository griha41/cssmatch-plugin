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

#ifndef __LIGNE_H__
#define __LIGNE_H__

/** Une ligne d'un Menu */
class Ligne
{
private:
	/** Texte associé à la ligne */
	std::string cle;

	/** Valeur associée à la ligne (ID ou code retour/suivant) <br>
	 * Le but de cette valeur est, par exemple, d'éviter qu'un joueur se nommant "Retour" ne perturbe le menu...
	 */
	int valeur;	

public:
	/** Constructeur implémenté en cas de stockage dans un vector susceptible de subir un "rezise" */
	Ligne();

	/** Construit une ligne éventuellement associée à une valeur 
	 *
	 * @param texte Texte associé à la ligne
	 * @param valeur Valeur associée à la ligne (ex : un userid choisit pour cible par un arbitre)
	 *
	 */
	Ligne(const std::string & texte, int valeur = 0);

	/** Accesseur sur le texte associé à la ligne
	 *
	 * @return Le texte
	 *
	 */
	const std::string & getTexte() const;

	/** Accesseur sur la valeur associée au texte de la ligne 
	 *
	 * @return La valeur associée à la ligne
	 *
	 */
	int getValeur() const;
};

#endif // __LIGNE_H__
