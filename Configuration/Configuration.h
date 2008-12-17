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

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

/** Exception spécifique aux configurations du plugin */
class CSSMatchConfigurationException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchConfigurationException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchConfigurationException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

/** Une configuration du match */
class Configuration
{
private:
	/** Nom du fichier contenant la configuration */
	std::string nomFichier;
public:
	/** Construit une configuration portant sur un fichier
	 *
	 * @param nomFichier Le nom du fichier de configuration situé dans le dossier "cfg/cssmatch/configuration"
	 *
	 */
	Configuration(const std::string & nomFichier = "default.cfg");

	/** Retourne une string contenant le nom du fichier de configuration 
	 *
	 * @return Le nom du fichier de configuration choisi (situé dans le dossier "cfg/cssmatch/configuration")
	 *
	 */
	const std::string & getNomFichier() const;

	/** Exécute le fichier de configuration
	 * 
	 * @throws CSSMatchConfigurationException si le fichier n'a pas été trouvé
	 */
	void execute() const throw (CSSMatchConfigurationException);
};

#endif // __CONFIGURATION_H__
