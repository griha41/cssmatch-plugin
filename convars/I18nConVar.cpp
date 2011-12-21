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

#include "I18nConVar.h"

#include "../messages/I18nManager.h"

using namespace cssmatch;

using std::string;

I18nConVar::I18nConVar(I18nManager * i18nManager, char const * pName, char const * pDefaultValue,
                       int flags)
    : ConVar(pName, pDefaultValue, flags), i18n(i18nManager)
{}

I18nConVar::I18nConVar(I18nManager * i18nManager, char const * pName, char const * pDefaultValue,
                       int flags,
                       char const * pHelpString)
    : ConVar(pName, pDefaultValue, flags, pHelpString), i18n(i18nManager)
{}

I18nConVar::I18nConVar( I18nManager * i18nManager,
                        char const * pName,
                        char const * pDefaultValue,
                        int flags,
                        char const * pHelpString,
                        bool bMin,
                        float fMin,
                        bool bMax,
                        float fMax)
    : ConVar(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax), i18n(i18nManager)
{}

I18nConVar::I18nConVar( I18nManager * i18nManager,
                        char const * pName,
                        char const * pDefaultValue,
                        int flags,
                        char const * pHelpString,
                        FnChangeCallback_t callback)
    : ConVar(pName, pDefaultValue, flags, pHelpString, callback), i18n(i18nManager)
{}

I18nConVar::I18nConVar( I18nManager * i18nManager,
                        char const * pName,
                        char const *pDefaultValue,
                        int flags,
                        char const * pHelpString,
                        bool bMin,
                        float fMin,
                        bool bMax,
                        float fMax,
                        FnChangeCallback_t callback)
    : ConVar(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax,
             callback), i18n(i18nManager)
{}

// FIXME: detect the language of the client using "rcon the_con_var"
char const * I18nConVar::GetHelpText() const
{
    // Return the translation

    string translation = i18n->getTranslation(i18n->getDefaultLanguage(), ConVar::GetHelpText());

    char * text = new char [translation.size()+1];
    V_strcpy(text, translation.c_str());

    return text;
}
