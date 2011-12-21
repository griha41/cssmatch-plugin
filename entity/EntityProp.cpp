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

#include "EntityProp.h"

#include "edict.h" // BaseEntity, edict_t
#include "dt_send.h" // SendTable

#include "../plugin/ServerPlugin.h"

#include <sstream>

using namespace cssmatch;

using std::string;
using std::getline;
using std::istringstream;

void EntityProp::getOffset(SendTable * table,  istringstream & propPathLeft)
{
    // Get the next step into the props table
    string pathNextStep;
    getline(propPathLeft, pathNextStep, '.');
    //Msg("Scanning %s...\n", propPathLeft.c_str());

    int nbrProps = table->GetNumProps();
    int i=0;
    while(i<nbrProps)
    {
        SendProp * sProp = table->GetProp(i);

        if (pathNextStep == sProp->GetName())
        {
            offset += sProp->GetOffset();
            switch(sProp->GetType())
            {
            case DPT_Int:
            case DPT_Float:
            case DPT_Vector:
            case DPT_String:
            case DPT_Array:
                // Found the prop itself, the offset is up to date
                i = nbrProps; // break
                break;
            case DPT_DataTable:
                // Step reached, go to the next step
                getOffset(sProp->GetDataTable(), propPathLeft);
                break;
            default:
                // Prop not found
                offset = 0;
                i = nbrProps; // break
            }
        }
        i++;
    }
}

void EntityProp::initialize()
{
    // Get all server classes
    IServerGameDLL * serverGameDll = ServerPlugin::getInstance()->getInterfaces()->serverGameDll;
    if (serverGameDll != NULL)
    {
        ServerClass * classes = serverGameDll->GetAllServerClasses();

        // Search the suitable class
        while (classes != NULL)
        {
            if (theClass == classes->GetName())
            {
                istringstream pathToProp(path);

                getOffset(classes->m_pTable, pathToProp);

                break;
            }
            else
                classes = classes->m_pNext; // until m_pNext is NULL
        }
    }
    else
        CSSMATCH_PRINT(string(
                        "Unable to find the offset of prop ") + theClass + 
                        ", IServerGameDll instance not ready")
}

EntityProp::EntityProp(const string & propClass, const string & propPath)
    : initialized(false), offset(0), theClass(propClass), path(propPath)
{
}
