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

#include "I18nManager.h"

#include "../configuration/TranslationFile.h"
#include "../misc/common.h"
#include "../plugin/ServerPlugin.h"

#include "../convars/convar.h"
#include "eiface.h"

#include <vector>

using namespace cssmatch;

using std::string;
using std::map;
using std::vector;

map<string, string> I18nManager::WITHOUT_PARAMETERS;

void I18nManager::updateMessageCache(	int recipientIndex,
									const std::string & language,
									const std::string & keyword,
									const std::map<std::string, std::string> & parameters)
{
	// Is the message already in the cache?
	map<string,I18nMessage>::iterator itCache = messageCache.find(language);
	if (itCache == messageCache.end())
	{
		// No, construct/cache it

		messageCache[language].message = getTranslation(language,keyword,parameters);
		messageCache[language].recipients.addRecipient(recipientIndex);
	}
	else
	{
		// Yes, just add the new recipient to the recipient list

		itCache->second.recipients.addRecipient(recipientIndex);
	}
}

I18nManager::I18nManager() : defaultLanguage(NULL)
{
}

I18nManager::~I18nManager()
{
	map<string,TranslationFile *>::iterator itLanguage;
	for(itLanguage = languages.begin(); itLanguage != languages.end(); itLanguage++)
	{
		delete itLanguage->second;
	}
}

void I18nManager::setDefaultLanguage(ConVar * language)
{
	defaultLanguage = language;
}

string I18nManager::getDefaultLanguage() const
{
	return (defaultLanguage != NULL) ? defaultLanguage->GetString() : "";
}

TranslationFile * I18nManager::getTranslationFile(const string & language)
{
	TranslationFile * translationSet = NULL;

	// Is the language already used/known (in the cache) ?
	map<string,TranslationFile *>::iterator itLanguages = languages.find(language);
	map<string,TranslationFile *>::iterator lastLanguages = languages.end();
	if (itLanguages == lastLanguages)
	{
		// No, put these translations in the cache
		try
		{
			translationSet = new TranslationFile(TRANSLATIONS_FOLDER + language + ".txt");
			
			languages[language] = translationSet;
		}
		catch(const ConfigurationFileException & e)
		{
			// The file was not found, we'll use the default language instead
			if (defaultLanguage != NULL)
			{
				std::string defaultLanguageName = defaultLanguage->GetString();
				std::map<std::string,TranslationFile *>::iterator itDefault = languages.find(defaultLanguageName);
				if (itDefault == lastLanguages)
				{
					try
					{
						translationSet = new TranslationFile(TRANSLATIONS_FOLDER + defaultLanguageName + ".txt");
						languages[defaultLanguageName] = translationSet;
					}
					catch(const ConfigurationFileException & e)
					{
						CSSMATCH_PRINT("ERROR ! Default translation file not found !");
					}
				}
				else
					translationSet = itDefault->second;
			}
			else
			{
				CSSMATCH_PRINT("Unable to get the default language");
			}
		}
	}
	else
	{
		// Yes, the translation is already available
		translationSet = itLanguages->second;
	}

	return translationSet;
}

string I18nManager::getTranslation(	const string & lang,
									const string & keyword,
									const map<string,string> & parameters)
{
	string message;

	// We have to get the translations corresponding to this language
	TranslationFile * translation = getTranslationFile(lang);
	
	if (translation != NULL)
	{
		try
		{
			message = (*translation)[keyword]; // copying it, because we will replace the parameters

			// Relace the parameters 
			// FIXME: unnecessarily called as many times as recipients
			//	Make a lang cache system in the I18nManager methods ?
			map<string,string>::const_iterator itParameters;
			for(itParameters = parameters.begin(); itParameters != parameters.end(); itParameters++)
			{
				const string * parameter = &itParameters->first;
				size_t parameterSize = parameter->size();
				const string * value = &itParameters->second;
				size_t valueSize = value->size();

				// One option can be found multiple times
				size_t iParam = message.find(*parameter);
				while(iParam != string::npos)
				{
					message.replace(iParam,parameterSize,*value,0,valueSize);
					iParam = message.find(*parameter);
				}
			}
		}
		catch(const TranslationException & e)
		{
			//CSSMATCH_PRINT_EXCEPTION(e);
			message = "Missing translation, please update your translation files";
		}
	}
	else
	{
		message = "Missing default translation file, please update cssmatch_language";
	}

	return message;
}

void I18nManager::i18nChatSay(	RecipientFilter & recipients,
								const string & keyword,
								const map<string,string> & parameters,
								int playerIndex)
{
	const vector<int> * recipientVector = recipients.getVector();
	vector<int>::const_iterator itIndex;
	for(itIndex = recipientVector->begin(); itIndex != recipientVector->end(); itIndex++)
	{
		string language = engine->GetClientConVarValue(*itIndex,"cl_language");
		updateMessageCache(*itIndex,language,keyword,parameters);
	}

	map<string,I18nMessage>::iterator itMessage;
	for(itMessage = messageCache.begin(); itMessage != messageCache.end(); itMessage++)
	{
		chatSay(itMessage->second.recipients,itMessage->second.message,playerIndex);
	}
	messageCache.clear();
}

void I18nManager::i18nChatWarning(	RecipientFilter & recipients, 
									const string & keyword,
									const map<string,string> & parameters)
{
	const vector<int> * recipientVector = recipients.getVector();
	vector<int>::const_iterator itIndex;
	for(itIndex = recipientVector->begin(); itIndex != recipientVector->end(); itIndex++)
	{
		string language = engine->GetClientConVarValue(*itIndex,"cl_language");
		updateMessageCache(*itIndex,language,keyword,parameters);
	}

	map<string,I18nMessage>::iterator itMessage;
	for(itMessage = messageCache.begin(); itMessage != messageCache.end(); itMessage++)
	{
		chatWarning(itMessage->second.recipients,itMessage->second.message);
	}
	messageCache.clear();
}

void I18nManager::i18nPopupSay(	RecipientFilter & recipients,
								const std::string & keyword,
								int lifeTime,
								const map<string,string> & parameters,
								int flags)
{
	const vector<int> * recipientVector = recipients.getVector();
	vector<int>::const_iterator itIndex;
	for(itIndex = recipientVector->begin(); itIndex != recipientVector->end(); itIndex++)
	{
		string language = engine->GetClientConVarValue(*itIndex,"cl_language");
		updateMessageCache(*itIndex,language,keyword,parameters);
	}

	map<string,I18nMessage>::iterator itMessage;
	for(itMessage = messageCache.begin(); itMessage != messageCache.end(); itMessage++)
	{
		popupSay(itMessage->second.recipients,itMessage->second.message,lifeTime,flags);
	}
	messageCache.clear();
}

void I18nManager::i18nHintSay(	RecipientFilter & recipients,
								const string & keyword,
								const map<string,string> & parameters)
{
	const vector<int> * recipientVector = recipients.getVector();
	vector<int>::const_iterator itIndex;
	for(itIndex = recipientVector->begin(); itIndex != recipientVector->end(); itIndex++)
	{
		string language = engine->GetClientConVarValue(*itIndex,"cl_language");
		updateMessageCache(*itIndex,language,keyword,parameters);
	}

	map<string,I18nMessage>::iterator itMessage;
	for(itMessage = messageCache.begin(); itMessage != messageCache.end(); itMessage++)
	{
		hintSay(itMessage->second.recipients,itMessage->second.message);
	}
	messageCache.clear();
}

void I18nManager::i18nCenterSay(RecipientFilter & recipients,
								const string & keyword,
								const map<string,string> & parameters)
{
	const vector<int> * recipientVector = recipients.getVector();
	vector<int>::const_iterator itIndex;
	for(itIndex = recipientVector->begin(); itIndex != recipientVector->end(); itIndex++)
	{
		string language = engine->GetClientConVarValue(*itIndex,"cl_language");
		updateMessageCache(*itIndex,language,keyword,parameters);
	}

	map<string,I18nMessage>::iterator itMessage;
	for(itMessage = messageCache.begin(); itMessage != messageCache.end(); itMessage++)
	{
		centerSay(itMessage->second.recipients,itMessage->second.message);
	}
	messageCache.clear();
}

void I18nManager::i18nConsoleSay(	RecipientFilter & recipients,
									const string & keyword,
									const map<string,string> & parameters)
{
	const vector<int> * recipientVector = recipients.getVector();
	vector<int>::const_iterator itIndex;
	for(itIndex = recipientVector->begin(); itIndex != recipientVector->end(); itIndex++)
	{
		string language = engine->GetClientConVarValue(*itIndex,"cl_language");
		updateMessageCache(*itIndex,language,keyword,parameters);
	}

	map<string,I18nMessage>::iterator itMessage;
	for(itMessage = messageCache.begin(); itMessage != messageCache.end(); itMessage++)
	{
		consoleSay(itMessage->second.recipients,itMessage->second.message);
	}
	messageCache.clear();
}

void I18nManager::i18nMsg(const string & keyword, const map<string,string> & parameters)
{
	string message = getTranslation(defaultLanguage->GetString(),keyword,parameters);
	Msg("%s\n",message.c_str());

	// FIXME: uses the default language
}

TimerI18nChatSay::TimerI18nChatSay(	float delay,
									RecipientFilter & recip,
									const string & key,
									const map<string,string> & param,
									int pIndex)
	: BaseTimer(delay), recipients(recip), keyword(key), parameters(param), playerIndex(pIndex)
{
}

void TimerI18nChatSay::execute()
{
	ServerPlugin::getInstance()->getI18nManager()->i18nChatSay(recipients,keyword,parameters,playerIndex);
}

TimerI18nPopupSay::TimerI18nPopupSay(	float delay,
										RecipientFilter & recip,
										const string & key,
										int life,
										const map<string,string> & param,
										int fl)
	:	BaseTimer(delay), recipients(recip), keyword(key), lifeTime(life), parameters(param), flags(fl)
{
}

void TimerI18nPopupSay::execute()
{
	ServerPlugin::getInstance()->getI18nManager()->i18nPopupSay(recipients,keyword,lifeTime,parameters,flags);
}
