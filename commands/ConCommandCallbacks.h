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

#ifndef __CON_COMMAND_CALLBACKS_H__
#define __CON_COMMAND_CALLBACKS_H__

#include "../misc/common.h" // CSSMATCH_BETA

#ifdef CSSMATCH_BETA
#define CSSMATCH_CHANGELOG_FILE "/plugin/changelogbeta.php"
#else
#define CSSMATCH_CHANGELOG_FILE "/plugin/changelog.php"
#endif

namespace cssmatch
{
    class ClanMember;

    /** Help for commands use */
    void cssm_help(const CCommand & args);

    /** Start a match */
    void cssm_start(const CCommand & args);

    /** Stop a match */
    void cssm_stop(const CCommand & args);

    /** Redetect the clan names */
    void cssm_retag(const CCommand & args);

    /** Declare a clan ready to end the warmup */
    void cssm_go(const CCommand & args);

    /** Restart the current round set */
    void cssm_restartmanche(const CCommand & args);

    /** Restart the current round */
    void cssm_restartround(const CCommand & args);

    /** Print the referee steamid list */
    void cssm_adminlist(const CCommand & args);
#ifdef _DEBUG
    /** Make a player referee by steamid (until the next map change) */
    void cssm_grant(const CCommand & args);

    /** Remove a referee by steamid (until the next map change) */
    void cssm_revoke(const CCommand & args);
#endif // _DEBUG
    /** Change the name of the terrorist clan */
    void cssm_teamt(const CCommand & args);

    /** Change the name of the counter-terrorist clan */
    void cssm_teamct(const CCommand & args);

    /** Swap a player by userid */
    void cssm_swap(const CCommand & args);

    /** Put a player to the spectator team, by userid */
    void cssm_spec(const CCommand & args);

    /** !go, !score, !teamt, etc. */
    bool say_hook(ClanMember * user, const CCommand & args);

    /* Want to stop the current record ? */
    //bool tv_stoprecord_hook(int userIndex);
}

#endif // __CON_COMMAND_CALLBACKS_H__
