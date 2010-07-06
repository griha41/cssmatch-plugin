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

#include "TvRecord.h"
#include "../plugin/ServerPlugin.h"

#include "filesystem.h"

using namespace cssmatch;

using std::string;
using std::ostringstream;

TvRecord::TvRecord(std::string & recordName) throw (TvRecordException) : recording(false)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();

	if (plugin->hltvConnected())
	{
		// Replace unsupported characters
		normalizeFileName(recordName);

		// Construct the path
		string basePath = plugin->getConVar("cssmatch_sourcetv_path")->GetString();
		string baseName;
		if (basePath.size() > 0)
			baseName = basePath + '/';
		baseName += recordName;

		/*if (baseName.size() == 0)
			throw TvRecordException("The demo name is empty");*/

		// Rename the file while it matches another existing file name on the hd
		// (Should be obselete with the current way we construct the demo name, but that can change)
		string tempName = baseName;
		int fileCount = 1;
		while(interfaces->filesystem->FileExists(tempName.c_str(),"MOD"))
		{
			ostringstream newName;

			newName << baseName << '_' << fileCount;

			tempName = newName.str();

			fileCount++;
		}
		name = baseName + ".dem";
	}
	else
		throw TvRecordException("SourceTv not connected");
}

const string * TvRecord::getName() const
{
	return &name;
}

bool TvRecord::isRecording() const
{
	return recording;
}

void TvRecord::start() throw (TvRecordException)
{
	if (! recording)
	{
		// Stops any record in progress (e.g. a record started by tv_autorecord)
		ServerPlugin::getInstance()->queueCommand("tv_stoprecord;tv_record " + name + "\n");

		recording = true;
	}
	else
		throw TvRecordException("Record already in progress");
}
		
void TvRecord::stop() throw (TvRecordException)
{
	if (recording)
	{
		recording = false;

		ServerPlugin::getInstance()->queueCommand("tv_stoprecord\n");
	}
	else
		throw TvRecordException("No record in progress");
}
