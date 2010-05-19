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

#include "SayHook.h"
#include "../../CSSMatch/CSSMatch.h"

using std::string;
using std::exception;
using std::istringstream;
using std::map;

SayHook::SayHook() : ConCommand("say", (FnCommandCallback_t)NULL, "[CSSMatch] : say hook command", FCVAR_GAMEDLL)
{
	say = NULL;
	succes = false;
} 

void SayHook::Init() 
{
	// Recherche de la commande "say" originale
	const ConCommandBase * commandes = NULL; 

	ICvar * cvars = CSSMatch::getInstance()->getCVars();
	if (cvars != NULL)
	{
		commandes = cvars->GetCommands();
		while(commandes != NULL)
		{
			if (commandes->IsCommand() && commandes != this && string(commandes->GetName()) == "say")
			{ 
				say = (ConCommand*)(const_cast<ConCommandBase*>(commandes)); 
				commandes = NULL;
				succes = true;
			}
			else
				commandes = commandes->GetNext();
		}
		if (succes)
			ConCommand::Init();
		else
			Api::debug("Unable to hook the command \"say\" : unable to find the command");
	}
	else
		Api::debug("Unable to hook the command \"say\" : the interface is not ready");
} 

void SayHook::Dispatch(const CCommand & args) 
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int index = cssmatch->GetCommandIndex()+1; 

	try
	{
		// Récupération du message sans les guillemets qui l'entoure
		string ligneMessage;
		int argc = args.ArgC();
		for(int i=1;i<argc;i++)
		{
			ligneMessage += args.Arg(i);
		}
		
		istringstream message(ligneMessage);
		string commande;
		
		message >> commande;

		// Accès au menu d'arbitrage
		if (commande == "cssmatch")
		{
			cmdCssmatch(index,message);
		}
		// Edition du tag du camp actuellement terroriste
		else if (commande == "!teamt")
		{
			cmdTeamt(index,message);
		}
		// Edition du tag du camp actuellement anti-terroriste
		else if (commande == "!teamct")
		{
			cmdTeamct(index,message);
		}
		else
			say->Dispatch(args);
	}
	catch(const CSSMatchApiException & e)
	{
		Api::debug(e.what());
		say->Dispatch(args);
	}
}

void SayHook::cmdCssmatch(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	// Aucun argument n'est attendu pour cette commande
	string test;
	std::getline(in,test);
	if (test.empty())
	{
		edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
		const char * steamID = engine->GetPlayerNetworkIDString(pEntity);

		if (steamID != NULL)
		{
			// Il faut que le joueur soit arbitre
			if (cssmatch->getAdminList()->estAdmin(steamID))
				Api::cexec(pEntity,"cssmatch\n");
			else
				Messages::sayTell(0,indexJoueur,"player_you_not_admin");
		}
	}
}

void SayHook::cmdTeamt(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException)
{
	IVEngineServer * engine = CSSMatch::getInstance()->getEngine();
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
	const char * steamID = engine->GetPlayerNetworkIDString(pEntity);
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (steamID != NULL)
	{
		// Il faut que le joueur soit arbitre
		if (cssmatch->getAdminList()->estAdmin(steamID))
		{
			//int mancheCourante = match.getMancheCourante();
			if (match->getPhase() != PHASE_OFF)
			{
				// Récupération du pseudo après la commande !teamt
				string nouvTag;
				std::getline(in,nouvTag);

				if (! nouvTag.empty())
				{
					// Retrait du premier espace séparant la commande de ses arguments
					string::iterator debutTag = nouvTag.begin();
					nouvTag.erase(debutTag,debutTag+1);

					// Détermination de le camp actuellement terroriste et enregistrement du tag
					match->modifieTag(TEAM_T,nouvTag);

					map<string,string> parametres;
					parametres["$team"] = nouvTag;
					Messages::sayMsg(0,"admin_new_t_team_name",parametres);


					match->formateHostname();
				}
				else
					Messages::sayTell(0,indexJoueur,"admin_new_tag");
			}
			else
				Messages::sayTell(0,indexJoueur,"match_not_in_progress");
		}
		else
			Messages::sayTell(0,indexJoueur,"player_you_not_admin");
	}
}

void SayHook::cmdTeamct(int indexJoueur, std::istringstream & in) throw(CSSMatchApiException)
{
	IVEngineServer * engine = CSSMatch::getInstance()->getEngine();
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
	const char * steamID = engine->GetPlayerNetworkIDString(pEntity);
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (steamID != NULL)
	{
		// Il faut que le joueur soit arbitre
		if (cssmatch->getAdminList()->estAdmin(steamID))
		{
			if (match->getPhase() != PHASE_OFF)
			{
				// Récupération du pseudo après la commande !teamt
				string nouvTag;
				std::getline(in,nouvTag);

				if (! nouvTag.empty())
				{
					// Retrait du premier espace séparant la commande de ses arguments
					string::iterator debutTag = nouvTag.begin();
					nouvTag.erase(debutTag,debutTag+1);

					// Détermination de le camp actuellement terroriste et enregistrement du tag
					match->modifieTag(TEAM_CT,nouvTag);

					map<string,string> parametres;
					parametres["$team"] = nouvTag;
					Messages::sayMsg(0,"admin_new_ct_team_name",parametres);

					match->formateHostname();
				}
				else
					Messages::sayTell(0,indexJoueur,"admin_new_tag");
			}
			else
				Messages::sayTell(0,indexJoueur,"match_not_in_progress");
		}
		else
			Messages::sayTell(0,indexJoueur,"player_you_not_admin");
	}
}

void SayHook::cmdTeam(int indexJoueur, std::istringstream & in, CodeTeam codeTeam) throw(CSSMatchApiException)
{
	IVEngineServer * engine = CSSMatch::getInstance()->getEngine();
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
	const char * steamID = engine->GetPlayerNetworkIDString(pEntity);
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (steamID != NULL)
	{
		// Il faut que le joueur soit arbitre
		if (cssmatch->getAdminList()->estAdmin(steamID))
		{
			//int mancheCourante = match.getMancheCourante();
			if (match->getPhase() != PHASE_OFF)
			{
				// Récupération du pseudo après la commande !teamt
				string nouvTag;
				std::getline(in,nouvTag);

				if (! nouvTag.empty())
				{
					// Retrait du premier espace séparant la commande de ses arguments
					string::iterator debutTag = nouvTag.begin();
					nouvTag.erase(debutTag,debutTag+1);

					switch(codeTeam)
					{
					case TEAM_T:
						Messages::sayMsg(0,"admin_new_t_team_name");
						break;
					case TEAM_CT:
						//break;
					default:
						Messages::sayMsg(0,"admin_new_ct_team_name");
						codeTeam = TEAM_CT;
					}
						
					// Enregistrement du tag
					match->modifieTag(codeTeam,nouvTag);

					// Mise à jour du nom du serveur si besoin
					match->formateHostname();
				}
				else
					Messages::sayTell(0,indexJoueur,"admin_new_tag");
			}
			else
				Messages::sayTell(0,indexJoueur,"match_not_in_progress");
		}
		else
			Messages::sayTell(0,indexJoueur,"player_you_not_admin");
	}
}
