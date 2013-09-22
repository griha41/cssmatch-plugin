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

#include "Countdown.h"

#include "../plugin/ServerPlugin.h"
#include "../messages/I18nManager.h"

#include <sstream>

using namespace cssmatch;

using std::ostringstream;
using std::list;

BaseCountdown::CountdownTick::CountdownTick(BaseCountdown * owner, int timeLeft)
    : countdown(owner), left(timeLeft)
{}

void BaseCountdown::CountdownTick::operator()()
{
    // Convert the time left to minutes/seconds values
    int seconds = left;
    int minutes = (int)(seconds/60);
    //seconds -= minutes*60;
    seconds %= 60;

    // Construct the message
    ostringstream message;
    if (seconds >= 10)
    {
        if (minutes >= 10)
            message << minutes << " : " << seconds;
        else
            message << "0" <<minutes << " : " << seconds;
    }
    else
    {
        if (minutes >= 10)
            message << minutes << " : " << "0" << seconds;
        else
            message << "0" << minutes << " : " << "0" << seconds;
    }

    ServerPlugin * plugin = ServerPlugin::getInstance();
    I18nManager * i18n = plugin->getI18nManager();

    RecipientFilter recipients;
    recipients.addAllPlayers();

    i18n->hintSay(recipients, message.str());

    countdown->tick();
}

void BaseCountdown::tick()
{
    if (--left >= 0)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        TimerEngine * timers = plugin->getTimerEngine();

        nextTick = timers->addTimer(1, CountdownTick(this, left));
    }
    else
    {
        nextTick = TimerEngine::INVALID_HANDLE;
        finish();
    }
}

BaseCountdown::BaseCountdown() : left(-1), nextTick(TimerEngine::INVALID_HANDLE)
{}

void BaseCountdown::fire(int seconds)
{
    left = seconds;
    CountdownTick(this, left)();
}

void BaseCountdown::stop()
{
    if (left >= 0)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        TimerEngine * timers = plugin->getTimerEngine();

        left = -1;
        timers->cancelTimer(nextTick);
        nextTick = TimerEngine::INVALID_HANDLE;
    }
}
