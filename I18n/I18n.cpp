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


#include "I18n.h"
#include "../CfgFile/CfgFile.h"

using std::string;
using std::map;
using std::runtime_error;
using std::locale;
using std::ofstream;
using std::ifstream;

map<string,string> I18n::SANS_PARAMETRE = map<string,string>();

I18n::I18n()
{
}

I18n::~I18n()
{
}

I18n * I18n::getInstance()
{
	static I18n instance;
	return &instance;
}

/*string I18n::getNomLocale(const string & nomLangage)
{
	string resultat;

	string convar = ConVars::cssmatch_language.GetString(); // La ConVar prime sur la locale
	if (convar.size() == 0)
	{
		locale loc;
		try
		{
			loc = locale(nomLangage.c_str());
		}
		catch(const runtime_error & e) // Locale non trouvée
		{
			loc = locale("");
		}
		resultat = loc.name();
		size_t posSoulign = resultat.find("_");
		if (posSoulign > 0)
		resultat = resultat.substr(0,posSoulign);
	}
	else
		resultat = convar;

	return resultat;
}*/

std::string I18n::getNomFichier(const std::string & nomLangage)
{
	// Est-ce qu'une traduction correspond à ce langage ?
	string nomFichier = string("cstrike_beta/cfg/cssmatch/languages/") + nomLangage + ".txt";
	ifstream fichierEnLecture(nomFichier.c_str());
	if (fichierEnLecture.fail())
	{
		// On utilisera le langage par défaut
		nomFichier = string("cstrike_beta/cfg/cssmatch/languages/") + ConVars::cssmatch_language.GetString() + ".txt";
	}

	return nomFichier;
}

string I18n::getMessage(const std::string & nomLocale, const string & tag, const map<string,string> & parametres) throw(CSSMatchI18nException)
{
	string nomFichier = getNomFichier(nomLocale);

	// Est-ce que le langage a déjà été utilisé (c-à-d est dans le cache)
	map<string,string> * langage = &traductions[nomLocale];
	if (langage->size() == 0) // Pas de find, car si l'élément n'existe pas nous voulons le créer
	{ // Le langage n'est pas le cache

		// Mise en cache
		try
		{
			CfgFile fichierLangage(nomFichier);
			(*langage) = *fichierLangage.getMap();

			// Les retours chariot doivent être pris en compte
			map<string,string>::iterator itTraduction = langage->begin();
			map<string,string>::iterator derniereTraduction = langage->end();
			while(itTraduction != derniereTraduction)
			{
				string * phrase = &itTraduction->second;
				size_t posRetourChariot = string::npos;
				// FIXME : avec et sans posRetourChariot dans find
				//while ((posRetourChariot = phrase->find("\\n", posRetourChariot invalide)) != string::npos)
				while ((posRetourChariot = phrase->find("\\n")) != string::npos)
					phrase->replace(posRetourChariot,2,"\n",0,1);

				itTraduction++;
			}
		}
		catch(const CfgFileException & e)
		{
			throw CSSMatchI18nException(e.what());
		}
	}
	
	// Est-ce que la traduction a pu être trouvée ?
	string phrase((*langage)[tag]); // Pas de find pour éviter 2 parcours de la map
										// La clé devrait de toute façon exister
								// Récupérer une copie ! Sinon les $variables ne seront remplacées qu'au premier appel
	if (phrase.size() != 0)
	{
		// Substitution des paramètres
		map<string,string>::const_iterator parametre = parametres.begin();
		map<string,string>::const_iterator dernierParametre = parametres.end();
		while(parametre != dernierParametre)
		{
			const string * nom = &parametre->first;

			const string * valeur = &parametre->second;

			// On remplace le paramètre chaque fois qu'il est trouvé
			size_t posParam = string::npos;
			while((posParam = phrase.find(*nom)) != string::npos)
				phrase.replace(posParam,nom->size(),*valeur,0,valeur->size());

			parametre++;
		}
		//Msg("======>>> %s\n",phrase.c_str());
		return phrase;
	}
	else
		throw CSSMatchI18nException(string("Unable to find a translation for \"") + tag + "\"");
}
