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

#ifndef __BASE_MATCH_STATE_H__
#define __BASE_MATCH_STATE_H__

namespace cssmatch
{
    class MatchManager;
    class Player;

    /** Base match state (e.g. disabled, kniferound, warmup, half, etc.) */
    class BaseMatchState
    {
    public:
        BaseMatchState();
        virtual ~BaseMatchState();

        /** Automatically called by the context (MatchManager) when this state starts */
        virtual void startState() = 0;

        /** Automatically called by the context (MatchManager) when this state ends */
        virtual void endState() = 0;

        /** Automatically called by the context (MatchManager) when the current round has to restart
          */
        virtual void restartRound() = 0;

        /** Automatically called by the context (MatchManager) when the current state has to restart
          */
        virtual void restartState() = 0;

        /** Send the menu to a player */
        virtual void showMenu(Player * recipient) = 0;
    };
}
#endif // __BASE_MATCH_STATE_H__
