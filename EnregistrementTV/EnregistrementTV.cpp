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

#include "EnregistrementTV.h"

using std::string;
using std::map;
using std::ostringstream;
using std::ifstream;

#include "../CSSMatch/CSSMatch.h"

void timerStoppeRecords(const string & parametre, const map<string,string> & parametres)
{
	Api::serveurExecute("tv_stoprecord\n");
}

EnregistrementTV::EnregistrementTV()
{
	nom = "";
	enregistrant = false;
}

EnregistrementTV::EnregistrementTV(const string & fichierSansPathSansExtension) throw(CSSMatchTVException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IFileSystem * filesystem = cssmatch->getFileSystem();

	// Est-ce que SourceTV est prête ?
	if (ConVars::tv_enable == NULL)
		throw CSSMatchTVException("error_tv_enable_not_found");
	if (! ConVars::tv_enable->GetBool())
		throw CSSMatchTVException("error_tv_enable_0");
	if (! Api::hltvPresente())
		throw CSSMatchTVException("error_tv_not_connected");

	string fichier(fichierSansPathSansExtension);

	enregistrant = false;

	// On supprime les caractères génant pour Windows ou pour la console
	Outils::valideNomDeFichier(fichier);

	// On construit le path relatif du fichier
	string path(ConVars::cssmatch_sourcetv_path.GetString());
	if (path.size() != 0)
		fichier = path + "/" + fichier + ".dem";

	const char * fichierC = fichier.c_str();

	nom = fichierC;

	// Correction du fait que SourceTV écrase les enregistrements du même nom
	int nbrFichier = 1;
	while (filesystem->FileExists(nom.c_str(), "MOD"))
	{
		ostringstream renomme;
		renomme << "[" << nbrFichier << "]" << fichierC;
		nom = renomme.str();
		nbrFichier++;
	}
}

void EnregistrementTV::enregistre() throw (CSSMatchTVException)
{
	if (nom.empty())
		throw CSSMatchTVException("Unable lauch a record with an empty name");

	string cmd("tv_record " + nom + "\n");

	Api::serveurExecute("tv_stoprecord\n");
	Api::serveurExecute("tv_autorecord 0\n");
	Api::serveurExecute(cmd);

	enregistrant = true;
}

void EnregistrementTV::coupe()
{
	CSSMatch::getInstance()->getTimers()->addTimer(Timer(2.0f,timerStoppeRecords,""));
	enregistrant = false;
}

const string & EnregistrementTV::getNom() const
{
	return nom;
}

bool EnregistrementTV::getEnregistrant() const
{
	return enregistrant;
}
