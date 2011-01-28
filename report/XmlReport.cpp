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

#include "../plugin/ServerPlugin.h"
#include "../match/MatchManager.h"
#include "../player/MatchClan.h"
#include "../player/ClanMember.h"
#include "../player/Player.h"
#include "../sourcetv/TvRecord.h"

// leave it here, or there are conflicting declarations for std::min & min, and std::max & max
#include "XmlReport.h"

#include <ctime>

using namespace cssmatch;

using std::list;

// FIXME: Loop ~4 times over the player list
//  clan1->getMembers
//		loop
//  clan2->getMembers
//	    loop
//  get spectators

void XmlReport::writeHeader()
{
	ticpp::Declaration * declaration = new ticpp::Declaration("1.0","UTF-8","yes");
	document.LinkEndChild(declaration);

	ticpp::StylesheetReference * stylesheet = new ticpp::StylesheetReference("text/xsl","cssmatch.xsl");
	document.LinkEndChild(stylesheet);
}

void XmlReport::writeCorpse()
{
	ticpp::Element * eCssmatch = new ticpp::Element("cssmatch");

	writePlugin(eCssmatch);
	writeMatch(eCssmatch);

	document.LinkEndChild(eCssmatch);
}

void XmlReport::writePlugin(ticpp::Element * cssmatch)
{
	ticpp::Element * ePlugin = new ticpp::Element("plugin");

	ticpp::Element * eVersion = new ticpp::Element("version",CSSMATCH_VERSION);
	ePlugin->LinkEndChild(eVersion);

	ticpp::Element * eUrl = new ticpp::Element("url",CSSMATCH_SITE);
	ePlugin->LinkEndChild(eUrl);

	cssmatch->LinkEndChild(ePlugin);
}

void XmlReport::writeMatch(ticpp::Element * cssmatch)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	ValveInterfaces * interfaces = plugin->getInterfaces();
	MatchManager * match = plugin->getMatch();

	tm * date = getLocalTime();

	char formatMatchDate[20];
	strftime(formatMatchDate,sizeof(formatMatchDate),"%Y/%m/%d",date);

	char formatEndMatchDate[20];
	strftime(formatEndMatchDate,sizeof(formatEndMatchDate),"%Hh%M",date);

	MatchLignup * lignup = match->getLignup();
	
	MatchInfo * infos = match->getInfos();

	char formatBeginMatchDate[20];
	strftime(formatBeginMatchDate,sizeof(formatBeginMatchDate),"%Hh%M",&infos->startTime);

	ticpp::Element * eMatch = new ticpp::Element("match");

	ticpp::Element * eDate = new ticpp::Element("date",formatMatchDate);
	eMatch->LinkEndChild(eDate);

	ticpp::Element * eDebut = new ticpp::Element("debut",formatBeginMatchDate);
	eMatch->LinkEndChild(eDebut);

	ticpp::Element * eFin = new ticpp::Element("fin",formatEndMatchDate);
	eMatch->LinkEndChild(eFin);

	ticpp::Element * eNom = new ticpp::Element("nom",*lignup->clan1.getName() + " versus " + *lignup->clan2.getName());
	eMatch->LinkEndChild(eNom);

	ticpp::Element * eMap = new ticpp::Element("map",interfaces->gpGlobals->mapname.ToCStr());
	eMatch->LinkEndChild(eMap);

	if (! infos->kniferoundWinner.empty())
	{
		ticpp::Element * eTagcutround = new ticpp::Element("tagcutround",infos->kniferoundWinner);
		eMatch->LinkEndChild(eTagcutround);
	}

	writeTeams(eMatch);
	writeSpectateurs(eMatch);
	writeSourcetv(eMatch);

	cssmatch->LinkEndChild(eMatch);
}

void XmlReport::writeTeams(ticpp::Element * eMatch)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	MatchLignup * lignup = match->getLignup();

	ticpp::Element * eTeams = new ticpp::Element("teams");

	writeTeam(eTeams,&lignup->clan1);
	writeTeam(eTeams,&lignup->clan2);

	eMatch->LinkEndChild(eTeams);
}

void XmlReport::writeTeam(ticpp::Element * eTeams, MatchClan * clan)
{
	ClanStats * stats = clan->getStats();

	ticpp::Element * eTeam = new ticpp::Element("team");

	ticpp::Element * eTag = new ticpp::Element("tag",*clan->getName());
	eTeam->LinkEndChild(eTag);

	ticpp::Element * eScore = new ticpp::Element("score",stats->scoreT + stats->scoreCT);
	eTeam->LinkEndChild(eScore);

	ticpp::Element * eScoret = new ticpp::Element("scoret",stats->scoreT);
	eTeam->LinkEndChild(eScoret);

	ticpp::Element * eScorect = new ticpp::Element("scorect",stats->scoreCT);
	eTeam->LinkEndChild(eScorect);

	writeJoueurs(eTeam,clan);

	eTeams->LinkEndChild(eTeam);
}

void XmlReport::writeJoueurs(ticpp::Element * eTeam, MatchClan * clan)
{
	list<ClanMember *> playerlist;
	clan->getMembers(&playerlist);	

	ticpp::Element * eJoueurs = new ticpp::Element("joueurs");

	list<ClanMember *>::const_iterator itPlayer;
	for(itPlayer = playerlist.begin(); itPlayer != playerlist.end(); itPlayer++)
	{
		writeJoueur(eJoueurs,*itPlayer);
	}

	eTeam->LinkEndChild(eJoueurs);
}

void XmlReport::writeJoueur(ticpp::Element * eJoueurs, ClanMember * player)
{
	IPlayerInfo * pInfo = player->getPlayerInfo();
	PlayerScore * stats = player->getCurrentScore();

	if (isValidPlayerInfo(pInfo)) // excludes SourceTv
	{
		ticpp::Element * eJoueur = new ticpp::Element("joueur");
		eJoueur->SetAttribute("steamid",pInfo->GetNetworkIDString());

		ticpp::Element * ePseudo = new ticpp::Element("pseudo",pInfo->GetName());
		eJoueur->LinkEndChild(ePseudo);
		
		ticpp::Element * eKills = new ticpp::Element("kills",stats->kills);
		eJoueur->LinkEndChild(eKills);

		ticpp::Element * eDeaths = new ticpp::Element("deaths",stats->deaths);
		eJoueur->LinkEndChild(eDeaths);

		eJoueurs->LinkEndChild(eJoueur);
	}
}

void XmlReport::writeSpectateurs(ticpp::Element * eMatch)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	list<ClanMember *> * playerlist = plugin->getPlayerlist();	

	ticpp::Element * eSpectateurs = new ticpp::Element("spectateurs");

	int nbSpec = 0;
	list<ClanMember *>::const_iterator itPlayer;
	for(itPlayer = playerlist->begin(); itPlayer != playerlist->end(); itPlayer++)
	{
		if ((*itPlayer)->getMyTeam() == SPEC_TEAM)
		{
			writeJoueur(eSpectateurs,*itPlayer);
			nbSpec++;
		}
	}

	if (nbSpec > 0)
		eMatch->LinkEndChild(eSpectateurs);
	else
		delete eSpectateurs;
}

void XmlReport::writeSourcetv(ticpp::Element * eMatch)
{
	ServerPlugin * plugin = ServerPlugin::getInstance();
	MatchManager * match = plugin->getMatch();
	list<TvRecord *> * recordlist = match->getRecords();

	if (! recordlist->empty())
	{
		ticpp::Element * eSourcetv = new ticpp::Element("sourcetv");

		int recordId = 1;
		list<TvRecord *>::const_iterator itRecord;
		for(itRecord = recordlist->begin(); itRecord != recordlist->end(); itRecord++)
		{
			ticpp::Element * eManche = new ticpp::Element("manche",*(*itRecord)->getName());
			eManche->SetAttribute("numero",recordId);

			eSourcetv->LinkEndChild(eManche);

			recordId++;
		}

		eMatch->LinkEndChild(eSourcetv);
	}
}

/*void XmlReport::writeFooter()
{
}*/

XmlReport::XmlReport(MatchManager * matchManager) : BaseReport(matchManager)
{
}

void XmlReport::write()
{
	try
	{
		writeHeader();
		writeCorpse();
		//writeFooter();

		document.SaveFile(reportPath + ".xml");
	}
	catch(const ticpp::Exception & e)
	{
		CSSMATCH_PRINT_EXCEPTION(e);
	}
}
