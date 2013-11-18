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

#include "ConfigurationFile.h"

#include <fstream>
#include <sstream>

using namespace cssmatch;

using std::string;
using std::getline;
using std::ifstream;
using std::list;

void ConfigurationFile::removeEndLine(string & line)
{
    size_t strSize = line.size();

    size_t iEndLine = line.find("\r"); // getline already removed \n
    if (iEndLine != string::npos)
        line = line.substr(0, iEndLine);
}

void ConfigurationFile::removeComments(string & line)
{
    size_t strSize = line.size();

    size_t iComment = line.find("//");

    if (iComment != string::npos)
        line = line.substr(0, iComment);
}

void ConfigurationFile::trim(string & line)
{
    size_t iDataBegin = 0;
    size_t iDataEnd = line.size();

    // Trim front
    string::const_iterator itChar = line.begin();
    string::const_iterator lastChar = line.end();
    while((itChar != lastChar) && ((*itChar == ' ') || (*itChar == '\t')))
    {
        iDataBegin++;
        itChar++;
    }

    // Trim back
    string::const_reverse_iterator itRChar = line.rbegin();
    string::const_reverse_iterator lastRChar = line.rend();
    while((itRChar != lastRChar) && ((*itRChar == ' ') || (*itRChar == '\t')))
    {
        iDataEnd--;
        itRChar++;
    }

    line = line.substr(iDataBegin, iDataEnd-iDataBegin);
}

ConfigurationFile::ConfigurationFile(const string & path) throw (ConfigurationFileException)
    : filePath(path)
{
    ifstream file(filePath.c_str());
    if (file.fail())
        throw ConfigurationFileException("The file " + filePath + " cannot be found");
}

string ConfigurationFile::getFileName() const
{
    string fileName;

    // Extract the file name from the file path
    size_t iPathSeparator = filePath.find_last_of("/");
    if ((iPathSeparator != string::npos) && (iPathSeparator+1 < filePath.length()))
        fileName = filePath.substr(iPathSeparator+1);
    else
        fileName = filePath;

    return fileName;
}

string ConfigurationFile::getPatchFromCfg() const
{
    string path;

    size_t iCfg = filePath.find_first_of(CFG_FOLDER_PATH);
    if (iCfg != string::npos)
        path = filePath.substr(iCfg+strlen(CFG_FOLDER_PATH));
    else
        CSSMATCH_PRINT(filePath + " isn't localized in the cstrike directory");

    return path;
}

void ConfigurationFile::getLines(list<string> & out)
{
    ifstream file(filePath.c_str());

    if (! file.fail())
    {
        string line;

        while(getline(file, line))
        {
            removeComments(line);
            removeEndLine(line);
            trim(line);

            if (line.length() > 0)
                out.push_back(line);
        }
    }
}
