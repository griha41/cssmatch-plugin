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

#ifndef __I18N_CON_COMMAND_H__
#define __I18N_CON_COMMAND_H__

//#include "../convars/convar.h" // ConCommand, FnCommandCallback, FnCommandCompletionCallback
#include "../misc/common.h" // because of the STL min/max conflict with Source SDK

namespace cssmatch
{
    class I18nManager;

    /** Internationalized ConCommand */
    class I18nConCommand : public ConCommand
    {
    protected:
        /** I18n manager */
        I18nManager * i18n;
    public:
        // Classic ConCommand constructors plus the i18n manager
        I18nConCommand(I18nManager * i18nManager, char const * pName, FnCommandCallback_t callback,
                       char const * pHelpString = 0, int flags = 0,
                       FnCommandCompletionCallback completionFunc = 0);

        // ConCommand methods override: dynamically allocates the memory!
        virtual char const * GetHelpText() const;
    };
}

#endif // __I18N_CON_COMMAND_H__
