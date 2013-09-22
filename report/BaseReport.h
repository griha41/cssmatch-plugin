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

#ifndef __BASE_REPORT_H__
#define __BASE_REPORT_H__

#include <string>

/** Default report folder */
#define REPORTS_PATH "cstrike/cfg/cssmatch/reports"

namespace cssmatch
{
    class MatchManager;

    /** Base class to write reports */
    class BaseReport
    {
    protected:
        /** The report path */
        std::string reportPath;

        /** Match manager */
        MatchManager * match;
    public:
        /**
         * @param matchManager The Match concerned by the report
         */
        BaseReport(MatchManager * matchManager);
        virtual ~BaseReport();

        /** Write the file on the disk */
        virtual void write() = 0;
    };
}

#endif // __BASE_REPORT_H__
