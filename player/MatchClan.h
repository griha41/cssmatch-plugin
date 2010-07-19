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

#ifndef __MATCH_CLAN_H__
#define __MATCH_CLAN_H__

#include "Player.h"

#include <string>
#include <list>

namespace cssmatch
{
	class ClanMember;

	/** Clan stats */
	struct ClanStats
	{
		/** Score in Terrorist */
		int scoreT;

		/** Score in Counter-Terrorist */
		int scoreCT;

		ClanStats() : scoreT(0), scoreCT(0){}
	};

	/** CSSMatch clan */
	class MatchClan
	{
	protected:
		/** Clan name */
		std::string name;

		/** false if the clan name musn't be automatically detected */
		bool allowAutoDetect;

		/** Clan stats in the previous half */
		ClanStats lastHalfStats;

		/** Current clan stats */
		ClanStats stats;

		/** Is the clan ready to end the warmup time? */
		bool ready;

		/** Valid a clan name (used when the clan name is automatically detected) */
		static bool isValidClanName(const std::string & newName)
		{
			return newName.size() >= 3;
		}
	public:
		MatchClan();

		/** Get the clan name */
		const std::string * getName() const;

		/** Set the clan name 
		 * @param newName The new clan name
		 * @param forbidDetection Forbid detectClanName from change the clan name
		 */
		void setName(const std::string & newName, bool forbidAutoDetect = false);

		/** Fill a list with the players of this clan */
		void getMembers(std::list<ClanMember *> * members);

		/** Get the clan stats of the previous half */
		ClanStats * getLastHalfState();

		/** Get clan stats */
		ClanStats * getStats();

		/** Prepare this clan for a new match (reset stats, etc.) */
		void reset();

		/** Allow the detection of the name of this clan 
		 * @param allow <code>true</code to allow the clan name detection
		 */
		void setAllowDetection(bool allow);

		/** Try to automatically detect the clan name 
		 * @param force <code>true</code> to bypass !teamt/!teamct
		 */
		void detectClanName(bool force);

		/** Is the clan ready to end the warmup time? */
		bool isReady() const;

		/** Set if this clan ready to end the warmup time */
		void setReady(bool ready);
	};
}

#endif // __MATCH_CLAN_H__
