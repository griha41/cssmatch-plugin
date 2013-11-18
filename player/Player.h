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

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "../exceptions/BaseException.h"
#include "../entity/EntityProp.h"
#include "../messages/RecipientFilter.h"
#include "../misc/common.h"
#include "../messages/I18nManager.h"
#include "../plugin/BaseTimer.h"

class IVEngineServer;
class CBasePlayer;
class CBaseCombatCharacter;
class CBaseCombatWeapon;

#include <map>
#include <string>

namespace cssmatch
{
    class RecipientFilter;
    class Menu;
    struct BaseMenuLineData;
    class MenuReSendTimer;

    class PlayerException : public BaseException
    {
    public:
        PlayerException(const std::string & message) : BaseException(message){};
    };

    /** Team id */
    enum TeamCode
    {
        INVALID_TEAM = TEAM_INVALID,
        UN_TEAM = TEAM_UNASSIGNED,
        SPEC_TEAM = TEAM_SPECTATOR,
        T_TEAM,
        CT_TEAM
    };

    /** Weapon slot codes */
    enum WeaponSlotCode
    {
        WEAPON_SLOT1 = 0,
        WEAPON_SLOT2,
        WEAPON_SLOT3,
        WEAPON_SLOT4,
        WEAPON_SLOT5
    };

	/** Player's info memorized by Player instances */
    struct PlayerIdentity
    {
        /** Pointer to the player entity instance corresponding to this player */
        edict_t * pEntity;

        /** Index of this player */
        int index;

        /** Userid of this player */
        int userid;

        /** Steamid of this player */
        std::string steamid;

        PlayerIdentity() : pEntity(NULL), index(CSSMATCH_INVALID_INDEX),
            userid(CSSMATCH_INVALID_USERID), steamid(""){};
    };

	/** Info about the menu the player is using */
    struct PlayerMenuHandler
    {
        /** The Menu the player uses */
        Menu * menu;

        /** The page the player uses */
        int page;

        /** Does the menu will have to be deleted when it will be closed */
        bool toDelete;

        /** Menu parameters */
        std::map<std::string, std::string> parameters;

        /** Data related to the selected lines in the menu */
        BaseMenuLineData * data;

        PlayerMenuHandler() : menu(NULL), page(1), toDelete(false), data(NULL) {}
    };

    /** Base class for player informations */
    class Player
    {
    private:
        // Functors
        friend struct PlayerHavingPEntity;
        friend struct PlayerHavingIndex;
        friend struct PlayerHavingUserid;
        friend struct PlayerHavingSteamid;
        friend struct PlayerHavingTeam;
        friend struct PlayerIsHltv;
        friend struct PlayerToRecipient;

        // Timer
        friend class MenuReSendTimer;
    protected:
        /** Player identity */
        PlayerIdentity identity;

        /** Last date when the player sent a command to the server  */
        float lastCommandDate;

        // Entity prop handler
        static EntityProp accountHandler;
        static EntityProp lifeStateHandler;
        static EntityProp playerStateHandler;
        //static EntityProp vecOriginHandler;
        //static EntityProp angRotationHandler;
        //static EntityProp eyeAngles0Handler;
        //static EntityProp eyeAngles1Handler;
        //static EntityProp armorHandler;

        /** Menus handler */
        PlayerMenuHandler menuHandler;
        MenuReSendTimer * menuTimer;

    public:
        /**
         * @param index The player index (valid!)
         * @throws PlayerException If some player info are unavailable (edict_t, IPlayerInfo, userid, steamid, ...)
         */
        Player(int index) throw(PlayerException);

        virtual ~Player();

        /** Get the identity informations about this player
         * @return A PlayerIdentity struct pointer
         * @see struct PlayerIdentity
         */
        PlayerIdentity * getIdentity();

        // Anti-flood
        /** Determines if the player is allowed to use a command then update <br>
         * Update the last command date
         * @return <code>true</code> if the player can use the command, <code>false</code> otherwise
         */
        bool canUseCommand();


        /** Send a menu to the player
         * @param usedMenu The menu
         * @param page The page of the menu actually used by the player
         * @param parameters The i18n parameters needed by the menu to send
         * @param toDelete If the menu has to be deleted once the player closes it
         */
        void sendMenu(  Menu * usedMenu,
                        int page,
                        const std::map<std::string,
                                       std::string> & parameters = I18nManager::WITHOUT_PARAMETERS,
                        bool toDelete = false);
        Menu * getMenu() const;
        int getPage() const;

        /** Memorize menu related data */
        void storeMenuData(BaseMenuLineData * data);
        BaseMenuLineData * const getMenuData();

        /** Send to the player the next page of the current menu */
        void nextPage();

        /** Send to the player the previous page of the current menu */
        void previousPage();

        /** Quit the current menu (and delete it if asked with Player::setMenu) */
        void quitMenu();

        /** Get the current team of this player
         * @return The team id of the player, or INVALID_TEAM if something was invalid
         * @see enum TeamCode
         */
        TeamCode getMyTeam() const;

        /** Get an IPlayerInfo instance corresponding to this entity <br>
         * Does not check the result with isValidPlayerInfo!
         * @return The IPlayerInfo instance corresponding to this entity, or NULL if something was invalid
         */
        IPlayerInfo * getPlayerInfo() const;

        /* Get a base player instance corresponding to this entity
         * @return The base player instance corresponding to this entity, or NULL if something was invalid
         */
        //CBasePlayer * getBasePlayer() const;

        /* Get a base combat character instance corresponding to this entity
         * @return The base combat character instance corresponding to this entity, or NULL if something was invalid
         */
        //CBaseCombatCharacter * getBaseCombatCharacter() const;

        /** Kick this player
         * @param reason Reason for the kick (i18n keyword)
         */
        void kick(const std::string & reason) const;

        /** Ban this player
         * @param duration Ban duration (in minutes, 0=permanent)
         * @param reason Reason for the kick/ban (i18n keyword)
         */
        void ban(int duration, const std::string & reason) const;

        /** Swap this player
         * @return <code>false</code> if the player is spectator
         */
        //@param respawn <code>true</code> to spawn the player (without class choice)
        bool swap(/*bool rspawn*/);

        /** Put this player in the spectator team
         * @return <code>false</code> if the player is already spectator
         */
        bool spec();

        /** Execute a command into the player console
         * @param command The command line to execute
         */
        void cexec(const std::string & command) const;

        /** Run a command for a player
        * @param command The command line to execute
        */
        void sexec(const std::string & command) const;

        /* Set the player account */
        void setAccount(int newCash);
        /* Get the player account (returns -1 if it fails) */
        int getAccount();

        /** Set the player life state */
        void setLifeState(int newState);
        /** Get the life state (returns -1 if it fails) */
        int getLifeState();

        /** Set the player state */
        void setPlayerState(int newState);
        /** Get the player state (returns -1 if it fails) */
        int getPlayerState();

        /* Set the player location */
        //void setVecOrigin(const Vector & vec);
        /* Get the player location (x,y,z are VEC_T_NAN if it fails) */
        //Vector getVecOrigin();

        /* Get the player rotation angle (x,y,z are VEC_T_NAN if it fails) */
        //Vector getAngRotation();
        /* Get the player view angle (x,y,z are VEC_T_NAN if it fails) */
        //QAngle getViewAngle();

        /* Set the player armor value */
        //void setArmor(int newArmor);
        /* Get the player armor value (returns -1 if it fails) */
        //int getArmor();


        /* Spawn this player */
        void spawn();


        /** Give an named item to the player
         * @param item Item to give
         */
        void give(const std::string & item);


        /* Set the view angle of the player
         * @param angle New view angle
         */
        //void setang(const QAngle & angle);
    };

    /** Functor to quickly find a Player instance by his edict_t instance */
    struct PlayerHavingPEntity
    {
        edict_t * pEntity;

        PlayerHavingPEntity(edict_t * pEnt) : pEntity(pEnt){}

        bool operator ()(const Player * player)
        {
            return player->identity.pEntity == pEntity;
        }
    };

    /** Functor to quickly find a Player instance by his index */
    struct PlayerHavingIndex
    {
        int index;

        PlayerHavingIndex(int ind) : index(ind){}

        bool operator ()(const Player * player)
        {
            return player->identity.index == index;
        }
    };

    /** Functor to quickly find a Player instance by his userid */
    struct PlayerHavingUserid
    {
        int userid;

        PlayerHavingUserid(int id) : userid(id){}

        bool operator ()(const Player * player)
        {
            return player->identity.userid == userid;
        }
    };

    /** Functor to quickly find a Player instance by his steamid */
    struct PlayerHavingSteamid
    {
        std::string steamid;

        PlayerHavingSteamid(const std::string & id) : steamid(id){}

        bool operator ()(const Player * player)
        {
            return player->identity.steamid == steamid;
        }
    };

    /** Functor to quickly find a Player instance by his team */
    struct PlayerHavingTeam
    {
        TeamCode team;

        PlayerHavingTeam(TeamCode t) : team(t){}

        bool operator ()(const Player * player)
        {
            return player->getMyTeam() == team;
        }
    };

    /** Functor to quickly find SourceTV */
    struct PlayerIsHltv
    {
        bool operator ()(const Player * player)
        {
            IPlayerInfo * pInfo = player->getPlayerInfo();

            return (pInfo != NULL) && pInfo->IsConnected() && pInfo->IsHLTV();
        }
    };

    /** Functor to add a player in a recipient list */
    struct PlayerToRecipient
    {
        RecipientFilter * recipientFilter;

        PlayerToRecipient(RecipientFilter * filter) : recipientFilter(filter){}

        void operator ()(Player * player)
        {
            recipientFilter->addRecipient(player);
        }
    };

    /** Functor to quickly remove a player */
    struct PlayerToRemove
    {
        void operator ()(const Player * player)
        {
            delete player;
        }
    };

    /** Timer to resend popup the player is viewing */
    class MenuReSendTimer : public BaseTimer
    {
    private:
        /** Player to resend the popup */
        int userid;
		// userid (connection id), because when this timer ends the player may have disconnected
    public:
        /**
         * @param delay Delay before sending
         * @param playerUserid Userid of the player who views the popup
         * @see BaseTimer
         */
        MenuReSendTimer(float delay, int playerUserid);

        /** @see BaseTimer */
        void execute();
    };
}

#endif // __PLAYER_H__
