/* 
 * Copyright 2008-2011 Nicolas Maingot
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

#include "Player.h"
#include "../plugin/ServerPlugin.h"
#include "../messages/Menu.h"

#include "toolframework/itoolentity.h"

#include <sstream>

using namespace cssmatch;

using std::string;
using std::map;
using std::ostringstream;

EntityProp Player::accountHandler("CCSPlayer","m_iAccount");
EntityProp Player::lifeStateHandler("CBasePlayer","m_lifeState");
EntityProp Player::playerStateHandler("CCSPlayer","m_iPlayerState");
//EntityProp Player::vecOriginHandler("CCSPlayer","baseclass.baseclass.baseclass.baseclass.baseclass.baseclass.m_vecOrigin");
//EntityProp Player::angRotationHandler("CBaseEntity","m_angRotation");
//EntityProp Player::eyeAngles0Handler("CCSPlayer","m_angEyeAngles[0]");
//EntityProp Player::eyeAngles1Handler("CCSPlayer","m_angEyeAngles[1]");
//EntityProp Player::armorHandler("CCSPlayer","m_ArmorValue");

Player::Player(int index) throw (PlayerException) : lastCommandDate(0.0f), menuTimer(NULL)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	identity.index = index;

	identity.pEntity = interfaces->engine->PEntityOfEntIndex(index);
	if (! isValidEntity(identity.pEntity))
		throw PlayerException("The plugin was unable to construct a Player instance => player entity not found");

	identity.userid = interfaces->engine->GetPlayerUserId(identity.pEntity);
	if (! isValidPlayerInfoUserid(identity.userid))
		throw PlayerException("The plugin was unable to construct a Player instance => player userid not found");

	const char * tempSteamid = interfaces->engine->GetPlayerNetworkIDString(identity.pEntity);
	if (tempSteamid == NULL)
		throw PlayerException("The plugin was unable to construct a Player instance => player steamid not found");
	else
		identity.steamid = tempSteamid;
}

Player::~Player()
{
	quitMenu();
	if (menuHandler.data != NULL)
		delete menuHandler.data;
}

PlayerIdentity * Player::getIdentity()
{
	return &identity;
}

bool Player::canUseCommand()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();	

	bool can = false;

	if (interfaces->gpGlobals->curtime - lastCommandDate  > 1.0f) // max 1 command/s
	{
		can = true;
		lastCommandDate = interfaces->gpGlobals->curtime;
	}

	return can;
}

void Player::sendMenu(Menu * usedMenu, int page, const map<string,string> & parameters, bool toDelete)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();

	quitMenu();

	menuTimer = new MenuReSendTimer(4.0f,this);
	plugin->addTimer(menuTimer);

	menuHandler.menu = usedMenu;
	menuHandler.page = page;
	menuHandler.toDelete = toDelete;
	menuHandler.parameters = parameters;

	menuHandler.menu->send(this,page,parameters);
}

Menu * Player::getMenu() const
{
	return menuHandler.menu;
}

int Player::getPage() const
{
	return menuHandler.page;
}

void Player::storeMenuData(BaseMenuLineData * data)
{
	if (menuHandler.data != NULL)
		delete menuHandler.data;
	menuHandler.data = data;
}

BaseMenuLineData * const Player::getMenuData()
{
	return menuHandler.data;
}

void Player::nextPage()
{
	menuHandler.page++;
	menuHandler.menu->send(this,menuHandler.page);
}

void Player::previousPage()
{
	if (menuHandler.page > 1)
	{
		menuHandler.page--;
		menuHandler.menu->send(this,menuHandler.page);
	}
}

void Player::quitMenu()
{
	if (menuHandler.menu != NULL)
	{
		menuTimer->cancel();

		if (menuHandler.toDelete)
			delete menuHandler.menu;
		menuHandler.menu = NULL;
	}
}

TeamCode Player::getMyTeam() const
{
	TeamCode team = INVALID_TEAM;
	IPlayerInfo * pInfo = getPlayerInfo();

	if (isValidPlayerInfo(pInfo))
	{
		team = (TeamCode)pInfo->GetTeamIndex();
	
		if ((team == INVALID_TEAM) && (! pInfo->IsHLTV()))
			CSSMATCH_PRINT("The plugin was unable to find the team of a Player");
	}

	return team;
}

/*CBasePlayer * Player::getBasePlayer() const
{
	CBasePlayer * bPlayer = NULL;
	IServerUnknown * sUnknown = getServerUnknow(identity.pEntity);

	if (isValidServerUnknown(sUnknown))
		bPlayer = reinterpret_cast<CBasePlayer *>(sUnknown->GetBaseEntity());

	if (! isValidBasePlayer(bPlayer))
	{
		CSSMATCH_PRINT("The plugin was unable to find the base player pointer of a Player");
		bPlayer = NULL;
	}
	
	return bPlayer;
}*/

/*CBaseCombatCharacter * Player::getBaseCombatCharacter() const
{
	CBaseCombatCharacter * bCombatCharacter = NULL;
	CBaseEntity * bEntity = getBaseEntity(identity.pEntity);

	if (isValidBaseEntity(bEntity))
		bCombatCharacter = bEntity->MyCombatCharacterPointer();

	if (! isValidBaseCombatCharacter(bCombatCharacter))
	{
		CSSMATCH_PRINT("The plugin was unable to find the base combat character of a Player");
		bCombatCharacter = NULL;
	}

	return bCombatCharacter;
}*/

IPlayerInfo * Player::getPlayerInfo() const
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	IPlayerInfo * pInfo = interfaces->playerinfomanager->GetPlayerInfo(identity.pEntity);

	//if (! isValidPlayerInfo(pInfo))
	// Don't use isValidPlayerInfo here because it excludes SourceTv
	// For now SourceTV is added to the playerlist because it should be able to recieve our messages
	//if (pInfo == NULL)
	//{
	//	CSSMATCH_PRINT("The plugin was unable to find the player's infos of a Player");
	//}
	// => normal if the player has just been disconnected

	return pInfo;
}

void Player::kick(const string & reason) const
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();
	
	string textReason;
	
    IPlayerInfo * pInfo = getPlayerInfo();
    if (isValidPlayerInfo(pInfo) && (! pInfo->IsFakeClient()))
		textReason = i18n->getTranslation(interfaces->engine->GetClientConVarValue(identity.index,"cl_language"),reason);
    else
        textReason = "Kick bot";

	ostringstream command;
	command << "kickid " << identity.userid << " " << textReason << "\n";
	plugin->queueCommand(command.str());
}

void Player::ban(int duration, const string & reason) const
{
	ServerPlugin * plugin = ServerPlugin::getInstance();

	ostringstream command;
	command << "banid " << duration << " " << identity.userid << "\n";
	plugin->queueCommand(command.str());
	if (duration == 0)
		plugin->queueCommand("writeid\n");

	kick(reason);
}

bool Player::swap(/*bool respawn*/)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	I18nManager * i18n = plugin->getI18nManager();
	RecipientFilter recipient;
	recipient.addRecipient(identity.index);

	bool success = true;

	IPlayerInfo * pInfo = getPlayerInfo();
	if (isValidPlayerInfo(pInfo))
	{
		switch(pInfo->GetTeamIndex())
		{
		case T_TEAM:
			pInfo->ChangeTeam((int)CT_TEAM);
			if (pInfo->IsFakeClient())
				spawn();
			/*if (respawn)
			{
				i18n->showPanel(recipient,"class_ct",false);
				spawn();
			}*/
			break;
		case CT_TEAM:
			pInfo->ChangeTeam((int)T_TEAM);
			if (pInfo->IsFakeClient())
				spawn();
			/*if (respawn)
			{
				i18n->showPanel(recipient,"class_ter",false);
				spawn();
			}*/

			break;
		default:
			success = false;
		}
	}

	return success;
}

bool Player::spec()
{
	bool success = true;

	IPlayerInfo * pInfo = getPlayerInfo();
	if (isValidPlayerInfo(pInfo))
	{
		if (pInfo->GetTeamIndex() != SPEC_TEAM)
			pInfo->ChangeTeam((int)SPEC_TEAM);
		else
			success = false;
	}

	return success;
}

void Player::cexec(const string & command) const
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	interfaces->engine->ClientCommand(identity.pEntity,command.c_str());
}

void Player::sexec(const string & command) const
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	interfaces->helpers->ClientCommand(identity.pEntity,command.c_str());
}

void Player::setAccount(int newCash)
{
	try
	{
		accountHandler.getProp<int>(identity.pEntity) = newCash;
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

int Player::getAccount()
{
	int account = -1;

	try
	{
		account = accountHandler.getProp<int>(identity.pEntity);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return account;
}

void Player::setLifeState(int newState)
{
	try
	{
		lifeStateHandler.getProp<int>(identity.pEntity) = newState;
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

int Player::getLifeState()
{
	int lifeState = -1;

	try
	{
		lifeState = lifeStateHandler.getProp<int>(identity.pEntity);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return lifeState;
}

void Player::setPlayerState(int newState)
{
	try
	{
		playerStateHandler.getProp<int>(identity.pEntity) = newState;
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

int Player::getPlayerState()
{
	int playerState = -1;

	try
	{
		playerState  = playerStateHandler.getProp<int>(identity.pEntity);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return playerState;
}

/*void Player::setVecOrigin(const Vector & vec)
{*/
	/*try
	{
		vecOriginHandler.getProp<Vector>(identity.pEntity) = vec;
		identity.pEntity->m_fStateFlags |= (FL_EDICT_CHANGED | FL_FULL_EDICT_CHANGED);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}*/
/*	ServerPlugin * plugin = ServerPlugin::getInstance();
	ConVar * sv_cheats = plugin->getConVar("sv_cheats");

	ostringstream command;
	command << "setpos " << vec.x << " " << vec.y << " " << vec.z << "\n";

	sv_cheats->m_nValue = 1;
	sexec(command.str());
	sv_cheats->m_nValue = 0;
}*/

/*Vector Player::getVecOrigin()
{
	Vector origin;

	try
	{
		origin = vecOriginHandler.getProp<Vector>(identity.pEntity);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return origin;
}*/

/*Vector Player::getAngRotation()
{
	Vector ang;

	try
	{
		ang = angRotationHandler.getProp<Vector>(identity.pEntity);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return ang;
}*/

/*QAngle Player::getViewAngle()
{
	QAngle view(VEC_T_NAN,VEC_T_NAN,VEC_T_NAN);

	try
	{
		Vector rotation = getAngRotation();
		if (rotation.IsValid())
		{
			view.x = eyeAngles0Handler.getProp<float>(identity.pEntity);

			float y = eyeAngles1Handler.getProp<float>(identity.pEntity);
			if (y < 0)
				y += 360;
			view.y = y;

			view.z = rotation.z;
		}
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return view;
}*/

/*void Player::setArmor(int newArmor)
{
	try
	{
		armorHandler.getProp<int>(identity.pEntity) = newArmor;
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}

int Player::getArmor()
{
	int armor = -1;

	try
	{
		armor = armorHandler.getProp<int>(identity.pEntity);
	}
	catch(const EntityPropException & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}

	return armor;
}*/

void Player::spawn()
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	// CS:S DispatchSpawn workaround:
	setPlayerState(0);
	setLifeState(512); // review me
	//

	IServerUnknown * unknown = getServerUnknow(identity.pEntity);
	if (isValidServerUnknown(unknown))
	{
		interfaces->serverTools->DispatchSpawn(unknown);
	}
	else
	{
		CSSMATCH_PRINT("Unable to find the player to spawn")
	}
}

void Player::give(const string & item)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ConVar * sv_cheats = plugin->getConVar("sv_cheats");

	ostringstream command;
	command << "give " << item << "\n";

	sv_cheats->m_nValue = 1;
	sexec(command.str());
	sv_cheats->m_nValue = 0;
}

/*void Player::setang(const QAngle & angle)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ConVar * sv_cheats = plugin->getConVar("sv_cheats");

	ostringstream command;
	command << "setang " << angle.x << " " << angle.y << " " << angle.z << "\n";

	sv_cheats->m_nValue = 1;
	sexec(command.str());
	sv_cheats->m_nValue = 0;
}*/

MenuReSendTimer::MenuReSendTimer(float delay, Player * pl) : BaseTimer(delay), player(pl)
{
}

void MenuReSendTimer::execute()
{
	if (player->menuHandler.menu != NULL)
	{
		ServerPlugin * plugin = ServerPlugin::getInstance();

		player->menuHandler.menu->send(player,player->menuHandler.page,player->menuHandler.parameters);
		player->menuTimer = new MenuReSendTimer(4.0f,player);
		plugin->addTimer(player->menuTimer);
	}
}
