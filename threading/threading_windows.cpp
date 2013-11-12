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
 * along with CSSMatch; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Portions of this code are also Copyright © 1996-2005 Valve Corporation, All rights reserved
 */

#ifdef _WIN32

#include <windows.h>

#include "threading.h"

using namespace threading;

struct threading::ThreadData
{
    HANDLE handle;
};

DWORD WINAPI threading_thread_routine(LPVOID param)
{
    Thread * thread = static_cast<Thread *>(param);
    if (thread != NULL)
    {
        thread->run();
    }
    return 0;
}

Thread::Thread() : data(NULL)
{
    data = (ThreadData *)calloc(1, sizeof(ThreadData));
    data->handle = NULL;
}

Thread::~Thread()
{
    if (data->handle != NULL)
        CloseHandle(data->handle), data->handle = NULL;
    free(data), data = NULL;
}

void Thread::start()
{
    if (data->handle != NULL)
        throw ThreadException("Thread::start() : Thread already started.");

    data->handle = CreateThread(NULL, 0, threading_thread_routine, this, 0, NULL);

    if (data->handle == NULL)
        throw ThreadException("Thread::start() : CreateThread did return NULL.");
}

void Thread::join()
{ 
    if (data->handle == NULL)
        throw ThreadException("Thread::join() : Thread not started.");

    DWORD result = WaitForSingleObject(data->handle, INFINITE);

    if (result != WAIT_OBJECT_0)
        throw ThreadException("Thread::join() : WaitForSingleObject didn't return WAIT_OBJECT_0.");
}


struct threading::MutexData
{
    HANDLE handle;
};

Mutex::Mutex() : data(NULL)
{
    HANDLE handle = CreateMutex(NULL, FALSE, NULL);

    if (handle == NULL)
        throw ThreadException("Mutex::Mutex() : CreateMutex did return NULL.");

    data = (MutexData *)calloc(1, sizeof(MutexData));
    data->handle = handle;
}

Mutex::~Mutex()
{
    CloseHandle(data->handle), data->handle = NULL;
    free(data), data = NULL;
}

void Mutex::lock()
{
    DWORD result = WaitForSingleObject(data->handle, INFINITE);

    if (result != WAIT_OBJECT_0)
        throw ThreadException("Mutex::lock() : WaitForSingleObject didn't return WAIT_OBJECT_0.");
}

void Mutex::unlock()
{
    BOOL result = ReleaseMutex(data->handle);

    if (! result)
        throw ThreadException("Mutex::unlock() : ReleaseMutex did return FALSE.");
}


struct threading::EventData
{
    HANDLE handle;
};

Event::Event() : data(NULL)
{
    HANDLE handle = CreateEvent(NULL, FALSE, FALSE, TEXT("threading::Event")); 

    if (handle == NULL)
        throw ThreadException("Event::Event() : CreateEvent did return NULL.");

    data = (EventData *)calloc(1, sizeof(EventData));
    data->handle = handle;
}

Event::~Event()
{
    CloseHandle(data->handle), data->handle = NULL;
    free(data), data = NULL;
}

void Event::wait()
{
    DWORD result = WaitForSingleObject(data->handle, INFINITE);

    if (result != WAIT_OBJECT_0)
        throw ThreadException("Event::wait() : WaitForSingleObject didn't return WAIT_OBJECT_0.");
}

void Event::signal()
{
    DWORD result = SetEvent(data->handle);

    if (! result)
        throw ThreadException("Event::signal() : SetEvent did return FALSE.");
}

#endif // _WIN32
