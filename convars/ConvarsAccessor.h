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

#ifndef __CONVARS_ACCESSOR_H__
#define __CONVARS_ACCESSOR_H__

#include "../misc/common.h"
#include "../exceptions/BaseException.h"

#include "icvar.h" // IConCommandBaseAccessor, CreateInterfaceFn
#include "convar.h"
class ConCommandBase;
class ICvar;

namespace cssmatch
{
	class ConvarsAccessorException : public BaseException
	{
	public:
		ConvarsAccessorException(const std::string & message) : BaseException(message){}
	};

	/** Implementation of a console variables/commands accessor */
	class ConvarsAccessor : public IConCommandBaseAccessor
	{
	private:
		/** Valve's console variables/commands interface */
		ICvar * cvars;
	public:
		ConvarsAccessor();

		/** Register this accessor and init the interface
		 * @param cvarFactory The factory used to retrieve the accessor
		 * @throws ConvarsAccessorException If the console variable/command accessor cannot be initialized
		 */
		void accessRegister(CreateInterfaceFn cvarFactory) throw (ConvarsAccessorException);

		/** Unregister this accessor */
		void accessUnregister();

		/** Get the plugin's console variables/commands accessor
		 * @return The console commands/variables interface
		 */
		ICvar * getConVarInterface();

		/** IConCommandBaseAccessor method <br>
		 * Automatically add the FCVAR_PLUGIN flag to all the ConVar/ConCommand by the plugin
		 */
		bool RegisterConCommandBase(ConCommandBase * variable);
	};
}

#endif // __CONVARS_ACCESSOR_H__
