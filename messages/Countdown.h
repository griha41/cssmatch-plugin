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

#ifndef __COUNTDOWN_H__
#define __COUNTDOWN_H__

#include "../misc/BaseSingleton.h"
#include "../plugin/Timer.h"

namespace cssmatch
{
    /** Base countdown displayed to each player */
    class BaseCountdown
    {
    protected:
        /** BaseCountdown step */
        class CountdownTick : public TimerCallback
        {
        private:
            /** Owner */
            BaseCountdown * countdown;

            /** Seconds left */
            int left;
        public:
            /**
             * @param owner Corresponding countdown instance
             * @param timeLeft Seconds left
             */
            CountdownTick(BaseCountdown * owner, int timeLeft);

            /** @see TimerCallback */
            void operator()();
        };
        /** Next countdown step (timer handle) */
        uint nextTick;

        /** Seconds left before the end of the countdown */
        int left;

        /** Decrement/Continue the countdown */
        void tick();

        /** Automatically executed when the countdown ends */
        virtual void finish() = 0;
    public:
        BaseCountdown();

        /** Start the countdown
         * @param seconds Seconds left until the end of the countdown
         */
        void fire(int seconds);

        /** Stop the countdown */
        void stop();
    };
}

#endif // __COUNT_DOWN_H__
