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
    HANDLE handle = CreateEvent(NULL, TRUE, FALSE, TEXT("threading::Event")); 

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

EventWaitResult Event::wait(long timeoutMs)
{
    DWORD result = WaitForSingleObject(data->handle, timeoutMs);

    EventWaitResult waitResult;
    if (result == WAIT_OBJECT_0)
        waitResult = THREADING_EVENT_SIGNALED;
    else if (result == WAIT_TIMEOUT)
    waitResult = THREADING_EVENT_TIMEOUT;
    else
        throw ThreadException("Event::wait() : WaitForSingleObject didn't return WAIT_OBJECT_0 nor WAIT_TIMEOUT.");
    return waitResult;
}

void Event::set()
{
    DWORD result = SetEvent(data->handle);

    if (! result)
        throw ThreadException("Event::set() : SetEvent did return FALSE.");
}

void Event::reset()
{
    DWORD result = ResetEvent(data->handle);

    if (! result)
        throw ThreadException("Event::reset() : ResetEvent did return FALSE.");
}

#endif // _WIN32
