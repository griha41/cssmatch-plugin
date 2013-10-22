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

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "../exceptions/BaseException.h"

// thread api
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif // _WIN32

namespace cssmatch
{
#ifdef _WIN32
    typedef HANDLE MutexHandle;
#else
    typedef pthread_mutex_t MutexHandle;
#endif // _WIN32

    class MutexException : public BaseException
    {
    public:
        MutexException(const std::string & message) : BaseException(message){}
    };

    /**
     * Basic mutex implementation.
     */
    class Mutex
    {
    private:
        /** Mutex handle. */
        MutexHandle handle;
    public:
        /** 
         * @throws MutexException
         */
        Mutex();

        ~Mutex();

        /** Lock the mutex. 
         * @throws MutexException
         */
        void lock();
        
        /** Unlock the mutex. 
         * @throws MutexException
         */
        void unlock();
    };
}

#endif // __MUTEX_H__