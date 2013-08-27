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
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#include "RunnableConfigurationFile.h"

#include "../plugin/ServerPlugin.h"

#include <sstream>
#include <fstream>

using namespace cssmatch;

using std::string;
//using std::pair;
//using std::list;
using std::ostringstream;

/*pair<string, string> RunnableConfigurationFile::searchData(string & line)
{
    string varName;
    string varValue;

    size_t strSize = line.length();
    string::const_iterator itChar = line.begin();
    string::const_iterator lastChar = line.end();

    // Try to find the server variable name
    size_t iDataBegin = 0;
    bool allFound = false;
    while((itChar != lastChar) && (! allFound))
    {
        if ((*itChar != ' ') && (*itChar != '\t'))
            ++iDataBegin;
        else
            allFound = true;

        ++itChar;
    }
    if (iDataBegin > 0)
        varName = line.substr(0,iDataBegin);

    // Pass the blank separators
    size_t iDataLength = 0;
    while((itChar != lastChar) && (*itChar == ' ') || (*itChar == '\t'))
    {
        ++iDataBegin;
        ++itChar;
    }

    // Try to find the server variable value
    //	We don't care about quotes, the console handles them
    if ((iDataBegin > 0) && (iDataBegin+1 < strSize))
        varValue = line.substr(iDataBegin+1,strSize-iDataBegin);

    return pair<string, string>(varName,varValue);
}

void RunnableConfigurationFile::getData()
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();
    ICvar * cvars = interfaces->convars->getConVarInterface();

    data.clear();

    list<string> lines;
    getLines(lines);

    list<string>::iterator itLine;
    for(itLine = lines.begin(); itLine != lines.end(); ++itLine)
    {
        pair<string, string> lineData = searchData(*itLine);
        if ((lineData.first.size() > 0) && (lineData.second.size() > 0))
        {
            // We only keep the server variables, and its current value
            //	The objective is to restore the value of these variables, as before the match
            ConVar * var = cvars->FindVar(lineData.first.c_str());
            if (var != NULL)
                data.insert(pair<ConVar *, string>(var,var->GetString()));
        }
    }
}*/

RunnableConfigurationFile::RunnableConfigurationFile(const string & filePath)
throw (ConfigurationFileException) : ConfigurationFile(filePath)
{
}

void RunnableConfigurationFile::execute() const
{
    execute(getPatchFromCfg());
}

void RunnableConfigurationFile::execute(const string & filePath)
{
    ServerPlugin * plugin = ServerPlugin::getInstance();
    ValveInterfaces * interfaces = plugin->getInterfaces();

    ostringstream buffer;
    buffer << "exec " << filePath << "\n";

    // Add the command to the console queue
    interfaces->engine->ServerCommand(buffer.str().c_str());

    // Execute the exec command
    interfaces->engine->ServerExecute();

    // Execute the commands added by the exec command in the console queue
    interfaces->engine->ServerExecute();
}
