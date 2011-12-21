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

#ifndef __CLIENT_COMMAND_CALLBACKS_H__
#define __CLIENT_COMMAND_CALLBACKS_H__

#include "engine/iserverplugin.h" // PLUGIN_RESULT

namespace cssmatch
{
    class ClanMember;

    /** Client command callbacks prototype <br>
     * The first parameters is the user
     * @return see enum PLUGIN_RESULT
     */
    typedef PLUGIN_RESULT (*ClientCmdCallback)(ClanMember *, const CCommand & args);

    // Client command callbacks

    // jointeam restriction in match
    PLUGIN_RESULT clientcmd_jointeam(ClanMember * user, const CCommand & args);

    // menu actions
    PLUGIN_RESULT clientcmd_menuselect(ClanMember * user, const CCommand & args);

    // cssmatch menu
    PLUGIN_RESULT clientcmd_cssmatch(ClanMember * user, const CCommand & args);

    // show players rates
    PLUGIN_RESULT clientcmd_rates(ClanMember * user, const CCommand & args);
}

#endif // __CLIENT_COMMAND_CALLBACKS_H__
