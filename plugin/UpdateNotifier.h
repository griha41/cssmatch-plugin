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

#ifndef __UPDATE_NOTIFIER_H__
#define __UPDATE_NOTIFIER_H__

// socket api
// Leave it here so Source SDK undef/redefine the microsoft's ARRAYSIZE macro
#include <string.h>
#ifdef _WIN32

#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#define SOCKET_ERROR_CODE WSAGetLastError()

#else

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close
#include <netdb.h> // gethostbyname
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
#define SD_BOTH SHUT_RDWR
typedef int SOCKET;
typedef sockaddr_in SOCKADDR_IN; // typedef struct sockaddr_in SOCKADDR_IN;
typedef sockaddr SOCKADDR;
typedef in_addr IN_ADDR;
#define SOCKET_ERROR_CODE errno

#endif

#include "../misc/common.h" // pragma
#include "../exceptions/BaseException.h"

#include "threadtools.h"

#include <string>

#ifdef CSSMATCH_BETA
#define CSSMATCH_VERSION_FILE "/plugin/versionbeta.php"
#else
#define CSSMATCH_VERSION_FILE "/plugin/version.php"
#endif

namespace cssmatch
{

    class UpdateNotifierException : public BaseException
    {
    public:
        UpdateNotifierException(const std::string & message) : BaseException(message){}
    };

    /** Compares the current plugin version with the one from cssmatch.com */
    class UpdateNotifier //: public CThread
    {
    private:
        /** Last plugin version found */
        std::string version;

        bool alive; // thread can continue?
        CThreadEvent wake; // kind of "interuptible" sleep (T O D O: better way?)
        CThreadMutex mutex;

        /** Query the server and update "version" */
        void query(const SOCKADDR_IN & serv, const SOCKET & socketfd, const std::string & hostname);
    public:
        /**
         * @throws UpdateNotifierException If the socket api cannot being initialized
         */
        UpdateNotifier(); /* throw(UpdateNotifierException); => only useful under Windows, but only
                           supported under Linux (see pragma) */
        ~UpdateNotifier();

        // CThread method
        int Run();

        /** Tell to the thread that it must exit */
        void End();

        std::string getLastVer() const;
    };
}

#endif // __UPDATE_NOTIFIER_H__
