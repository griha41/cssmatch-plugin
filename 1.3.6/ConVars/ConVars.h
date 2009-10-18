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

#ifndef __CONVARS_H__
#define __CONVARS_H__

/** Classe statique initialisant les ConVars */
class ConVars
{
public:
	/** Référence sur la ConVar "sv_cheats" du jeu */
	static ConVar * sv_cheats;

	/** Référence sur la ConVar "sv_alltalk" du jeu */
	static ConVar * sv_alltalk;

	/** Référence sur la ConVar "hostname" du jeu */
	static ConVar * hostname;

	/** Référence sur la ConVar "sv_password" du jeu */
	static ConVar * sv_password;

	/** Référence sur la ConVar "tv_enable" du jeu */
	static ConVar * tv_enable;


	/** Contient le nom de la version du plugin */
	static ConVar cssmatch_version;

	/** Détermine si la branche administrative du menu doit être affichée */
	static ConVar cssmatch_advanced;

	/** Détermine le fichier de langage utilisé */
	static ConVar cssmatch_language;

	/** Détermine si un fichier de rapport doit être généré à la fin du match */
	static ConVar cssmatch_report;

	
	/** Détermine si un round au couteau doit être joué */
	static ConVar cssmatch_kniferound;

	/** Détermine la quantité d'argent autorisée pendant le round au couteau */
	static ConVar cssmatch_kniferound_money;

	/** Détermine si la pose du C4 est autorisée pendant le round au couteau */
	static ConVar cssmatch_kniferound_allows_c4;

	/** Détermine la durée (en secondes) de transition entre le round au couteau et la suite du match */
	static ConVar cssmatch_end_kniferound;


	/** Détermine le nombre de round par manche */
	static ConVar cssmatch_rounds;

	/** Détermine le nombre de manches par match */
	static ConVar cssmatch_sets;

	/** Détermine la durée (en secondes) de transition entre les deux manches */
	static ConVar cssmatch_end_set;

	/** Détermine si SourceTV doit enregistrer le match */
	static ConVar cssmatch_sourcetv;

	/** Détermine le dossier de destination des enregistrements SourceTV (dossier fils du dossier "cstrike") */
	static ConVar cssmatch_sourcetv_path;

	/** Détermine la durée (en minutes) du Strats Time */
	static ConVar cssmatch_warmup_time;

	/** Détermine le nom du serveur pendant le match
	 *
	 * Si les chaînes "%1" et "%2" sont présentes dans la valeur de la ConVar, <br>
	 * elles seront remplacée par les tags des teams
	 *
	 */
	static ConVar cssmatch_hostname;

	/** Détermine le mot de passe du serveur pendant le match */
	static ConVar cssmatch_password;

	/** Détermine le nom du fichier de configuration par défaut (exécuté en fin de match) <br>
	 * 
	 * Le chemin commence à partir du contenu du dossier "cstrike"
	 *
	 */
	static ConVar cssmatch_default_config;

	/** Recherche les pointeurs vers le variables de VALVE */
	static void initialise();

	/** Active ou Désactive le alltalk suivant sa valeur */
	static void swapAlltalk();
};

#endif // __CONVARS_H__
