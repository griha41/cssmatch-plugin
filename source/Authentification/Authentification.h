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

#ifndef __AUTHENTIFICATION_H__
#define __AUTHENTIFICATION_H__

/** Système répertoriant le steamID des arbitres */
class Authentification
{
private:
	/** Liste des steamID arbitres (admins du plugin) */
	std::vector<std::string> adminlist;
public:
	/** Détermine si le steamID est arbitre
	 *
	 * @param steamID Le steamID du joueur
	 * @return true si le joueur est arbitre
	 *
	 */
	bool estAdmin(const std::string & steamID) const;

	/** Log la liste des steamID arbitres */
	void logAdminlist() const;

	/** Ajoute un steamID à authentifier comme arbitre
	 *
	 * @param steamID Le steamID du joueur
	 * @return false si le steamID est déjà authentifié arbitre
	 *
	 */
	bool addAdmin(const std::string & steamID);

	/** Retire un steamID de la liste des authentifiés arbitre
	 *
	 * @param steamID Le steamID du joueur
	 * @return false si le steamID n'est pas authentifié arbitre
	 *
	 */
	bool removeAdmin(const std::string & steamID);

	/** Récupère une liste de steamID à authentifier à partir d'un fichier
	 *
	 * @param pathFichier Le chemin vers le fichier (à partir du dossier cstrike)
	 * @return false si le fichier n'a pu être trouvé ou n'a pu être lu
	 *
	 */
	bool parseAdminlist(const std::string & pathFichier);
};

#endif // __AUTHENTIFICATION_H__
