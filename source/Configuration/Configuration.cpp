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
 
#include "Configuration.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;
using std::ostringstream;

Configuration::Configuration(const string & nomFichier)
{
	this->nomFichier = nomFichier;
}

const string & Configuration::getNomFichier() const
{
	return nomFichier;
}

void Configuration::execute() const throw (CSSMatchConfigurationException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IFileSystem * filesystem = cssmatch->getFileSystem();

	// On vérifie que le fichier existe toujours
	//	(Est-ce qu'il aurait pu être supprimé entre l'affichage du menu et le choix dans le menu ? lol)
	//	(La loi de Murphy m'oblige à faire ce test :-P)
	string path("cfg/cssmatch/configurations/" + nomFichier);
	if (filesystem->FileExists(path.c_str(),"MOD"))
		Api::configExecute("cssmatch/configurations/" + nomFichier);
	else
		throw CSSMatchConfigurationException("Unable to find the file \"" + path + "\"");
}
