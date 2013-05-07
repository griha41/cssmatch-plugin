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

#ifndef __TIMEOUT_MATCH_STATE_H__
#define __TIMEOUT_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "MatchManager.h"
#include "../misc/BaseSingleton.h"
#include "../messages/Countdown.h"
#include "../messages/Menu.h"
#include "../plugin/ServerPlugin.h"

namespace cssmatch
{
    class TimeoutMatchTimer;

    /** Time-out between two match states
     * Ended by a timer
     */
    class TimeoutMatchState : public BaseMatchState, private BaseSingleton<TimeoutMatchState>
    {
    private:
        /** Timer used for the time-out */
        class TimeoutCountdown : public BaseCountdown
        {
        private:
            /** The state to lauch once the time-out is ended */
            BaseMatchState * nextState;
        public:
			TimeoutCountdown() : nextState(NULL){}

            // BaseCountdown method
            /**
             * @param seconds Countdown duration
             * @param state Next match state
             */
            void fire(int seconds, BaseMatchState * state)
            {
                nextState = state;
                BaseCountdown::fire(seconds);
            }

            void finish()
            {
                ServerPlugin::getInstance()->getMatch()->setMatchState(nextState);
            }
        };

        /** The time-out duration (in secs) before lauch the next match state */
        int duration;

        /** The state to lauch once the time-out ended */
        BaseMatchState * nextState;

        /** "end of time-out" countdown */
        TimeoutCountdown countdown;

        /** Menus of this state */
        Menu * timeoutMenu;
        Menu * menuWithAdmin; // if cssmatch_advanced == 1

        friend class BaseSingleton<TimeoutMatchState>;
        TimeoutMatchState();
        ~TimeoutMatchState();
    public:
        /** Do a time-out
         * @param duration The time-out duration (in secs)
         * @param nextState The state the lauch once the time-out ends
         */
        static void doTimeout(int duration, BaseMatchState * nextState);

        // BaseMatchState methods
        void startState();
        void endState();
        void restartRound();
        void restartState();
        void showMenu(Player * recipient);

        // Menus callbacks
        void timeoutMenuCallback(Player * player, int choice, MenuLine * selected);
        void menuWithAdminCallback(Player * player, int choice, MenuLine * selected);
    };
}

#endif // __TIMEOUT_MATCH_STATE_H__
