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
