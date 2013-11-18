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
    bufferedReportPath << REPORTS_PATH << '/' << formatDate << '_' <<
    interfaces->gpGlobals->mapname.ToCStr();

    reportPath = bufferedReportPath.str();
}

BaseReport::~BaseReport()
{}
