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

void ConvarsAccessor::initializeInterface(CreateInterfaceFn cvarFactory) throw (ConvarsAccessorException)
{
	cvars = (ICvar *)cvarFactory(VENGINE_CVAR_INTERFACE_VERSION, NULL);

	if (cvars != NULL)
		ConCommandBaseMgr::OneTimeInit(this);
	else
		throw ConvarsAccessorException("Unable to initialize the console variables interface !");
}

ICvar * ConvarsAccessor::getConVarInterface()
{
	return cvars;
}

bool ConvarsAccessor::RegisterConCommandBase(ConCommandBase * variable)
{
	// Add the specific plugin's flag
	variable->AddFlags(FCVAR_PLUGIN);

	// Unlink from plugin only list
	variable->SetNext(0);

	// Link to engine's list instead
	cvars->RegisterConCommandBase(variable);

	return true;
}
