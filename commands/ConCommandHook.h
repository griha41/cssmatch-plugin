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

#ifndef __CONCOMMAND_HOOK_H__
#define __CONCOMMAND_HOOK_H__

#include "../misc/common.h" // because of the STL min/max conflict with Source SDK

namespace cssmatch
{
    class ClanMember;

    /** Callback for a hook <br>
     * The first parameter is the user <br>
     * The second parameter is the command args tokenizer
     * Returns <code>true</code> to eat the command
     */
    typedef bool (*HookCallback)(ClanMember *, const CCommand & args);

    /** Hook a command at runtime <br>
     * Do not instanciate this class once ConCommandBaseMgr::OneTimeInit invoked
     */
    class ConCommandHook : public ConCommand
    {
    protected:
        /** Pointer to the hooked command */
        ConCommand * hooked;

        /** Callback for this hook */
        HookCallback callback;

        /** Is this command protected against spam? */
        bool nospam;
    public:
        /** Similar to the ConCommand's constructor
         * @param name The name must be dynamically allocated with new!
         * @param hookCallback The function to call when the hooked command is used
         * @param antispam Does this command need to be protected against spam?
         */
        ConCommandHook(const char * name, HookCallback hookCallback, bool antispam);

        /**
         * @see ConCommand
         */
        void Init();

        /**
         * @see ConCommand
         */
        void Dispatch(const CCommand & args);
    };
}

#endif // __CONCOMMAND_HOOK_H__
