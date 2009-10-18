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

#ifndef __GESTIONTIMERS_H__
#define __GESTIONTIMERS_H__

/** Un gestionnaire de timers <br>
 *
 * Un timer ajouté à un gestionnaire est en attente d'exécution <br>
 * Veillez à faire fonctionner le gestionnaire à intervalle régulier (ex: à chaque frame ou dans un thread)
 *
 * @see GestionTimers#execute
 *
 */
class GestionTimers
{
private:
	/** Tableau dynamique de timers */
	Timer * timers;

	/** Nombre maximum de timers gérés */
	int taille;

public:
	GestionTimers();

	/** Construit un gestionnaire de timers d'une capacité donnée
	 *
	 * @param taille Le nombre maximal de timers gérés par le gestionnaire
	 *
	 */
	GestionTimers(int taille);

	/** Détruit proprement le gestionnaire */
	~GestionTimers();

	/** Copie proprement le gestionnaire 
	 *
	 * @param gTimers Le gestionnaire à copier
	 *
	 */
	GestionTimers(const GestionTimers & gTimers);


	/** Affecte proprement le gestionnaire à une variable
	 *
	 * @param gTimers Le gestionnaire à copier
	 *
	 */
	GestionTimers & operator = (const GestionTimers & gTimers);

	/** Ajoute un timer au gestionnaire 
	 *
	 * @param timer Le timer à ajouter
	 *
	 */
	void addTimer(const Timer & timer);

	/** Exécute les timers qui le doivent */
	void execute();

	/** Efface tous les timers en attente */
	void purge();
};

#endif // __GESTIONTIMERS_H__
