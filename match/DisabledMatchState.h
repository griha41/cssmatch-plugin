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

#ifndef __DISABLE_MATCH_STATE_H__
#define __DISABLE_MATCH_STATE_H__

#include "BaseMatchState.h"
#include "../misc/BaseSingleton.h"
#include "../messages/Menu.h"

namespace cssmatch
{
	class MatchManager;
	class Player;

	/** Base class for "no match in progress" state */
	class DisabledMatchState : public BaseMatchState, public BaseSingleton<DisabledMatchState>
	{
	private:
		/** Data carried by the player through all menus that config the match to start */
		struct MatchMenuLineData : public BaseMenuLineData
		{
			BaseMatchState * state;
			bool warmup;

			MatchMenuLineData(BaseMatchState * firstState, bool doWarmup) : state(firstState), warmup(doWarmup){};
		};

		/** Menus for this state*/
		Menu * disabledMenu;
		Menu * menuWithAdmin; // if cssmatch_advanced == 1

		/* New match menus */
		Menu * kniferoundQuestion;
		Menu * warmupQuestion;
		friend class BaseSingleton<DisabledMatchState>;
		DisabledMatchState();
		~DisabledMatchState();
	public:
		// BaseMatchState methods
		void startState();
		void endState();
		void restartRound();
		void restartState();
		void showMenu(Player * recipient);

		// Other menus
		void showKniferoundQuestion(Player * recipient);
		void showWarmupQuestion(Player * recipient);
		void showConfigQuestion(Player * recipient);

		// Menus callbacks
		void disabledMenuCallback(Player * player, int choice, MenuLine * selected);
		void menuWithAdminCallback(Player * player, int choice, MenuLine * selected);
		void kniferoundQuestionCallback(Player * player, int choice, MenuLine * selected);
		void warmupQuestionCallback(Player * player, int choice, MenuLine * selected);
		void configQuestionCallback(Player * player, int choice, MenuLine * selected);
	};
}

#endif // __DISABLE_MATCH_STATE_H__
