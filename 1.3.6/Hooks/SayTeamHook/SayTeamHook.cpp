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

#include "SayTeamHook.h"
#include "../SayHook/SayHook.h"
#include "../../CSSMatch/CSSMatch.h"

using std::string;
using std::exception;
using std::istringstream;

SayTeamHook::SayTeamHook() : ConCommand("say_team", NULL, "say_team hook command", FCVAR_GAMEDLL)
{
	say_team = NULL;
	succes = false;
} 

void SayTeamHook::Init() 
{
	// Recherche de la commande "say_team" originale
	const ConCommandBase * commandes = NULL;

	ICvar * cvars = CSSMatch::getInstance()->getCVars();
	if (cvars != NULL)
	{
		commandes = cvars->GetCommands();
		while(commandes != NULL)
		{
			if (commandes->IsCommand() && commandes != this && string(commandes->GetName()) == "say_team")
			{ 
				say_team = (ConCommand*)(const_cast<ConCommandBase*>(commandes)); 
				commandes = NULL;
				succes = true;
			}
			else
				commandes = commandes->GetNext();
		}
		if (succes)
			ConCommand::Init();
		else
			Api::debug("Unable to hook the command \"say_team\" : unable to find the command");
	}
	else
		Api::debug("Unable to hook the command \"say_team\" : the interface is not ready");
} 

void SayTeamHook::Dispatch() 
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	int index = cssmatch->GetCommandIndex()+1; 

	try
	{
		// Récupération du message sans les guillemets qui l'entoure
		string ligneMessage;
		int argc = engine->Cmd_Argc();
		for(int i=1;i<argc;i++)
		{
			ligneMessage += engine->Cmd_Argv(i);
		}
		
		istringstream message(ligneMessage);
		string commande;
		
		message >> commande;

		// Accès au menu d'arbitrage
		if (commande == "cssmatch")
		{
			SayHook::cmdCssmatch(index,message);
		}
		// Edition du tag du camp actuellement terroriste
		else if (commande == "!teamt")
		{
			SayHook::cmdTeamt(index,message);
		}
		// Edition du tag du camp actuellement anti-terroriste
		else if (commande == "!teamct")
		{
			SayHook::cmdTeamct(index,message);
		}
		else
			say_team->Dispatch();
	}
	catch(const CSSMatchApiException & e)
	{
		Api::debug(e.what());
		say_team->Dispatch();
	}
}
