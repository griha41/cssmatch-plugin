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

#ifndef __MENU_H__
#include "../Menu/Menu.h"
#endif

#ifndef __JOUEUR_H__
#define __JOUEUR_H__

/** Un joueur */
class Joueur
{
private:
	/** index du joueur sur le serveur */
	int index;

	/** Kills du joueur en match */
	int kills;

	/** deaths du joueur en match */
	int deaths;

	/** Sauvegarde des kills du round précédent */
	int killsDernierRound;

	/** Sauvegarde des kills de la manche précédente */
	int killsDerniereManche;

	/** Sauvegarde des deaths du round précédent */
	int deathsDernierRound;

	/* Sauvegarde des deaths de la manche précédente */
	int deathsDerniereManche;

	/** Menu utilisé <br>
	 * FIXME : le menu utilisé par le joueur sera copié pour palier les problèmes d'actualisations
	 */
	Menu menu;

	/** Numéro de la page du menu utilisé */
	int pageMenu;

	/** Un arbitre peut prendre l'ID d'un Joueur pour cible dans les options d'administration <br>
	 * FIXME : il n'y a pas de classe séparant le simple joueur de l'arbitre, <br>
	 * car à tout moment un joueur peut devenir arbitre ce qui implique des boucles et des <br>
	 * tests lourds pour 1 variable de différence je trouve <br>
	 */
	int useridCible;
public:
	/** Construiteur utilisé par le vector */
	Joueur();

	/** Construit un joueur */
	Joueur(int index);

	/** Accesseur sur l'index du joueur 
	 *
	 * @param index Le nouvel index du joueur
	 *
	 */
	void setIndex(int index);


	/** Accesseur sur l'index du joueur 
	 *
	 * @return L'index du joueur
	 *
	 */
	int getIndex() const;

	/** Accesseur sur les kills du joueur 
	 *
	 * @return Le nombre de kills du joueur
	 *
	 */
	int getKills() const;

	/** Incrémente le nombre de kills du joueur
	 *
	 * @return Le nouveau nombre de kills du joueur
	 *
	 */
	int incKills();

	/** Décrémente les kills du joueur 
	 *
	 * @return Le nouveau nombre de kills du joueur
	 *
	 */
	int decKills();

	/** Accesseur sur les kills du joueur 
	 *
	 * @param kills Le nouveau nombre de kills du joueur
	 *
	 */
	void setKills(int kills);

	/** Accesseur sur les deaths du joueur 
	 *
	 * @return Le nombre de deaths du joueur
	 *
	 */
	int getDeaths() const;

	/** Incrémente le nombre de deaths du joueur
	 *
	 * @return Le nouveau nombre de deaths du joueur
	 *
	 */
	int incDeaths();

	/** Décrémente les deaths du joueur 
	 *
	 * @return Le nouveau nombre de deaths du joueur
	 *
	 */
	int decDeaths();

	/** Accesseur sur les deaths du joueur 
	 *
	 * @param deaths Le nouveau nombre de kills du joueur
	 *
	 */
	void setDeaths(int deaths);

	/** Calcule le ratio kills/deaths du joueur 
	 *
	 * @return Le ratio ou le nombre de kills si le nombre de deaths est égal à 0
	 *
	 */
	float getRatio() const;


	/** Met à jour notre historique des kills */
	void sauveStatsRound();

	/** Met à jour notre historique des deaths */
	void sauveStatsManche();

	/** Restaure les kills et les deaths du joueur correspondant au dernier round sauvegardé
	 *
	 * @see Joueur#sauveStatsRound
	 *
	 */
	void restoreStatsRound();

	/** Restaure les kills et les deaths du joueur correspondant à la dernière manche sauvegardée
	 *
	 * @see Joueur#sauveStatsManche
	 *
	 */
	void restoreStatsManche();


	/** Accesseur sur le menu utilisé par le joueur 
	 *
	 * @return Un pointeur sur le menu
	 *
	 */
	Menu * getMenu();

	/** Accesseur sur le menu utilisé par le joueur
	 *
	 * @param menu Le menu utilisé
	 *
	 */
	void setMenu(Menu menu);

	
	/** Accesseur sur la page du menu utilisé par le joueur 
	 *
	 * @return Le numéro de la page du menu
	 *
	 */
	int getPageMenu() const;

	/** Accesseur sur la page du menu utilisé par le joueur 
	 *
	 * @param numPage Le numéro de la page du menu
	 *
	 */
	void setPageMenu(int numPage);

	/** Incrémente la page du menu utilisé par le joueur (n'a aucun effet sur l'affichage !)
	 *
	 * @return Le nouveau numéro de la page du menu
	 *
	 */
	int incPageMenu();

	/** Décrémente la page du menu utilisé par le joueur (n'a aucun effet sur l'affichage !)
	 *
	 * @return Le nouveau numéro de la page du menu
	 *
	 */
	int decPageMenu();

	/** Accesseur sur l'userid choisit pour cible par le joueur arbitre 
	 *
	 * @return L'userid cible du joueur
	 *
	 */
	int getUseridCible() const;

	/** Accesseur sur l'userid choisit pour cible par le joueur arbitre 
	 *
	 * @param useridCible L'userid cible du joueur
	 *
	 */
	void setUseridCible(int useridCible);
};

#endif // __JOUEUR_H__
