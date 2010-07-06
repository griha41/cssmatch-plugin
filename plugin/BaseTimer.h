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

#ifndef __BASE_TIMER_H__
#define __BASE_TIMER_H__

namespace cssmatch
{
	/** Any timer has to be derived from this "stub" class in order to use the timer in a generic way */
	class BaseTimer
	{		
	protected:
		// see struct TimerOutOfDate
		friend struct TimerOutOfDate;

		/** When (in server time seconds) the delayed function must be executed */
		float date;

		/** Is the timer cancelled ? */
		bool cancelled;
	public:
		/** Prepare a timer
		 * @param delay Delay (in seconds) before the timer callback is executed
		 */
		BaseTimer(float delay);

		virtual ~BaseTimer();

		/** Cancel this timer */
		void cancel();

		/** Execute the delayed function */
		virtual void execute() = 0;
	};

	/** Functor to quickly determine if a timer must be executed <br>
	 * If yes, the timer is executed and the operator () returns <code>true</code>
	 */
	struct TimerOutOfDate
	{
		/** Reference date */
		float date;

		/** 
		 * @param currentDate The date to compare with the timer's execution date
		 */
		TimerOutOfDate(float currentDate) : date(currentDate){}

		bool operator()(BaseTimer * timer)
		{
			bool outofdate = false;

			if (timer->date <= date)
			{
				if (! timer->cancelled)
				{
					timer->execute();
				}

				delete timer;
				outofdate = true;
			}

			return outofdate;
		}	
	};

	/** Functor to delete timers */
	struct TimerToDelete
	{
		void operator()(BaseTimer * timer)
		{
			delete timer;
		}
	};
}

#endif // __BASE_TIMER_H__
