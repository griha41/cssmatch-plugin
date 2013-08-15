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

#ifndef __USER_MESSAGES_MANAGER_H__
#define __USER_MESSAGES_MANAGER_H__

#include "../misc/common.h"

class IVEngineServer;

#include <string>
#include <map>

/** Max popup size <br>
 * Beyond this value, the message can't be send in one message to the client, <br>
 * It has to be split in two or more message
 */
#define CSSMATCH_MAX_MSG_SIZE 250 // Only used for popup for now

/** Invalid message type id */
#define CSSMATCH_INVALID_MSG_TYPE 0

namespace cssmatch
{
    class RecipientFilter;

    /** Popup options which can be selected */
    enum PopupSensitivityFlags
    {
        OPTION_1 = 1<<0,
        OPTION_2 = 1<<1,
        OPTION_3 = 1<<2,
        OPTION_4 = 1<<3,
        OPTION_5 = 1<<4,
        OPTION_6 = 1<<5,
        OPTION_7 = 1<<6,
        OPTION_8 = 1<<7,
        OPTION_9 = 1<<8,

        /** The client closes the popup */
        OPTION_CANCEL = 1<<9,

        /** Any option is sensitive */
        OPTION_ALL = OPTION_1 |
                     OPTION_2 |
                     OPTION_3 |
                     OPTION_4 |
                     OPTION_5 |
                     OPTION_6 |
                     OPTION_7 |
                     OPTION_8 |
                     OPTION_9 |
                     OPTION_CANCEL
    };

    /** All MOTD type supported */
    enum MotdType
    {
        /** The MOTD URL is the message */
        TEXT = 0,
        /** The MOTD URL is a distant/local file */
        URL = 2
    };

    /** Send different kind of message to the players */
    class UserMessagesManager
    {
    private:
        /** {message type name => user message id} map filled by findMessageType */
        std::map<std::string, int> messageTypes;
    protected:
        /** Valve's IVEngineServer instance */
        IVEngineServer * engine; // Optimisation

        /** Lookup a message type id
         * @param typeName The message type name (e.g. ShowMenu)
         * @return The message type id, CSSMATCH_INVALID_MSG_TYPE otherwise
         */
        int findMessageType(const std::string & typeName);

        /* Send a console message to a single client
         * @param index The client index
         * @param message The message to send
         */
        //void consoleTell(int index, const std::string & message);
    public:
        UserMessagesManager();
        virtual ~UserMessagesManager();

        /** Send a chat message <br>
         * \001, \003 and \004 will colour the message
         * @param recipients Recipient list
         * @param message The message to send
         * @param playerIndex If specified, any part of the message after \003 will appear in the color corresponding to the player's team
         */
        void chatSay(   RecipientFilter & recipients,
                        const std::string & message,
                        int playerIndex = CSSMATCH_INVALID_INDEX);


        /** Send a colorful chat message
         * @param recipients Recipient list
         * @param message The message to send
         */
        void chatWarning(RecipientFilter & recipients, const std::string & message);

        /** Send a (windowed) popup message to the clients
         * @param recipients Recipient list
         * @param message The message to send
         * @param lifeTime Display time (in seconds)
         * @param flags Options that the player can select (see PopupSensitivityFlags)
         */
        void popupSay(  RecipientFilter & recipients,
                        const std::string & message,
                        int lifeTime,
                        int flags = OPTION_ALL);

        /** Send a centered (windowed) popup message
         * @param recipients Recipient list
         * @param message The message to send
         */
        void hintSay(RecipientFilter & recipients, const std::string & message);

        /** Send a MOTD-like message
         * @param recipients Recipient list
         * @param type Motd type
         * @param title Window title
         * @param message Message or URL
         * @see enum MotdType
         */
        void motdSay(RecipientFilter & recipients, MotdType type, const std::string & title,
                     const std::string & message);

        /** Show/Hide an existing vgui panel
         * @param recipients Recipient list
         * @param panelName Panel name
         * @param show <code>true</code>: show the panel, <code>false</code>: hide the panel
         */
        void showPanel(RecipientFilter & recipients, const std::string & panelName, bool show);

        /** Send a centered message
         * @param recipients Recipient list
         * @param message The message to send
         */
        void centerSay(RecipientFilter & recipients, const std::string & message);

        /** Send a console message
         * @param recipients Recipient list
         * @param message The message to send
         */
        void consoleSay(RecipientFilter & recipients, const std::string & message);

        /** Send a right-side (windowed) popup message
         * @param recipients Recipient list
         * @param message The message to send
         */
        void keyHintSay(RecipientFilter & recipients, const std::string & message);
    };
}

#endif // __USER_MESSAGES_MANAGER_H__
