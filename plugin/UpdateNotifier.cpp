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

#include "UpdateNotifier.h"
#include "ServerPlugin.h"

#include <sstream>

using namespace cssmatch;
using namespace threading;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::getline;

UpdateNotifier::UpdateNotifier() : alive(true), version(CSSMATCH_VERSION)
{
#ifdef _WIN32
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
    WSACleanup();
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
                try
                {
                    mutex.lock();
                    version = temp;
                    mutex.unlock();
                }
                catch (const ThreadException & e)
                {
                    CSSMATCH_PRINT(e.getMessage());
                }
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

void UpdateNotifier::run()
{
    while(alive)
    {
        try
        {
            wake.reset();    
        }
        catch (const ThreadException & e)
        {
            CSSMATCH_PRINT(e.getMessage());
        }

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
                query(serv, socketfd, hostname);

                shutdown(socketfd, SD_BOTH);
                closesocket(socketfd);
            }
        }
        try
        {
#ifdef _DEBUG
            wake.wait(1*60*1000);
#else
            wake.wait(24*60*60*1000); // 24h
#endif // _DEBUG
        }
        catch (const ThreadException & e)
        {
            alive = false;
            CSSMATCH_PRINT(e.getMessage());
        }
    }
}

void UpdateNotifier::end()
{
    alive = false;
    try
    {
        wake.set();
    }
    catch (const ThreadException & e)
    {
        CSSMATCH_PRINT(e.getMessage());
    }
}

std::string UpdateNotifier::getLastVer()
{
    string result;

    try
    {
        mutex.lock();
        result = version;
        mutex.unlock();
    }
    catch (const ThreadException & e)
    {
        CSSMATCH_PRINT(e.getMessage());
        result = CSSMATCH_VERSION;
    }

    return result;
}
