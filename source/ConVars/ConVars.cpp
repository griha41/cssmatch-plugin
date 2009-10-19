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

#include "ConVars.h"
#include "../GestionMenus/GestionMenus.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;

/** Rétablit la valeur par défaut de la ConVar "cssmatch_version" si sa valeur est modifiée 
 *
 * @param var Le pointeur sur la ConVar concernée
 * @param pOldString L'ancienne valeur de la ConVar
 * @see ConVars::cssmatch_version
 *
 */
static void versionConstante(ConVar * var, const char * pOldString)
{
	if (string(var->GetString()) != CSSMATCH_VERSION_LIGHT)
		var->SetValue(CSSMATCH_VERSION_LIGHT);
}

/*static void debugLogOn(ConVar * var, const char * pOldString)
{
	if (var->GetBool())
		Api::serveurExecute("log on\n");
}*/

/** Charge ou décharge le menu d'administration selon la valeur de la ConVar "cssmatch_advanced"
 *
 * @param var Le pointeur sur la ConVar concernée
 * @param pOldString L'ancienne valeur de la ConVar
 * @see ConVars::cssmatch_advanced
 *
 */
static void swapAdvanced(ConVar * var, const char * pOldString)
{
	GestionMenus * menus = GestionMenus::getInstance();
	if (var->GetBool())
		menus->initialiseMenusAvecAdministration();
	else
		menus->initialiseMenus();
}

ConVar * ConVars::sv_cheats = NULL;
ConVar * ConVars::sv_alltalk = NULL;
ConVar * ConVars::hostname = NULL;
ConVar * ConVars::sv_password = NULL;
ConVar * ConVars::tv_enable = NULL;

ConVar ConVars::cssmatch_version = ConVar("cssmatch_version", CSSMATCH_VERSION_LIGHT, FCVAR_PLUGIN|FCVAR_NOTIFY|FCVAR_REPLICATED, "CSSMatch : Please use cssm_help", versionConstante);
ConVar ConVars::cssmatch_advanced = ConVar("cssmatch_advanced", "0", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, true, 1.0f,swapAdvanced);
ConVar ConVars::cssmatch_language = ConVar("cssmatch_language", "english", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help");
ConVar ConVars::cssmatch_report = ConVar("cssmatch_report", "1", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, true, 1.0f);

ConVar ConVars::cssmatch_kniferound = ConVar("cssmatch_kniferound", "1", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_kniferound_money = ConVar("cssmatch_kniferound_money", "0", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, true, 16000.0f);
ConVar ConVars::cssmatch_kniferound_allows_c4 = ConVar("cssmatch_kniferound_allows_c4", "1", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_end_kniferound = ConVar("cssmatch_end_kniferound", "20", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 5.0f, false, 0.0f);

ConVar ConVars::cssmatch_rounds = ConVar("cssmatch_rounds", "12", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help",true,0.0f,false,0.0f);;
ConVar ConVars::cssmatch_sets = ConVar("cssmatch_sets", "2", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help",true,0.0f,false,0.0f);;
ConVar ConVars::cssmatch_end_set = ConVar("cssmatch_end_set", "10", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 5.0f, false, 0.0f);
ConVar ConVars::cssmatch_sourcetv = ConVar("cssmatch_sourcetv", "1", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, true, 1.0f);
ConVar ConVars::cssmatch_sourcetv_path = ConVar("cssmatch_sourcetv_path", "cfg/cssmatch/sourcetv", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help");

ConVar ConVars::cssmatch_warmup_time = ConVar("cssmatch_warmup_time", "5", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help", true, 0.0f, false, 0.0f);

ConVar ConVars::cssmatch_hostname = ConVar("cssmatch_hostname", "CSSMatch : %s VS %s", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help");
ConVar ConVars::cssmatch_password = ConVar("cssmatch_password", "inwar", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help");
ConVar ConVars::cssmatch_default_config = ConVar("cssmatch_default_config", "server.cfg", FCVAR_PLUGIN, "CSSMatch : Please use cssm_help");

void ConVars::initialise()
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	ICvar * cvars = cssmatch->getCVars();

	sv_cheats = cvars->FindVar("sv_cheats");
	sv_alltalk = cvars->FindVar("sv_alltalk");
	hostname = cvars->FindVar("hostname");
	sv_password = cvars->FindVar("sv_password");
	tv_enable = cvars->FindVar("tv_enable");
}

void ConVars::swapAlltalk()
{
	if (sv_alltalk != NULL)
	{
		// On passe par la console pour préserver la notification
		if (sv_alltalk->GetBool())
			Api::serveurExecute("sv_alltalk 0\n");
		else
			Api::serveurExecute("sv_alltalk 1\n");
	}
	else
		Api::debug("Unable to find sv_alltalk");
}
