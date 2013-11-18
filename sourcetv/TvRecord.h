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

#ifndef __TV_RECORD_H__
#define __TV_RECORD_H__

#include "../misc/common.h" // pragma
#include "../exceptions/BaseException.h"

#include <string>

namespace cssmatch
{
    /** Exception threw by TvRecord */
    class TvRecordException : public BaseException
    {
    public:
        TvRecordException(const std::string & message) : BaseException(message){}
    };

    class TvRecord
    {
    protected:
        /** Record name */
        std::string name;

        /** Is the record in progress? */
        bool recording;

        // Functors
        friend struct TvRecordToRemove;
    public:
        /** Prepares (but not starts) a new SourceTv record <br>
         * Unsupported characters in the record name will be replaced (see normalizeFileName)
         * @param recordName The record name (can be changed to remove the unsupported characters)
         * @throws TvRecordException if SourceTv is not connected
         */
        TvRecord(std::string & recordName) throw (TvRecordException);

        /** Get the record name */
        const std::string * getName() const;

        /** Is the record in progress? */
        bool isRecording() const;

        /** Start the record
         * @throws TvRecordException if the record is already in progress
         */
        void start() throw (TvRecordException);

        /** Stop the record
         * @throws TvRecordException if the record isn't started
         */
        void stop() throw (TvRecordException);
    };

    struct TvRecordToRemove
    {
        void operator()(TvRecord * record)
        {
            if (record->recording)
                record->stop();
            delete record;
        }
    };
}

#endif // __TV_RECORD_H__
