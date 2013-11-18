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

#ifndef __RUNNABLE_CONFIGURATION_FILE_H__
#define __RUNNABLE_CONFIGURATION_FILE_H__

/** Match config files path (not including cstrike nor cfg) */
#define MATCH_CONFIGURATIONS_PATH "cssmatch/configurations/"

/** Default config name */
#define DEFAULT_CONFIGURATION_FILE "mr15.cfg"

#include "ConfigurationFile.h"

//#include <map>
#include <string>

namespace cssmatch
{
    /** Configuration file designed to be executed <br>
     * In the future, CSSMatch may be able to restore the previous server configuration at the end of the match. <br>
     * So it can store the value of the variables modified by the match configuration file.
     */
    class RunnableConfigurationFile : public ConfigurationFile
    {
    private:
        /* {Server variable pointer => old value} map */
        //std::map<ConVar *, std::string> data;

        /* Extract a server variable name and its value from a text line
         * @param line The line to parse
         */
        //std::pair<std::string, std::string> searchData(std::string & line);

        /* Retrieve the server variables and their value */
        //void getData();
    public:
        /** Prepare a runnable configuration file
         * @param filePath The path of the file
         */
        RunnableConfigurationFile(const std::string & filePath) throw (ConfigurationFileException);

        /** Immediatly execute the configuration file (see RunnableConfigurationFile::execute below)
          */
        void execute() const;

        /** Immediatly execute a configuration file, then immediatly execute the commands read from the file
         * @param filePath The path of the file to execute (from - not including - the cstrike directory)
         */
        static void execute(const std::string & filePath);
    };
}

#endif // __RUNNABLE_CONFIGURATION_FILE_H__
