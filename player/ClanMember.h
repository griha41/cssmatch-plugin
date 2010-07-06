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

#ifndef __TEAM_MEMBER_H__
#define __TEAM_MEMBER_H__

#include "Player.h"

#include <string>

namespace cssmatch
{
	/** Player stats */
	struct PlayerScore
	{
		/** Score */
		int kills;

		/** Deaths */
		int deaths;

		PlayerScore() : kills(0), deaths(0){}
	};

	/** Player state (used when the round restarts) */
	struct PlayerState
	{
		PlayerScore score;

		int health, armor;
		bool hasHelmet;

		int account;

		/** Primary/Secondary weapon */
		std::string primary, secondary;
		int hegrenades, flashbangs, smokegrenades;
		bool c4;

		/** Player coords */
		Vector vecOrigin;

		/* Player angle */
		QAngle angle;

		bool hasDefuser, hasNightVision;

		PlayerState()
			: health(0), armor(0), hasHelmet(false), account(0), hasDefuser(false), hasNightVision(false){}
	};

	/** CSSMatch player */
	class ClanMember : public Player
	{
	protected:
		/** Player state for the previous round (used if the round restarts) */
		PlayerState lastRoundState;

		/** Player state for the previous half (used if the half is restarted) */
		PlayerState lastHalfState;

		/** Current players stats */
		PlayerScore currentScore;

		/** Is this player a referee (admin)? */
		bool referee;

		// Functors
		friend struct ResetClanMember;
		friend struct SaveHalfPlayerState;
		friend struct SaveRoundPlayerState;
		friend struct RestoreHalfPlayerScore;
		friend struct RestoreRoundPlayerState;
	public:
		/** 
		 * @param index The player index
		 * @param referee Is the player a referee (admin)?
		 */
		ClanMember(int index, bool referee = false);

		// Accessors to the differents stats/states
		PlayerState * getLastRoundState();
		PlayerState * getLastHalfState();
		PlayerScore * getCurrentScore();

		// Save/Restore the current state to/from a given state var
		void saveState(PlayerState * state);
		void restoreState(PlayerState * state);

		// Rights management
		bool isReferee() const;
		void setReferee(bool isReferee);
	};

	/** Functor to quickly reset the player infos */
	struct ResetClanMember
	{
		void operator()(ClanMember * member)
		{
			member->lastRoundState = PlayerState();
			member->lastHalfState = PlayerState();
			member->currentScore = PlayerScore();
		}
	};

	/** Functor to quickly save the player infos */
	struct SaveHalfPlayerState
	{
		void operator()(ClanMember * member)
		{
			member->saveState(&member->lastHalfState);
		}
	};


	/** Functor to quickly restore the player infos */
	struct RestoreHalfPlayerScore
	{
		void operator()(ClanMember * member)
		{
			member->restoreState(&member->lastHalfState);
		}
	};

	/** Functor to quickly save the player infos */
	struct SaveRoundPlayerState
	{
		void operator()(ClanMember * member)
		{
			member->saveState(&member->lastRoundState);
		}
	};

	/** Functor to quickly restore the player infos */
	struct RestoreRoundPlayerState
	{
		void operator()(ClanMember * member)
		{
			member->restoreState(&member->lastRoundState);
		}
	};
}

#endif // __TEAM_MEMBER_H__
