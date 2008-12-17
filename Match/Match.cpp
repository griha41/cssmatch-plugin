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

#include "Match.h"

#include "../CSSMatch/CSSMatch.h"

#include "../GestionMenus/GestionMenus.h"

//#include "../Rapport/RapportTxt/RapportTxt.h"
#include "../Rapport/RapportXml/RapportXml.h"

using std::string;
using std::map;
using std::ostringstream;
using std::vector;

/** Finalise la fin d'un match (fin du temps mort)
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerFinMatch(const string & parametre, const map<string,string> & parametres)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_FIN)
		match->finTempsMortFinMatch();
}

/** Lance les strats
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerLanceStrats(const string & parametre, const map<string,string> & parametres)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_PASSAGE_STRATS)
		match->lanceStratsTime();
}

/** Lance la manche suivante
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerLanceManche(const string & parametre, const map<string,string> & parametres)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_TRANSITION_MANCHE)
		match->lanceManche();
}

/** Met fin aux strats
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerFinStrats(const string & parametre, const map<string,string> & parametres)
{
	Match * match = CSSMatch::getInstance()->getMatch();
	if (match->getPhase() == PHASE_STRATS)
		match->finStratsTime();
}

/** Lance le décompte des strats
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerChronoStrats(const string & parametre, const map<string,string> & parametres)
{
	Decompte::getInstance()->lanceDecompte(ConVars::cssmatch_warmup_time.GetInt()*60,timerFinStrats);
}

/** Surveille la valeur d'sv_alltalk durant le match
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerVerifAllTalk(const string & parametre, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (match->getPhase() != PHASE_OFF)
	{
		if (ConVars::sv_alltalk->GetBool())
			Messages::centerMsg("sv_alltalk");

		GestionTimers * timers = cssmatch->getTimers();
		timers->addTimer(Timer(1.5f,timerVerifAllTalk,""));
	}
}

/** Surveille la valeur d'sv_cheats durant le match
 *
 * @param parametre Inutile ici
 * @see La classe Timer
 *
 */
void timerVerifCheats(const string & parametre, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (match->getPhase() != PHASE_OFF)
	{
		if (ConVars::sv_cheats->GetBool())
			Messages::centerMsg("sv_cheats");

		GestionTimers * timers = cssmatch->getTimers();
		timers->addTimer(Timer(1.5f,timerVerifCheats,""));
	}
}


void timerRetag(const string & parametre, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	Match * match = cssmatch->getMatch();

	if (match->getPhase() != PHASE_OFF)
	{
		match->detecteTags(true);
	}
}

Match::Match()
{
	phase = PHASE_OFF;

	//tvManche1 = NULL;
	//tvManche2 = NULL;

	mancheCourante = 1;
	roundCourant = 0;

	tagGagnantCutRound = "";

	team1 = Team();
	team2 = Team();

	cutround = true;
	strats = true;
}

Match::~Match()
{
	/*if (tvManche1 != NULL)
		delete tvManche1;

	if (tvManche2 != NULL)
		delete tvManche2;	*/
}

CodePhase Match::getPhase() const
{
	return phase;
}

void Match::setPhase(CodePhase codePhase)
{
	phase = codePhase;
}

/*EnregistrementTV * Match::getTvManche1() const
{
	return tvManche1;
}

EnregistrementTV * Match::getTvManche2() const
{
	return tvManche2;
}*/
const vector<EnregistrementTV> * Match::getEnregistrementsTV() const
{
	return &enregistrements;
}

int Match::getMancheCourante() const
{
	return mancheCourante;
}

int Match::getRoundCourant() const
{
	return roundCourant;
}

void Match::setRoundCourant(int numeroRound)
{
	roundCourant = numeroRound;
}

int Match::incRoundCourant()
{
	return ++roundCourant;
}

const string & Match::getTagGagnantCutRound() const
{
	return tagGagnantCutRound;
}

void Match::setTagGagnantCutRound(const std::string & tag)
{
	tagGagnantCutRound = tag;
}

Team * Match::getTeam(CodeTeam codeTeam) throw(CSSMatchMatchException)
{
	Team * team = NULL;

	switch(codeTeam)
	{
	case TEAM_T:
		if (mancheCourante & 1)
			team = &team1;
		else
			team = &team2;
		break;
	case TEAM_CT:
		if (mancheCourante & 1)
			team = &team2;
		else
			team = &team1;
		break;
	default:
		throw CSSMatchMatchException("getTeam() on a spectator team !");
	}

	return team;
}

Team * Match::getTeam1()
{
	return &team1;
}

Team * Match::getTeam2()
{
	return &team2;
}

void Match::modifieTag(CodeTeam codeTeam, const std::string & nouvTag)
{
	Team * cible = getTeam(codeTeam);
	const string * tagCourant = &cible->getTag();

	// Si la team est celle qui a gagné le round au couteau, on met à jour le tag enregistré comme tel
	if (*tagCourant == tagGagnantCutRound)
		tagGagnantCutRound = nouvTag;

	// Mise à jour du tag
	cible->setTag(nouvTag);

	// Mise à jour du nom du serveur
	formateHostname();
}

bool Match::getCutRound() const
{
	return cutround;
}

void Match::setCutRound(bool cutround)
{
	this->cutround = cutround;
}

bool Match::getStrats() const
{
	return strats;
}

void Match::setStrats(bool strats)
{
	this->strats = strats;
}

const string & Match::getDate() const
{
	return horaire;
}

void Match::reset()
{
	phase = PHASE_OFF;

	enregistrements.clear();

	mancheCourante = 1;
	roundCourant = 0;

	tagGagnantCutRound = "";
	team1 = Team();
	team2 = Team();
}

void Match::lanceMatch(Configuration * fichier, edict_t * pEntity)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	GestionTimers * timers = cssmatch->getTimers();
	Decompte * decompte = Decompte::getInstance();

	// On stoppe tout chrono ou timer en cours
	timers->purge(); // A purger en premier pour permettre l'exécution de la fonction associée à la fin du décompte !
	decompte->finDecompte();

	try
	{
		// Exécution de la configuration
		fichier->execute();

		// On commence avec un match tout neuf
		reset();

		// Récupération de la date du début de match
		tm * dateDebut = Outils::getDateLocale();
		char datebuf[96];
		strftime(datebuf,sizeof(datebuf),"%Hh%M",dateDebut);
		horaire = datebuf;

		// Recherche du tag de chaque team et modification du nom du serveur en conséquence
		detecteTags(false);

		// Listage des plugins présents sur le serveur
		//Api::engine->LogPrint("CSSMatch : liste des plugins installés et leur statut :\n");
		Api::serveurExecute("plugin_print\n");

		// Mise à jour du mot de passe
		if (ConVars::sv_password != NULL)
			ConVars::sv_password->SetValue(ConVars::cssmatch_password.GetString());
		else
			Api::debug("Unable to find sv_password !");

		map<string,string> parametres;
		parametres["$password"] = ConVars::cssmatch_password.GetString();
		cssmatch->getTimers()->addTimer(Timer(5.0f,Messages::timerSayPopup,"match_password_popup",parametres));

		// On signale le lancement d'un match
		try
		{
			map<string,string> parametres;
			parametres["$admin"] = Api::getPlayerName(pEntity);
			Messages::sayMsg(Api::getIndexFromPEntity(pEntity),"match_started_by",parametres);
		}
		catch(const CSSMatchApiException & e)
		{
			Messages::sayMsg(0,"match_started");
			// Api::reporteException(e,__FILE__,__LINE__); // Match lancé par RCON
		}

		// On lance la vérification du alltalk est d'sv_cheats
		// On passe par des timers car la phase est encore PHASE_OFF ici
		if (ConVars::sv_alltalk != NULL)
			timers->addTimer(Timer(1.0f,timerVerifAllTalk,""));
		else
			Api::debug("Unable to find sv_alltalk !");
		if (ConVars::sv_cheats != NULL)
			timers->addTimer(Timer(1.0f,timerVerifCheats,""));
		else
			Api::debug("Unable to find sv_cheats !");

		// On lance la première phase du match
		//	Si le match a été lancé via le menu, le choix du round au couteau et des strats ont été déterminés via le menu
		if (Api::isValidePEntity(pEntity))
		{
			// On écrase la configuration concernée si nécessaire
			ConVars::cssmatch_kniferound.SetValue((int)cutround);

			if (strats)
			{
				// Si aucun temps de strats n'a été défini on rétablit la valeur par défaut
				if (ConVars::cssmatch_warmup_time.GetInt() == 0)
					ConVars::cssmatch_warmup_time.SetValue(ConVars::cssmatch_warmup_time.GetDefault());
				// Sinon on laisse le temps déterminé par la configuration
			}
			else
				ConVars::cssmatch_warmup_time.SetValue(0);
		}
		// Si la match n'a pas été lancé via le menu on se fit à la valeur de cssmatch_kniferound pour savoir si on doit lancer un CutRound
		else
			cutround = ConVars::cssmatch_kniferound.GetBool();

		

		if (ConVars::cssmatch_kniferound.GetBool())
			lanceCutRound();
		else if (ConVars::cssmatch_warmup_time.GetInt() > 0)
		{
			Api::serveurExecute("mp_restartgame 1\n");
			lanceStratsTime();
		}
		else if (ConVars::cssmatch_sets.GetInt() > 0)
				lanceManche();
		else
		{
			Messages::sayColor("match_config_error"); 
			finMatch();
		}
	}
	catch(const CSSMatchConfigurationException & e)
	{
		Messages::sayMsg(0,e.what());
	}
}

void Match::finMatch(edict_t * pEntity)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	GestionTimers * timers = cssmatch->getTimers();
	Decompte * decompte = Decompte::getInstance();

	// On stopppe tout enregistrement en cours
	vector<EnregistrementTV>::iterator enregistrement = enregistrements.begin();
	vector<EnregistrementTV>::iterator finListe = enregistrements.end();
	while(enregistrement != finListe)
	{
		if (enregistrement->getEnregistrant())
			enregistrement->coupe();
		enregistrement++;
	}
	
	Messages::sayMsg(0,"match_end");

	const string * tagTeam1 = &team1.getTag();
	const string * tagTeam2 = &team2.getTag();
	int scoreTeam1 = team1.getScore();
	int scoreTeam2 = team2.getScore();

	// Envoi d'une annonce
	map<string,string> parametresAnnonce;
	parametresAnnonce["$team1"] = *tagTeam1;
	parametresAnnonce["$score1"] = Outils::toString(scoreTeam1);
	parametresAnnonce["$team2"] = *tagTeam2;
	parametresAnnonce["$score2"] = Outils::toString(scoreTeam2);
	Messages::sayPopup("match_end_popup",6,parametresAnnonce);

	// Envoi de l'annonce dans la console de tous les joueurs
	Messages::clientPrintMsg("match_end_popup");

	map<string,string> parametres;
	if (scoreTeam1 > scoreTeam2)
	{
		parametres["$team"] = *tagTeam1;
		Messages::sayMsg(0,"match_winner",parametres);
	}
	else if (scoreTeam1 < scoreTeam2)
	{
		parametres["$team"] = *tagTeam2;
		Messages::sayMsg(0,"match_winner",parametres);
	}
	else
		Messages::sayMsg(0,"match_no_winner");

	// Mise à jour des scores en fonction des camps
	team1.setScoreCT(team1.getScore()-team1.getScoreT());
	team2.setScoreT(team2.getScore()-team2.getScoreCT());

	if (ConVars::cssmatch_report.GetBool())
	{
		//RapportTxt(this).ecrit(); // ?
		RapportXml(this).ecrit();
	}

	phase = PHASE_FIN;

	// Annonce du temps mort pour la prise de screenshots
	int tempsMort = ConVars::cssmatch_end_set.GetInt();
	map<string,string> parametresTempsMort;
	parametresTempsMort["$time"] = Outils::toString(tempsMort);
	timers->addTimer(Timer(2.0f,Messages::timerSayMsg,"match_dead_time",parametresTempsMort));

	decompte->lanceDecompte(tempsMort + 2,timerFinMatch);
}

void Match::finTempsMortFinMatch()
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IFileSystem * filesystem = cssmatch->getFileSystem();

	// On exécute la configuration par défaut
	string configParDefaut(ConVars::cssmatch_default_config.GetString());
	if (filesystem->FileExists(("cfg/" + configParDefaut).c_str(),"MOD"))
		Api::configExecute(configParDefaut);
	else
	{
		map<string,string> parametres;
		parametres["$file"] = configParDefaut;
		Messages::sayColor("error_file_not_found",parametres);
		Messages::sayColor("match_please_changelevel");
	}

	// On finalise la fin du match (un nouveau match peut être lancé)
	phase = PHASE_OFF;
}

void Match::lanceCutRound()
{
	roundCourant = 0;

	Messages::sayMsg(0,"kniferound_restarts");
	phase = PHASE_PASSAGE_CUTROUND;

	// C'est le round_start qui déclenchera les 2 autres restarts afin d'éviter les conflits avec les restarts du jeu ou lancés par un admin
	Api::serveurExecute("mp_restartgame 2\n");
}

void Match::finCutRound(CodeTeam codeTeamGagnante)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	GestionTimers * timers = cssmatch->getTimers();

	// Nous n'acceptons la fin du CutRound que si une des 2 teams a gagnée
	if (codeTeamGagnante == TEAM_T || codeTeamGagnante == TEAM_CT)
	{
		phase = PHASE_TRANSITION_MANCHE;
			// => Pour éviter que lors de la mise en spectateur les tags soient redétectés

		// On met le camp perdante en spectateur, et on affiche le menu de choix de team à le camp gagnante
		if (codeTeamGagnante == TEAM_T)
		{
			timers->addTimer(Timer(1.0f,timerCexecT,"chooseteam\n"));
			Api::specGroupe(TEAM_CT);

			// Mémorisation du tag de la team gagnante
			tagGagnantCutRound = team1.getTag();
		}
		else
		{
			timers->addTimer(Timer(1.0f,timerCexecCT,"chooseteam\n"));
			Api::specGroupe(TEAM_T);

			tagGagnantCutRound = team2.getTag();
		}

		// Affichage du tag de la team gagnante
		map<string,string> parametres;
		parametres["$team"] = tagGagnantCutRound;
		Messages::sayMsg(0,"kniferound_winner",parametres);

		// Mise en place d'un temps mort pour laisser le temps aux camps de choisir leur nouvelle team
		// et de la redétection des tags
		int tempsAttente = ConVars::cssmatch_end_kniferound.GetInt();
		if (tempsAttente > 0)
		{
			map<string,string> parametres;
			parametres["$team"] = tagGagnantCutRound;
			parametres["$time"] = Outils::toString(tempsAttente);
			Messages::sayMsg(0,"kniferound_dead_time",parametres);

			Decompte * decompte = Decompte::getInstance();

			// Préparation de la prochaine phase de match (s'il y en a une)
			if (ConVars::cssmatch_warmup_time.GetInt()>0 && strats)
			{
				phase = PHASE_PASSAGE_STRATS;
				decompte->lanceDecompte(tempsAttente,timerLanceStrats);
			}
			else
			{
				if (ConVars::cssmatch_sets.GetInt() > 0)
				{
					phase = PHASE_TRANSITION_MANCHE;
					decompte->lanceDecompte(tempsAttente,timerLanceManche);
				}
				else
					finMatch();
			}
		}
		else // S'il n'y en a pas on lance directement la suite (s'il y en a une)
		{
			if (ConVars::cssmatch_warmup_time.GetInt()>0 && strats)
				lanceStratsTime();
			//else
			else if (ConVars::cssmatch_sets.GetInt() > 0)
				lanceManche();
			else
				finMatch();
		}
	}
}

void Match::lanceStratsTime()
{
	// La situation a peut-être changée suite au round au couteau, redétection des tags et mise à jour du nom du serveur
	/*if (mancheCourante == 1) // peu importe cssmatch_sets
		CSSMatch::getInstance()->getTimers()->addTimer(Timer(5.0,timerRetag,""));*/
	// // => devenu inutile depuis la détection des changements de situation ? // non, désactivé lors des transitions

	// On s'assure qu'un précédent !go n'a pas été mémorisé
	team1.setGoStrats(false);
	team2.setGoStrats(false);

	// Si cssmatch_warmup_time est à zéro on passe directement au match (s'il y a lieu)
	int tempsStrats = ConVars::cssmatch_warmup_time.GetInt();
	if (tempsStrats <= 0)
	{
		if (ConVars::cssmatch_sets.GetInt() > 0)
			lanceManche();
		else
			finMatch();
	}
	// sinon on lance le StratsTime
	else
	{
		CSSMatch * cssmatch = CSSMatch::getInstance();
		GestionTimers * timers = cssmatch->getTimers();

		Api::serveurExecute("mp_restartgame 2\n");
		phase = PHASE_STRATS;

		tempsStrats *= 60;
		timers->addTimer(Timer(3.0f,timerChronoStrats,""));
	}
}

void Match::finStratsTime()
{
	Messages::sayMsg(0,"warmup_end");

	// S'il y a des manches à jouer on les lance
	if (ConVars::cssmatch_sets.GetInt() > 0)
		lanceManche();
	// Sinon c'est la fin du match
	else
		finMatch();
}

void Match::lanceManche()
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	CGlobalVars * gpGlobals = cssmatch->getGlobalVars();

	map<string,string> parametres;
	parametres["$current"] = Outils::toString(mancheCourante);
	parametres["$total"] = ConVars::cssmatch_sets.GetString();
	Messages::sayMsg(0,"match_start_manche",parametres);

	// Le nombre de round a été utilisé précédemment
	roundCourant = 0;

	// Gestion de l'enregistrement sourceTV
	if (ConVars::cssmatch_sourcetv.GetBool())
	{
		// Préparation du nom de l'enregistrement
		tm * locale = Outils::getDateLocale();
		char datebuf[96];
		strftime(datebuf,sizeof(datebuf),"%Y-%m-%d_%Hh%M",locale);

		// Constitution du nom de la TV
		ostringstream nomTV;
		nomTV << datebuf << "_" << gpGlobals->mapname.ToCStr() << "_" << "manche_" << mancheCourante;
		// Lancement de l'enregistrement (si possible)
		try
		{
			EnregistrementTV enregistrement = EnregistrementTV(nomTV.str());
			enregistrement.enregistre();
			enregistrements.push_back(enregistrement);
		}
		catch(const CSSMatchTVException & e)
		{
			Messages::sayColor(e.what());
		}
	}

	Messages::sayMsg(0,"match_restarts");

	phase = PHASE_PASSAGE_MANCHE;

	// Lancement du match
	Api::serveurExecute("mp_restartgame 2\n");
}

void Match::finManche()
{
	// Listage des plugins présents sur le serveur
	Api::serveurExecute("plugin_print\n");

	// Cas où il reste des manches à jouer
	if (mancheCourante < ConVars::cssmatch_sets.GetInt())
	{
		CSSMatch * cssmatch = CSSMatch::getInstance();
		GestionTimers * timers = cssmatch->getTimers();
		GestionJoueurs * joueurs = GestionJoueurs::getInstance();
		Decompte * decompte = Decompte::getInstance();

		// On stoppe l'éventuel enregistrement en cours
		int nbrEnregistrements = enregistrements.size();
		if (nbrEnregistrements>0)
		{
			EnregistrementTV enregistrement = enregistrements[nbrEnregistrements-1];
			if (enregistrement.getEnregistrant())
				enregistrement.coupe();
		}

		map<string,string> parametres;
		parametres["$current"] = Outils::toString(mancheCourante);
		Messages::sayMsg(0,"match_end_current_manche",parametres);

		const string * tagTeam1 = &team1.getTag();
		const string * tagTeam2 = &team2.getTag();
		int scoreTeam1 = team1.getScore();
		int scoreTeam2 = team2.getScore();

		// Envoie d'une annonce
		//map<string,string> parametres;
		//parametres["$current"] = current;
		parametres["$team1"] = *tagTeam1;
		parametres["$score1"] = Outils::toString(scoreTeam1);
		parametres["$team2"] = *tagTeam2;
		parametres["$score2"] = Outils::toString(scoreTeam2);
		Messages::sayPopup("match_end_manche_popup",6,parametres);

		// Envoie de l'annonce dans la console de tous les joueurs
		Messages::clientPrintMsg("match_end_manche_popup");


		// On lance la prochaine phase en léger décalage avec le swap
		float tempsTransition = ConVars::cssmatch_end_set.GetFloat();
		//		Ici il est devenue inutile de contrôler la réponse aux questions en début de match, la config a été écrasée
		if (ConVars::cssmatch_warmup_time.GetInt() > 0)
		{
			phase = PHASE_PASSAGE_STRATS;
			timers->addTimer(Timer(tempsTransition + 2.0f,timerLanceStrats,""));
		}
		else
		{
			phase = PHASE_TRANSITION_MANCHE;
			timers->addTimer(Timer(tempsTransition + 2.0f,timerLanceManche,""));
		}

		// On prépare la manche suivante
		//mancheCourante = 2;
		mancheCourante++;
		

		// Le swap des joueurs est légèrement décalé, ce qui laisse le temps de prendre des screenshots
		decompte->lanceDecompte((int)tempsTransition,timerSwapALL);
		map<string,string> parametresTempsMort;
		parametresTempsMort["$time"] = Outils::toString((int)tempsTransition);
		timers->addTimer(Timer(2.0f,Messages::timerSayMsg,"match_dead_time",parametresTempsMort));

		// On rappelle le mot de passe
		map<string,string> parametresPass;
		parametresPass["$password"] = ConVars::cssmatch_password.GetString();
		timers->addTimer(Timer(tempsTransition + 5.0f,Messages::timerSayPopup,"match_password_popup",parametresPass));

		// On créer le point de restauration des scores des joueurs sur la manche qui vient d'être jouée
		joueurs->sauvStatsManche();

		// On créer le point de restauration des scores des teams sur la manche qui vient d'être jouée
		team1.setScoreT(team1.getScore());
		team2.setScoreCT(team2.getScore());
	}
	else
	{	
		// Cas où le match est fini
		finMatch();
	}
}

void Match::detecteTags(bool affiche)
{
	// Variable qui contiendra un pointeur sur la team dont le tag correspond à la team gagnante du round au couteau
	// (à mettre à jour)
	Team * teamCutRound = NULL;

	// Recherche de la team gagnante du round au couteau
	if (team1.getTag() == tagGagnantCutRound)
		teamCutRound = &team1;
	else if (team2.getTag() == tagGagnantCutRound)
		teamCutRound = &team2;

	// On détermine quel est le camp courant de chacune des teams
	CodeTeam codeTeam1 = TEAM_T;
	CodeTeam codeTeam2 = TEAM_CT;
	if (mancheCourante % 2 == 0)
	{
		codeTeam1 = TEAM_CT;
		codeTeam2 = TEAM_T;
	}

	// Redétection des tags
	if (affiche)
	{
		map<string,string> parametres;
		parametres["$team1"] = team1.chercheTag(codeTeam1);
		parametres["$team2"] = team2.chercheTag(codeTeam2);
		Messages::sayMsg(0,"match_name",parametres);
	}
	else
	{
			team1.chercheTag(codeTeam1);
			team2.chercheTag(codeTeam2);
	}

	// Si l'une des teams avait gagné le round au couteau, on met à jour le tag enregistré
	if (teamCutRound != NULL)
		tagGagnantCutRound = teamCutRound->getTag();

	formateHostname();
}

void Match::formateHostname()
{
	if (ConVars::hostname != NULL)
	{
		string nomServeur(ConVars::cssmatch_hostname.GetString());

		size_t posTag = nomServeur.find("%s");
		if (posTag != string::npos)
		{
			const string * tagTeam1 = &team1.getTag();
			nomServeur.replace(posTag,2,*tagTeam1,0,tagTeam1->size());
		}

		posTag = nomServeur.find("%s");
		if (posTag != string::npos)
		{
			const string * tagTeam2 = &team2.getTag();
			nomServeur.replace(posTag,2,*tagTeam2,0,tagTeam2->size());
		}

		ConVars::hostname->SetValue(nomServeur.c_str());
	}
	else
		Api::debug("Unable to find hostname !");
}

void Match::afficheScores()
{
	map<string,string> parametres1;
	parametres1["$team"] = team1.getTag();
	parametres1["$score"] = Outils::toString(team1.getScore());
	map<string,string> parametres2;
	parametres2["$team"] = team2.getTag();
	parametres2["$score"] = Outils::toString(team2.getScore());

	Messages::sayMsg(0,"match_scores");
	Messages::sayMsg(0,"match_scores_team",parametres1);
	Messages::sayMsg(0,"match_scores_team",parametres2);
}

void Match::restartRound(edict_t * pEntity)
{
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();

	// Restauration des scores du round précédent
	joueurs->restaureStatsRound();

	// Si nous en étions encore au premier round...
	if (roundCourant == 1)
	{
		// ... Nous faisons comme si nous commençions les manches
		//			pour obtenir 2 restarts de plus (voir round_start)
		phase = PHASE_PASSAGE_MANCHE;
		roundCourant = 0;
	}

	// Restart du round
	Api::serveurExecute("mp_restartgame 1\n");
	/*CSSMatch * cssmatch = CSSMatch::getInstance();
	cssmatch->propScoreT.setEntityProp(68);
	cssmatch->propScoreCT.setEntityProp(69);*/

	try
	{
		map<string,string> parametres;
		parametres["$admin"] = Api::getPlayerName(pEntity);
		Messages::sayMsg(Api::getIndexFromPEntity(pEntity),"admin_round_restarted_by",parametres);
	}
	catch(const CSSMatchApiException & e)
	{
		Messages::sayMsg(0,"admin_round_restarted");
		// Api::reporteException(e,__FILE__,__LINE__); // restart fait pas RCON
	}
}

void Match::restartManche(edict_t * pEntity)
{
	GestionJoueurs * joueurs = GestionJoueurs::getInstance();

	// Restauration des scores de la manche précédente
	joueurs->restaureStatsManche();

	// Restauration des scores des teams selon la manche en cours
	if (mancheCourante == 1) // peu importe cssmatch_sets
	{
		// Aucune match n'a été jouée avant
		team1.setScore(0);
		team2.setScore(0);
	}
	else
	{
		// Restauration des scores de la manche précédente
		team1.setScore(team1.getScoreT());
		team2.setScore(team2.getScoreCT());
	}

	// Nous faisons croire au plugin que nous commençons les manches
	//		pour obtenir 2 restarts de plus (voir round_start)
	phase = PHASE_PASSAGE_MANCHE;

	// Restart du round
	Api::serveurExecute("mp_restartgame 1\n");

	// Nous reprennons tout depuis le début
	roundCourant = 0;

	try
	{
		map<string,string> parametres;
		parametres["$admin"] = Api::getPlayerName(pEntity);
		Messages::sayMsg(Api::getIndexFromPEntity(pEntity),"admin_manche_restarted_by",parametres);
	}
	catch(const CSSMatchApiException & e)
	{
		Messages::sayMsg(0,"admin_manche_restarted");
		// Api::reporteException(e,__FILE__,__LINE__); // restart fait par RCON
	}
}

void Match::forceGo(edict_t * pEntity)
{
	Decompte * decompte = Decompte::getInstance();

	// Si nous n'en somme qu'au passage au strats on refuse le !go
	if (phase != PHASE_STRATS)
	{
		if (pEntity != NULL)
			Messages::sayTell(0,Api::getIndexFromPEntity(pEntity),"warmup_disable");
		else
			Messages::rconPrintTell("warmup_disable");
		return;
	}

	// On actualise proprement le !go des teams
	team1.setGoStrats(true);
	team2.setGoStrats(true);

	// Le !go de l'arbitre déclenchera la suite du match
	try
	{
		map<string,string> parametres;
		parametres["$admin"] = Api::getPlayerName(pEntity);
		Messages::sayMsg(Api::getIndexFromPEntity(pEntity),"admin_all_teams_say_ready_by",parametres);
	}
	catch(const CSSMatchApiException & e)
	{
		Messages::sayMsg(0,"admin_all_teams_say_ready");
		// Api::reporteException(e,__FILE__,__LINE__); // déclaration par RCON
	}

	// Arrêt du décompte et lancement de la suite du match
	decompte->finDecompte();
	//finStratsTime();
}

void Match::goTeam(int indexJoueur, Team * team)
{
	map<string,string> parametres;
	if (team->getGoStrats())
	{
		parametres["$team"] = team->getTag();
		Messages::sayMsg(indexJoueur,"warmup_already_ready",parametres);
	}
	else
	{
		team->setGoStrats(true);
		parametres["$team"] = team->getTag();
		Messages::sayMsg(indexJoueur,"warmup_ready",parametres);
	}
}
