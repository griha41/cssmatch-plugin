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
