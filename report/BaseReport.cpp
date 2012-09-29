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

#include "BaseReport.h"

#include "../misc/common.h"
#include "../plugin/ServerPlugin.h"
#include "../match/MatchManager.h"

#include <sstream>
#include <ctime>

using std::ostringstream;

using namespace cssmatch;

BaseReport::BaseReport(MatchManager * matchManager) : match(matchManager)
{
    // Initialize date & path

    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();

    tm * date = getLocalTime();
    char formatDate[20];
    strftime(formatDate, sizeof(formatDate), "%Y-%m-%d_%Hh%M", date);

    ostringstream bufferedReportPath;
    bufferedReportPath << plugin->getGameDir() << "/cfg/cssmatch/reports/" << formatDate << '_' <<
    interfaces->gpGlobals->mapname.ToCStr();

    reportPath = bufferedReportPath.str();
}

BaseReport::~BaseReport()
{}
