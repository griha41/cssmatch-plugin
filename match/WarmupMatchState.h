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

#ifndef __WARMUP_MATCH_STATE_H__
#define __WARMUP_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "../misc/BaseSingleton.h"
#include "../messages/Menu.h"
#include "../messages/Countdown.h"

#include "igameevents.h" // IGameEventListener2, IGameEvent

#include <map>
#include <string>

namespace cssmatch
{
    class MatchManager;
    class WarmupTimer;
    class Player;
    class Menu;

    /** Warmup time in progress <br>
     * Ends with a timeout, or if at least one player of each side types "ready"
     */
    class WarmupMatchState
        : public BaseMatchState, public BaseSingleton<WarmupMatchState>, public IGameEventListener2
    {
    private:
        /** "End of warmup" countdown */
        class WarmupCountdown : public BaseCountdown
        {
        public:
            // BaseCountdown method
            void finish()
            {
                WarmupMatchState::getInstance()->endWarmup();
            }
        };

        WarmupCountdown countdown;

        typedef void (WarmupMatchState::*EventCallback)(IGameEvent * event);

        /** {event => callback} map used in FireGameEvent */
        std::map<std::string, EventCallback> eventCallbacks;

        /** Menus for this state*/
        Menu * warmupMenu;
        Menu * menuWithAdmin; // if cssmatch_advanced == 1

        /** Does this state finished ? */
        bool finished;

        /** Remove the C4 */
        void removeC4();

        friend class BaseSingleton<WarmupMatchState>;
        WarmupMatchState();
        ~WarmupMatchState();
    public:
        /** End the warmup time <br>
         * Here is the code which musn't be executed if the match is interupted
         */
        void endWarmup();

        /** A player typed !go */
        void doGo(Player * player);

        // BaseMatchState methods
        void startState();
        void endState();
        void restartRound();
        void restartState();
        void showMenu(Player * recipient);

        // Menus callbacks
        void warmupMenuCallback(Player * player, int choice, MenuLine * selected);
        void menuWithAdminCallback(Player * player, int choice, MenuLine * selected);

        // Game event callbacks
        void FireGameEvent(IGameEvent * event); // IGameEventListener2 method
        void player_spawn(IGameEvent * event);
        void round_start(IGameEvent * event);
        void item_pickup(IGameEvent * event);
        //void bomb_beginplant(IGameEvent * event);
    };
}

#endif // __WARMUP_MATCH_STATE_H__
