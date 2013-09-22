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

#ifndef __TIMER_H__
#define __TIMER_H__

#include "../misc/CannotBeCopied.h"

#include <list>

#include <platform.h>

namespace cssmatch
{
    /** Timer callback */
    struct TimerCallback
    {
        /** Override the call operator to define the code to run when the corresponding timer expires */
        virtual void operator()() = 0;
    };

    /** Internal informations about timers */
    struct Timer
    {
        /** Handler of this timer */
        uint handle;

        /** Date of execution */
        float date;

        /** Callback to execute when the timer exprires */
        TimerCallback * callback;

        /**
         * @param timerHandle Handle of the timer
         * @param execDate Execution date
         * @param timerCallback Callback to run when the timer is out of date
         */
        Timer(uint timerHandle, float execDate, TimerCallback * timerCallback);
    };

    /** Functor to retrieve a timer */
    struct TimerHavingHandle
    {
        /** The handle of the timer */
        uint searchHandle;

        /**
         * @param handle Handle of the timer to search
         */
        TimerHavingHandle(uint handle);

        bool operator()(const Timer & timer) const;
    };

    /** Functor to run timers that are out of date (expired) */
    struct TimerRun
    {
        /** Current time (date) */
        float time;

        /**
         * @param curtime Current time to compare with the date of execution of each timer
         */
        TimerRun(float curtime);

        bool operator() (Timer & timer);
    };

    /** Timer manager */
    class TimerEngine : public CannotBeCopied
    {
    private:
        /** Handle of the next timer */
        static uint nextHandle;

        /** Pending timer list */
        std::list<Timer> timers;

    public:
        /** Invalid handle value */
        static const uint INVALID_HANDLE;

        ~TimerEngine();

        /**
         * Add a timer. Delay the execution of <code>callback</code> of <code>delay</code> seconds.
         * Requires <code>ServerPlugin</code> to be declared.
         * @param delay Delay in seconds before execution.
         * @param callback Callback to execute. 
         * Instance of <code>TimerCallback</code>. <b>Will be copied</b>.
         */
        template<typename TimerCallbackInst>
        uint addTimer(float delay, const TimerCallbackInst & callback)
        {
            ServerPlugin * plugin = ServerPlugin::getInstance();
            ValveInterfaces * interfaces = plugin->getInterfaces();

            Timer timer(nextHandle, interfaces->gpGlobals->curtime + delay,
                new TimerCallbackInst(callback));
            timers.push_front(timer);
            // push front as timers could use timers
            nextHandle++;

            return timer.handle;
        }

        /**
         * Cancel a timer
         * @param handle Handle of the timer to cancel
         */
        void cancelTimer(uint handle);

        /**
         * Tell the timer manager about the current time (date). So the timer manager execute
         * timers that are out of date
         * @param time Current time
         */
        void frame(float time);

        /** Cancel all the timers. */
        void cancelAll();
    };
}

#endif // __TIMER_H__