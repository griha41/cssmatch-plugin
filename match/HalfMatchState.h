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
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#ifndef __HALF_MATCH_STATE_H__
#define __HALF_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "../misc/BaseSingleton.h"
#include "../plugin/BaseTimer.h"
#include "../messages/Menu.h"

#include "igameevents.h" // IGameEventListener2, IGameEvent

#include <map>
#include <string>

namespace cssmatch
{
    class MatchManager;
    class Menu;

    /** Half in progress <br>
     * Ends when the number of scheduled rounds is reached
     */
    class HalfMatchState
        : public BaseMatchState, public BaseSingleton<HalfMatchState>, public IGameEventListener2
    {
    private:
        typedef void (HalfMatchState::*EventCallback)(IGameEvent * event);

        /** {event => callback} map used in FireGameEvent */
        std::map<std::string, EventCallback> eventCallbacks;

        /** Menus for this state*/
        Menu * halfMenu;
        Menu * menuWithAdmin; // if cssmatch_advanced == 1

        /** Does this state finished ? */
        bool finished;

        // The last round/half has been restarted?
        bool roundRestarted, halfRestarted;

        friend class BaseSingleton<HalfMatchState>;
        HalfMatchState();
        ~HalfMatchState();
    public:
        /** End the current half <br>
         * Here is the code which musn't be executed if the match is interupted
         */
        void endHalf();

        /** Declare the half finished, so the next round start will end this state */
        void finish();

        // BaseMatchState methods
        void startState();
        void endState();
        void restartRound();
        void restartState();
        void showMenu(Player * recipient);

        // Menus callbacks
        void halfMenuCallback(Player * player, int choice, MenuLine * selected);
        void menuWithAdminCallback(Player * player, int choice, MenuLine * selected);

        // Game event callbacks
        void FireGameEvent(IGameEvent * event); // IGameEventListener2 method
        void player_death(IGameEvent * event);
        void round_start(IGameEvent * event);
        void round_end(IGameEvent * event);
    };

    /** Delayed swap */
    class SwapTimer : public BaseTimer
    {
    public:
        SwapTimer(float date);

        // BaseTimer method
        void execute();
    };
}

#endif // __HALF_MATCH_STATE_H__
