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
 * along with CSSMatch; if not, see <http://www.gnu.org/licenses>.
 *
 * Additional permission under GNU GPL version 3 section 7
 *
 * If you modify CSSMatch, or any covered work, by linking or combining
 * it with "Source SDK" (or a modified version of that SDK), containing
 * parts covered by the terms of Source SDK licence, the licensors of 
 * CSSMatch grant you additional permission to convey the resulting work.
 */

#ifndef __COUNTDOWN_H__
#define __COUNTDOWN_H__

#include "../misc/BaseSingleton.h"
#include "../plugin/BaseTimer.h"

namespace cssmatch
{
    /** Base countdown displayed to each player */
    class BaseCountdown
    {
    protected:
        /** BaseCountdown step */
        class CountdownTick : public BaseTimer
        {
        private:
            /** Owner */
            BaseCountdown * countdown;

            /** Seconds left */
            int left;
        public:
            /**
             * @param owner Corresponding countdown instance
             * @param executionDate see BaseTimer
             * @param timeLeft Seconds left
             */
            CountdownTick(BaseCountdown * owner, float executionDate, int timeLeft);

            /**
             * @see BaseTimer
             */
            virtual void execute();
        };
        /** Next countdown step */
        CountdownTick * nextTick;

        /** Seconds left before the end of the countdown */
        int left;

        /** Decrement/Continue the countdown */
        void tick();

        /** Automatically executed when the countdown ends */
        virtual void finish() = 0;
    public:
        BaseCountdown();
        virtual ~BaseCountdown();

        /** Start the countdown
         * @param seconds Seconds left until the end of the countdown
         */
        void fire(int seconds);

        /** Stop the countdown */
        void stop();
    };
}

#endif // __COUNT_DOWN_H__
