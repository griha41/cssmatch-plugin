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

#include "ConCommandHook.h"
#include "../plugin/ServerPlugin.h"
#include "../player/ClanMember.h"

#include <string>
#include <list>

using namespace cssmatch;

using std::string;
using std::list;

ConCommandHook::ConCommandHook(const char * name, HookCallback hookCallback, bool antispam)
    : ConCommand(name, (FnCommandCallback_t)NULL, CSSMATCH_NAME " Hook",
                 FCVAR_GAMEDLL), hooked(NULL), callback(hookCallback), nospam(antispam)
{
}

void ConCommandHook::Init()
{
    // Find the command to hook

    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();
    const char * name = GetName();

    if (interfaces->cvars != NULL)
    {
        const ConCommandBase * listedCommand = interfaces->cvars->GetCommands();
        bool success = false;

        while(listedCommand != NULL)
        {
            if (listedCommand->IsCommand() &&
                (listedCommand != this) &&
                (V_strcmp(listedCommand->GetName(), name) == 0))
            {
                hooked = static_cast<ConCommand *>(const_cast<ConCommandBase *>(listedCommand));
                success = true;
                break;
            }
            listedCommand = listedCommand->GetNext();
        }

        if (success)
        {
            Msg(CSSMATCH_NAME ": %s hooked\n", name);
            ConCommand::Init();
        }
        else
        {
            Msg(CSSMATCH_NAME ": failed to hook %s\n", name);
        }
    }
    else
    {
        Msg(CSSMATCH_NAME ": failed to hook %s (interface not ready)\n", name);
    }
}

void ConCommandHook::Dispatch(const CCommand & args)
{
    if (hooked != NULL)
    {
        // Call the corresponding callback, and eat the command call if asked
        try
        {
            ServerPlugin * plugin = ServerPlugin::getInstance();
            ClanMember * user = NULL;
            CSSMATCH_VALID_PLAYER(PlayerHavingIndex, plugin->GetCommandClient()+1, user)
            {
                if (nospam)
                {
                    if (user->isReferee() || user->canUseCommand())
                    {
                        if (! callback(user, args))
                            hooked->Dispatch(args);
                    }
                    else
                        hooked->Dispatch(args); // TODO: review me
                }
                else if (! callback(user, args))
                    hooked->Dispatch(args);
            }
            else // console?
                hooked->Dispatch(args);
        }
        catch(const BaseException & e)
        {
            CSSMATCH_PRINT_EXCEPTION(e);
        }
    }
}
