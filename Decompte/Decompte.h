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
#include "../Timer/Timer.h"
#include "../GestionTimers/GestionTimers.h"
#include "../Messages/Messages.h"

#ifndef __DECOMPTE_H__
#define __DECOMPTE_H__

/** Singleton lançant un décompte à l'écran de tous les joueurs */
class Decompte : public Incopiable<>
{
private:
	/* Notre classe est un singleton, constructeurs et destructeurs sont donc privés */
	Decompte();
	~Decompte();

	/** Temps restant (en secondes) du décompte */
	int tempsRestant;

	/** Fonction à exécuter en fin de décompte */
	void (*fonction)(const std::string &, const std::map<std::string,std::string> & informations);

	/** Gestionnaire des timers émis par le décompte */
	GestionTimers moteur;
		
public:
	/** Récupère l'instance unique de Decompte 
	 *
	 * @return Un pointeur sur l'instance du décompte
	 *
	 */
	static Decompte * getInstance();

	/** Décrémente le temps restant du décompte 
	 *
	 * @return Le temps restant en secondes
	 *
	 */
	int decTempsRestant();

	/** Lance un Decompte
	 *
	 * @param duree Durée en secondes du décompte
	 * @param fonction Un référence sur une fonction à exécuter en fin de décompte
	 *
	 */
	void lanceDecompte(int duree, void (*fonction)(const std::string &, const std::map<std::string,std::string> &));
	
	/** Stoppe le décompte */
	void finDecompte();

	/** Accesseur sur le temps restant du décompte
	 *
	 * @return Le temps restant en secondes
	 *
	 */
	int getTempsRestant();

	/** Accesseur sur le gestionnaire de timers du décompte 
	 *
	 * @return Un pointeur sur le gestionnaire de timers
	 *
	 */
	GestionTimers * getMoteur();
};

#endif // __DECOMPTE_H__
