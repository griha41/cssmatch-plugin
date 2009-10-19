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

#include "../../Api/Api.h"

#ifndef __SAYHOOK_H__
#define __SAYHOOK_H__

/** Intercepteur de la commande "say" */
class SayHook : public ConCommand 
{ 
private:
	/** Indicateur de succès du hook */
	bool succes;

	/** Pointeur sur la commande "say" originale ("de VALVE") */
	ConCommand * say;
	
public:
	/** Constructeur initialisant une nouvelle commande nommée "say" */
	SayHook();

	/** Surcharge de la méthode "Init" de la classe ConCommand 
	 *
	 * @see ConCommand::Init
	 */
	void Init();

	/** Surcharge de la méthode "Dispatch" de la classe ConCommand 
	 *
	 * @see ConCommand::Dispatch
	 */
	void Dispatch();

	/** Commande d'appel du menu d'arbitrage 
	 *
	 * @param indexJoueur L'index du joueur
	 * @param in Le flux contenant les arguments de la commande
	 */
	static void cmdCssmatch(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException);

	/** Commande d'édition du tag de la team terroriste
	 *
	 * @param indexJoueur L'index du joueur
	 * @param in Le flux contenant les arguments de la commande
	 */
	static void cmdTeamt(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException);

	/** Commande d'édition du tag de la team anti-terroriste
	 *
	 * @param indexJoueur L'index du joueur
	 * @param in Le flux contenant les arguments de la commande
	 */
	static void cmdTeamct(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException);

	/** Commande d'édition du tag d'une team
	 *
	 * @param indexJoueur L'index du joueur
	 * @param in Le flux contenant les arguments de la commande
	 * @param codeTeam Le code de la team dont le tag est modifié
	 */
	static void cmdTeam(int indexJoueur, std::istringstream & in, CodeTeam codeTeam) throw(CSSMatchApiException);
}; 

#endif // __SAYHOOK_H__
