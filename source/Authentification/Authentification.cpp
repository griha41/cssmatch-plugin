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

#include "Authentification.h"
#include "../CfgFile/CfgFile.h"

using std::string;
using std::vector;
using std::ifstream;

bool Authentification::estAdmin(const string & steamID) const
{
	bool admin = false;

	vector<string>::const_iterator premier = adminlist.begin();
	vector<string>::const_iterator dernier = adminlist.end();
	return std::find(premier,dernier,steamID) != dernier;
}

void Authentification::logAdminlist() const
{
	Api::debug("Admin list :");
	vector<string>::const_iterator premier = adminlist.begin();
	vector<string>::const_iterator dernier = adminlist.end();
	std::for_each(premier,dernier,Api::debug);
}

bool Authentification::addAdmin(const string & steamID)
{
	bool succes = false;

	if (! estAdmin(steamID))
	{
		adminlist.push_back(steamID);
		succes = true;
	}

	return succes;
}

bool Authentification::removeAdmin(const string & steamID)
{
	bool succes = false;

	vector<string>::iterator id = adminlist.begin();
	vector<string>::iterator finListe = adminlist.end();
	while(id != finListe)
	{
		if (*id == steamID)
		{
			adminlist.erase(id);
			succes = true;
			id = finListe;
		}
		else
			id++;
	}
	return succes;
}

bool Authentification::parseAdminlist(const string & pathFichier)
{
	bool succes = false;

	ifstream fichier(pathFichier.c_str());

	if (! fichier.fail())
	{
		succes = true;
		adminlist.clear();

		string ligne;
		while(std::getline(fichier,ligne))
		{
			CfgFile::stripEndLine(ligne);
			CfgFile::stripComments(ligne);
			CfgFile::stripString(ligne);
			// Si la ligne n'est pas un commentaire ou une ligne vide...
			if (ligne.size() > 0)
				// ... extraction correcte du steamID puis ajout du steamid à la liste
				adminlist.push_back(ligne);
		}
	}

	return succes;
}
