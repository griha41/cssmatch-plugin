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
#include "../CSSMatch/CSSMatch.h"
#include "../Messages/Messages.h"
#include "../Match/Match.h"
#include "../I18n/I18n.h"

using std::ostringstream;
using std::string;
using std::map;

/*int testCompletion(char const *partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ] )
{
	string sug1(partial);
	sug1 += " ok";

	string sug2(partial);
	sug2 += " ok2";

	strcpy(commands[0], sug1.c_str());
	strcpy(commands[1], sug2.c_str());

	return 2;
}

ConCommand test = ConCommand("cssm_test",NULL,"une aide",NULL,testCompletion);*/

/*#include "../EntityProp/EntityProp.h"
CON_COMMAND(cssm_list, "CSSMatch : Développement")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	CGlobalVars * globals = cssmatch->getGlobalVars();
	IVEngineServer * engine = cssmatch->getEngine();

	for (int i=0;i<globals->maxEntities;i++)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromIndex(i);
			string nom = pEntity->GetClassName();
			if (nom == "cs_player_manager")
			{
				EntityProp temp(pEntity,"DT_PlayerResource.m_iScore","001");

				temp.setEntityProp<int>(69);
				Msg("============= Entity prop = %i\n",temp.getEntityProp<int>());
			}
		}
		catch(const CSSMatchApiException & e) {}
	}
}*/

/*CON_COMMAND(cssm_maplist, "CSSMatch : Déboguage")
{
	Menu admin_changelevel;
	admin_changelevel.setCodeMenu(CODE_MENU_CHANGELEVEL);
	admin_changelevel.setTitre("test_map");
	admin_changelevel.specialise(MAPLIST);
	admin_changelevel.filelist(FICHIER_MAPLIST);
}*/

CON_COMMAND(cssm_help, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	if (args.ArgC()>1)
	{
		string command = args.Arg(1);
		string traduction;
		try
		{
			string nomLocale = "";
			traduction = I18n::getInstance()->getMessage(nomLocale,command);
			Msg((traduction+"\n").c_str());
		}
		catch(const CSSMatchI18nException & e)
		{
			map<string,string> parametres;
			parametres["$command"] = command;
			Messages::rconPrintTell("error_command_not_found",parametres);	
		}
	}
	else
		Msg("cssm_help command\n");
}

CON_COMMAND(cssm_start, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	IFileSystem * filesystem = cssmatch->getFileSystem();
	Match * match = cssmatch->getMatch();

	if (match->getPhase() == PHASE_OFF)
	{
		// Si aucun fichier de configuration n'a été spécifié en argument
		//		le fichier default.cfg est exécuté
		if (args.ArgC()==1)
		{
			// ... on tente de lancer le match avec le fichier default.cfg
			if (filesystem->FileExists("cfg/cssmatch/configurations/default.cfg", "MOD"))
			{
				match->lanceMatch(new Configuration("default.cfg"));	
			}
			else
				Messages::rconPrintTell("error_default.cfg");
		}
		else
		{
			// Sinon nous supposons que le premier argument est un nom de fichier de configuration

			const char * arg1 = args.Arg(1);
			if (arg1 != NULL)
			{
				string pathFichier("cfg/cssmatch/configurations/" + string(arg1));

				if (filesystem->FileExists(pathFichier.c_str(), "MOD"))
					match->lanceMatch(new Configuration(arg1));
				else
				{
					map<string,string> parametres;
					parametres["$file"] = arg1;
					Messages::rconPrintTell("error_file_not_found",parametres);
				}
					
			}
			else
				Messages::rconPrintTell("error_cmd");
		}
	}
	else
		Messages::rconPrintTell("match_in_progress");
}

CON_COMMAND(cssm_stop, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (match->getPhase() != PHASE_OFF)
		match->finMatch();
	else
		Messages::rconPrintTell("match_not_in_progress");
}

CON_COMMAND(cssm_retag, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	match->detecteTags(true);
}

CON_COMMAND(cssm_go, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	match->forceGo();
}

CON_COMMAND(cssm_restartmanche, "CSSMatch : Please use cssm_help")
{
	Match * match = CSSMatch::getInstance()->getMatch();

	match->restartManche();
}

CON_COMMAND(cssm_restartround, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	match->restartRound();
}

CON_COMMAND(cssm_adminlist, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();

	cssmatch->getAdminList()->logAdminlist();
}

CON_COMMAND(cssm_grant, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	// La différence de traitement des guillemets dans la console Windows et la console Linux nous oblige à 
	// opter pour une capture de tous les arguments

	if (args.ArgC()>1)
	{
		const char * steamIDvalve = args.ArgS();
		if (steamIDvalve != NULL)
		{
			string sSteamId(steamIDvalve);

			// Supression des espaces ajoutés par la console si le steamID n'était pas entre guillemets sous Windows
			size_t posCar;
			while((posCar = sSteamId.find(' ')) != string::npos)
				sSteamId.replace(posCar,1,"",0,0);

			// Supression des éventuels guillemets passés en argument par la console sous Linux
			while((posCar = sSteamId.find('"')) != string::npos)
				sSteamId.replace(posCar,1,"",0,0);

			// Supression des tabulations éventuelles
			while((posCar = sSteamId.find('\t')) != string::npos)
				sSteamId.replace(posCar,1,"",0,0);

			map<string,string> parametres;
			parametres["$steamid"] = sSteamId;
			if (cssmatch->getAdminList()->addAdmin(sSteamId))
			{
				Messages::rconPrintTell("admin_new_admin",parametres);
			}
			else
			{
				Messages::rconPrintTell("admin_is_already_admin",parametres);
			}
		}
		else
			Messages::rconPrintTell("error_cmd");
	}
	else
		Msg("cssm_grant steamID\n");
}

CON_COMMAND(cssm_revoke, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	// La différence de traitement des guillemets dans la console Windows et la console Linux nous oblige à 
	// opter pour une capture de tous les arguments

	if (args.ArgC()>1)
	{
		const char * steamIDvalve = args.ArgS();
		if (steamIDvalve != NULL)
		{
			string sSteamId(steamIDvalve);

			// Supression des espaces ajoutés par la console si le steamID n'était pas entre guillemets sous Windows
			size_t posCar;
			while((posCar = sSteamId.find(' ')) != string::npos)
				sSteamId.replace(posCar,1,"",0,0);

			// Supression des éventuels guillemets passés en argument par la console sous Linux
			while((posCar = sSteamId.find('"')) != string::npos)
				sSteamId.replace(posCar,1,"",0,0);

			// Supression des tabulations éventuelles
			while((posCar = sSteamId.find('\t')) != string::npos)
				sSteamId.replace(posCar,1,"",0,0);

			map<string,string> parametres;
			parametres["$steamid"] = sSteamId;
			if (cssmatch->getAdminList()->removeAdmin(sSteamId))
				Messages::rconPrintTell("admin_old_admin",parametres);
			else
				Messages::rconPrintTell("admin_is_not_admin",parametres);
		}
		else
			Messages::rconPrintTell("error_cmd");
	}
	else
		Msg("cssm_revoke steamID\n");
}

CON_COMMAND(cssm_teamt, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	Match * match = cssmatch->getMatch();

	const char * tag = args.ArgS();
	if (tag != NULL)
	{
		if (match->getPhase() != PHASE_OFF)
		{
			match->modifieTag(TEAM_T,tag);

			map<string,string> parametres;
			parametres["$team"] = tag;
			Messages::rconPrintTell("admin_new_t_team_name",parametres);
		}
		else
			Messages::rconPrintTell("match_not_in_progress");
	}
	else
		Messages::rconPrintTell("error_cmd");
}

CON_COMMAND(cssm_teamct, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	Match * match = cssmatch->getMatch();

	const char * tag = args.ArgS();
	if (tag != NULL)
	{	
		if (match->getPhase() == PHASE_OFF)
		{
			// Mise à jour du tag
			match->modifieTag(TEAM_CT,tag);

			map<string,string> parametres;
			parametres["$team"] = tag;
			Messages::rconPrintTell("admin_new_t_team_name",parametres);
		}
		else
			Messages::rconPrintTell("match_not_in_progress");
	}
	else
		Messages::rconPrintTell("error_cmd");
}

CON_COMMAND(cssm_swap, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	if (args.ArgC()>1)
	{
		const char * userid = args.Arg(1);
		if (userid != NULL)
		{
			try
			{
				edict_t * pEntity = Api::getPEntityFromID(atoi(userid));
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);

				if (player->GetTeamIndex() > TEAM_SPEC)
					Api::swap(player);
				else
					Messages::rconPrintTell("admin_spectator_player");
			}
			catch(const CSSMatchApiException & e)
			{
				Messages::rconPrintTell("error_invalid_player");
			}
		}
		else
			Messages::rconPrintTell("error_cmd");
	}
	else
		Msg("cssm_swap ID\n");
}

CON_COMMAND(cssm_spec, "CSSMatch : Please use cssm_help")
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	if (args.ArgC()>1)
	{
		const char * userid = args.Arg(1);
		if (userid != NULL)
		{
			try
			{
				edict_t * pEntity = Api::getPEntityFromID(atoi(userid));
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);

				if (player->GetTeamIndex() > TEAM_SPEC)
					Api::spec(player);
				else
					Messages::rconPrintTell("admin_spectator_player");
			}
			catch(const CSSMatchApiException & e)
			{
				Messages::rconPrintTell("error_invalid_player");
			}
		}
		else
			Messages::rconPrintTell("error_cmd");
	}
	else
		Msg("cssm_spec ID\n");
}
