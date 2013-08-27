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

#include "UserMessagesManager.h"

#include "RecipientFilter.h"
#include "../plugin/ServerPlugin.h"

#include "bitbuf.h"
#include "IEngineSound.h"
#include "soundflags.h" // CHAN_STATIC

#include <algorithm>
#include <sstream>

using namespace cssmatch;

using std::map;
using std::string;
using std::ostringstream;
using std::vector;

int UserMessagesManager::findMessageType(const string & typeName)
{
    int id = CSSMATCH_INVALID_MSG_TYPE;

    // Does this message type already known?
    map<string, int>::iterator itTypeId = messageTypes.find(typeName);
    if (itTypeId == messageTypes.end())
    {
        // No, search/cache it
        ServerPlugin * plugin = ServerPlugin::getInstance();
        ValveInterfaces * interfaces = plugin->getInterfaces();

        char foundName[20];
        int foundNameSize = 0;
        int i = 0;
        int nbTypes = plugin->getConVar("cssmatch_usermessages")->GetInt();
        while(i < nbTypes)
        {
            if (interfaces->serverGameDll->GetUserMessageInfo(i, foundName, sizeof(foundName),
                                                              foundNameSize))
            {
                if (typeName == foundName)
                {
                    messageTypes[typeName] = i;
                    id = i;
                    break;
                }
            }
            ++i;
        }
        if (id == CSSMATCH_INVALID_MSG_TYPE)
            CSSMATCH_PRINT("Unknown message type " + typeName)
            }
            else
            {
                // Yes
                id = itTypeId->second;
            }

        return id;
    }

    UserMessagesManager::UserMessagesManager()
    {
        engine = ServerPlugin::getInstance()->getInterfaces()->engine;
    }

    UserMessagesManager::~UserMessagesManager()
    {}

    void UserMessagesManager::chatSay(RecipientFilter & recipients, const string & message,
                                      int playerIndex)
    {
        ostringstream output;
        output << "\004[" << CSSMATCH_NAME << "]\001 " << message << "\n";

        bf_write * pBitBuf = engine->UserMessageBegin(&recipients, findMessageType("SayText"));

        pBitBuf->WriteByte(playerIndex);
        pBitBuf->WriteString(output.str().c_str());
        pBitBuf->WriteByte(1); // DOCUMENT ME

        engine->MessageEnd();
    }

    void UserMessagesManager::chatWarning(RecipientFilter & recipients, const string & message)
    {
        ostringstream output;
        output << "\004[" << CSSMATCH_NAME << "]\007FF0000 " << message << "\n";

        bf_write * pBitBuf = engine->UserMessageBegin(&recipients, findMessageType("SayText"));

        pBitBuf->WriteByte(00); // no player index
        pBitBuf->WriteString(output.str().c_str());
        pBitBuf->WriteByte(1); // DOCUMENT ME

        engine->MessageEnd();
    }

    void UserMessagesManager::popupSay( RecipientFilter & recipients,
                                        const string & message,
                                        int lifeTime,
                                        int flags)
    {
        // Only CSSMATCH_MAX_MSG_SIZE bytes can be sent in one user message
        // So, as the popup menus are generally large, they are split in n messages of
        // CSSMATCH_MAX_MSG_SIZE bytes

        int iBegin = 0;
        int popupSize = message.size();
        bool moreToSend = false;

        do
        {
            string toSend = message.substr(iBegin, CSSMATCH_MAX_MSG_SIZE);
            iBegin += CSSMATCH_MAX_MSG_SIZE;

            bf_write * pBuffer = engine->UserMessageBegin(&recipients, findMessageType("ShowMenu"));

            pBuffer->WriteShort(flags); // set the flags
            pBuffer->WriteChar(lifeTime); // set the lifetime

            moreToSend = iBegin < popupSize;
            pBuffer->WriteByte(moreToSend); // Is the message completed?

            pBuffer->WriteString(toSend.c_str()); // set the text

            engine->MessageEnd();
        }
        while(moreToSend);
    }

    void UserMessagesManager::hintSay(RecipientFilter & recipients, const string & message)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        ValveInterfaces * interfaces = plugin->getInterfaces();

        bf_write * pWrite = engine->UserMessageBegin(&recipients, findMessageType("HintText"));

        pWrite->WriteString(message.c_str());

        engine->MessageEnd();

        // Stop the HintText annoying sound
        const vector<int> * recipientlist = recipients.getVector();
        vector<int>::const_iterator itIndex;
        for (itIndex = recipientlist->begin(); itIndex != recipientlist->end(); ++itIndex)
        {
            interfaces->sounds->StopSound(*itIndex, CHAN_STATIC, "UI/hint.wav");
        }
    }

	void UserMessagesManager::keyHintSay(RecipientFilter & recipients, const std::string & message)
    {
        ServerPlugin * plugin = ServerPlugin::getInstance();
        ValveInterfaces * interfaces = plugin->getInterfaces();

        bf_write * pWrite = engine->UserMessageBegin(&recipients, findMessageType("KeyHintText"));

		pWrite->WriteByte(1); // DOCUMENT ME
        pWrite->WriteString(message.c_str());

        engine->MessageEnd();
    }

    void UserMessagesManager::motdSay(RecipientFilter & recipients, MotdType type,
                                      const string & title,
                                      const string & message)
    {
        bf_write * pWrite = engine->UserMessageBegin(&recipients, findMessageType("VGUIMenu"));

        pWrite->WriteString("info"); // Let give some info about this message
        pWrite->WriteByte(1); // 1=Show this message, 0=otherwise
        pWrite->WriteByte(3); // "title", "type" and "msg"

        pWrite->WriteString("title");
        pWrite->WriteString(title.c_str());

        pWrite->WriteString("type");
        pWrite->WriteString(toString(type).c_str());

        pWrite->WriteString("msg");
        pWrite->WriteString(message.c_str());

        engine->MessageEnd();
    }

    void UserMessagesManager::showPanel(RecipientFilter & recipients, const std::string & panelName,
                                        bool show)
    {
        bf_write * pWrite = engine->UserMessageBegin(&recipients, findMessageType("VGUIMenu"));

        pWrite->WriteString(panelName.c_str());
        pWrite->WriteByte((int)show);
        pWrite->WriteByte(0); // No more data

        engine->MessageEnd();
    }

    void UserMessagesManager::centerSay(RecipientFilter & recipients, const string & message)
    {
        bf_write * pWrite = engine->UserMessageBegin(&recipients, findMessageType("TextMsg"));

        pWrite->WriteByte(4); // DOCUMENT ME
        pWrite->WriteString(message.c_str());
        pWrite->WriteByte(0); // DOCUMENT ME

        engine->MessageEnd();
    }

    void UserMessagesManager::consoleSay(RecipientFilter & recipients, const string & message)
    {
        bf_write * pWrite = engine->UserMessageBegin(&recipients, findMessageType("TextMsg"));

        pWrite->WriteByte(HUD_PRINTNOTIFY);
        pWrite->WriteString(message.c_str());

        engine->MessageEnd();
    }

/*void UserMessagesManager::consoleTell(int index, const string & message)
{
    edict_t * pEntity = engine->PEntityOfEntIndex(index);

    if (isValidEntity(pEntity))
    {
        engine->ClientPrintf(pEntity,message.c_str()); // Crash if pEntity is a fake client or SourceTv
        //engine->ClientCommand(pEntity,("echo " + message + "\n").c_str());
    }
}*/
