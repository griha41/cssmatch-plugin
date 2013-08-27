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

#ifndef __CONFIGURATION_FILE_H__
#define __CONFIGURATION_FILE_H__

#include "../exceptions/BaseException.h"

#include "../misc/common.h"

#include <string>
#include <list>

/** cfg folder path */
#define CFG_FOLDER_PATH "cstrike/cfg/"

namespace cssmatch
{
    class ConfigurationFileException : public BaseException
    {
    public:
        ConfigurationFileException(const std::string & message) : BaseException(message){}
    };

    /** Configuration file <br>
     * Commented line statements start with //
     */
    class ConfigurationFile
    {
    protected:
        /** File's path */
        std::string filePath;
    public:
        /** Make sure that the string doesn't contain a Windows/Linux end line character <br>
         * (Typically usefull when a file is saved under Windows then read under Linux.)
         * @param line The line to parse
         */
        static void removeEndLine(std::string & line);

        /** Remove the commented part of a string
         * @param line The line to parse
         */
        static void removeComments(std::string & line);

        /** Trim the line
         * @param line The line to parse
         */
        static void trim(std::string & line);

        /** Construct a handler for a configuration file
         * @param filePath The path of the file
         * @throw ConfigurationFileException if the file was not found
         */
        ConfigurationFile(const std::string & filePath) throw (ConfigurationFileException);

        /** Get the file name */
        std::string getFileName() const;

        /** Get the file path from -not including- the cfg folder */
        std::string getPatchFromCfg() const;

        /** Get a list of all the file lines (stripped, without comments)
         * @param out Out list
         */
        void getLines(std::list<std::string> & out);
    };
}

#endif // __CONFIGURATION_FILE_H__
