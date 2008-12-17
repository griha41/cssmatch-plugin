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

#include "Api.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;
using std::map;
using std::vector;
using std::ostringstream;
using std::exception;

void Api::serveurExecute(const string & cmd)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	// Insertion de la commande
	engine->ServerCommand(cmd.c_str());

	// Exécution des commandes en attente
	//engine->ServerExecute();
}

void Api::configExecute(const string & configPath)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	string commande("exec " + configPath + "\n");

	// Insertion de la commande
	engine->ServerCommand(commande.c_str());

	// Exécution des commandes en attente
	engine->ServerExecute();

	// Exécution des commandes en attente (celles lues dans le fichier de configuration)
	engine->ServerExecute();
}

bool Api::hltvPresente()
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IPlayerInfoManager * playerinfomanager = cssmatch->getPlayerInfoManager();
	int maxplayers = cssmatch->getMaxPlayers();

	/*if (!ConVars::tv_enable || !ConVars::tv_enable.GetBool())
		return false;
	Ce n'est pas le rôle de cette fonction ! */

	bool hltvPresente = false;

	// Recherche SourceTV parmis les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);

			// On n'utilise pas getIPlayerFromEntity qui exclue sourceTV
			player = playerinfomanager->GetPlayerInfo(pEntity);
			if (player && player->IsConnected() && player->IsHLTV())
			{
				hltvPresente = true;
				i = maxplayers + 1;
			}
		}
		catch(const CSSMatchApiException & e) {}
	}
	return hltvPresente;
}

IPlayerInfo * Api::getIPlayerInfoFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IPlayerInfoManager * playerinfomanager = cssmatch->getPlayerInfoManager();

	if (isValidePEntity(pEntity))
	{
		IPlayerInfo * player = playerinfomanager->GetPlayerInfo(pEntity);
		if (isValidePlayer(player))
			return player;
		else
			throw CSSMatchApiException("Unable to find the IPlayerInfo of this entity");
	}
	else
		throw CSSMatchApiException("Can't find the IPlayerInfo of an invalid entity");
}

edict_t * Api::getPEntityFromID(int userid) throw (CSSMatchApiException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	int maxplayers = cssmatch->getMaxPlayers();

	edict_t * pEntity = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);
			if (engine->GetPlayerUserId(pEntity) == userid)
				return pEntity;
		}
		catch(const CSSMatchApiException & e) {}
	}

	throw CSSMatchApiException("Unable to find the entity of this userid");
}

edict_t * Api::getPEntityFromIndex(int index) throw (CSSMatchApiException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	edict_t * pEntity = engine->PEntityOfEntIndex(index);
	if (isValidePEntity(pEntity))
		return pEntity;

	throw CSSMatchApiException("Unable to find the entity of this index");
}

int Api::getIndexFromUserId(int userid)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	int index = INDEX_INVALIDE;

	try
	{
		edict_t * pEntity = Api::getPEntityFromID(userid);
		index = getIndexFromPEntity(pEntity);
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
	}
	return index;
}

int Api::getIndexFromPEntity(edict_t * pEntity)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	int index = engine->IndexOfEdict(pEntity);

	if (! isValideIndex(index))
		index = INDEX_INVALIDE;

	return index;
}

int Api::getUseridFromPEntity(edict_t * pEntity)
{
	int userid = USERID_INVALIDE;

	try
	{
		IPlayerInfo * player = getIPlayerInfoFromEntity(pEntity);
		userid = player->GetUserID();
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
	}
	return userid;
}

int Api::getUseridFromUsername(const string & pseudo)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	int userid = USERID_INVALIDE;

	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);
			player = getIPlayerInfoFromEntity(pEntity);
			string name(player->GetName());
			if (pseudo == name)
			{
				userid = player->GetUserID();
				i = maxplayers + 1;
			}
		}
		catch(const CSSMatchApiException & e){}
	}
	return userid;
}

int Api::getIndexFromUsername(const string & pseudo)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	int index = INDEX_INVALIDE;

	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			pEntity = getPEntityFromIndex(i);
			player = getIPlayerInfoFromEntity(pEntity);
			string name(player->GetName());
			if (pseudo == name)
				index = Api::getIndexFromPEntity(pEntity);
		}
		catch(const CSSMatchApiException & e){}
	}
	return index;
}

CodeTeam Api::getPEntityTeam(edict_t * pEntity)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	CodeTeam indexTeam = TEAM_INVALIDE;

	if (isValidePEntity(pEntity))
	{
		edict_t * pEntityTemp = NULL;
		IPlayerInfo * player = NULL;
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntityTemp = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (pEntity == pEntityTemp)
					indexTeam = (CodeTeam)player->GetTeamIndex();
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	return indexTeam;
}

IServerEntity * Api::getIServerEntityFromEdict(edict_t * pEntity) throw (CSSMatchApiException)
{
	if (isValidePEntity(pEntity))
	{
		IServerEntity * sEntity = pEntity->GetIServerEntity();
		if (sEntity != NULL)
			return sEntity;
		else
			throw CSSMatchApiException("The IServerEntity of this entity is null");
	}
	else
		throw CSSMatchApiException("Unable to find the IServerEntity of an invalid entity");
}

CBaseEntity * Api::getCBaseEntityFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	IServerEntity * sEntity = NULL;
	CBaseEntity * bEntity = NULL;
	try
	{
		sEntity = getIServerEntityFromEdict(pEntity);
		bEntity = sEntity->GetBaseEntity();
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
		throw CSSMatchApiException("Unable to find the BaseEntityEdict of this entity");
	}

	if (bEntity != NULL)
		return bEntity;
	else
		throw CSSMatchApiException("The CBaseEntity of this entity is null");
}

CBaseCombatCharacter * Api::getCBaseCombatCharacterFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	CBaseEntity * bEntity = NULL;
	CBaseCombatCharacter * cCharacter = NULL;
	try
	{
		bEntity = getCBaseEntityFromEntity(pEntity);

		cCharacter = bEntity->MyCombatCharacterPointer();
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
		throw CSSMatchApiException("Unable to find the CBaseCombatCharater of an invalid CBaseEntity");
	}

	if (cCharacter != NULL)
		return cCharacter;
	else
		throw CSSMatchApiException("Unable to find the CBaseCombatCharater of this entity");
}

CBaseCombatWeapon * Api::getCBaseCombatWeaponFromSlot(edict_t * pEntity, CodeWeaponSlot slot) throw (CSSMatchApiException)
{
	CBaseCombatCharacter * cCharacter = NULL;
	CBaseCombatWeapon * cWeapon = NULL;
	try
	{
		cCharacter = getCBaseCombatCharacterFromEntity(pEntity);

		// Récupération d'un pointeur sur l'arme situé à ce slot
		cWeapon = cCharacter->Weapon_GetSlot(slot);
	}
	catch(const CSSMatchApiException & e)
	{
		reporteException(e,__FILE__,__LINE__);
		throw CSSMatchApiException("Unable to find the CBaseCombatWeapon of an invalid CBaseCombatCharacter");
	}
	if (cWeapon != NULL)
		return cWeapon;
	else
		throw CSSMatchApiException("Unable to find the CBaseCombatWeapon of this entity");
}

CBasePlayer * Api::getCBasePlayerFromEntity(edict_t * pEntity) throw (CSSMatchApiException)
{
	if (isValidePEntity(pEntity))
	{
		IServerUnknown * unknown = pEntity->GetUnknown();
		if (unknown != NULL)
		{
			CBasePlayer * cPlayer = reinterpret_cast<CBasePlayer*>(unknown->GetBaseEntity());
			if (cPlayer != NULL)
				return cPlayer;
			else
				throw CSSMatchApiException("Unable to find the CBasePlayer of this entity");
		}
		else
			throw CSSMatchApiException("Unable to find the CBasePlayer of this entity");
	}
	else
		throw CSSMatchApiException("Unable to find the CBasePlayer of an invalid entity");
}

int Api::getPlayerCount(CodeTeam codeTeam /* critère de recherche : un camp particulière ou tous*/)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	int count = 0;

	// Le codeTeam correspond à un camp particulier
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;
	if (codeTeam != TEAM_ALL)
	{
		// On compte tous les joueurs dans l'index de la team correspond au codeTeam
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex() == codeTeam)
					count++;
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Le codeTeam correspond à toutes les teams confondues
	else
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				// On compte tous les joueurs
				pEntity = getPEntityFromIndex(i); // Vérifie aussi si le joueur  est valide
				count++;
			}
			catch(const CSSMatchApiException & e) {}
		}
	}
	return count;
}

string Api::getPlayerName(edict_t * pEntity) throw (CSSMatchApiException)
{
	if (isValidePEntity(pEntity))
	{
		IPlayerInfo * player = NULL;
		try
		{
			player = getIPlayerInfoFromEntity(pEntity);
		}
		catch(const CSSMatchApiException & e)
		{
			// ...Arrive typiquement lorsque le match est arbitré par RCON
			//reporteException(e,__FILE__,__LINE__);
			throw CSSMatchApiException("Unable to find the username of an invalid player");
		}
		const char * pseudo = player->GetName();

		if (pseudo != NULL)
			return pseudo;
		else
			throw CSSMatchApiException("Unable to find the username of this entity");
	}
	else
		throw CSSMatchApiException("Unable to find the username of an invalid entity");
}

string Api::getPlayerSteamID(edict_t * pEntity) throw (CSSMatchApiException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	if (isValidePEntity(pEntity))
	{
		const char * steamID = engine->GetPlayerNetworkIDString(pEntity);
		if (steamID != NULL)
			return steamID;
		else
			throw CSSMatchApiException("Unable to find the steamID of this entity");
	}
	else
		throw CSSMatchApiException("Unable to find the steamID of an invalid entity");
}

void Api::kickid(int useridCible, const string & motif)
{
	ostringstream cmd;
	cmd << "kickid " << useridCible << " " << motif << "\n";

	Api::serveurExecute(cmd.str());
}

void Api::banid (int temps, int useridCible, const std::string & motif)
{
	ostringstream cmd;
	cmd << "banid " << temps << " " << useridCible  << "\n";
	Api::serveurExecute(cmd.str());

	// Si le ban est permanent, mise à jour du fichier banned_user.cfg
	if (temps == 0)
		Api::serveurExecute("writeid\n");

	kickid(useridCible,motif);
}

void Api::cexec(edict_t * pEntity, const string & cmd)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	// On vérifie que la team du joueur est valide et différent de "non-assigné" (cas particulier d'un bot créé malgré le paramètre -nobots)
	CodeTeam codeTeam = getPEntityTeam(pEntity); // contrôle également la validité de l'entité
	if (codeTeam>TEAM_UN)
		engine->ClientCommand(pEntity,cmd.c_str());
}

void Api::cexecGroupe(CodeTeam codeTeam, const string & cmd)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (! player->IsFakeClient())
					cexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e) {}
		}
	}
	else
	{
		// Commande à n'exécuter que pour une certaine camp
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->IsFakeClient()==false && player->GetTeamIndex()==codeTeam)
					cexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e) {}
		}
	}
}

void Api::sexec(edict_t * pEntity, const string & cmd)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IServerPluginHelpers * helpers = cssmatch->getServerPluginHelpers();

	// On vérifie que la team du joueur est valide et différent de "non-assigné" (cas particulier d'un bot créé malgré le paramètre -nobots)
	CodeTeam codeTeam = getPEntityTeam(pEntity); // contrôle également la validité de l'entité
	if (codeTeam>TEAM_UN)
		helpers->ClientCommand(pEntity,cmd.c_str());
}

void Api::sexecGroupe(CodeTeam codeTeam, const string & cmd)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam == TEAM_ALL)
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (!player->IsFakeClient())
					sexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Commande à n'exécuter que pour un certain camp
	else
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (!player->IsFakeClient() && player->GetTeamIndex()==codeTeam)
					sexec(pEntity,cmd);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Api::swap(IPlayerInfo * player)
{
	if (isValidePlayer(player))
	{
		// Selon le camp actuel du joueur, nous le swappons dans le camp opposée
		// Les bots seront kickés plutôt que swappés, pour éviter qu'ils ne renaissent pas
		int teamActuelle = player->GetTeamIndex();
		switch(teamActuelle)
		{
		case TEAM_T:
			if (player->IsFakeClient())
				kickid(player->GetUserID(),"Swap Bot");
			else
				player->ChangeTeam(TEAM_CT);
			break;
		case TEAM_CT:
			if (player->IsFakeClient())
				kickid(player->GetUserID(),"Swap Bot");
			else
				player->ChangeTeam(TEAM_T);
		}
	}
}

void Api::swapGroupe(CodeTeam codeTeam)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				swap(player); // nous savons que swap valide l'IPlayerInfo
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Commande à n'exécuter que pour un certain camp
	else
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex()==codeTeam)
					swap(player);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Api::spec(IPlayerInfo * player)
{
	if (isValidePlayer(player))
	{
		// On ne placera pas les bots en spectateurs, on les kick pour éviter qu'ils ne soient "bloqués"
		if (player->IsFakeClient())
			Api::kickid(player->GetUserID(),"CSSMatch : Spec Bot");
		else
			player->ChangeTeam(TEAM_SPEC);
	}
}

void Api::specGroupe(CodeTeam codeTeam)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	// Nous ferons 2 boucles séparées pour optimiser le code

	// Commande à exécuter chez tous les joueurs
	edict_t * pEntity = NULL;
	IPlayerInfo * player = NULL;

	if (codeTeam==TEAM_ALL)
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				spec(player); // spec valide l'IPlayerInfo
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	// Commande à n'exécuter que pour un certain camp
	else
	{
		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				pEntity = getPEntityFromIndex(i);
				player = getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex()==codeTeam)
					spec(player);
			}
			catch(const CSSMatchApiException & e){}
		}
	}
}

void Api::setHealth(edict_t * pEntity, int nombreHP)
{
	if (isValidePEntity(pEntity))
	{
		IPlayerInfo * player = NULL;
		try
		{
			player = getIPlayerInfoFromEntity(pEntity);
			if (! player->IsDead())
			{
				// Si la vie que l'on va ôter au joueur le nécessite, on le tue simplement
				if (nombreHP <= 0)
					cexec(pEntity,"kill\n");
				// Sinon on modifie sa vie
				else
				{
					CBasePlayer * bPlayer = getCBasePlayerFromEntity(pEntity);
					bPlayer->SetHealth(nombreHP);
				}
			}
		}
		catch(const CSSMatchApiException & e)
		{
			reporteException(e,__FILE__,__LINE__);
			return;
		}
	}
}

void Api::detruitArme(edict_t * pEntity, CodeWeaponSlot slot)
{
	try
	{
		CBaseCombatWeapon * weapon = getCBaseCombatWeaponFromSlot(pEntity,slot);
		weapon->Kill();
	}
	catch(const CSSMatchApiException & e)
	{
		//reporteException(e,__FILE__,__LINE__);
	}
}

bool Api::isValidePEntity(edict_t * pEntity)
{
	return pEntity && !pEntity->IsFree();
}

bool Api::isValidePlayer(IPlayerInfo * player)
{
	return player && player->IsConnected() && player->IsPlayer() && !player->IsHLTV();
}

bool Api::isValideIndex(int index)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	return index>=1 || index<=maxplayers;
}

bool Api::isOnline(int index)
{
	bool online = false;

	try
	{
		IPlayerInfo * pInfo = getIPlayerInfoFromEntity(getPEntityFromIndex(index));
		online = pInfo->IsConnected();
	}
	catch(const CSSMatchApiException & e){}

	return online;
}

void Api::debug(const string & message)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	string commande("CSSMatch : " + message + "\n");

	const char * debug = commande.c_str();
	Warning(debug);
	engine->LogPrint(debug);
}

void Api::reporteException(const exception & e, const string & fichier, int ligne)
{
	ostringstream message;
	message << e.what() << " (" << fichier << ", l." << ligne << ")";
	debug(message.str());
}

void timerCexecT(const string & commande, const map<string,string> & parametres)
{
	Api::cexecGroupe(TEAM_T,commande);
}

void timerCexecCT(const string & commande, const map<string,string> & parametres)
{
	Api::cexecGroupe(TEAM_CT,commande);
}

void timerSwapALL(const string & parametre, const map<string,string> & parametres)
{
	Api::swapGroupe(TEAM_ALL);
}
