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

#include "GestionMenus.h"
#include "../CSSMatch/CSSMatch.h"
#include "../I18n/I18n.h"

using std::string;
using std::map;

Menu GestionMenus::cutround_question;
Menu GestionMenus::strats_question;
Menu GestionMenus::phase_off;
Menu GestionMenus::phase_cutround;
Menu GestionMenus::phase_strats;
Menu GestionMenus::phase_match;
Menu GestionMenus::configurations;

Menu GestionMenus::administration;
Menu GestionMenus::admin_changelevel;
Menu GestionMenus::admin_swap;
Menu GestionMenus::admin_spec;
Menu GestionMenus::admin_kick;
Menu GestionMenus::admin_ban;
Menu GestionMenus::admin_ban_time;


void cutround_question_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		match->setCutRound(true); // la variable est déjà à true dans le constructeur de match, mais le lancement du match peut encore être annulé
		menus->afficheMenuRef(GestionMenus::strats_question,indexJoueur);
		return;
	case 2:
		match->setCutRound(false);
		menus->afficheMenuRef(GestionMenus::strats_question,indexJoueur);
		return;
	case 3:
		Api::cexec(pEntity,"cssmatch\n");
		return;
	}
	menus->quitteMenu(indexJoueur);
}

void strats_question_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		match->setStrats(true); // la variable est déjà à true dans le constructeur de match,
								//	mais le lancement du match peut encore être annulé
		menus->afficheMenuRef(GestionMenus::configurations,indexJoueur);
		return;
	case 2:
		match->setStrats(false);
		menus->afficheMenuRef(GestionMenus::configurations,indexJoueur);
		return;
	case 3:
		Api::cexec(pEntity,"cssmatch\n");
		return;
	}
	menus->quitteMenu(indexJoueur);
}

void phase_off_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		menus->afficheMenuRef(GestionMenus::cutround_question,indexJoueur);
		break;
	default:
		menus->quitteMenu(indexJoueur);
	}		
}

void phase_cutround_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		match->restartRound(pEntity);
		break;
	case 3:
		match->finMatch(pEntity);
		break;
	case 4:
		match->detecteTags(true);
	}
	menus->quitteMenu(indexJoueur);
}

void phase_strats_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		match->restartRound(pEntity);
		break;
	case 3:
		match->finMatch(pEntity);
		break;
	case 4:
		match->detecteTags(true);
		break;
	case 5:
		match->forceGo(pEntity);
	}
	menus->quitteMenu(indexJoueur);
}

void phase_match_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 2:
		match->restartRound(pEntity);
		break;
	case 3:
		match->finMatch(pEntity);
		break;
	case 4:
		match->detecteTags(true);
		break;
	case 5:
		match->restartManche(pEntity);
		break;
	}
	menus->quitteMenu(indexJoueur);
}

void configurations_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	if (choix!=10)
	{
		const Ligne * config;
		try
		{
			config = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(config->getValeur())
			{
			case MENU_CODE_RETOUR:
				menus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				menus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				match->lanceMatch(new Configuration(config->getTexte()),pEntity); // Présence du fichier recontrôlée dans lanceMatch()
				menus->quitteMenu(indexJoueur);
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			menus->quitteMenu(indexJoueur);
		}
	}
	else
		menus->quitteMenu(indexJoueur);
}

void phase_off_choixAvecAdmin(int indexJoueur, int choix)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 3:
		menus->afficheMenuRef(GestionMenus::cutround_question,indexJoueur);
		break;
	default:
		menus->quitteMenu(indexJoueur);
	}
}

void phase_cutround_choixAvecAdmin(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		menus->quitteMenu(indexJoueur);
		break;
	case 3:
		match->restartRound(pEntity);
		menus->quitteMenu(indexJoueur);
		break;
	case 4:
		match->finMatch(pEntity);
		menus->quitteMenu(indexJoueur);
		break;
	case 5:
		match->detecteTags(true);
		menus->quitteMenu(indexJoueur);
	default:
		menus->quitteMenu(indexJoueur);
	}
}

void phase_strats_choixAvecAdmin(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 3:
		match->restartRound(pEntity);
		break;
	case 4:
		match->finMatch(pEntity);
		break;
	case 5:
		match->detecteTags(true);
		break;
	case 6:
		match->forceGo(pEntity);
		break;
	default:
		menus->quitteMenu(indexJoueur);
	}
	
}

void phase_match_choixAvecAdmin(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		break;
	case 2:
		ConVars::swapAlltalk();
		Api::cexec(pEntity,"cssmatch\n");
		break;
	case 3:
		match->restartRound(pEntity);
		break;
	case 4:
		match->finMatch(pEntity);
		break;
	case 5:
		match->detecteTags(true);
		break;
	case 6:
		match->restartManche(pEntity);
		break;
	default:
		menus->quitteMenu(indexJoueur);
	}
}

void administration_choix(int indexJoueur, int choix) throw(CSSMatchApiException)
{
	edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);

	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	switch(choix)
	{
	case 1:
		menus->afficheMenuRef(GestionMenus::admin_changelevel,indexJoueur);
		break;
	case 2:
		menus->afficheMenuRef(GestionMenus::admin_swap,indexJoueur);
		break;
	case 3:
		menus->afficheMenuRef(GestionMenus::admin_spec,indexJoueur);
		break;
	case 4:
		menus->afficheMenuRef(GestionMenus::admin_kick,indexJoueur);
		break;
	case 5:
		menus->afficheMenuRef(GestionMenus::admin_ban,indexJoueur);
		break;
	case 6:
		Api::cexec(pEntity,"cssmatch\n");
		break;
	default:
		menus->quitteMenu(indexJoueur);
	}
}

void admin_changelevel_choix(int indexJoueur, int choix)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				menus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				menus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					const string * nomMap = &ligne->getTexte();
					if (engine->IsMapValid(nomMap->c_str()))
						Api::serveurExecute("changelevel " + *nomMap + "\n");
					else
					{
						map<string,string> parametres;
						parametres["$map"] = *nomMap;
						Messages::sayTell(0,indexJoueur,"admin_map_not_found",parametres);
					}
					menus->quitteMenu(indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			menus->quitteMenu(indexJoueur);
		}
	}
	else
		menus->quitteMenu(indexJoueur);
}

void admin_swap_choix(int indexJoueur, int choix)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	edict_t * pEntity = engine->PEntityOfEntIndex(indexJoueur);

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				menus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				menus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					map<string,string> parametres;

					int useridCible = ligne->getValeur();
					edict_t * pEntityCible = NULL;
					try
					{
						pEntityCible = Api::getPEntityFromID(useridCible);
						Api::swap(Api::getIPlayerInfoFromEntity(pEntityCible));
						parametres["$username"] = ligne->getTexte();
						parametres["$admin"] = Api::getPlayerName(pEntity);
						Messages::sayMsg(indexJoueur,"admin_swap",parametres);
					}
					catch(const CSSMatchApiException & e)
					{
						parametres["$username"] = ligne->getTexte();
						Messages::sayTell(0,indexJoueur,"admin_is_not_connected",parametres);
						Api::reporteException(e,__FILE__,__LINE__);
					}
					menus->afficheMenuRef(GestionMenus::admin_swap,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			menus->quitteMenu(indexJoueur);
		}
	}
	else
		menus->quitteMenu(indexJoueur);
}

void admin_spec_choix(int indexJoueur, int choix)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	edict_t * pEntity = engine->PEntityOfEntIndex(indexJoueur);

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				menus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				menus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					map<string,string> parametres;

					int useridCible = ligne->getValeur();
					edict_t * pEntityCible = NULL;
					try
					{
						pEntityCible = Api::getPEntityFromID(useridCible);
						Api::spec(Api::getIPlayerInfoFromEntity(pEntityCible));
						parametres["$username"] = ligne->getTexte();
						parametres["$admin"] = Api::getPlayerName(pEntity);
						Messages::sayMsg(indexJoueur,"admin_spec",parametres);
					}
					catch(const CSSMatchApiException & e)
					{
						parametres["$username"] = ligne->getTexte();
						Messages::sayTell(0,indexJoueur,"admin_is_not_connected",parametres);
						Api::reporteException(e,__FILE__,__LINE__);
					}
					menus->afficheMenuRef(GestionMenus::admin_spec,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			menus->quitteMenu(indexJoueur);
		}
	}
	else
		menus->quitteMenu(indexJoueur);
}

void admin_kick_choix(int indexJoueur, int choix)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	edict_t * pEntity = engine->PEntityOfEntIndex(indexJoueur);

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				menus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				menus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					map<string,string> parametres;
					
					int useridCible = ligne->getValeur();
					edict_t * pEntityCible = NULL;
					try
					{
						pEntityCible = Api::getPEntityFromID(useridCible); // Vérifie si le joueur est connecté
						Api::kickid(useridCible,"Kick par un arbitre");	
						parametres["$username"] = ligne->getTexte();
						parametres["$admin"] = Api::getPlayerName(pEntity);
						Messages::sayMsg(indexJoueur,"admin_kick",parametres);
					}
					catch(const CSSMatchApiException & e)
					{
						parametres["$username"] = ligne->getTexte();
						Messages::sayTell(0,indexJoueur,"admin_is_not_connected",parametres);
						Api::reporteException(e,__FILE__,__LINE__);
					}
					menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		}
	}
	else
		menus->quitteMenu(indexJoueur);
}

void admin_ban_choix(int indexJoueur, int choix)
{
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	if (choix!=10)
	{
		const Ligne * ligne = NULL;
		try
		{
			ligne = &menuJoueur->getChoix(choix,joueur->getPageMenu());
			switch(ligne->getValeur())
			{
			case MENU_CODE_RETOUR:
				menus->afficheMenuPrecedent(indexJoueur);
				break;
			case MENU_CODE_SUIVANT:
				menus->afficheMenuSuivant(indexJoueur);
				break;
			default:
				{
					int useridCible = ligne->getValeur();
					edict_t * pEntityCible = NULL;
					try
					{
						pEntityCible = Api::getPEntityFromID(useridCible); // Vérifie si le joueur est connecté
					}
					catch(const CSSMatchApiException & e)
					{
						map<string,string> parametres;
						parametres["$username"] = ligne->getTexte();
						Messages::sayTell(0,indexJoueur,"admin_is_not_connected",parametres);
						Api::reporteException(e,__FILE__,__LINE__);
					}
					joueur->setUseridCible(useridCible);
					menus->afficheMenuRef(GestionMenus::admin_ban_time,indexJoueur);
				}
			}
		}
		catch(const CSSMatchMenuException & e)
		{
			Api::reporteException(e,__FILE__,__LINE__);
			menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
		}	
	}
	else
		menus->quitteMenu(indexJoueur);
}

void admin_ban_time_choix(int indexJoueur, int choix)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	GestionMenus * menus = GestionMenus::getInstance();

	edict_t * pEntity = engine->PEntityOfEntIndex(indexJoueur);

	try
	{
		switch(choix)
		{
		case 1:
			{
				int useridCible = joueur->getUseridCible();
				Api::banid(5,useridCible,"Ban par un arbitre");
				map<string,string> parametres;
				parametres["$username"] = Api::getPlayerName(Api::getPEntityFromID(useridCible));
				parametres["$admin"] = Api::getPlayerName(pEntity);
				Messages::sayMsg(indexJoueur,"admin_temporaly_ban",parametres);
				menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				break;
			}
		case 2:
			{
				int useridCible = joueur->getUseridCible();
				Api::banid(60,useridCible,"Ban par un arbitre");
				map<string,string> parametres;
				parametres["$username"] = Api::getPlayerName(Api::getPEntityFromID(useridCible));
				parametres["$admin"] = Api::getPlayerName(pEntity);
				Messages::sayMsg(indexJoueur,"admin_temporaly_ban",parametres);
				menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				break;
			}
		case 3:
			{
				int useridCible = joueur->getUseridCible();
				Api::banid(0,useridCible,"Ban par un arbitre");
				map<string,string> parametres;
				parametres["$username"] = Api::getPlayerName(Api::getPEntityFromID(useridCible));
				parametres["$admin"] = Api::getPlayerName(pEntity);
				Messages::sayMsg(indexJoueur,"admin_permanently_ban",parametres);
				menus->afficheMenuRef(GestionMenus::administration,indexJoueur);
				break;
			}
		case 4:
			menus->afficheMenuRef(GestionMenus::admin_ban,indexJoueur);
			break;
		default:
			menus->quitteMenu(indexJoueur);
		}
	}
	catch(const CSSMatchApiException & e)
	{
		Api::reporteException(e,__FILE__,__LINE__);
		menus->quitteMenu(indexJoueur);
	}
}

GestionMenus::GestionMenus()
{
}

GestionMenus::~GestionMenus()
{
}

GestionMenus * GestionMenus::getInstance()
{
	static GestionMenus instance;
	return &instance;
}

void GestionMenus::initialiseMenuChoixCutRound()
{
	cutround_question.setCodeMenu(CODE_MENU_CUTROUND_QUESTION);
	cutround_question.setTitre("menu_play_kniferound");
	cutround_question.addLigne("menu_yes");
	cutround_question.addLigne("menu_no");
	cutround_question.addLigne(Menu::retour.getTexte()); // Pas de codeMenuParent, car la page précédente 
														 //		dépend de sourceTV
}

void GestionMenus::initialiseMenuChoixStrats()
{
	strats_question.setCodeMenu(CODE_MENU_STRATS_QUESTION);
	strats_question.setTitre("menu_play_warmup");
	strats_question.addLigne("menu_yes");
	strats_question.addLigne("menu_no");
	strats_question.addLigne(Menu::retour.getTexte()); // Pas de codeMenuParent, car la page précédente 
														 //		dépend de la config
}

void GestionMenus::initialiseMenuOff()
{
	phase_off.setCodeMenu(CODE_MENU_OFF);
	phase_off.setTitre("menu_no_match");
	phase_off.addLigne("menu_alltalk");
	phase_off.addLigne("menu_start");
}

void GestionMenus::initialiseMenuCutRound()
{
	phase_cutround.setCodeMenu(CODE_MENU_CUTROUND);
	phase_cutround.setTitre("menu_kniferound");
	phase_cutround.addLigne("menu_alltalk");
	phase_cutround.addLigne("menu_restart");
	phase_cutround.addLigne("menu_stop");
	phase_cutround.addLigne("menu_retag");
}

void GestionMenus::initialiseMenuStratsTime()
{
	phase_strats.setCodeMenu(CODE_MENU_STRATS);
	phase_strats.setTitre("menu_warmup");
	phase_strats.addLigne("menu_alltalk");
	phase_strats.addLigne("menu_restart");
	phase_strats.addLigne("menu_stop");
	phase_strats.addLigne("menu_retag");
	phase_strats.addLigne("menu_go");
}

void GestionMenus::initialiseMenuMatch()
{
	phase_match.setCodeMenu(CODE_MENU_MATCH);
	phase_match.setTitre("menu_match");
	phase_match.addLigne("menu_alltalk");
	phase_match.addLigne("menu_restart");
	phase_match.addLigne("menu_stop");
	phase_match.addLigne("menu_retag");
	phase_match.addLigne("menu_restart_manche");
}

void GestionMenus::initialiseMenuConfiguration()
{
	configurations.setCodeMenu(CODE_MENU_CONFIG);
	configurations.setCodeMenuParent(CODE_MENU_CUTROUND_QUESTION);
	configurations.setTitre("menu_config");
	configurations.specialise(CFGLIST);
	// Corps mit à jour à chaque affichage du menu
}

void GestionMenus::initialiseMenuAdministration()
{
	administration.setCodeMenu(CODE_MENU_ADMIN);
	administration.setTitre("menu_administration");
	administration.addLigne("menu_changelevel");
	administration.addLigne("menu_swap");
	administration.addLigne("menu_spec");
	administration.addLigne("menu_kick");
	administration.addLigne("menu_ban");
	administration.addLigne(Menu::retour.getTexte()); // Remplace le codeMenuParent, car le menu parent dépend de la phase de match
}

void GestionMenus::initialiseMenuChangelevel()
{
	admin_changelevel.setCodeMenu(CODE_MENU_CHANGELEVEL);
	admin_changelevel.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_changelevel.setTitre("menu_map");
	admin_changelevel.specialise(MAPLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuSwap()
{
	admin_swap.setCodeMenu(CODE_MENU_SWAP);
	admin_swap.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_swap.setTitre("menu_player");
	admin_swap.specialise(PLAYERLIST_SANS_SPEC);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuSpec()
{
	admin_spec.setCodeMenu(CODE_MENU_SPEC);
	admin_spec.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_spec.setTitre("menu_player");
	admin_spec.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuKick()
{
	admin_kick.setCodeMenu(CODE_MENU_KICK);
	admin_kick.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_kick.setTitre("menu_player");
	admin_kick.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuBan()
{
	admin_ban.setCodeMenu(CODE_MENU_BAN);
	admin_ban.setCodeMenuParent(CODE_MENU_ADMIN);
	admin_ban.setTitre("menu_player");
	admin_ban.specialise(PLAYERLIST);
	// Corps en fonction des joueurs connectés
}

void GestionMenus::initialiseMenuBanTime()
{
	admin_ban_time.setCodeMenu(CODE_MENU_BAN_TIME);
	admin_ban_time.setTitre("menu_ban_time");
	admin_ban_time.addLigne("menu_5_min");
	admin_ban_time.addLigne("menu_1_h");
	admin_ban_time.addLigne("menu_permanent");
	admin_ban_time.addLigne(Menu::retour.getTexte()); // Remplace le codeMenuParent, car le menu parent dépend des joueurs connectés
}

void GestionMenus::initialiseMenus()
{
	cutround_question.purge();	
	initialiseMenuChoixCutRound();
	cutround_question.setCallBack(cutround_question_choix);

	strats_question.purge();	
	initialiseMenuChoixStrats();
	strats_question.setCallBack(strats_question_choix);

	phase_off.purge();
	initialiseMenuOff();
	phase_off.setCallBack(phase_off_choix);

	phase_cutround.purge();
	initialiseMenuCutRound();
	phase_cutround.setCallBack(phase_cutround_choix);

	phase_strats.purge();
	initialiseMenuStratsTime();
	phase_strats.setCallBack(phase_strats_choix);

	phase_match.purge();
	initialiseMenuMatch();
	phase_match.setCallBack(phase_match_choix);

	// configurations.purge(); // dans Menu::cfglist()
	initialiseMenuConfiguration();
	configurations.setCallBack(configurations_choix);

	// On purge les menus que l'on utilisera plus si l'on passe cssmatch_advaced de 1 à 0
	administration.purge();
	admin_changelevel.purge();
	admin_swap.purge();
	admin_spec.purge();
}

void GestionMenus::initialiseMenusAvecAdministration()
{
	phase_off.purge();
	phase_off.addLigne("menu_administration_options");
	initialiseMenuOff();
	phase_off.setCallBack(phase_off_choixAvecAdmin);

	phase_cutround.purge();
	phase_cutround.addLigne("menu_administration_options");
	initialiseMenuCutRound();
	phase_cutround.setCallBack(phase_cutround_choixAvecAdmin);

	phase_strats.purge();
	phase_strats.addLigne("menu_administration_options");
	initialiseMenuStratsTime();
	phase_strats.setCallBack(phase_strats_choixAvecAdmin);

	phase_match.purge();
	phase_match.addLigne("menu_administration_options");
	initialiseMenuMatch();
	phase_match.setCallBack(phase_match_choixAvecAdmin);

	// configurations.purge(); // dans Menu::cfglist()
	initialiseMenuConfiguration();
	configurations.setCallBack(configurations_choix);

	administration.purge();
	initialiseMenuAdministration();
	administration.setCallBack(administration_choix);

	// admin_changelevel.purge(); // 
	initialiseMenuChangelevel();
	admin_changelevel.setCallBack(admin_changelevel_choix);

	// admin_swap.purge(); // dans le playerlist()
	initialiseMenuSwap();
	admin_swap.setCallBack(admin_swap_choix);

	// admin_spec.purge(); // dans le playerlist()
	initialiseMenuSpec();
	admin_spec.setCallBack(admin_spec_choix);

	// admin_kick.purge() // dans le playerlist()
	initialiseMenuKick();
	admin_kick.setCallBack(admin_kick_choix);

	// admin_ban.purge() dans le playerlist()
	initialiseMenuBan();
	admin_ban.setCallBack(admin_ban_choix);

	admin_ban_time.purge();
	initialiseMenuBanTime();
	admin_ban_time.setCallBack(admin_ban_time_choix);
}

void GestionMenus::afficheMenuAlltalkOption(Menu & menu, int ligne, int indexJoueur)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	I18n * i18n = I18n::getInstance();
	GestionMenus * menus = GestionMenus::getInstance();

	// Récupération du nom de la langue du joueur
	//string nomLocale = I18n::getNomLocale(engine->GetClientConVarValue(indexJoueur,"cl_langage"));
	string nomLocale = engine->GetClientConVarValue(indexJoueur,"cl_language");

	map<string,string> parametres;
	if (ConVars::sv_alltalk->GetBool())
		parametres["$action"] = i18n->getMessage(nomLocale,"menu_disable");
	else
		parametres["$action"] = i18n->getMessage(nomLocale,"menu_enable");
	menus->afficheMenuRef(menu,indexJoueur,parametres);
}

void GestionMenus::afficheMenuRef(Menu & menu,int indexJoueur, const map<string,string> & parametres)
{
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);

	// On donne au joueur une du menu (voir FIXME sur la copie des menus)
	joueur->setMenu(menu);

	// On récupère l'adresse mémoire de la copie du menu spécifique au joueur
	Menu * menuJoueur = joueur->getMenu();

	// Si le menu est spécialisé, on le construit 
	switch(menuJoueur->getSpecialisation())
	{
	case PLAYERLIST:
		menuJoueur->playerlist();
		break;
	case PLAYERLIST_SANS_SPEC:
		menuJoueur->playerlist(false);
		break;
	case CFGLIST:
		menuJoueur->cfglist(DOSSIER_CONFIGURATIONS_MATCH);
		break;
	case MAPLIST:
		menuJoueur->filelist(FICHIER_MAPLIST);
	}

	try
	{
		menuJoueur->affiche(indexJoueur,1,parametres);
	}
	catch(const CSSMatchMenuException & e)
	{
		map<string,string> parametresSite;
		parametresSite["$site"] = CSSMATCH_SITE;
		Messages::sayTell(0,indexJoueur,"menu_cant_display",parametresSite);
		Api::debug("Unable to display a page of the menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
	}
}

void GestionMenus::afficheMenu(CodePhase phase, int indexJoueur)
{
	// En fonction de la valeur de cssmatch_advanced,
	//	les options d'administration sont prises en charge, ou non
	if (ConVars::cssmatch_advanced.GetBool())
		afficheMenuAvecAdministration(phase,indexJoueur);
	else
		afficheMenuSansAdministration(phase,indexJoueur);
}

void GestionMenus::afficheMenuSansAdministration(CodePhase phase, int indexJoueur)
{
	switch(phase)
	{
	case PHASE_OFF:
	case PHASE_FIN:
		afficheMenuAlltalkOption(phase_off,1,indexJoueur);
		break;
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		afficheMenuAlltalkOption(phase_cutround,1,indexJoueur);
		break;
	case PHASE_PASSAGE_STRATS:
	case PHASE_STRATS:
		afficheMenuAlltalkOption(phase_strats,1,indexJoueur);
		break;
	case PHASE_TRANSITION_MANCHE:
	case PHASE_PASSAGE_MANCHE:
	case PHASE_MANCHE:
		afficheMenuAlltalkOption(phase_match,1,indexJoueur);
	}
}

void GestionMenus::afficheMenuAvecAdministration(CodePhase phase, int indexJoueur)
{
	switch(phase)
	{
	case PHASE_OFF:
	case PHASE_FIN:
		afficheMenuAlltalkOption(phase_off,2,indexJoueur);
		break;
	case PHASE_PASSAGE_CUTROUND:
	case PHASE_CUTROUND:
		afficheMenuAlltalkOption(phase_cutround,2,indexJoueur);
		break;
	case PHASE_PASSAGE_STRATS:
	case PHASE_STRATS:
		afficheMenuAlltalkOption(phase_strats,2,indexJoueur);
		break;
	case PHASE_TRANSITION_MANCHE:
	case PHASE_PASSAGE_MANCHE:
	case PHASE_MANCHE:
		afficheMenuAlltalkOption(phase_match,2,indexJoueur);
	}
}

void GestionMenus::actionMenu(int indexJoueur, int choix)
{
	// Récupération du menu actuellement utilisé par le joueur
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();
	Joueur * joueur = joueurs->getJoueur(indexJoueur);
	Menu * menu = joueur->getMenu();

	// Si un menu du plugin est effectivement utilisé...
	if (menu->getCodeMenu() != CODE_MENU_INVALIDE)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromIndex(indexJoueur);
			Api::cexec(pEntity,"playgamesound UI/buttonrollover.wav\n");
		}
		catch(CSSMatchApiException) {}

		// Appel de la fonction associée
		menu->callBack(indexJoueur,choix);
	}
}

void GestionMenus::afficheMenuPrecedent(int indexJoueur)
{
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	int pageMenu = joueur->decPageMenu();
	if (pageMenu > 0)
	{
		try
		{
			menuJoueur->affiche(indexJoueur,pageMenu);
		}
		catch(const CSSMatchMenuException & e)
		{
			map<string,string> parametres;
			parametres["$site"] = CSSMATCH_SITE;
			Messages::sayTell(0,indexJoueur,"menu_cant_display",parametres);
			Api::debug("Unable to display the page of the menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
		}
	}
	else
	{
		switch(menuJoueur->getCodeMenuParent())
		{
		case CODE_MENU_CUTROUND_QUESTION:
			afficheMenuRef(cutround_question,indexJoueur);
			break;
		case CODE_MENU_ADMIN:
			afficheMenuRef(administration,indexJoueur);
		}
	}
}

void GestionMenus::afficheMenuSuivant(int indexJoueur)
{
	Joueur * joueur = GestionJoueurs::getInstance()->getJoueur(indexJoueur);
	Menu * menuJoueur = joueur->getMenu();
	try
	{
		menuJoueur->affiche(indexJoueur,joueur->incPageMenu());
	}
	catch(const CSSMatchMenuException & e)
	{
		map<string,string> parametres;
		parametres["$site"] = CSSMATCH_SITE;
		Messages::sayTell(0,indexJoueur,"menu_cant_display",parametres);
		Api::debug("Unable to display the page of the menu \"" + menuJoueur->getTitre() + "\" ! (" + string(e.what()) + ")");
	}
}

void GestionMenus::quitteMenu(int indexJoueur)
{
	// On altère la copie du menu du joueur
	GestionJoueurs::getInstance()->getJoueur(indexJoueur)->getMenu()->setCodeMenu(CODE_MENU_INVALIDE);
}
