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

#ifndef __KNIFEROUND_MATCH_STATE_H__
#define __KNIFEROUND_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "../misc/BaseSingleton.h"
#include "../player/Player.h" // TeamCode
#include "../messages/Menu.h"

#include "igameevents.h" // IGameEventListener2, IGameEvent

#include <map>
#include <string>

namespace cssmatch
{
	class MatchManager;
	class Menu;

	/** Knife round in progress <br>
	 * End with the end of the round (if there is a winner)
	 */
	class KnifeRoundMatchState 
		: public BaseMatchState, public BaseSingleton<KnifeRoundMatchState>, public IGameEventListener2
	{
	private:
		typedef void (KnifeRoundMatchState::*EventCallback)(IGameEvent * event);

		/** {event => callback} map used in FireGameEvent */
		std::map<std::string,EventCallback> eventCallbacks;

		/** Menus for this state*/
		Menu * kniferoundMenu;	
		Menu * menuWithAdmin; // if cssmatch_advanced == 1

		friend class BaseSingleton<KnifeRoundMatchState>;
		KnifeRoundMatchState();
		~KnifeRoundMatchState();
	public:
		/** End the warmup time <br>
		 * Here is the code which musn't be executed if the match is interupted during the kniferound
		 * @param winner Id of the team which win the round
		 */
		void endKniferound(TeamCode winner);

		// BaseMatchState methods
		void startState();
		void endState();
		void restartRound();
		void restartState();
		void showMenu(Player * recipient);

		// Menus callbacks
		void kniferoundMenuCallback(Player * player, int choice, MenuLine * selected);
		void menuWithAdminCallback(Player * player, int choice, MenuLine * selected);

		// Game event callbacks
		void FireGameEvent(IGameEvent * event); // IGameEventListener2 method
		void round_start(IGameEvent * event);
		void item_pickup(IGameEvent * event);
		void player_spawn(IGameEvent * event);
		void round_end(IGameEvent * event);
		//void bomb_beginplant(IGameEvent * event);
	};
}

#endif // __KNIFEROUND_MATCH_STATE_H__
