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

#include "Menu.h"
#include "I18nManager.h"
#include "../player/Player.h"
#include "../plugin/ServerPlugin.h"

#include <sstream>

using namespace cssmatch;
using std::string;
using std::vector;
using std::map;
using std::ostringstream;

Menu::Menu(/*Menu * parentMenu, */const string & menuTitle, BaseMenuCallback * menuCallback)
	: /*parent(parentMenu), */title(menuTitle), callback(menuCallback)
{
}

Menu::~Menu()
{
	vector<MenuLine *>::iterator itLine;
	for(itLine = lines.begin(); itLine != lines.end(); itLine++)
	{
		if ((*itLine)->data != NULL)
			delete (*itLine)->data;
		delete *itLine;
	}
	lines.clear();

	delete callback;
}

void Menu::doCallback(Player * user, int choice)
{
	// FIXME: BACK/NEXT loose the i18n parameters
	int page = user->getPage();
	MenuLine * selected = getLine(page,choice);

	switch(selected->type)
	{
	case BACK:
		user->previousPage();
		/*else if (parent != NULL)
			parent->send(user,1);*/
		break;
	case NEXT:
		user->nextPage();
		break;
	default:
		callback->execute(user,choice,selected);
	}
}

void Menu::addLine(bool isI18nKeyword, const string & line, BaseMenuLineData * data)
{
	int linecount = lines.size();
	MenuLine * toAdd = new MenuLine(NORMAL,isI18nKeyword,line,data);

	if (linecount < 9) // Is the first page not full?
	{
		// Yes, no problem
		lines.push_back(toAdd);

		/*if ((linecount == 0) && (parent != NULL))
		{
			lines.push_back(new MenuLine(BACK,true,"menu_back"));
		}*/
	}
	else if (linecount == 9) // Does this lines can be added on the first page?
	{
		// Yes, so there are 3 options to change/add:
		// - "More"
		// - the new line
		// - and "Back"
		// (The last two are put on a new page.)
		
		MenuLine * toMove = lines[8];
		lines[8] = new MenuLine(NEXT,true,"menu_more");
		lines.push_back(toMove);

		lines.push_back(toAdd);
		lines.push_back(new MenuLine(BACK,true,"menu_back"));
	}
	else if ((linecount % 9) == 0) // Is the current page (which is not the first page) full?
	{
		// Yes, so there are 4 option to change/add:
		// - "Back"
		// - "More"
		// - the new line
		// - "Back"

		MenuLine * toMove = lines[linecount-2];
		lines[linecount-2] = new MenuLine(BACK,true,"menu_back");
		lines[linecount-1] = new MenuLine(NEXT,true,"menu_more");
		lines.push_back(toMove);

		lines.push_back(toAdd);
		lines.push_back(new MenuLine(BACK,true,"menu_back"));
	}
	else // The new line can be added to the current page
	{
		MenuLine * back = lines[linecount-1];
		lines[linecount-1] = toAdd;
		lines.push_back(back);
	}
}

MenuLine * Menu::getLine(int page, int choice) throw(MenuException)
{
	static MenuLine close(NORMAL,true,"menu_close");

	MenuLine * line = NULL;

	if (choice == 10)
		line = &close;
	else
	{
		choice--;

		size_t index = (page-1)*9 + choice;
		if ((index < 0) || (index >= lines.size()))
			throw MenuException("Bad line index for the menu " + title);
		line = lines[index];
	}

	return line;
}

void Menu::send(Player * recipient, int page, const map<string,string> & parameters)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	I18nManager * i18n = plugin->getI18nManager();

	int playerIndex = recipient->getIdentity()->index;
	string language = interfaces->engine->GetClientConVarValue(playerIndex,"cl_language");
	int linecount = lines.size();
	RecipientFilter recipientlist;
	recipientlist.addRecipient(recipient);

	int iBegin = (page-1)*9;
	if ((iBegin >= 0) && (iBegin <= linecount))
	{
		//int iEnd = iBegin+9; // multi-page => bad sensibility
		int iEnd = iBegin + min(linecount-iBegin,9);

		// Set the options that the player will be able to select
		int optioncount = iEnd-iBegin;
		int sensibilityFlags = OPTION_CANCEL;
		if (optioncount == 9)
		{
			sensibilityFlags |= OPTION_ALL;
		}
		else
		{
			optioncount--;
			while(optioncount >= 0)
			{
				sensibilityFlags |= (1<<optioncount);
				optioncount--;
			}
		}

		ostringstream menu;
		menu << i18n->getTranslation(language,title) << "\n";

		if (linecount == 0) // Is the menu empty ?
		{
			menu << " \n" << i18n->getTranslation(language,"menu_empty") << "\n \n";

			CSSMATCH_PRINT("Empty menu " + title);
		}
		else if ((iBegin < 0) || (iEnd > linecount))
		{
			map<string,string> errorParam;
			errorParam["$site"] = CSSMATCH_SITE;
			i18n->i18nChatSay(recipientlist,"menu_cant_display",errorParam);
		}
		else
		{
			int iOption = 1;
			while(iBegin < iEnd)
			{
				MenuLine * line = lines[iBegin];

				menu << "->" << iOption << ". ";
				if (line->i18n)
					menu << i18n->getTranslation(language,line->text,parameters);
				else
					menu << line->text;
				menu << "\n";

				iOption++;
				iBegin++;
			}
		}
		menu << "0. " << i18n->getTranslation(language,"menu_close");
		i18n->popupSay(recipientlist,menu.str(),-1,sensibilityFlags);
	}
	else
		CSSMATCH_PRINT("Invalid menu page");
}
