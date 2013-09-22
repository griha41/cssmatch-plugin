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

#include <algorithm>

#include "Timer.h"

using std::find_if;
using std::list;

using namespace cssmatch;

uint TimerEngine::nextHandle = 1;

const uint TimerEngine::INVALID_HANDLE = 0;

Timer::Timer(uint timerHandle, float execDate, TimerCallback * timerCallback)
    : handle(timerHandle), date(execDate), callback(timerCallback)   
{}

TimerHavingHandle::TimerHavingHandle(uint handle) : searchHandle(handle)
{}

bool TimerHavingHandle::operator()(const Timer & timer) const
{
    return timer.handle == searchHandle;
}

TimerRun::TimerRun(float curtime) : time(curtime)
{}

bool TimerRun::operator()(Timer & timer)
{
    bool outOfDate = false;
    
    if (timer.date <= time)
    {
        (*timer.callback)();
        outOfDate = true;
        delete timer.callback;
    }

    return outOfDate;
}

TimerEngine::~TimerEngine()
{
    cancelAll();
}

void TimerEngine::cancelTimer(uint handle)
{
    const list<Timer>::iterator invalidTimer = timers.end();
    const list<Timer>::iterator itTimer =
        find_if(timers.begin(), invalidTimer, TimerHavingHandle(handle));
    if (itTimer != invalidTimer)
    {
        delete itTimer->callback;
        timers.erase(itTimer);
    }
}

void TimerEngine::frame(float time)
{
    // Execute and remove the timers out of date
    timers.remove_if(TimerRun(time));
}

void TimerEngine::cancelAll()
{
    list<Timer>::iterator itTimer;
    for (itTimer = timers.begin(); itTimer != timers.end(); ++itTimer)
    {
        delete itTimer->callback;
    }
    timers.clear();
}
