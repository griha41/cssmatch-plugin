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

#ifndef __TEAM_H__
#define __TEAM_H__

#include "../Api/Api.h"
#include "../Joueur/Joueur.h"

/** Une team, plus sous forme d'agrégat qu'autre chose... (pas de liste de joueurs) */
class Team
{
private:
	/** Tag de la team */
	std::string tag;

	/* Lignup de la team */
	//std::vector<Joueur> lignup;


	/** Score de la team */
	int score;

	/** Score de la team en terroriste */
	int scoreT;

	/** Score de la team en anti-terroriste */
	int scoreCT;


	/** Passe à vrai lorsque la team a tapée !go lors du StratsTime */
	bool goStrats;

	/** Combien de joueurs sont dans la team ? */
	int nbrMembres;

	/** Fonction appelée si aucun joueur n'est présent sur le serveur <br>
	 * Note : attribue un tag reflétant cette situation
	 */
	void aucunTag();

	/** Fonction appelée s'il n'y a qu'un seul joueur dans le camp 
	 *
	 * @param codeTeam Le code de la team concercée par la recherche de tag
	 * @see L'énumération CodeTeam
	 *
	 */
	void aucuneTeam(CodeTeam codeTeam);


	/** Fonction appelée s'il y a au moins 2 joueurs dans le camp 
	 *
	 * @param codeTeam Le code de la team concercée par la recherche de tag
	 * @see L'énumération CodeTeam
	 *
	 */
	void determineTag(CodeTeam codeTeam);


	/** Fonction localisant un éventuel tag dans 2 pseudos, à partir d'un caractère commun entre 2 pseudos <br>
	 *
	 * @param i index du premier caractère commun dans le pseudo pseudoJ1
	 * @param j index du premier caractère commun dans le pseudo pseudoJ1
	 * @param pseudoJ1 Pseudo du joueur 1
	 * @param pseudoJ2 Pseudo du joueur 2
	 * @return true si un tag d'au moins 3 caractères a pu être trouvé
	 *
	 */
	bool confirmeTag(const std::string & pseudoJ1, int i, const std::string & pseudoJ2, int j);
	// Nom alternatif : confirmePointCommun...

public:
	Team();

	/* Gestion de la lignup */
	//std::vector<Joueur> * getLignup();

	/** Accesseur sur le score total de la team 
	 *
	 * @return Le score
	 *
	 */
	int getScore() const;

	/** Incrémente le score total de la team 
	 *
	 * @return Le nouveau score de la team
	 *
	 */
	int incScore();

	/** Accesseur sur le score total de la team
	 *
	 * @param score Le nouveau score de la team
	 *
	 */
	void setScore(int score);


	/** Accesseur sur le score effectué par la team en terroriste
	 *
	 * @return Le score
	 *
	 */
	int getScoreT() const;

	/** Incrémente le score effectué par la team en terroriste
	 *
	 * @return Le nouveau score de la team
	 *
	 */
	int incScoreT();

	/** Accesseur sur le score effectué par la team en terroriste
	 *
	 * @param scoreT Le nouveau score de la team
	 *
	 */
	void setScoreT(int scoreT);

	
	/** Accesseur sur le score effectué par la team en anti-terroriste
	 *
	 * @return Le score
	 *
	 */
	int getScoreCT() const;

	/** Incrémente le score effectué par la team en anti-terroriste
	 *
	 * @return Le nouveau score de la team
	 *
	 */
	int incScoreCT();

	/** Accesseur sur le score effectué par la team en anti-terroriste
	 *
	 * @param scoreCT Le nouveau score de la team
	 *
	 */
	void setScoreCT(int scoreCT);

	
	/** Accesseur sur le tag de la team
	 *
	 * @return Le tag de la team
	 *
	 */
	const std::string & getTag() const;

	/** Accesseur sur le tag de la team
	 *
	 * @param tag Le nouveau tag de la team
	 *
	 */
	void setTag(const std::string & tag);

	
	/** Détermine si la team est prête à passer les strats
	 *
	 * @param go true si la team est prête
	 *
	 */
	void setGoStrats(bool go);

	
	/** Permet de savoir si la team est prête à passer les strats
	 *
	 * @return true si la team est prête
	 *
	 */
	bool getGoStrats() const;

	
	/** Accesseur sur le nombre de joueurs appartenant à la team
	 *
	 * @return Le nombre de joueur
	 *
	 */
	int getNbrMembres() const;

	/** Incrémente le nombre de joueurs appartenant à la team
	 *
	 * @return Le nouveau nombre de joueur
	 *
	 */
	int incNbrMembres();

	/** Décrémente le nombre de joueurs appartenant à la team
	 *
	 * @return Le nouveau nombre de joueur
	 *
	 */
	int decNbrMembres();


	/** Recherche le tag de la team désignée par son codeTeam
	 *
	 * @param codeTeam Le code de la team concernée
	 * @return Le tag trouvé
	 * @see L'énumération CodeTeam
	 *
	 */
	const std::string & chercheTag(CodeTeam codeTeam);
};

#endif // __TEAM_H__
