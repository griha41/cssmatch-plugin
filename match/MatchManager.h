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

#ifndef __MATCH_MANAGER_H__
#define __MATCH_MANAGER_H__

#include "../player/MatchClan.h" // MatchClan [, ClanMember]
#include "../misc/common.h"
#include "../exceptions/BaseException.h"
#include "../messages/Countdown.h"
#include "../messages/I18nManager.h"
#include "BaseMatchState.h"

#include "igameevents.h" // IGameEventListener2, IGameEvent

#include <string>
#include <map>

namespace cssmatch
{
	class RunnableConfigurationFile;
	class ConVarMonitorTimer;
	class TvRecord;
	class Player;

	class MatchManagerException : public BaseException
	{
	public:
		MatchManagerException(const std::string & message) : BaseException(message){};
	};

	/** A match lign-up */
	struct MatchLignup
	{
		MatchClan clan1;
		MatchClan clan2;
	};

	/** Information related to a match */
	struct MatchInfo
	{
		/** Set number */
		int halfNumber;

		/** Round number */
		int roundNumber;

		/** Start date */
		tm startTime;

		/** Knife round winner */
		std::string kniferoundWinner;

		/** Did a warmup was asked by the admin? */
		bool warmup;

		MatchInfo()
			: halfNumber(1), roundNumber(1), startTime(*getLocalTime()), warmup(false){}
	};

	/** Match manager <br>
	 * Each match can be decomposed in somes states: <br>
	 * - a knife round <br>
	 * - a warmup time <br>
	 * - one or more halfs of n rounds <br>
	 * The states plus this class implement a state pattern (This class is the context)
	 */
	class MatchManager : public IGameEventListener2
	{
	private:
		/** End of match time-out countdown */
		class EndOfMatchCountdown : public BaseCountdown
		{
		private:
			int duration;
		public:
			// BaseCountdown methods
			void finish();
		};

		EndOfMatchCountdown endCountdown;

		typedef void (MatchManager::*EventCallback)(IGameEvent * event);

		/** {event => callback} map used in FireGameEvent */
		std::map<std::string,EventCallback> eventCallbacks;

		/** hostname value when the match was launched */
		std::string hostnameTemplate;

		/** Initial state */
		BaseMatchState * initialState;

		/** Current match state (e.g. kniferound, warmup, etc.) info */
		BaseMatchState * currentState;

		/** Access to the clans */
		MatchLignup lignup;

		/** Access to some information about the match */
		MatchInfo infos;

		/** SourceTv record list */
		std::list<TvRecord *> records;
		
	public:
		/** 
		 * @param initialState Initial state of the match (e.g. "no match")
		 * @throws MatchManagerException if initialState is NULL
		 */
		MatchManager(BaseMatchState * initialState) throw(MatchManagerException);

		/** Note: stop any current countdown here */
		~MatchManager();

		/** Get the match lignup */
		MatchLignup * getLignup();

		/** Get the match info */
		MatchInfo * getInfos();

		/** Get the record list */
		std::list<TvRecord *> * getRecords();

		/** Get the initial/default match state (when no match is running) */
		BaseMatchState * getInitialState() const;

		/** Get a clan by team, depending to the current half
		 * @param code The clan's team code 
		 * @throws MatchManagerException if no match is running or if the team code corresponds to a spec team
		 */
		MatchClan * getClan(TeamCode code) throw(MatchManagerException);

		// Game event callbacks
		void FireGameEvent(IGameEvent * event); // IGameEventListener2 method
		void player_activate(IGameEvent * event);
		void player_disconnect(IGameEvent * event);
		void player_team(IGameEvent * event);
		void player_changename(IGameEvent * event);

		/** Redetect a clan name
		 * @param code The clan's team code
		 * @param force <code>true</code> to bypass !teamt/!teamct
		 * @throws MatchManagerException if no match is running
		 */
		void detectClanName(TeamCode code, bool force) throw(MatchManagerException);

		/** Update "hostname" according to the clan names */
		void updateHostname();

		/** Set a new match state <br>
		 * Call the endState method of the previous state, then the startState of the new state
		 * @param newState The new match state
		 */
		void setMatchState(BaseMatchState * newState);

		/** Get the current match state */
		BaseMatchState * getMatchState() const;

		/** Start a new match <br>
		 * Do nothing if a match is already running
		 * @param config The configuration of the match
		 * @param warmup If a warmup must be done
		 * @param state The first state of the match
		 * @throws MatchManagerException if there already is a match running
		 */
		void start(RunnableConfigurationFile & config, bool warmup, BaseMatchState * state)
			 throw(MatchManagerException);

		/** Stop a running match and return to the initial state  <br>
		 * Do nothing if no match is running
		 * @throws MatchManagerException if no match is running	
		 */
		void stop() throw(MatchManagerException);

		/** Return to the initial state (plus remove all listeners/timers/records) */
		void switchToInitialState();

		/** Send to a player the admin menu corresponding to the current match state */
		void showMenu(Player * player);

		/** Restart the current round 
		 * @throws MatchManagerException if no match is running	
		 */
		void restartRound() throw (MatchManagerException);

		/** Restart the current half
		 * @throws MatchManagerException if no match is running	
		 */
		void restartHalf() throw (MatchManagerException);

		/** Send to all players a status-like message */
		void sendStatus(RecipientFilter & recipients) const;
	};


	/** Timer used to redetect the clan names (e.g. after player_changename) */
	class ClanNameDetectionTimer : public BaseTimer
	{
	private:
		/* Clan's team */
		TeamCode team;
	public:
		ClanNameDetectionTimer(float date, TeamCode teamCode);

		/** @see BaseTimer */
		void execute();
	};

	/** Timer used to watch a ConVar value (e.g. sv_alltalk) <br>
	 * Installing a callback for the value changes isn't satisfying because it's easy to get around
	 */
	class ConVarMonitorTimer : public BaseTimer
	{
	private:
		/** The ConVar to watch */
		ConVar * toWatch;

		/** The expected value of this variable */
		std::string value;

		/** Message to send if the ConVar value is different from the expected value */
		std::string message;
	public:
		/**
		 * @param varToWatch The ConVar to watch
		 * @param expectedValue The expected value of this variable
		 * @param warningMessage Message (its i18n keyword) to send if the ConVar is not equalto to the expected value
		 * @see BaseTimer
		 */
		ConVarMonitorTimer(	float date,
							ConVar * varToWatch, 
							const std::string & expectedValue,
							const std::string & warningMessage);

		/** @see BaseTimer */
		void execute();
	};
}

#endif // __MATCH_MANAGER_H__
