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

#include "I18nConCommand.h"

#include "../messages/I18nManager.h"

#include <string>

using namespace cssmatch;
using std::string;

I18nConCommand::I18nConCommand(I18nManager * i18nManager, char const * pName,
                               FnCommandCallback_t callback, char const * pHelpString, int flags,
                               FnCommandCompletionCallback completionFunc)
    : ConCommand(pName, callback, pHelpString, flags, completionFunc), i18n(i18nManager)
{
}

// FIXME: detect the language of the client using "rcon the_con_command"
char const * I18nConCommand::GetHelpText() const
{
    // Return the translation

    string translation = i18n->getTranslation(i18n->getDefaultLanguage(), ConCommand::GetHelpText());

    char * text = new char [translation.size()+1];
    V_strcpy(text, translation.c_str());

    return text;
}
