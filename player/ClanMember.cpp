/* 
 * Copyright 2008-2010 Nicolas Maingot
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

#include "ClanMember.h"

using namespace cssmatch;

ClanMember::ClanMember(int index, bool ref) : Player(index), referee(ref)
{
}

PlayerState * ClanMember::getLastRoundState()
{
	return &lastRoundState;
}

PlayerState * ClanMember::getLastHalfState()
{
	return &lastHalfState;
}

PlayerScore * ClanMember::getCurrentScore()
{
	return &currentScore;
}

void ClanMember::saveState(PlayerState * state)
{
	state->score.deaths = currentScore.deaths;
	state->score.kills = currentScore.kills;

	state->health = getHealth();
	state->armor = getArmor();

	state->hasHelmet = hasHelmet();
	state->account = getAccount();

	CBaseCombatWeapon * primary = getWeaponFromWeaponSlot(WEAPON_SLOT1);
	if (primary != NULL)
		state->primary = primary->GetName();
	CBaseCombatWeapon * secondary = getWeaponFromWeaponSlot(WEAPON_SLOT2);
	if (secondary != NULL)
		state->secondary =  secondary->GetName();
	
	state->hegrenades = getHeCount();
	state->flashbangs = getFbCount();
	state->smokegrenades = getSgCount();

	state->c4 = getWeaponFromWeaponSlot(WEAPON_SLOT5) != NULL;

	//state->vecOrigin = getVecOrigin();
	//state->angle = getViewAngle();
	state->hasDefuser = hasDefuser();
	state->hasNightVision = hasNightVision();
}

void ClanMember::restoreState(PlayerState * state)
{
	currentScore.deaths = state->score.deaths;
	currentScore.kills = state->score.kills;

	if (state->health > 0)
		setHealth(state->health);

	if (state->armor > 0)
		setArmor(state->armor);

	hasHelmet(state->hasHelmet);
	if (state->account > -1)
		setAccount(state->account);

	/*if (state->vecOrigin.IsValid())
		setVecOrigin(state->vecOrigin);*/

	if (! state->primary.empty())
	{
		removeWeapon(WEAPON_SLOT1);
		give(state->primary);
	}
	if (! state->secondary.empty())
	{
		removeWeapon(WEAPON_SLOT2);
		give(state->secondary);
	}

	if (state->hegrenades > 0)
	{
		give("weapon_hegrenade");
	}
	int fb = state->flashbangs;
	while(fb > 0)
	{
		give("weapon_flashbang");
		fb--;
	}
	if (state->smokegrenades > 0)
	{
		give("weapon_smokegrenade");
	}

	removeWeapon(WEAPON_SLOT5);
	if (state->c4)
	{
		give("weapon_c4");
	}

	hasDefuser(state->hasDefuser);
	hasNightVision(state->hasNightVision);

	/*if (state->angle.IsValid())
		setang(state->angle);*/

	// TODO: Restore the kills/deaths in the scoreboard ?			
}

bool ClanMember::isReferee() const
{
	return referee;
}

void ClanMember::setReferee(bool isReferee)
{
	referee = isReferee;
}
