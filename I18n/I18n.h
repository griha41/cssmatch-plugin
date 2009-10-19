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
#include "../ConVars/ConVars.h"

#ifndef __I18N_H__
#define __I18N_H__

/** Exception spécifique à la gestion de l'internationna */
class CSSMatchI18nException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchI18nException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchI18nException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

/** Support pour l'internalisation des messages du plugin */
class I18n : public Incopiable<>
{
private:
	/** Cache contenant un tableau associatif "tag => traduction" en fonction du langage recherché <br>
	 * Note : Le système de cache évite d'analyser les KeyValues pour chaque joueur
	 */
	std::map<std::string,std::map<std::string,std::string> > traductions;

	I18n();
	~I18n();
public:
	/** Instance std::map vide pour getMessage */
	static std::map<std::string,std::string> SANS_PARAMETRE;

	/** Accesseur sur l'instance unique de la classe
	 *
	 * @return L'instance unique de la classe
	 */
	static I18n * getInstance();

	/* Recherche le nom interne d'une locale <br>
	 * Note : Seul le nom interne du langage est retournée (ex : "French_France.1252" donnera "French") <br>
	 * Note : Si la ConVar cssmatch_langage contient une valeur, c'est cette valeur qui est retournée
	 *
	 * @param nomLangage Le nom du langage
	 * @return Le nom de la locale trouvée, ou le nom de la locale de l'OS sinon
	 */
	//static std::string getNomLocale(const std::string & nomLangage = "");

	/** Retourne le nom du fichier de langage à utiliser pour un langage donné
	 *
	 * @param nomLangage Le nom du langage à utiliser
	 * @return Le nom du fichier à utiliser (cssmatch_language si aucune traduction correspondante n'a été trouvée)
	 */
	static std::string getNomFichier(const std::string & nomLangage);

	/** Retourne la traduction correspondant au tag 
	 *
	 * @param tag Le nom de la phrase dont la traduction est demandée
	 * @param parametres Le tableau associatif "parametre => valeur" des paramètres de la phrase à traduire
	 * @return La traduction
	 * @throws CSSMatchI18nException Si la phrase n'existe pas
	 */
	std::string getMessage(const std::string & nomLocale, const std::string & tag,
		const std::map<std::string,std::string> & parametres = SANS_PARAMETRE)
			throw(CSSMatchI18nException);
};

#endif // __I18N_H__
