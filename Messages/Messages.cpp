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

#include "Messages.h"
#include "../I18n/I18n.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;
using std::map;
using std::ostringstream;

void Messages::sayMsg(int coloration, const string & message, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	for (int i=1;i<=maxplayers;i++)
		sayTell(coloration,i,message,parametres);
}

void Messages::sayColor(const string & message, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();
	IVEngineServer * engine = cssmatch->getEngine();

	
	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(Api::getPEntityFromIndex(i));

			if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
			{
				string traduction;
				try
				{
					//string nomLocale = I18n::getNomLocale(engine->GetClientConVarValue(i,"cl_language"));
					string nomLocale = engine->GetClientConVarValue(i,"cl_language");
					traduction = I18n::getInstance()->getMessage(nomLocale,message,parametres);
				}
				catch(const CSSMatchI18nException & e)
				{
					Api::reporteException(e,__FILE__,__LINE__);
				}

				string format("\004[" CSSMATCH_NOM "] :\001 " + traduction + "\n");
				const char * msg = format.c_str();

				MRecipientFilter filter;
				filter.addRecipient(i);
				bf_write * pBitBuf = engine->UserMessageBegin(&filter,MESSAGE_SAYTEXT);

				pBitBuf->WriteByte(0x02);
				pBitBuf->WriteString(msg);
				pBitBuf->WriteByte(0x01);
				pBitBuf->WriteByte(1);
				engine->MessageEnd();

			}
		}
		catch(const CSSMatchApiException & e){}
	}

	try
	{
		engine->LogPrint(I18n::getInstance()->getMessage(ConVars::cssmatch_language.GetString(),message,parametres).c_str());
	}
	catch(const CSSMatchI18nException & e)
	{
		Api::reporteException(e,__FILE__,__LINE__);
	}
}

void Messages::sayTell(int coloration, int indexJoueur, const string & message, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	try
	{
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(Api::getPEntityFromIndex(indexJoueur));

		if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
		{
			string traduction;
			try
			{
				//string nomLocale = I18n::getNomLocale(engine->GetClientConVarValue(indexJoueur,"cl_language"));
				string nomLocale = engine->GetClientConVarValue(indexJoueur,"cl_language");
				traduction = I18n::getInstance()->getMessage(nomLocale,message,parametres);
			}
			catch(const CSSMatchI18nException & e)
			{
				Api::reporteException(e,__FILE__,__LINE__);
			}

			string format("\004[" CSSMATCH_NOM "] :\001 " + traduction + "\n");
			const char * msg = format.c_str();

			MRecipientFilter filter;
			filter.addRecipient(indexJoueur);
			bf_write * pBitBuf = engine->UserMessageBegin(&filter,MESSAGE_SAYTEXT);

			//pBitBuf->WriteByte(NULL);
			// Ici l'index d'un joueur permet de colorer une partie du message précédée par un \003
			//		de la couleur de la team du joueur
			// CT <=> bleu
			// T <=> rouge
			// SPEC/UN <=> blanc
			pBitBuf->WriteByte(coloration);
			pBitBuf->WriteString(msg);
			//pBitBuf->WriteByte(NULL);
			pBitBuf->WriteByte(1);
			engine->MessageEnd();
		}
	}
	catch(const CSSMatchApiException & e){}
}

void Messages::showMenu(MRecipientFilter & filter, const std::string & texte, int vie, int flagsSensibilite)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	int indexDebutMessage = 0;
	int tailleMenu = texte.size();
	bool finMenu = false;

	do
	{
		string aEnvoyer = texte.substr(indexDebutMessage,MENU_TAILLE_LIMITE);
		indexDebutMessage += MENU_TAILLE_LIMITE;

		bf_write * pBuffer = engine->UserMessageBegin(&filter,MESSAGE_SHOWMENU);

		// Options sélectionnables
		pBuffer->WriteShort(flagsSensibilite);

		// Durée d'affichage illimité
		pBuffer->WriteChar(vie);

		// Est-ce que le menu est terminé ?
		//	=> max 255 caractères par message, un menu peut être construit sur plusieurs messages
		pBuffer->WriteByte((finMenu = indexDebutMessage < tailleMenu));
		
		// On écrit le corps du menu dans le message
		pBuffer->WriteString(aEnvoyer.c_str());

		// Envoi du message
		engine->MessageEnd();
	}
	while(finMenu);
}

void Messages::sayPopup(const string & message, int vie, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	for (int i=1;i<=maxplayers;i++)
		tellPopup(i,message,vie,parametres);
}

void Messages::tellPopup(int indexJoueur, const string & message, int vie, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	try
	{
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(Api::getPEntityFromIndex(indexJoueur));

		if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
		{
			string traduction;
			try
			{
				//string nomLocale = I18n::getNomLocale(engine->GetClientConVarValue(indexJoueur,"cl_language"));
				string nomLocale = engine->GetClientConVarValue(indexJoueur,"cl_language");
				traduction = I18n::getInstance()->getMessage(nomLocale,message,parametres);
			}
			catch(const CSSMatchI18nException & e)
			{
				Api::reporteException(e,__FILE__,__LINE__);
			}

			
			GestionJoueurs * joueurs = GestionJoueurs::getInstance();
			Joueur * joueur = joueurs->getJoueur(indexJoueur);
			Menu * menuJoueur = joueur->getMenu();

			// Nous devons éviter que le joueur choisisse par accident une option dans un menu du plugin caché par une fenêtre AMX
			menuJoueur->setCodeMenu(CODE_MENU_INVALIDE);
			
			MRecipientFilter filter;
			filter.addRecipient(indexJoueur);
			showMenu(filter,traduction,vie);
		}
	}
	catch(const CSSMatchApiException & e){}
}

void Messages::hintMsg(const string & message)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	for (int i=1;i<=maxplayers;i++)
		hintTell(i,message);
}

void Messages::hintTell(int indexJoueur, const string & message)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	try
	{
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(Api::getPEntityFromIndex(indexJoueur));

		if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
		{
			const char * msg = message.c_str();
			MRecipientFilter filter;
			filter.addRecipient(indexJoueur);
			bf_write * pWrite = engine->UserMessageBegin(&filter,MESSAGE_HINTTEXT);
			
			pWrite->WriteByte(1);
			pWrite->WriteString(msg);
			pWrite->WriteByte(NULL);
			engine->MessageEnd();
		}
	}
	catch(const CSSMatchApiException & e){}
}

void Messages::centerMsg(const string & message, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	for (int i=1;i<=maxplayers;i++)
		centerTell(i,message,parametres);
}

void Messages::centerTell(const int indexJoueur, const string & message, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	try
	{
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(Api::getPEntityFromIndex(indexJoueur));

		if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
		{
			string traduction;
			try
			{
				//string nomLocale = I18n::getNomLocale(engine->GetClientConVarValue(indexJoueur,"cl_language"));
				string nomLocale = engine->GetClientConVarValue(indexJoueur,"cl_language");
				traduction = I18n::getInstance()->getMessage(nomLocale,message,parametres);
			}
			catch(const CSSMatchI18nException & e)
			{
				Api::reporteException(e,__FILE__,__LINE__);
			}

			const char * msg = traduction.c_str();
			MRecipientFilter filter; 
			filter.addRecipient(indexJoueur); 
			bf_write * pWrite = engine->UserMessageBegin(&filter,MESSAGE_TEXTMSG);
			
			pWrite->WriteByte(4); 
			pWrite->WriteString(msg); 
			pWrite->WriteByte(NULL); 
			engine->MessageEnd();
		}
	}
	catch(const CSSMatchApiException & e){}
}

void Messages::clientPrintMsg(const string & message)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();

	for (int i=1;i<=maxplayers;i++)
	{
		try
		{
			edict_t * pEntity = Api::getPEntityFromIndex(i);
			clientPrintTell(pEntity,message);
		}
		catch(const CSSMatchApiException & e){}
	}
}

void Messages::clientPrintTell(edict_t * pEntity,const string & message)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();

	// On vérifie que le joueur n'est pas un bot
	//		Il parait que ClientPrintf peut planter avec les bots
	try
	{
		IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(pEntity);
		if (! pInfo->IsFakeClient())
			engine->ClientPrintf(pEntity,message.c_str());
	}
	catch(const CSSMatchApiException & e){}
}

void Messages::rconPrintTell(const string & message, const map<string, string> & parametres)
{
	string traduction;
	try
	{
		//string nomLocale = I18n::getNomLocale("");
		string nomLocale = "";
		traduction = I18n::getInstance()->getMessage(nomLocale,message,parametres);
		Msg((traduction+"\n").c_str());
	}
	catch(const CSSMatchI18nException & e)
	{
		Api::reporteException(e,__FILE__,__LINE__);
	}
}

void Messages::getRatesTab(edict_t * pEntity)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	int maxplayers = cssmatch->getMaxPlayers();

	if (Api::isValidePEntity(pEntity))
	{
		/*ostringstream entete;
		entete	<< "+ " << std::setfill('=') << std::setw(85) << "=" << std::setfill(' ') << " +\n"
				<< "| " << std::setw(30) << std::left << "Pseudo du joueur"
				<< " | " << std::setw(13) << std::left << "cl_updaterate"
				<< " | " << std::setw(10) << std::left << "cl_cmdrate"
				<< " | " << std::setw(14) << std::left << "cl_interpolate"
				<< " | " << std::setw(6) << std::left << "rate"	<< " |" << "\n"
				<< "+ " << std::setfill('=') << std::setw(85) << "=" << std::setfill(' ') << " +\n";
		clientPrintTell(pEntity,entete.str().c_str());*/

		for (int i=1;i<=maxplayers;i++)
		{
			try
			{
				edict_t * joueur = Api::getPEntityFromIndex(i);
				IPlayerInfo * pInfo = Api::getIPlayerInfoFromEntity(joueur);
				
				if (pInfo->IsConnected() && (! pInfo->IsFakeClient()))
				{
					/*ostringstream ligne;
					ligne	<< "| " << std::setw(30) << std::left << string(engine->GetClientConVarValue(i,"name")).substr(0,30)
							<< " | " << std::setw(13) << std::right << engine->GetClientConVarValue(i,"cl_updaterate")
							<< " | " << std::setw(10) << std::right << engine->GetClientConVarValue(i,"cl_cmdrate")
							<< " | " << std::setw(14) << std::right << engine->GetClientConVarValue(i,"cl_interpolate")
							<< " | " << std::setw(6) << std::right << engine->GetClientConVarValue(i,"rate") << " |" << "\n"
							<< "+ " << std::setfill('-') << std::setw(85) << "-" << std::setfill(' ') << " +\n";;
					clientPrintTell(pEntity,ligne.str().c_str());*/

					ostringstream ligne;
					ligne	/*<< std::endl*/
							<< string(engine->GetClientConVarValue(i,"name")) << " : " << std::endl
							<< "\t" << "cl_updaterate  : " << engine->GetClientConVarValue(i,"cl_updaterate") << std::endl
							<< "\t" << "cl_cmdrate     : " << engine->GetClientConVarValue(i,"cl_cmdrate") << std::endl
							<< "\t" << "cl_interpolate : " << engine->GetClientConVarValue(i,"cl_interpolate") << std::endl
							<< "\t" << "rate           : " << engine->GetClientConVarValue(i,"rate") << std::endl
							<< std::endl;
					clientPrintTell(pEntity,ligne.str().c_str());

				}
			}
			catch(const CSSMatchApiException & e){}

		}
	}
}

void Messages::timerSayMsg(const string & message, const map<string,string> & parametres)
{
	Messages::sayMsg(0,message,parametres);
}

void Messages::timerSayPopup(const string & message, const map<string,string> & parametres)
{
	Messages::sayPopup(message,5,parametres);
}
