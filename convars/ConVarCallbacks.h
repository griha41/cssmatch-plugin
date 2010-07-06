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

#ifndef __CON_VAR_CALLBACKS_H__
#define __CON_VAR_CALLBACKS_H__

class ConVar;

namespace cssmatch
{
	/** Make cssmatch_version constant */
	void cssmatch_version(ConVar * var, char const * pOldString);

	/* Apply changes related to cssmatch_advanced */
	//void cssmatch_advanced(ConVar * var, char const * pOldString);
}

#endif // __CON_VAR_CALLBACKS_H__
