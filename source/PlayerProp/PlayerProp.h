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


#ifndef __PLAYERPROP_H__
#define __PLAYERPROP_H__

/** Exception spécifique à l'accès aux props des joueurs */
class CSSMatchPlayerPropException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchPlayerPropException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchPlayerPropException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};


/** Interface avec une prop */
class PlayerProp
{
private:
	/** La classe à laquelle appartient la prop */
	std::string classe;

	/** Le nom de la prop */
	std::string prop;

	/** Offet de la prop dans les classes du serveur */
	int offset;
	
	/** Recherche l'offset associé à la prop 
	 *
	 * @throws CSSMatchPlayerPropException Si la prop n'a pu être trouvée
	 */
	void getClasseOffet() throw(CSSMatchPlayerPropException);
public:
	PlayerProp();

	/** Constructeur
	 *
	 * @param classe Le nom de la classe à laquelle la prop appartient
	 * @param nom Le nom de la prop
	 */
	PlayerProp(const std::string & classe, const std::string & nom);

	/** Accesseur
	 *
	 * @return Le nom de la classe à laquelle appartient la prop
	 */
	const std::string & getClasse() const;

	/** Accesseur
	 *
	 * @return Le nom de la prop
	 */
	const std::string & getProp() const;

	/** Accesseur en lecture et écriture sur la prop <br>
	 * Note : On ne jette pas l'exception à la construction, car le plugin peut fonctionner sans utiliser les prop
	 *
	 * @param pEntity L'entité à laquelle appartient la prop
	 * @throws CSSMatchPlayerPropException Si la prop n'a pu être trouvée <br>
	 */
	template<typename T>
	T & getPlayerProp(edict_t * pEntity) throw(CSSMatchPlayerPropException)
	{
		if (offset != 0)
		{
			try
			{
				CBaseEntity * baseEntity = Api::getCBaseEntityFromEntity(pEntity);
				return * reinterpret_cast<T *>(reinterpret_cast<char *>(baseEntity)+ offset);
			}
			catch(const CSSMatchApiException & e)
			{
				Api::reporteException(e,__FILE__,__LINE__);
				throw CSSMatchPlayerPropException(e.what());
			}
		}
		else
			throw CSSMatchPlayerPropException(std::string("Unable to find the offset of the prop ") + classe + "." + prop + " !");
	}
};

#endif // __PLAYERPROP_H__
