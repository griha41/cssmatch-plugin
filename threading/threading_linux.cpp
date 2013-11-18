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

#ifndef _WIN32

#include <cstdlib> // calloc
#include <ctime>   // timespec, timeval
#include <sys/time.h> // gettimeofday
#include <pthread.h>
#include <errno.h> // ETIMEOUT

#include "threading.h"

using namespace threading;

/**
 * Fill a timespec structure based on a milliseconds duration.
 */
static inline void ms_to_timespec(long ms, timespec & out)
{
    out.tv_nsec = (ms % 1000) * 1000000;
    out.tv_sec = ms / 1000;
    // Fix tv_nsec so it's not greater than 1 sec
    out.tv_sec += out.tv_nsec / 1000000000;
    out.tv_nsec %= 1000000000;
}

struct threading::ThreadData
{
    pthread_t handle;
    bool started;
};

void * threading_thread_routine(void * param)
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
    data->started = false;
}

Thread::~Thread()
{
    free(data), data = NULL;
}

void Thread::start() throw(ThreadException)
{
    if (data->started)
        throw ThreadException("Thread::start() : Thread already started.");

    int result = pthread_create(&data->handle, NULL, threading_thread_routine, this);

    if (result != 0)
        throw ThreadException("Thread::start() : pthread_create didn't return 0.");
    data->started = true;
}

void Thread::join() throw(ThreadException)  
{ 
    if (! data->started)
        throw ThreadException("Thread::join() : Thread not started.");

    int result = pthread_join(data->handle, NULL);

    if (result != 0)
        throw ThreadException("Thread::join() : pthread_join didn't return 0.");
}


struct threading::MutexData
{
    pthread_mutex_t handle;
};

Mutex::Mutex() throw(ThreadException) : data(NULL) 
{
    pthread_mutex_t handle;
    int result = pthread_mutex_init(&handle, NULL);

    if (result != 0)
        throw ThreadException("Mutex::Mutex() : pthread_mutex_init didn't return 0.");
    
    data = (MutexData *)calloc(1, sizeof(MutexData));
    data->handle = handle;
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&data->handle);
    free(data), data = NULL;
}

void Mutex::lock() throw(ThreadException)
{
    int result = pthread_mutex_lock(&data->handle);

    if (result != 0)
        throw ThreadException("Mutex::lock() : pthread_mutex_lock didn't return 0.");
}

void Mutex::unlock() throw(ThreadException)
{
    int result = pthread_mutex_unlock(&data->handle);

    if (result != 0)
        throw ThreadException("Mutex::unlock() : pthread_unmutex_lock didn't return 0.");
}

void threading::sleep(long ms)
{
    timespec timeout;
    ms_to_timespec(ms, timeout);
    nanosleep(timeout, NULL);
}

#if 0
struct threading::EventData
{
    pthread_cond_t handle;
    pthread_mutex_t mutex;
};

Event::Event() : data(NULL)
{
    pthread_cond_t handle;
    int resultCondInit = pthread_cond_init(&handle, NULL);

    if (resultCondInit != 0)
        throw ThreadException("Event::Event() : pthread_cond_init didn't return 0.");
    
    pthread_mutex_t mutex;    
    int resultMutexInit = pthread_mutex_init(&mutex, NULL);

    if (resultMutexInit != 0)
    {
        pthread_cond_destroy(&handle);
        throw ThreadException("Event::Event() : pthread_mutex_init didn't return 0.");
    }
   
    data = (EventData *)calloc(1, sizeof(EventData));
    data->handle = handle;
    data->mutex = mutex;
}

Event::~Event()
{
    pthread_cond_destroy(&data->handle);
    pthread_mutex_destroy(&data->mutex);
    free(data), data = NULL;
}

EventWaitResult Event::wait(long timeoutMs) throw(ThreadException)
{
    int resultLock = pthread_mutex_lock(&data->mutex);
    if (resultLock != 0)
        throw ThreadException("Event::wait() : pthread_mutex_lock didn't return 0.");

    timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_nsec += (timeoutMs % 1000) * 1000000;
    timeout.tv_sec += timeoutMs / 1000;
    // Fix tv_nsec so it's not greater than 1 sec
    timeout.tv_sec += timeout.tv_nsec / 1000000000;
    timeout.tv_nsec %= 1000000000;
    int resultWait = pthread_cond_timedwait(&data->handle, &data->mutex, &timeout);

    EventWaitResult waitResult;
    if (resultWait == 0)
        waitResult = THREADING_EVENT_SIGNALED;
    else if (resultWait == ETIMEDOUT)
        waitResult = THREADING_EVENT_TIMEOUT;
    else
        throw ThreadException("Event::wait() : pthread_cond_wait didn't return 0 nor ETIMEDOUT.");

    int resultUnlock = pthread_mutex_unlock(&data->mutex);
    if (resultUnlock != 0)
        throw ThreadException("Event::wait() : pthread_mutex_unlock didn't return 0.");

    return waitResult;
}

void Event::set() throw(ThreadException)
{
    int resultLock = pthread_mutex_lock(&data->mutex);
    if (resultLock != 0)
        throw ThreadException("Event::set() : pthread_mutex_lock didn't return 0.");

    int resultSignal = pthread_cond_broadcast(&data->handle);
    if (resultSignal != 0)
        throw ThreadException("Event::set() : pthread_cond_signal didn't return 0.");

    int resultUnlock = pthread_mutex_unlock(&data->mutex);
    if (resultUnlock != 0)
        throw ThreadException("Event::set() : pthread_mutex_unlock didn't return 0.");
}
#endif

#endif // ! _WIN32
