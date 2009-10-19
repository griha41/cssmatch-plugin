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
//GetSpewOutputFunc();
#include "CSSMatch.h"
#include "../Hooks/SayHook/SayHook.h"
#include "../Hooks/SayTeamHook/SayTeamHook.h"

#include "../GestionMenus/GestionMenus.h"

// #include "ICommandLine.h" // CommandLine_Tier0()->GetCmdLine()
// <=> getParam

using std::string;
using std::map;
using std::ostringstream;
using std::exception;

CSSMatch * CSSMatch::getInstance()
{
	static CSSMatch instance;
	return &instance;
}

CSSMatch::CSSMatch()
{
	Msg("CSSMatch : Creating singleton...\n");
	
	m_iClientCommandIndex = 0;

	timers = GestionTimers(10);

	engine = NULL;
	filesystem = NULL;
	gameeventmanager = NULL;
	playerinfomanager = NULL;
	helpers = NULL;
	gpGlobals = NULL;
	cvars = NULL;
	serverDll = NULL;

	// Initialisons de maxplayers à MAX_PLAYERS (nombre maximum de slots) 
	//		tant que ce nombre ne peut pas être plus précis
	// Au premier appel de CSSMatch::ServerActivate(), 
	//		ce nombre doit être actualisé de manière à ce que la suite du code en prenne compte
	maxplayers = MAX_PLAYERS;
}

template<typename T>
bool CSSMatch::getInterface(CreateInterfaceFn factory, T * & aInitialiser, const string & versionInterface)
{
	bool succes = false;

	aInitialiser = (T *)factory(versionInterface.c_str(),NULL);

	if (aInitialiser == NULL)
		Msg(string("CSSMatch : Unable to get the \"" + versionInterface + "\" interface !\n").c_str());
	else
		succes = true;

	return succes;
}

bool CSSMatch::initialisation(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	bool succes =	getInterface<IPlayerInfoManager>(gameServerFactory,playerinfomanager,INTERFACEVERSION_PLAYERINFOMANAGER)	&&
					getInterface<IVEngineServer>(interfaceFactory,engine,INTERFACEVERSION_VENGINESERVER)						&&
					getInterface<IGameEventManager2>(interfaceFactory,gameeventmanager,INTERFACEVERSION_GAMEEVENTSMANAGER2)		&&
					// getInterface<IBotManager>(gameServerFactory,botmanager,INTERFACEVERSION_PLAYERBOTMANAGER)				&&
					getInterface<IFileSystem>(interfaceFactory,filesystem,FILESYSTEM_INTERFACE_VERSION)							&&
					getInterface<IServerPluginHelpers>(interfaceFactory,helpers,INTERFACEVERSION_ISERVERPLUGINHELPERS)			&&
					//getInterface<IEngineTrace>(interfaceFactory,enginetrace,INTERFACEVERSION_ENGINETRACE_SERVER)				&&
					//getInterface<IUniformRandomStream>(interfaceFactory,randomStr,VENGINE_SERVER_RANDOM_INTERFACE_VERSION)	&&
					getInterface<ICvar>(interfaceFactory,cvars,VENGINE_CVAR_INTERFACE_VERSION)									&&
					//getInterface<IServerGameDLL>(gameServerFactory,serverDll,INTERFACEVERSION_SERVERGAMEDLL)					&&
					getInterface<IServerGameDLL>(gameServerFactory,serverDll,"ServerGameDLL006");

	if (playerinfomanager != NULL)
			succes &= (gpGlobals = playerinfomanager->GetGlobalVars()) != NULL;

	if (gpGlobals == NULL)
		Msg("CSSMatch : Unable to access the globals variables\n");

	return succes;
}

CSSMatch::~CSSMatch()
{
	//Msg("CSSMatch : Destroying singleton..\n");
}

bool CSSMatch::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	bool succes = false; // we require all these interface to function

	Msg("CSSMatch : Loading...\n");
	if(initialisation(interfaceFactory,gameServerFactory)) // Initilisation des interfaces
	{
		InitCVars(interfaceFactory); // register any cvars we have defined
		MathLib_Init(2.2f,2.2f,0.0f,2);

		// Recherche des ConVars créées par VALVE qui vont nous servir
		ConVars::initialise();

		// Initialisation des menus par défaut (cssmatch_advanced peut modifier les menus)
		GestionMenus::getInstance()->initialiseMenus();

		// Initialisation des props
		propCash = PlayerProp("CCSPlayer","m_iAccount");
		propLifeState = PlayerProp("CBasePlayer","m_lifeState");

		succes = true;
		Msg("CSSMatch : Load complete !\n");
	}

	return succes;
}

void CSSMatch::Unload()
{
	Msg("CSSMatch : Unloading...\n");

	if (gameeventmanager != NULL) // Si le déchargement est provoqué par l'impossibilité d'initialiser l'interface GameEventManager
		gameeventmanager->RemoveListener(&ecouteur); // make sure we are unloaded from the event system

	Msg("CSSMatch : Unload complete !\n");
}

void CSSMatch::Pause()
{
}

void CSSMatch::UnPause()
{
}

const char * CSSMatch::GetPluginDescription()
{
	return CSSMATCH_VERSION;
}

void CSSMatch::LevelInit(const char * pMapName)
{
/*	// Instruction à exécuter lors du premier changement de map
	static bool premiereMap = true;
	if (premiereMap)
	{
		

		premiereMap = false;
	}*/

	// Récupération des évènements qui nous intéressent
	ecouteur.initialise();

	// Effacement des timers en attente
	timers.purge();

	// Si le changement de map a été fait pendant un match on stoppe le match
	if (match.getPhase() != PHASE_OFF)
		match.finTempsMortFinMatch();

	// Récupération de la liste des steamID arbitre
	if (! adminlist.parseAdminlist(FICHIER_ADMINLIST))
		engine->LogPrint("CSSMatch : Warning, \"" FICHIER_ADMINLIST "\" is missing !");

	// Mise à jour du nom de la map courante
	mapCourante = pMapName;
}

void CSSMatch::ServerActivate(edict_t * pEdictList, int edictCount, int clientMax)
{
	// Mise à jour de la variable maxplayers
	maxplayers = clientMax;

	// Mise à jour de la taille de la liste des joueurs
	// Le resize n'est pas risqué, les index concernés n'auraient jamais été pris par un joueur
	GestionJoueurs::getInstance()->setTaillePlayerList(clientMax+1);
}

void CSSMatch::GameFrame(bool simulating)
{
	// Alimentation temporel du décompte ;)
	Decompte::getInstance()->getMoteur()->execute();

	// Exécution des timers en attente d'exécution
	timers.execute();
}

void CSSMatch::LevelShutdown() // !!!!this can get called multiple times per map change
{
}

void CSSMatch::ClientActive(edict_t * pEntity)
{
}

void CSSMatch::ClientDisconnect(edict_t * pEntity)
{
}

void CSSMatch::ClientPutInServer(edict_t * pEntity, const char * playername)
{
	if (Api::isValidePEntity(pEntity))
	{
		int indexJoueur = Api::getIndexFromPEntity(pEntity);

		if (Api::isValideIndex(indexJoueur))
		{
			// On s'assure que le joueur entrant sur le serveur n'utilisait
			//	pas un menu du plugin sur l'éventuelle map précédente
			GestionMenus::getInstance()->quitteMenu(indexJoueur);

			if (match.getPhase() != PHASE_OFF)
			{
				map<string,string> parametres;
				parametres["$username"] = playername;
				Messages::sayMsg(indexJoueur,"player_join_game",parametres);
				Messages::tellPopup(indexJoueur,"player_match_hosted_popup",0);
			}
		}
	}
}

void CSSMatch::SetCommandClient(int index)
{
	m_iClientCommandIndex = index;
}

void CSSMatch::ClientSettingsChanged(edict_t * pEdict)
{
}

PLUGIN_RESULT CSSMatch::ClientConnect(bool * bAllowConnect,
									  edict_t * pEntity,
									  const char * pszName,
									  const char * pszAddress,
									  char * reject,
									  int maxrejectlen)
{
	PLUGIN_RESULT decision = PLUGIN_CONTINUE;

	if (Api::isValidePEntity(pEntity)) 
	{
		int indexJoueur = Api::getIndexFromPEntity(pEntity);
		if (Api::isValideIndex(indexJoueur))
		{
			// On met à jour l'instance Joueur situé à cet index dans la liste de joueurs
			GestionJoueurs::getInstance()->connexion(indexJoueur);
		}
	}

	return decision;
}

PLUGIN_RESULT CSSMatch::ClientCommand(edict_t * pEntity)
{
	PLUGIN_RESULT decision = PLUGIN_CONTINUE;

	// Récupération du nom de la commande exécutée
	const char * pcmd = engine->Cmd_Argv(0);

	int indexJoueur = Api::getIndexFromPEntity(pEntity);

	const char * steamIDvalve = engine->GetPlayerNetworkIDString(pEntity);

	// Récupération des arguments de la commande (peut être NULL)
	const char * args = engine->Cmd_Args();

	CodePhase phase = match.getPhase();

	GestionMenus * menus = GestionMenus::getInstance();

	try
	{
		// Vérification de la validité des informations
		if (Api::isValidePEntity(pEntity) &&
			pcmd != NULL &&
			Api::isValideIndex(indexJoueur) &&
			steamIDvalve != NULL)
		{
			string commande(pcmd);
			string steamID(steamIDvalve);
			string arguments(args != NULL ? args : "");

			/*// Si nous sommes en match nous loguons toutes les commandes passant dans la console des joueurs
			if (phase != PHASE_OFF)
			{
				try
				{
					ostringstream log;
					log << "CSSMatch : " << Api::getPlayerName(pEntity) << " <id=" << Api::getUseridFromPEntity(pEntity) << "> <steamID=\"" << steamID << "\"> runs " << commande << " " + arguments << "\n";
					Api::engine->LogPrint(log.str().c_str());
				}
				catch(const CSSMatchApiException & e)
				{
					Api::reporteException(e,__FILE__,__LINE__);
				}
			}*/

			// *********************
			// Utilisation d'un menu
			// *********************
			if (commande == "menuselect")
			{
				const char * parametre = engine->Cmd_Argv(1);
				if (parametre != NULL)
				{
					// Récupération du choix du joueur dans le menu
					int choix = atoi(parametre);

					// Exécution du code associé à l'action dans le menu
					menus->actionMenu(indexJoueur,choix);
				}
			}

			// *****************************
			// Changement volontaire de camp
			// *****************************
			else if (commande == "jointeam")
			{
				// Si un match est en cours, on interdit les changements de camp
				CodePhase phaseLimite = PHASE_MANCHE;
				if (match.getMancheCourante() > 1)
					phaseLimite = PHASE_PASSAGE_CUTROUND; // PHASE_OFF + 1
				try
				{
					if (phase >= phaseLimite)
					{
						// Contrôle de la validité de l'argument
						if (arguments.size() == 1) 
						{
							if (arguments[0] == '2' || arguments[0] == '3')
							{
								if (Api::getPEntityTeam(pEntity) > TEAM_SPEC)
								{
									Messages::sayTell(0,indexJoueur,"player_no_swap_in_match");
									decision = PLUGIN_STOP;
								}
							}
							// Argument invalide (ex : lettre) : refus du swap
							else if (atoi(arguments.c_str()) == 0)
								decision = PLUGIN_STOP;
							// Le cas où l'index dépasse 3 est pris en charge par le jeu
						}
						else // argument invalide : refus du swap
							decision = PLUGIN_STOP;
					}
				}
				catch(const CSSMatchApiException & e)
				{
					Api::reporteException(e,__FILE__,__LINE__);
				}
			}

			// ************
			// Menu arbitre
			// ************
			else if (commande == "cssmatch")
			{
				if (adminlist.estAdmin(steamID))
				{
					// Son d'ouverture du menu
					Api::cexec(pEntity,"playgamesound UI/buttonrollover.wav\n");

					menus->afficheMenu(phase,indexJoueur);
				}
				else
				{
					Messages::sayTell(0,indexJoueur,"player_you_not_admin");
					string log("CSSMatch : steamID " + string(steamIDvalve) + " is not admin\n");
					engine->LogPrint(log.c_str());
					adminlist.logAdminlist();
				}
				decision = PLUGIN_STOP;
			}

			// *****************
			// Rates des joueurs
			// *****************
			else if (commande == "cssm_rates")
			{
				Messages::getRatesTab(pEntity);
				
				decision = PLUGIN_STOP;
			}
		}
	}
	catch (const exception & e)
	{
		map<string,string> parametres;
		parametres["$site"] = CSSMATCH_SITE;
		Messages::sayTell(0,indexJoueur,"error_general",parametres);
		Api::reporteException(e,__FILE__,__LINE__);
		//throw;
	}
	return decision;
}

PLUGIN_RESULT CSSMatch::NetworkIDValidated(const char * pszUserName, const char * pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

/*void CSSMatch::FireGameEvent(IGameEvent * event)
{
} Voir la classe EventListener */

IVEngineServer * CSSMatch::getEngine()
{
	return engine;
}

IFileSystem * CSSMatch::getFileSystem()
{
	return filesystem;
}

IGameEventManager2 * CSSMatch::getGameEventManager()
{
	return gameeventmanager;
}

IPlayerInfoManager * CSSMatch::getPlayerInfoManager()
{
	return playerinfomanager;
}

IServerPluginHelpers * CSSMatch::getServerPluginHelpers()
{
	return helpers;
}

CGlobalVars * CSSMatch::getGlobalVars()
{
	return gpGlobals;
}

ICvar * CSSMatch::getCVars()
{
	return cvars;
}

IServerGameDLL * CSSMatch::getServerGameDll()
{
	return serverDll;
}

int CSSMatch::getMaxPlayers() const
{
	return maxplayers;
}

const string & CSSMatch::getMapCourante() const
{
	return mapCourante;
}

int CSSMatch::GetCommandIndex()
{
	return m_iClientCommandIndex;
}

Authentification * CSSMatch::getAdminList()
{
	return &adminlist;
}

Match * CSSMatch::getMatch()
{
	return &match;
}

GestionTimers * CSSMatch::getTimers()
{
	return &timers;
}

// 
// The plugin is a static singleton that is exported as an interface
//
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSSMatch,
								  IServerPluginCallbacks,
								  INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
								  *CSSMatch::getInstance());

SayHook g_sayHook;
SayTeamHook g_sayTeamHook;
