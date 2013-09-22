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

#include "UpdateNotifier.h"
#include "ServerPlugin.h"

#include <sstream>

using namespace cssmatch;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::getline;


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

// thread api
#ifdef _WIN32
    typedef DWORD ThreadReturn;
#define ThreadReturn ThreadReturn WINAPI
    typedef LPVOID ThreadParam;
    
    const HANDLE INVALID_HANDLE = INVALID_HANDLE_VALUE;
#else
#include <pthread.h>
    typedef void * ThreadReturn;
    typedef void * ThreadParam;

    const int INVALID_HANDLE = -1;
#endif
    
static ThreadReturn updateNotifierInternalRun(ThreadParam param)
{
    UpdateNotifier * notifier = static_cast<UpdateNotifier *>(param);
    SOCKADDR_IN serv;
    memset(&serv, 0, sizeof(serv));

    const char * hostname =
        ServerPlugin::getInstance()->getConVar("cssmatch_updatesite")->GetString();

    hostent * host = gethostbyname(hostname);
    if (host == NULL)
    {
        CSSMATCH_PRINT("gethostbyname() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
    }
    else
    {
        memcpy(&serv.sin_addr, host->h_addr, host->h_length);

        serv.sin_port = htons(80);
        serv.sin_family = AF_INET;

        SOCKET socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketfd == INVALID_SOCKET)
        {
            CSSMATCH_PRINT("socket() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
        }
        else
        {
            notifier->query(serv, socketfd, hostname);

            shutdown(socketfd, SD_BOTH);
            closesocket(socketfd);
        }
    }
#ifdef _WIN32
    return 0l;
#else
    return NULL;
#endif
}

UpdateNotifier::UpdateNotifier() : threadHandle(INVALID_HANDLE), version(CSSMATCH_VERSION)
{
#ifdef _WIN32
    // Init use of Winsock DLL
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        throw UpdateNotifierException("WSAStartup failed");
    }
#endif // _WIN32
}

UpdateNotifier::~UpdateNotifier()
{
#ifdef _WIN32
    // Terminates use of the Winsock 2 DLL
    WSACleanup();
    
    // Close the thread handle
    if (threadHandle != INVALID_HANDLE)
        CloseHandle(threadHandle);

#endif // _WIN32
}

void UpdateNotifier::query(const SOCKADDR_IN & serv, const SOCKET & socketfd,
                           const string & hostname)
{
    // Connect to the host
    if (connect(socketfd, (SOCKADDR *)&serv, sizeof(serv)) == SOCKET_ERROR)
    {
        CSSMATCH_PRINT("connect() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
        return;
    }

    // Query the file
    ostringstream requestbuf;
    requestbuf << "GET " CSSMATCH_VERSION_FILE " HTTP/1.0\r\n"
    "Host: " << hostname << "\r\n"
    "Connection: Close\r\n"
    "\r\n";
    const string & tempRequest = requestbuf.str();
    const char * request = tempRequest.c_str();
    if (send(socketfd, request, strlen(request), 0) == SOCKET_ERROR)
    {
        CSSMATCH_PRINT("send() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
        return;
    }

    // Wait for a response
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(socketfd, &readfs);
    timeval timeout = {30L, 0L}; // 30 sec + 0 microsec
    int ready = select(socketfd + 1, &readfs, NULL, NULL, &timeout);
    switch(ready)
    {
    case SOCKET_ERROR:
        CSSMATCH_PRINT("select() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
        break;

    case 0: // timeout expired
        CSSMATCH_PRINT(hostname + " timeout");
        break;

    default:
        if(FD_ISSET(socketfd, &readfs))
        {
            char buffer[300];
            int received = recv(socketfd, buffer, sizeof(buffer)-1, 0);
            if (received == SOCKET_ERROR)
            {
                CSSMATCH_PRINT("recv() failed (error " + toString(SOCKET_ERROR_CODE) + ")");
                return;
            }

            buffer[received] = '\0';
            istringstream stream(buffer);

            // Check for the success
            string temp;
            stream >> temp; // HTTP/x.x
            stream >> temp; // status  code
            if (temp == "200") // 200 OK
            {
                while(getline(stream, temp)) {} // get the last line

                // Update the last version name found
                version = temp;
            }
#ifdef _DEBUG
            else
            {
                CSSMATCH_PRINT("Error code " + temp);
            }
#endif // _DEBUG
        }
    }
}

void UpdateNotifier::start()
{
    if (threadHandle == INVALID_HANDLE)
    {
#if defined _WIN32
        DWORD threadId;
        threadHandle = CreateThread(NULL, 0, updateNotifierInternalRun, this, 0, &threadId);
        if (threadId == NULL)
#else
        if (pthread_ceate(&threadHandle, &threadAttr, updateNotifierInternalRun, this) != 0)
#endif
            throw UpdateNotifierException("Thread initialization failed");
    }
    else
        CSSMATCH_PRINT("Update notifier already started");
}

void UpdateNotifier::join()
{
#if defined _WIN32
    if (WaitForSingleObject(threadHandle, INFINITE) == WAIT_FAILED)
#else
    if (pthread_join(&threadHandle, NULL) != 0)
#endif
        throw UpdateNotifierException("Thread join failed");
}

std::string UpdateNotifier::getLastVer() const
{
    return version;
}
