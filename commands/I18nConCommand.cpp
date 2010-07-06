/* 
 * Copyright 2008-2010 Nicolas Maingot
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

#include "I18nConCommand.h"

#include "../messages/I18nManager.h"

#include <string>

using namespace cssmatch;
using std::string;

I18nConCommand::I18nConCommand(	I18nManager * i18nManager,
								char const * pName,
								FnCommandCallback callback,
								char const * pHelpString,
								int flags,
								FnCommandCompletionCallback completionFunc)
	: ConCommand(pName,callback,pHelpString,flags,completionFunc), i18n(i18nManager)
{
}

// FIXME: detect the language of the client using "rcon the_con_command"
char const * I18nConCommand::GetHelpText() const
{
	// Return the translation

	string translation = i18n->getTranslation(i18n->getDefaultLanguage(),ConCommand::GetHelpText());

	char * text = new char [translation.size()+1];
	V_strcpy(text,translation.c_str());

	return text;
}
