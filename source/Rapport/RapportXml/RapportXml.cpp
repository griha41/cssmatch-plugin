/* 
 * Copyright 2007, 2008 Nicolas Maingot
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

#include "RapportXml.h"
#include "../../CSSMatch/CSSMatch.h"

using std::vector;
using std::string;
using std::ostringstream;
using std::ofstream;

RapportXml::RapportXml(Match * match) : Rapport(match)
{
}

string RapportXml::valideXml(const string & chaine) const
{
	string xml(chaine);

	size_t debutRecherche = 0;

	while((debutRecherche = xml.find("&",debutRecherche)) != string::npos)
	{
		xml.replace(debutRecherche,1,"&amp;",0,5);
		debutRecherche += 4;
	}

	debutRecherche = 0;
	while((debutRecherche = xml.find("<",debutRecherche)) != string::npos)
	{
		xml.replace(debutRecherche,1,"&lt;",0,4);
	}	

	return xml;
}

void RapportXml::ecritEntete(ofstream & fichier) const
{
	fichier << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
			<< "<?xml-stylesheet type=\"text/xsl\" href=\"cssmatch.xsl\"?>" << std::endl
			<< "<cssmatch>" << std::endl;
}

void RapportXml::ecritCorps(ofstream & fichier) const
{
	ecritPlugin(fichier);
	ecritMatch(fichier);
}

void RapportXml::ecritPied(ofstream & fichier) const
{
	fichier << "</cssmatch>" << std::endl;
}

void RapportXml::ecritPlugin(ofstream & fichier) const
{
	fichier << "	<plugin>" << std::endl
			<< "		<version>" << CSSMATCH_VERSION_LIGHT << "</version>" << std::endl
			<< "		<url>" << CSSMATCH_SITE << "</url>" << std::endl
			<< "	</plugin>" << std::endl;
}

void RapportXml::ecritMatch(ofstream & fichier) const
{
	CGlobalVars * gpGlobals = CSSMatch::getInstance()->getGlobalVars();

	// Formatage de la date de génération du fichier
	char datebuf[96];
	strftime(datebuf,sizeof(datebuf),"%Y/%m/%d",dateLocale);

	// Formatage de la date de début du match
	char dateFin[96];
	strftime(dateFin,sizeof(dateFin),"%Hh%M",dateLocale);

	// Récupération de l'adresse des teams
	Team * team1 = match->getTeam1();
	Team * team2 = match->getTeam2();

	// Récupération des tags des teams dénués de tout caractère indésirable
	string tagTeam1(valideXml(team1->getTag()));
	string tagTeam2(valideXml(team2->getTag()));

	// Récupération du tag de la team gagnante du round au couteau
	string tagcutround(valideXml(match->getTagGagnantCutRound()));


	// Ecriture du XML
	fichier << "	<match>" << std::endl
			<< "		<date>" << datebuf << "</date>" << std::endl
			<< "		<debut>" << match->getDate() << "</debut>" << std::endl
			<< "		<fin>" << dateFin << "</fin>" << std::endl
			<< "		<nom>" << tagTeam1 << " versus " << tagTeam2 << "</nom>" << std::endl
			<< "		<map>" << gpGlobals->mapname.ToCStr() << "</map>" << std::endl;
	if (tagcutround != "")
		fichier << "		<tagcutround>" << tagcutround << "</tagcutround>" << std::endl;
	ecritTeams(fichier,team1,team2);
	ecritSpectateurs(fichier);
	ecritSourcetv(fichier);

	fichier << "	</match>" << std::endl;
}

void RapportXml::ecritTeams(ofstream & fichier, Team * team1, Team * team2) const
{
	fichier << "		<teams>" << std::endl;

	// Teams concourantes
	if (match->getMancheCourante() & 1)
	{
		ecritTeam(fichier,team1,lignupT);
		ecritTeam(fichier,team2,lignupCT);
	}
	else
	{
		ecritTeam(fichier,team2,lignupT);
		ecritTeam(fichier,team1,lignupCT);
	}

	fichier << "		</teams>" << std::endl;
}

void RapportXml::ecritTeam(ofstream & fichier, Team * team, const vector<int> & lignup) const
{
	// Récupération du tag de la team dénué de tout caractère indésirable
	string tag(valideXml(team->getTag()));

	fichier << "			<team>" << std::endl
			<< "				<tag>" << tag << "</tag>" << std::endl
			<< "				<score>" << team->getScore() << "</score>" << std::endl
			<< "				<scoret>" << team->getScoreT() << "</scoret>" << std::endl
			<< "				<scorect>" << team->getScoreCT() << "</scorect>" << std::endl;
	ecritJoueurs(fichier,lignup);

	fichier << "			</team>" << std::endl;
}

void RapportXml::ecritJoueurs(ofstream & fichier, const vector<int> & lignup) const
{
	IVEngineServer * engine = CSSMatch::getInstance()->getEngine();

	fichier << "				<joueurs>" << std::endl;

	vector<int>::const_iterator indexJoueur = lignup.begin();
	vector<int>::const_iterator dernierIndex = lignup.end();
	while(indexJoueur != dernierIndex)
	{
		try
		{
			ecritJoueur("					",fichier,GestionJoueurs::getInstance()->getJoueur(*indexJoueur),Api::getIPlayerInfoFromEntity(engine->PEntityOfEntIndex(*indexJoueur)));
		}
		catch(const CSSMatchApiException & e){}

		indexJoueur++;
	}

	fichier << "				</joueurs>" << std::endl;
}

void RapportXml::ecritJoueur(const string & indentation, ofstream & fichier, Joueur * joueur, IPlayerInfo * playerInfo) const
{
	// Récupération du pseudo du joueur dénué de tout caractère indésirable
	string pseudo(valideXml(playerInfo->GetName()));

	fichier << indentation << "<joueur steamid=\"" << playerInfo->GetNetworkIDString() << "\">" << std::endl
			<< indentation << "	<pseudo>" << pseudo << "</pseudo>" << std::endl
			<< indentation << "	<kills>" << joueur->getKills() << "</kills>" << std::endl
			<< indentation << "	<deaths>" << joueur->getDeaths() << "</deaths>" << std::endl
			<< indentation << "</joueur>" << std::endl;
}

void RapportXml::ecritSpectateurs(ofstream & fichier) const
{
	IVEngineServer * engine = CSSMatch::getInstance()->getEngine();

	// S'il n'y a pas de spectateurs, on ne place pas la balise
	if (lignupSpec.size() > 0)
	{
		fichier << "		<spectateurs>" << std::endl;

		vector<int>::const_iterator indexJoueur = lignupSpec.begin();
		vector<int>::const_iterator dernierSpec = lignupSpec.end();
		while(indexJoueur != dernierSpec)
		{
			try
			{
				ecritJoueur("			",fichier,GestionJoueurs::getInstance()->getJoueur(*indexJoueur),Api::getIPlayerInfoFromEntity(engine->PEntityOfEntIndex(*indexJoueur)));
			}
			catch(const CSSMatchApiException & e){}

			indexJoueur++;
		}

		fichier << "		</spectateurs>" << std::endl;
	}
}

void RapportXml::ecritSourcetv(ofstream & fichier) const
{
	// Récupération de l'adresse des enregistrements SourceTV
	const vector<EnregistrementTV> * enregistrements = match->getEnregistrementsTV();

	// S'il n'y a aucun enregistrement, il n'y a pas de balise sourcetv
	if (! enregistrements->empty())
	{
		// Ecriture du XML
		fichier << "		<sourcetv>" << std::endl;

		for(unsigned int i=0;i<enregistrements->size();i++)
			fichier << "			<manche numero=\"" << i+1 << "\">" << (*enregistrements)[i].getNom() << "</manche>" << std::endl;

		fichier << "		</sourcetv>" << std::endl;
	}
}

void RapportXml::ecrit()
{
	ofstream fichier((nom + ".xml").c_str());
	if (fichier)
	{
		ecritEntete(fichier);
		ecritCorps(fichier);
		ecritPied(fichier);
	}
	else
		Api::debug("The file " + nom + ".xml can't be write");
}
