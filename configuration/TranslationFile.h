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

#ifndef __TRANSLATION_FILE_H__
#define __TRANSLATION_FILE_H__

#include "ConfigurationFile.h"

#include "../exceptions/BaseException.h"
#include "../misc/BaseSingleton.h"

#include <map>

namespace cssmatch
{
	class TranslationException : public ConfigurationFileException
	{
	public:
		TranslationException(const std::string & message) : ConfigurationFileException(message){};
	};

	/** Translation file <br>
	 * Description: <br>
	 *	- // marks the begin a commentary statement <br>
	 *	- The file must begin with a [header], e.g. [French] <br>
	 *	- 1 translation/line <br>
	 *	- Each translation line must have this format:  <br>
	 *		keyword = translation <br>
	 *	- Both keyword and translation can have quotation marks to delimit where the data begins/ends <br>
	 *  Support ANSI and UTF-8 encoding <br>
	 * Note: \n are supported
	 */
	class TranslationFile : public ConfigurationFile
	{
	private:
		/** [header] */
		std::string header;

		/** {keyword => translation} map */
		std::map<std::string,std::string> translations;

		/** Parse the translation file 
		 * @throws TranslationException if not header was found
		 */
		void parse() throw(TranslationException);

		/** Search the header 
		 * @param lines The lines to parse
		 */
		void parseHeader(std::list<std::string> & lines);

		/** Search the translations
		 * @param lines The remaining lines to parse
		 */
		void parseTranslations(std::list<std::string> & lines);
	public:
		/** Load and parse a new translation file
		 * @param filePath The path of the file to parse
		 * @throws TranslationException if not header was found (incorrect translations are just ignored)
		 */
		TranslationFile(const std::string & filePath) throw(ConfigurationFileException,TranslationException);
		virtual ~TranslationFile();

		/** Get the header content */
		std::string getHeader() const;

		/** In-memory header modification */
		void setHeader(const std::string & newHeader);

		/** Add a translation 
		 * @param keyword Identifier for the translation
		 * @param translation The translation to add
		 */
		void addTranslation(const std::string & keyword, const std::string & translation);

		/** Check if a translation exists
		 * @param keyword The keyword corresponding to the translation
		 * @return <code>true</code> if the translation exists, <code>false</code> otherwise
		 */
		bool keywordExists(const std::string & keyword) const;

		/** Get the translation corresponding to a keyword
		 * @param keyword The keyword corresponding to the translation
		 * @return The translation
		 * @throws TranslationException if the translation does not exist
		 */
		std::string operator [](const std::string & keyword) throw (TranslationException);
	};
}

#endif // __TRANSLATION_FILE_H__
