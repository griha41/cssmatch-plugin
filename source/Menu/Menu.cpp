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

#include "Menu.h"
#include "../Messages/Messages.h"
#include "../I18n/I18n.h"
#include "../CfgFile/CfgFile.h"
#include "../CSSMatch/CSSMatch.h"

using std::string;
using std::map;
using std::ostringstream;
using std::ifstream;

const Ligne Menu::retour = Ligne("menu_back",MENU_CODE_RETOUR);
const Ligne Menu::suivant = Ligne("menu_more",MENU_CODE_SUIVANT);
const int Menu::maxOptions = 9;

Menu::Menu()
{
	titre = "CSSMatch";
	codeMenu = CODE_MENU_INVALIDE;
	codeMenuParent = CODE_MENU_INVALIDE;
	type = NOLIST;
	fonction = NULL;
}

void Menu::setTitre(const string & titre)
{
	this->titre = titre;
}

const string & Menu::getTitre() const
{
	return titre;
}

void Menu::setCodeMenu(CodeMenu codeMenu)
{
	this->codeMenu = codeMenu;
}

CodeMenu Menu::getCodeMenu() const
{
	return codeMenu;
}

void Menu::setCodeMenuParent(CodeMenu codeMenuParent)
{
	this->codeMenuParent = codeMenuParent;
}

CodeMenu Menu::getCodeMenuParent() const
{
	return codeMenuParent;
}

void Menu::specialise(TypeSpeMenu type)
{
	this->type = type;
}

TypeSpeMenu Menu::getSpecialisation() const
{
	return type;
}

void Menu::setCallBack(void (* fonction)(int indexJoueur, int choix))
{
	this->fonction = fonction;
}

void Menu::callBack(int indexJoueur, int choix)
{
	if (fonction != NULL)
		fonction(indexJoueur,choix);
}

int Menu::getNbrLignes() const
{
	return lignes.size();
}

void Menu::addLigne(const string & ligne, int valeurAssociee)
{
	// Association d'une valeur à la ligne
	Ligne aAjouter(ligne,valeurAssociee);

	// Récupération du nombre de ligne contenues dans le menu
	int nbrLignes = lignes.size();

	// Si le menu n'a pas de menu parent le traitement est plus compliqué : pas de "retour" dès la première page
	if (codeMenuParent == CODE_MENU_INVALIDE)
	{
		// Si le menu ne fait toujours qu'une page ET si aucun menu parent n'existe
		if (nbrLignes < maxOptions)
		{
			// Aucun problème, on ajoute simplement la ligne
			lignes.push_back(aAjouter);
			return;
		}
		// Le sinon est tout à la fin de la fonction ;)

		// Si l'élément à insérer ne tient plus sur le première page
		if (nbrLignes == maxOptions) 
		{
			// Notre liste va compter 3 éléments en plus : "suivant", l'élément à insérer, et "retour"
			// maxOptions + 3 élément en plus

			// On sauvegarde l'élément qui va être décalé (remplacé par un "suivant")
			Ligne dernierElement = lignes[maxOptions-1]; // les éléments sont indéxés à partir de 0

			// On remplace le dernier élément par un "suivant"
			lignes[maxOptions-1] = suivant;

			// On place l'élément décalé en début de seconde page
			lignes.push_back(dernierElement);

			// On insérer la nouvelle ligne
			lignes.push_back(aAjouter);

			// On place un "retour" après le nouvel élément
			lignes.push_back(retour);

			return;
		}
	}
	else if (nbrLignes == 0)
	{
		// On ajoute la ligne
		lignes.push_back(aAjouter);
		// On place un "retour" après la ligne
		lignes.push_back(retour);

		return;
	}

	// Si la taille actuelle du menu est multiple de maxOptions, on doit ajouter une page
	if (nbrLignes % maxOptions == 0)
	{
		// Notre liste va compter 4 éléments en plus : "suivant", l'élément à insérer, "retour" et "suivant"
		// lignes.resize(nbrLigne+4);

		// On sauvegarde l'élément qui va être décalé (remplacé par le "retour")
		Ligne elementDecale = lignes[nbrLignes-2]; // les éléments sont indexés à partir de 0

		// On place un "retour" sur à la place de l'élément que l'on vient de sauvegarder
		lignes[nbrLignes-2] = retour;

		// L'ancienne place du "retour" doit maintenant contenir un "suivant"
		lignes[nbrLignes-1] = suivant;

		// La seconde page commance avec l'élément que l'on a sauvegardé
		lignes.push_back(elementDecale);

		// Insertion de la nouvelle ligne
		lignes.push_back(aAjouter);

		// Insertion d'un "retour" après cette nouvelle ligne
		lignes.push_back(retour);

		return;
	}

	// Sinon, on continue de remplir l'actuelle dernière page (qui n'est pas pleine)

	// On remplace le "retour" par la nouvelle ligne
	lignes[nbrLignes-1] = aAjouter;

	// On replace un "retour" après cette nouvelle ligne
	lignes.push_back(retour);
}

void Menu::setLigne(int indexLigne, const string & nouvTexte, int valeurAssociee)
{
	Ligne ligne = Ligne(nouvTexte,valeurAssociee);

	indexLigne--;

	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		lignes[indexLigne] = ligne; // Modification de la ligne
	// Aucune exception jettée car c'est un bug facilement détectable pendant la phase de test O_O
}

const Ligne & Menu::getLigne(int indexLigne) const
{
	indexLigne--;

	// Si l'index est valide on retourne la ligne
	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		return lignes[indexLigne];

	// Sinon on jette une exception
	throw CSSMatchMenuException("Line index in the menu \"" + titre + "\" is invalid");
}

const Ligne & Menu::getChoix(int choix, int pageMenu) const
{
	// Les vectors étant indéxés à partie de 0, on décrément de 1 le choix dans le menu
	choix--;
	// Calcul de l'index de la lige sélectionnée par le joueur
	int indexLigne = (pageMenu-1)*9+choix;

	// Si l'index de la ligne est valide, on peut retourner la ligne
	if (indexLigne>=0 && indexLigne<(int)lignes.size())
		return lignes[indexLigne];

	// Sinon on jette une exception
	throw CSSMatchMenuException("Invalid index in the menu \"" + titre + "\"");
}

void Menu::affiche(int indexJoueur, int pageMenu, const map<string,string> & parametres)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IVEngineServer * engine = cssmatch->getEngine();
	I18n * i18n = I18n::getInstance();

	// Récupération du nom de la langue du joueur
	string nomLocale = engine->GetClientConVarValue(indexJoueur,"cl_language");

	// Si l'index du joueur est invalide on ne jettera pas d'exception
	if (Api::isValideIndex(indexJoueur))
	{
		// Variable contenant le nombre de lignes du menu
		int nbrLignes = lignes.size();

		// Construction du menu
		MRecipientFilter filter;
		filter.addRecipient(indexJoueur);
		
		// Il faut décrémenter pageMenu car lignes est indéxé à partir de 0
		pageMenu--;

		// On calcul l'index de la première ligne de la page
		int depart = pageMenu*9;
		// Si on obtient une valeur abérante, on sort sans rien afficher
		if (pageMenu<0 || depart>nbrLignes) // depart==nbrLignes==0 traité plus bas
			throw CSSMatchMenuException("This page of the menu \"" + titre + "\" is invalid");

		// On calcul l'index de la dernière ligne de la page
		int arrivee = depart+9;
		
		// Cette variable contiendra les options senssibles à la sélection du joueur dans le menu
		int flagsSensibilite = OPTION_ANNULER;

		// Variable contenant le nombre d'options senssibles à la sélection
		int sensibilite = nbrLignes-depart;
		// Si toutes les options sont senssibles on évite une boucle
		if (sensibilite>=9)
			flagsSensibilite |= OPTION_ALL;
		// Sinon on ajoute dynamiquement les options sensibles
		else
		{
			// Les options senssibles commencent à (1<<0)
			sensibilite--;
			// Tant qu'il reste une option sensible à positionner...
			while(sensibilite>=0)
			{
				flagsSensibilite |= (1<<sensibilite);
				sensibilite--;
			}
		}

		// Corps du menu (titre compris)
		ostringstream menu;

		// On place le titre du menu en début de menu
		menu << i18n->getMessage(nomLocale,titre) << "\n";

		// Si le menu ne contient aucune ligne
		if (nbrLignes==0)
		{
			// On le dit dans le menu
			menu << " \n";
			menu << i18n->getMessage(nomLocale,"menu_empty") << "\n \n";
			menu << "0. " << i18n->getMessage(nomLocale,"menu_close");

			// Envoi un message dans le log du serveur
			Api::debug("The menu \"" + titre + "\" is empty");
		}
		else
		{
			// On ajoute dynamiquement les lignes du menu au corps du menu
			int indexMenu = 1;
			int i=depart;
			//Api::debug(titre + "\n");

			switch(type)
			{
				// Si le contenu est une liste de joueurs, de config ou de maps, pas de traduction à rechercher
				case PLAYERLIST:
				case PLAYERLIST_SANS_SPEC:
				case CFGLIST:
				case MAPLIST:
					while(i<nbrLignes && i<arrivee)
					{
						//Api::debug(lignes[i] + "\n");
						menu << "->" << indexMenu << ". ";

						//Affichage de la ligne telle quelle, sauf les options "Retour" et "Suivant"
						Ligne * ligne = &lignes[i];
						switch(ligne->getValeur())
						{
						case MENU_CODE_RETOUR:
						case MENU_CODE_SUIVANT:
							menu << i18n->getMessage(nomLocale,ligne->getTexte());
							break;
						default:
							menu << ligne->getTexte();
						}

						menu << "\n";
						indexMenu++;
						i++;
					}
					break;
				default:
					while(i<nbrLignes && i<arrivee)
					{
						//Api::debug(lignes[i] + "\n");
						menu	<< "->" << indexMenu << ". "
								<< i18n->getMessage(nomLocale,lignes[i].getTexte(),parametres/*ex : Alltalk */);

						menu << "\n";
						indexMenu++;
						i++;
					}
				}
			//Api::debug("0. Fermer ce menu");

			// Ajout de la dernière option (annuler)
			menu << "0. " << i18n->getMessage(nomLocale,"menu_close");

			Messages::showMenu(filter,menu.str(),-1,flagsSensibilite);
		}
	}
}

void Menu::purge()
{
	lignes.clear();
}

void Menu::playerlist(bool avecSpec)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	int maxplayers = cssmatch->getMaxPlayers();
	IVEngineServer * engine = cssmatch->getEngine();

	// On fera correspondre à chaque joueur son userid unique via la classe Ligne pour pouvoir le retrouver
	// de manière efficace lors du choix de l'arbitre dans le menu

	// On efface le contenu précédent du menu
	purge();

	// Si on doit prendre en compte les spectateurs et les non-assignés
	if (avecSpec)
	{
		// On ajoute tous les joueurs
		for (int i=1;i<=maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);
				const char * pseudo = player->GetName();
				if (pseudo != NULL)
				{
					string sPseudo(pseudo) ;
					/*if (sPseudo.size() > MENU_TAILLE_PSEUDO_LIMITE+1)
						sPseudo = sPseudo.substr(0,MENU_TAILLE_PSEUDO_LIMITE) + ".";*/
					addLigne(sPseudo,player->GetUserID());
				}
			}
			catch(const CSSMatchApiException & e){}
		}
	}
	else
	{
		// Sinon on ajoute que les joueurs non spectateurs et non-assignés
		for (int i=1;i<=maxplayers;i++)
		{
			edict_t * pEntity = engine->PEntityOfEntIndex(i);
			try
			{
				IPlayerInfo * player = Api::getIPlayerInfoFromEntity(pEntity);
				if (player->GetTeamIndex() >= TEAM_T)
				{
					const char * pseudo = player->GetName();
					if (pseudo != NULL)
					{
						string sPseudo(pseudo) ;
						/*if (sPseudo.size() > MENU_TAILLE_PSEUDO_LIMITE+1)
							sPseudo = sPseudo.substr(0,MENU_TAILLE_PSEUDO_LIMITE) + ".";*/
						addLigne(sPseudo,player->GetUserID());
					}
				}
			}
			catch(const CSSMatchApiException & e){}
		}
		// 2 boucles séparées pour éviter un test indépendant dans la boucle
	}
}

void Menu::cfglist(const string & repertoire)
{
	CSSMatch * cssmatch = CSSMatch::getInstance();
	IFileSystem * filesystem = cssmatch->getFileSystem();

	// On forme le motif de recherche et on le place en char* pour interagir avec l'interface de VALVE
	string dossierRecherche(repertoire + "/*.cfg");

	// Récupération du pointeur sur le premier nom de fichier du dossier
	FileFindHandle_t findHandleRenomme;
	const char * cfg = filesystem->FindFirstEx(dossierRecherche.c_str(),"MOD",&findHandleRenomme);
	// Tant que l'on trouve un fichier
	while(cfg != NULL)
	{
		string nomFinal(cfg);

		// Les espaces ne sont pas souhaitables pour une utilisation avec la ConCommand exec
		size_t posEspace;
		bool renomme = false;
		while((posEspace = nomFinal.find(" ")) != string::npos)
		{
			// On les remplace par un _
			nomFinal.replace(posEspace,1,"_",0,1);

			renomme = true;
		}
		if (renomme)
		{
			// On les renomme sur le FTP
			filesystem->RenameFile((repertoire + "/" + cfg).c_str(),(repertoire + "/" + nomFinal).c_str(),"MOD");
			Api::debug("The configuration file \"" + string(cfg) + "\" has been renamed in \"" + nomFinal + "\"");
		}

		addLigne(nomFinal);

		// Récupération du fichier suivant
		cfg = filesystem->FindNext(findHandleRenomme);
	}
}

void Menu::filelist(const std::string & pathFichier)
{
	ifstream fichier(pathFichier.c_str());

	// Si le fichier n'existe pas on ne fait rien, le joueur sera avertit à l'affichage de la liste
	if (! fichier.fail())
	{
		string ligne;
		while(std::getline(fichier,ligne))
		{
			CfgFile::stripEndLine(ligne);
			CfgFile::stripComments(ligne);
			CfgFile::stripString(ligne);
			// Si la ligne n'est pas un commentaire ou une ligne vide
			if (ligne.size() > 0)
			{
				//Msg("---> %s\n",ligne.c_str());
				// On extrait correctement le nom de la map et on l'ajoute à la liste
				addLigne(ligne);
			}
		}
	}
	else
		Api::debug("The file \"" + pathFichier + "\" is not readable !");
}
