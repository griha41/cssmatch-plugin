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

#ifndef __XML_REPORT_H__
#define __XML_REPORT_H__

#include "BaseReport.h"

#include "../ticpp/ticpp.h"

namespace cssmatch
{
    class MatchClan;
    class ClanMember;
    class Player;

    /** XML report */
    class XmlReport : public BaseReport
    {
    protected:
        /** The xml document */
        ticpp::Document document;

        /** Create the xml document */
        void writeHeader();

        /** Write the report content (<cssmatch>)*/
        void writeCorpse();

        /** <plugin> */
        void writePlugin(ticpp::Element * cssmatch);
        /** <match> */
        void writeMatch(ticpp::Element * cssmatch);
        /** <teams> */
        void writeTeams(ticpp::Element * eMatch);
        /** <team> */
        void writeTeam(ticpp::Element * eTeams, MatchClan * clan);
        /** <joueurs> */
        void writeJoueurs(ticpp::Element * eTeam, MatchClan * clan);
        /** <joueur> */
        void writeJoueur(ticpp::Element * eJoueurs, ClanMember * player);
        /** <spectateurs> */
        void writeSpectateurs(ticpp::Element * eMatch);
        /** <sourcetv> */
        void writeSourcetv(ticpp::Element * eMatch);

        /* End and save the xml document */
        //void writeFooter();

    public:
        /**
         * @see BaseReport
         */
        XmlReport(MatchManager * matchManager);

        /**
         * @see BaseReport
         */
        void write();
    };
}

#endif // __BASE_REPORT_H__
