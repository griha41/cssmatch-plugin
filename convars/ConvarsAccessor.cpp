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

#include "ConvarsAccessor.h"

using namespace cssmatch;

ConvarsAccessor::ConvarsAccessor() : cvars(NULL)
{
}

void ConvarsAccessor::accessRegister(CreateInterfaceFn cvarFactory) throw (ConvarsAccessorException)
{
    g_pCVar = (ICvar *)cvarFactory(CVAR_INTERFACE_VERSION, NULL);
	cvars = g_pCVar;

	if (cvars != NULL)
		ConVar_Register(0,this);
	else
		throw ConvarsAccessorException("Unable to initialize the console variables interface !");
}

void ConvarsAccessor::accessUnregister()
{
	ConVar_Unregister();
}

ICvar * ConvarsAccessor::getConVarInterface()
{
	return cvars;
}

bool ConvarsAccessor::RegisterConCommandBase(ConCommandBase * variable)
{
	// Link to engine's list
	cvars->RegisterConCommand(variable);
	
	return true;
}

