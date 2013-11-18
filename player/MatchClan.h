/*
 * Copyright 2008-2013 Nicolas Maingot
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
 * along with CSSMatch; if not, see <http://www.gnu.org/licenses>.
 *
 * Additional permission under GNU GPL version 3 section 7
 *
 * If you modify CSSMatch, or any covered work, by linking or combining
 * it with "Source SDK" (or a modified version of that SDK), containing
 * parts covered by the terms of Source SDK licence, the licensors of 
 * CSSMatch grant you additional permission to convey the resulting work.
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

        /** Valid a clan name (used when the clan name is automatically detected)
         * @param newName Clan name to test
         * @param memberlist Clan members
         */
        static bool isValidClanName(const std::string & newName,
                                    const std::list<ClanMember *> & memberlist);
    public:
        MatchClan();

        /** Get the clan name */
        const std::string * getName() const;

        /** Set the clan name
         * @param newName The new clan name
         * @param forbidAutoDetect Forbid detectClanName from change the clan name
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
         * @param allow <code>true</code> to allow the clan name detection
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
