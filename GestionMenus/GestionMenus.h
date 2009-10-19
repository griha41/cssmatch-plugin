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
#include "../Messages/Messages.h"
#include "../ConVars/ConVars.h"
#include "../Menu/Menu.h"
#include "../GestionJoueurs/GestionJoueurs.h"
#include "../Configuration/Configuration.h"
#include "../I18n/I18n.h"
#include "../Match/Match.h"

#ifndef __GESTIONMENUS_H__
#define __GESTIONMENUS_H__

/** Singleton fournissant des fonctions d'aides à l'utilisation sécurisée des menus */
class GestionMenus : public Incopiable<>
{
private:
	/** Initialise le menu de choix du CutRound */
	void initialiseMenuChoixCutRound();

	/** Initialise le menu de choix des Strats */
	void initialiseMenuChoixStrats();

	/** Initialise le menu hors match */
	void initialiseMenuOff();

	/** Initialise le menu du CutRound */
	void initialiseMenuCutRound();

	/** Initialise le menu du StratsTime */
	void initialiseMenuStratsTime();

	/** Initialise le menu du match */
	void initialiseMenuMatch();

	/** Initialise le menu de choix d'une configuration */
	void initialiseMenuConfiguration();


	/** Initialise le menu d'administration */
	void initialiseMenuAdministration();

	/** Initialise le menu de changement de map */
	void initialiseMenuChangelevel();

	/** Initialise le menu de swap */
	void initialiseMenuSwap();

	/** Initialise le menu de mise en spectateur */
	void initialiseMenuSpec();

	/** Initialise le menu de kick */
	void initialiseMenuKick();

	/** Initialise le menu de ban */
	void initialiseMenuBan();

	/** Initialise le menu de choix d'une durée pour le ban */
	void initialiseMenuBanTime();

	GestionMenus();
	~GestionMenus();

	/** Affiche un menu SANS les options d'administration à un joueur désigné par son index (valide!) 
	 *
	 * @param phase Le phase de match actuelle
	 * @param indexJoueur L'index du destinataire du menu
	 * @see L'énumération CodePhase
	 *
	 */
	void afficheMenuSansAdministration(CodePhase phase, int indexJoueur);

	/** Affiche un menu AVEC les options d'administration à un joueur désigné par son index (valide!) 
	 *
	 * @param phase Le phase de match actuelle
	 * @param indexJoueur L'index du destinataire du menu
	 * @see L'énumération CodePhase
	 *
	 */
	void afficheMenuAvecAdministration(CodePhase phase, int indexJoueur);

public:
	/** Menu proposant un round au couteau */
	static Menu cutround_question;

	/** Menu proposant les strats */
	static Menu strats_question;

	/** Menu hors match */
	static Menu phase_off;

	/** Menu du round au couteau */
	static Menu phase_cutround;

	/** Menu des strats */
	static Menu phase_strats;

	/** Menu des manches */
	static Menu phase_match;

	/** Menu de choix d'une configuration */
	static Menu configurations;


	/** Menu d'administration */
	static Menu administration;

	/** Menu de changement de map */
	static Menu admin_changelevel;

	/** Menu de swap */
	static Menu admin_swap;

	/** Menu de mise en spectateur */
	static Menu admin_spec;

	/** Menu de kick */
	static Menu admin_kick;

	/** Menu de ban */
	static Menu admin_ban;

	/** Menu de choix d'une durée pour le ban */
	static Menu admin_ban_time;

	/** Accesseur sur l'instance unique de la classe 
	 *
	 * @return Un pointeur sur l'instance
	 *
	 */
	static GestionMenus * getInstance();

	/** Construit les menus SANS les options d'administration */
	void initialiseMenus();

	/** Construit les menus AVEC les options d'administration */
	void initialiseMenusAvecAdministration();

	/** Remplace "%s" par Activer ou Désactiver selon la valeur actuelle du alltalk
	 *
	 * @param menu Le menu contenant l'option à modifier
	 * @param ligne Le numéro de la ligne contenant l'option d'activation/désactivation du alltalk
	 * @param indexJoueur L'index du joueur destinataire du menu
	 */
	static void afficheMenuAlltalkOption(Menu & menu, int ligne, int indexJoueur);

	/** Finalise l'affiche d'un menu 
	 *
	 * @param menu Une référence sur le menu à afficher
	 * @param indexJoueur L'index du destinataire
	 * @param parametres Paramètres des lignes du menu
	 *
	 */
	void afficheMenuRef(Menu & menu,int indexJoueur,
		const std::map<std::string,std::string> & parametres = I18n::SANS_PARAMETRE);

	/** Affiche le menu d'arbitrage à un joueur désigné par son index (valide!) 
	 *
	 * @param phase Le phase de match actuelle
	 * @param indexJoueur L'index du destinataire du menu
	 * @see L'énumération CodePhase
	 *
	 */
	void afficheMenu(CodePhase phase, int indexJoueur);

	/** Exécute le code associé au choix d'un joueur dans un menu
	 *
	 * @param indexJoueur L'index du joueur
	 * @param choix Le choix du joueur dans le menu
	 *
	 */
	void actionMenu(int indexJoueur, int choix);

	/** Affiche la page précédente du menu utilisé par le joueur 
	 *
	 * @param indexJoueur L'index du joueur
	 *
	 */
	void afficheMenuPrecedent(int indexJoueur);

	/** Affiche la page précédente du menu utilisé par le joueur
	 *
	 * @param indexJoueur L'index du joueur
	 *
	 */
	void afficheMenuSuivant(int indexJoueur);

	/** Le joueur n'aura plus la main sur aucun menu (n'efface pas le menu à l'écran)
	 *
	 * @param indexJoueur L'index du joueur
	 *
	 */
	void quitteMenu(int indexJoueur);
};

#endif  // __GESTIONMENUS_H__
