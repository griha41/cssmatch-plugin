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

#include "TranslationFile.h"

using namespace cssmatch;

using std::string;
using std::list;
using std::map;

void TranslationFile::parse() throw(TranslationException)
{
    list<string> lines;
    getLines(lines);

    // I - Where is the header ?
    parseHeader(lines);
    // No header => no translations
    if (header.empty())
        throw TranslationException("The file " + filePath + " does not have a valid header");

    // II - Header found, try to get the translations
    parseTranslations(lines);
}

void TranslationFile::parseHeader(list<string> & lines)
{
    while((! lines.empty()) && header.empty())
    {
        list<string>::const_reference line = lines.front();

        // Search for [some header] statement
        size_t iHeaderBegins = line.find("[");
        if (iHeaderBegins != string::npos)
        {
            size_t iHeaderEnds = line.find("]");
            if (iHeaderEnds != string::npos)
            {
                setHeader(line.substr(iHeaderBegins, iHeaderEnds - iHeaderBegins + 1));
                //Msg("Found header: '%s'\n",translationFile->getHeader().c_str());
            }
        }
        // Else we ignore this invalid line
        lines.pop_front();
    }
}

// BEURK !
void TranslationFile::parseTranslations(list<string> & lines)
{
    // Search for "keyword = translation" statement
    //	(quotation marks are optionnals, but can delimit the begin/end of a string)

    while(! lines.empty())
    {
        list<string>::const_reference line = lines.front();

        size_t iEqual = string::npos;
        size_t lineSize = line.size();

        bool betweenQuotes = false; // true if we are between quotation marks

        // Where is the "=" symbol ?
        string::const_iterator itChar = line.begin();
        string::const_iterator lastChar = line.end();
        bool equalFound = false;
        while((itChar != lastChar) && (! equalFound))
        {
            ++iEqual;
            switch(*itChar)
            {
            case '"': // Quoted string
                betweenQuotes = (! betweenQuotes);
                break;

            case '=':
                equalFound = (! betweenQuotes); // We found the equal symbol
                break;
            }

            ++itChar;
        }

        // Is the symbol between quotation marks ?
        if (iEqual != lineSize-1)
        { // No, we can get the data
            string dataName = line.substr(0, iEqual); //
            ConfigurationFile::trim(dataName);

            // Keyword part :

            size_t iLastCharDataName = dataName.size() - 1;
            if (iLastCharDataName>0)
            {
                // Remove the quotes if needed
                if ((dataName[0] == '"') && (dataName[iLastCharDataName] == '"'))
                    dataName = dataName.substr(1, iLastCharDataName - 1);

                // Get the translation value part
                string dataValue = line.substr(iEqual+1, lineSize); // +1 to pass the equal symbol
                ConfigurationFile::trim(dataValue);

                // Translation part :

                // Remove the quotes if needed
                size_t iLastCharDataValue = dataValue.size()-1;
                if (iLastCharDataValue>0)
                {
                    if ((dataValue[0] == '"') && (dataValue[iLastCharDataValue] == '"'))
                        dataValue = dataValue.substr(1, iLastCharDataValue - 1);

                    // \n are replaced by the corresponding escape sequence
                    size_t iEndLine = dataValue.find("\\n");
                    while (iEndLine != string::npos)
                    {
                        dataValue.replace(iEndLine, 2, "\n", 0, 1);
                        iEndLine = dataValue.find("\\n");
                    }

                    if ((dataName.size() > 0) && (dataValue.size() > 0))
                    {
                        addTranslation(dataName, dataValue);
                        //Msg("Found: '%s'='%s'\n",dataName.c_str(),dataValue.c_str());
                    }
                    // Else we ignore this invalid line
                }
            }
        }
        // Else we ignore this invalid line

        lines.pop_front();
    }
}

TranslationFile::TranslationFile(const string & filePath) throw(ConfigurationFileException,
                                                                TranslationException)
    : ConfigurationFile(filePath)
{
    parse();
}

TranslationFile::~TranslationFile()
{}

string TranslationFile::getHeader() const
{
    return header;
}

void TranslationFile::setHeader(const string & newHeader)
{
    header = newHeader;
}

void TranslationFile::addTranslation(const string & keyword, const string & translation)
{
    translations[keyword] = translation;
}

bool TranslationFile::keywordExists(const string & keyword) const
{
    return translations.find(keyword) != translations.end();
}

string TranslationFile::operator [](const string & keyword) throw (TranslationException)
{
    if (keywordExists(keyword))
        return translations[keyword];
    else
        throw TranslationException(
            keyword + " does not correspond to a known translation in the file " + filePath);
}
