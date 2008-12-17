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

// Visual C++ 2005 ne supporte pas la spécification des exceptions dans la déclaration des fonctions
#pragma warning(disable : 4290)

// Je suis au courant ! (variables locales non référencées, typiquement dans les catch(std::exception e){})
#pragma warning(disable :4101)

#include <vector>
#include <map>
#include <stdio.h>
#include <ctime>
#include <string>
#include <locale>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <time.h>

#include "../Outils/Outils.h"

#include "interface.h"
/**
 * @see http://developer.valvesoftware.com/wiki/KeyValues_class#Important_Notes
 */
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "dlls/iplayerinfo.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
//#include "Color.h"
//#include "vstdlib/random.h"
//#include "engine/IEngineTrace.h"
/** Permet de manipuler les ConVars */
#include "icvar.h"
#include "bitbuf.h"
/** Permet de manipuler des listes de joueurs destinataires des messages du plugin
 *
 * @author Voir la liste des auteurs sur le Wiki de VALVE
 *
 */
#include "../MRecipientFilter/MRecipientFilter.h"

/** Permet l'inclusion de cbase.h */
#define GAME_DLL 1
#include "cbase.h"
//#include "engine/IStaticPropMgr.h"
//#include "engine/IEngineSound.h"
//#include "entityapi.h"
//#include "entitylist.h"
#include "baseentity.h"

/** memdbgon must be the last include file in a .cpp file!!! */
#include "tier0/memdbgon.h"

#ifndef __API_H__
#define __API_H__

/** Exception spécifique à l'API du plugin */
class CSSMatchApiException : public std::exception
{
private:
    std::string msg;
public:
	CSSMatchApiException(const std::string & msg) : std::exception()
    {
        this->msg = msg;
    }

    virtual ~CSSMatchApiException() throw() {}

    virtual const char * what() const throw()
    {
		return this->msg.c_str();
    }
};

/** Définit le nom du plugin */
#define CSSMATCH_NOM "CSSMatch"

/** Définit le nom de la version du plugin. */
#define CSSMATCH_VERSION_LIGHT "1.3.5"

/** Définit le site Web officiel du plugin. */
#define CSSMATCH_SITE "http://code.google.com/p/cssmatch-plugin-en/"

/** Définit les informations sur le nom de version, l'auteur et le site associé au plugin */
//#define CSSMATCH_VERSION "CSSMatch v" CSSMATCH_VERSION_LIGHT ", by Nicolous " CSSMATCH_SITE
#define CSSMATCH_VERSION "Nico's " CSSMATCH_NOM ", version " CSSMATCH_VERSION_LIGHT " (" __DATE__ "), " CSSMATCH_SITE


/** Définit le nombre représentant un userid erroné */
#define USERID_INVALIDE -1

/* Définit le nombre représentant un index erroné */
#define INDEX_INVALIDE 0

/* Définit quel est le nombre maximum de slots d'un serveur, avant que celui-ci ne soit éventuellement changé */
//#define DEFAUT_MAXPLAYERS 64

/* Macro permettant d'obtenir les interfaces utilisées par le plugin
 *
 * @author VALVE Corporation
 *
 */
/*#define GETINTERFACE(factory, retvar, interfacename, rettype, required) retvar = (rettype *)factory(interfacename, NULL);\
	if (retvar == NULL)\
	{ \
		Msg("Impossible d'obtenir l'interface \"" interfacename "\"\n");\
		if (required)\
		{\
			return false;\
		}\
	}
		// Remplacé par la fonction générique Api::getInterface
	*/

/** Associe à une constante les index des teams possibles sous CSS */
typedef enum CodeTeam
{
	/** Team inconnue */
	TEAM_INVALIDE = TEAM_INVALID,

	/** Les non-assignés */
	TEAM_UN = TEAM_UNASSIGNED,

	/** Les spectateurs */
	TEAM_SPEC = TEAM_SPECTATOR,

	/** Les Terro */
	TEAM_T,

	/** Les CT */
	TEAM_CT,

	/** Toutes les teams */
	TEAM_ALL
};

/** Associe à une constante les index des slots possibles sous CSS */
typedef enum CodeWeaponSlot
{
	WEAPON_SLOT1 = 0,
	WEAPON_SLOT2,
	WEAPON_SLOT3,
	WEAPON_SLOT4,
	WEAPON_SLOT5
};

/** Espace de nom contenant différentes fonctions agissant sur la partie */
namespace Api
{
	/** Force l'exécution immédiate d'une commande dans la console
	 *
	 * @param cmd La commande à exécuter
	 *
	 */
	void serveurExecute(const std::string & cmd);

	/** Exécute un fichier de configuration via la commande "exec", <br>
	 * Force également l'exécution des commandes ainsi placées dans la file d'attente de la console
	 *
	 * @param configPath Le nom du fichier
	 *
	 */
	void configExecute(const std::string & configPath);

	/** Détermine si HLTV est présente sur le serveur
	 *
	 * @return true si HLTV est présente
	 */
	bool hltvPresente();

	/** Recherche le pointeur IPlayerInfo représentant une entité représentant un joueur
	*
	* @param pEntity L'entité représentant le joueur
	* @return Un pointeur de type IPlayerInfo sur l'entité
	* @throws CSSMatchApiException si l'IPlayerInfo n'a pu être trouvé
	*/
	IPlayerInfo * getIPlayerInfoFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche l'entité correspondant à un userID
	*
	* @param userid L'userid du joueur
	* @return Un pointeur de type edict_t sur l'entité trouvée
	* @throws CSSMatchApiException si l'entité n'a pu être trouvée
	*/
	edict_t * getPEntityFromID(int userid) throw (CSSMatchApiException);

	/** Recherche l'entité correspondant à un index
	*
	* @param index L'index du joueur
	* @return Un pointeur de type edict_t sur l'entité trouvée
	* @throws CSSMatchApiException si l'entité n'a pu être trouvée
	*
	*/
	edict_t * getPEntityFromIndex(int index) throw (CSSMatchApiException);

	/** Recherche l'index d'un joueur portant un userID donné
	 *
	 * @param userid L'userid du joueur
	 * @return L'index du joueur (peut être égal à INDEX_INVALIDE)
	 *
	 */
	int getIndexFromUserId(int userid);

	/** Recherche l'index d'un joueur correspondant à une entité
	 *
	 * @param pEntity Le joueur
	 * @return L'index du joueur (peut être égal à INDEX_INVALIDE)
	 *
	 */
	int getIndexFromPEntity(edict_t * pEntity);

	/** Recherche l'userID d'un joueur désigné par son pseudo
	 *
	 * @param pseudo Le pseudo du joueur
	 * @return L'userid du joueur (peut être égal à USERID_INVALIDE)
	 *
	 */
	int getUseridFromUsername(const std::string & pseudo);

	/** Recherche l'userID d'une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @return L'userid du joueur
	 *
	 */
	int getUseridFromPEntity(edict_t * pEntity);

	/** Recherche l'index d'un joueur à partir de son pseudo
	 *
	 * @param pseudo Le pseudo du joueur
	 * @return L'userid du joueur
	 *
	 */
	int getIndexFromUsername(const std::string & pseudo);

	/** Recherche un pointeur de type IServerEntity sur une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant un joueur
	 * @return Un pointeur de type IServerEntity représentant le joueur
	 * @throws CSSMatchApiException si l'IServerEntity n'a pu être trouvée
	 */
	IServerEntity * getIServerEntityFromEdict(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche un pointeur de type CBaseEntity sur une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant un joueur
	 * @return Un pointeur de type CBaseEntity sur le joueur
	 * @throws CSSMatchApiException si le pointeur CBaseEntity correspondant n'a pu être trouvé
	 */
	CBaseEntity * getCBaseEntityFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche le CodeTeam d'une entité représentant un joueur
	 *
	 * @param pEntity L'une entité représentant le joueur
	 * @return Le CodeTeam du joueur
	 * @see L'unumération CodeTeam
	 *
	 */
	CodeTeam getPEntityTeam(edict_t * pEntity);

	/** Recherche un pointeur de type CBaseCombatCharacter sur une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant un joueur
	 * @return Un pointeur de type CBaseCombatCharacter sur le joueur
	 * @throws CSSMatchApiException si le pointeur CBaseCombatCharacter correspondant n'a pu être trouvé
	 *
	 */
	CBaseCombatCharacter * getCBaseCombatCharacterFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche un pointeur de type CBaseCombatWeapon sur une arme portée par une entité représentant un joueur, à un slot donné
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @param slot Le CodeWeaponSlot de l'arme
	 * @throws CSSMatchApiException si le CBaseEntity n'a pu être trouvé
	 * @see L'énumération CodeWeaponSlot
	 *
	 */
	CBaseCombatWeapon * getCBaseCombatWeaponFromSlot(edict_t * pEntity, CodeWeaponSlot slot) throw (CSSMatchApiException);

	/** Recherche un pointeur de type CBasePlayer sur une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @return Un pointeur de type CBasePlayer sur le joueur
	 * @throws CSSMatchApiException si le pointeur CBasePlayer correspondant n'a pu être trouvé
	 *
	 */
	CBasePlayer * getCBasePlayerFromEntity(edict_t * pEntity) throw (CSSMatchApiException);

	/** Détermine le nombre de joueurs appartenant à une team
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @return Le nombre de joueurs appartenant à la team
	 * @see L'énumération CodeTeam
	 *
	 */
	int getPlayerCount(CodeTeam codeTeam);

	/** Recherche le pseudo d'une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @return Le pseudo du joueur
	 * @throws CSSMatchApiException si le pseudo n'a pu être trouvé
	 *
	 */
	std::string getPlayerName(edict_t * pEntity) throw (CSSMatchApiException);

	/** Recherche la steamID d'une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @return La steamID du joueur
	 * @throws CSSMatchApiException si la steamID n'a pu être trouvée
	 */
	std::string getPlayerSteamID(edict_t * pEntity) throw (CSSMatchApiException);

	/** Kick un joueur désigné par userID pour un motif déterminée
	 *
	 * @param useridCible L'userid du joueur
	 * @param motif Le motif du kick
	 *
	 */
	void kickid(int useridCible, const std::string & motif);

	/** Banni un joueur désigné par userID pour un motif déterminée, pour un temps donné
	 *
	 * @param temps La durée (en minutes) du ban
	 * @param useridCible L'userid du joueur
	 * @param motif Le motif du kick
	 *
	 */
	void banid(int temps, int useridCible, const std::string & motif);

	/** Exécute une commande dans la console d'une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @param cmd La commande à exécuter
	 *
	 */
	void cexec(edict_t * pEntity, const std::string & cmd);

	/** Exécute une commande dans la console des joueurs d'une team
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @param cmd La commande à exécuter
	 * @see L'énumération CodeTeam
	 *
	 */
	void cexecGroupe(CodeTeam codeTeam, const std::string & cmd);

	/** Force l'exécution d'une commande dans la console d'un joueur <br>
	 * Fonctionne même si la commande a subit un alias, et pour certaines commandes normalement protégées
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @param cmd La commande à exécuter
	 *
	 */
	void sexec(edict_t * pEntity, const std::string & cmd);

	/** Force l'exécution d'une commande dans la console des joueurs d'une team <br>
	 * Fonctionne même si la commande a subit un alias, et pour certaines commandes normalement protégées
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @param cmd La commande à exécuter
	 * @see L'énumération CodeTeam
	 *
	 */
	void sexecGroupe(CodeTeam codeTeam, const std::string & cmd);

	/** Swap un joueur dans le camp d'en face (si possible)
	 *
	 * @param player Un pointeur de type IPlayerInfo sur le joueur
	 *
	 */
	void swap(IPlayerInfo * player);

	/** Swap les joueurs d'une team dans le camp d'en face (si possible)
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @see L'énumération CodeTeam
	 *
	 */
	void swapGroupe(CodeTeam codeTeam);

	/** Met un joueur en spectateur
	 *
	 * @param player Un pointeur de type IPlayerInfo sur le joueur
	 *
	 */
	void spec(IPlayerInfo * player);

	/** Met les joueurs d'une team en spectateur
	 *
	 * @param codeTeam Le CodeTeam de la team
	 * @see L'énumération CodeTeam
	 *
	 */
	void specGroupe(CodeTeam codeTeam);

	/** Modifie la vie d'une entité représentant un joueur
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @param nombreHP Le nombre de points de vie à attribuer au joueur
	 *
	 */
	void setHealth(edict_t * pEntity, int nombreHP);

	/** Détruit l'arme d'une entité représentant un joueur, situé à un slot donné
	 *
	 * @param pEntity L'entité représentant le joueur
	 * @param slot Le CodeWeaponSlot de l'arme
	 *
	 */
	void detruitArme(edict_t * pEntity, CodeWeaponSlot slot);

	/** Evalue la validité d'une entité edict_t
	 *
	 * @param pEntity L'entité à valider
	 * @return true si l'entité est valide
	 *
	 */
	bool isValidePEntity(edict_t * pEntity);

	/** Evalue la validité d'une entité IPlayerInfo
	 *
	 * @param player L'entité à valider
	 * @return true le l'entité est valide
	 *
	 */
	bool isValidePlayer(IPlayerInfo * player);

	/** Evalue la validité de l'index d'un joueur
	 *
	 * @param index L'index à analyser
	 * @return true si l'index est valide
	 *
	 */
	bool isValideIndex(int index);

	/** Détermine si un index est occupé par un joueur connecté
	 *
	 * @param index L'index à analyser
	 * @return true si un joueur occupe cet index
	 *
	 */
	bool isOnline(int index);

	/** Ecrit un message dans la console et dans le log du serveur
	 *
	 * @param message Le message
	 *
	 */
	void debug(const std::string & message);

	/** Reporte une exception dans les logs du serveur
	 *
	 * @param e L'exception
	 * @param fichier Le nom du fichier dans lequel l'exception a été rattrapée
	 * @param ligne La ligne du fichier où l'exception a été rattrapée
	 */
	void reporteException(const std::exception & e, const std::string & fichier, int ligne);
};

// Fonctions outils pour les timers (nos timers ne supportent pas les fonctions membres)
/** Exécute une commande dans la console des joueurs terroristes
 *
 * @param commande La commande à exécuter
 *
 */
 void timerCexecT(const std::string & commande, const std::map<std::string,std::string> & parametres);

/** Exécute une commande dans la console des joueurs anti-terroristes
 *
 * @param commande La commande à exécuter
 *
 */
void timerCexecCT(const std::string & commande, const std::map<std::string,std::string> & parametres);

/** Swap tous les joueurs
 *
 * @param parametre Une chaîne vide
 *
 */
void timerSwapALL(const std::string & parametre, const std::map<std::string,std::string> & parametres);

/** Function to initialize any cvars/command in this plugin
 *
 * @author VALVE Corporation
 *
 */
void InitCVars(CreateInterfaceFn cvarFactory);

/* Détermine si 2 chaînes de caractères char* sont égales
 *
 * @author VALVE Corporation
 * @return true si les chaînes sont égales
 *
 */
//inline bool FStrEq(const char *sz1, const char *sz2);

/** Nom du fichier contenant la liste des arbitres */
#define FICHIER_ADMINLIST "cstrike/cfg/cssmatch/adminlist.txt"

/** Nom du fichier contenant la liste des maps à lister dans le menu */
#define FICHIER_MAPLIST "cstrike/cfg/cssmatch/maplist.txt"

/** Dossier dans lequel sont recherchés les configurations de match */
#define DOSSIER_CONFIGURATIONS_MATCH "cfg/cssmatch/configurations"

/** Dossier dans lequel sont enregistrés les rapports de match */
#define DOSSIER_RAPPORTS_MATCH "cstrike/cfg/cssmatch/reports"

#endif // __API_H__
