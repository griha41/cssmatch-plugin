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

#include "../Rapport.h"

#ifndef __RAPPORTXML_H__
#define __RAPPORTXML_H__

/** Un rapport de match sauvegardé dans le dossier cssmatch/reports au format .xml */
class RapportXml : public Rapport
{
private:
	/** Nettoie une chaîne XML de caractères interdits dans les contenus de balises (ex : <) <br>
	 * Note : les caractères indésirables sont remplacés par leur équivalent entité HTML
	 *
	 * @param chaine La chaîne XML
	 * @return La chaîne nettoyée des caractères indésirables
	 *
	 */
	std::string valideXml(const std::string & chaine) const;

	/** Surcharge de la méthode qui écrit l'entête du fichier (entetes xml et racine)
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritEntete(std::ofstream & fichier) const;

	/** Surcharge de la méthode qui écrit le corps du fichier
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritCorps(std::ofstream & fichier) const;

	/** Surcharge de la méthode qui écrit le pied du fichier (fermeture de la racine)
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritPied(std::ofstream & fichier) const;

	/** Ecriture des données relatives à la balise plugin
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritPlugin(std::ofstream & fichier) const;

	/** Ecriture des données relatives à la balise match
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritMatch(std::ofstream & fichier) const;

	/** Ecriture des données relatives à la balise match/teams
	 *
	 * @param fichier La référence sur le flux de sortie
	 * @param team1 Un pointeur sur la première team
	 * @param team2 Un pointeur sur la deuxième team
	 *
	 */
	void ecritTeams(std::ofstream & fichier, Team * team1, Team * team2) const;

	/** Ecriture des données relatives aux balises match/teams/team
	 *
	 * @param fichier La référence sur le flux de sortie
	 * @param team La team concernée par la balise
	 * @param lignup La lignup de la team
	 *
	 */
	void ecritTeam(std::ofstream & fichier, Team * team, const std::vector<int> & lignup) const;

	/** Ecriture des données relatives à la balise match/teams/team/joueurs
	 *
	 * @param fichier La référence sur le flux de sortie
	 * @param lignup La lignup de la team
	 *
	 */
	void ecritJoueurs(std::ofstream & fichier, const std::vector<int> & lignup) const;

	/** Ecriture des données relatives à la balise match/teams/team/joueurs/joueur 
	 *
	 * @param indentation Niveau d'indentation de la balise
	 * @param fichier La référence sur le flux de sortie
	 * @param joueur Un pointeur sur le joueur concerné
	 * @param playerInfo Un pointeur de type IPLayerInfo sur le joueur concerné
	 *
	 */
	void ecritJoueur(const std::string & indentation, std::ofstream & fichier, Joueur * joueur, IPlayerInfo * playerInfo) const;

	/** Ecriture des données relatives à la balise match/spectateurs
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritSpectateurs(std::ofstream & fichier) const;

	/** Ecriture des données relatives à la balise sourcetv
	 *
	 * @param fichier La référence sur le flux de sortie
	 *
	 */
	void ecritSourcetv(std::ofstream & fichier) const;

public:
	/** Surcharge de la méthode qui écrit le fichier sur le disque */
	void ecrit();

	/** Surcharge du constructeur 
	 *
	 * @see Rapport
	 *
	 */
	RapportXml(Match * match);
};

#endif // __RAPPORTXML_H__
