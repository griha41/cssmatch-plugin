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

#ifndef __RECIPIENT_FILTER_H__
#define __RECIPIENT_FILTER_H__

#include "irecipientfilter.h"

#include <vector>

namespace cssmatch
{
    class Player;

    /** User messages recipient list */
    class RecipientFilter : public IRecipientFilter
    {
    protected:
        /** Recipient list */
        std::vector<int> recipients;

    public:
        /** FIXME: not implemented
         * @see IRecipientFilter::IsReliable
         */
        virtual bool IsReliable() const;

        /** FIXME: not implemented
         * @see IRecipientFilter::IsInitMessage
         */
        virtual bool IsInitMessage() const;

        /** Get the number of recipients in this list */
        virtual int GetRecipientCount() const;

        /** Get the player index at a given recipient slot
         * @param slot Position in the list
         */
        virtual int GetRecipientIndex(int slot) const;

        /** Add a player to the recipient list
         * @param recipient The recipient to add
         */
        void addRecipient(Player * recipient);

        /** Add a player to the recipient list (DOES NOT CHECK IF THE PLAYER IS A BOT)
         * @param index The recipient index
         */
        void addRecipient(int index);

        /** Add every players to the recipient list */
        void addAllPlayers();

        /** Get a vector of the recipient list */
        const std::vector<int> * getVector() const;
    };
}

#endif // __RECIPIENT_FILTER_H__
