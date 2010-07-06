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
