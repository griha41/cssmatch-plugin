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

#ifndef __BASE_SINGLETON_H__
#define __BASE_SINGLETON_H__

#include "CannotBeCopied.h"

namespace cssmatch
{
	/** Pattern Singleton base */
	template<class T>
	class BaseSingleton : public CannotBeCopied
	{
	public:
		static T * getInstance()
		{
			static T instance; 
			// so:
			// - an abstract class can't be a BaseSingleton
			// - we cannot control the destruction of the instance:
			//		be carefull using the valve's interfaces in a singleton (don't use them in the destructor!)

			return &instance;
		}
	};
}

#endif // __BASE_SINGLETON_H__
