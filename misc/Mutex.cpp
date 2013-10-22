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

#include "Mutex.h"

using namespace cssmatch;

Mutex::Mutex()                                                    
{      
#if defined _WIN32                                                
    handle = CreateMutex(NULL, FALSE, NULL);                      
    if (handle == NULL)                                           
#else
    if (pthread_mutex_init(&handle, NULL) != 0)                   
#endif // _WIN32                                                  
        throw MutexException("Mutex initialization failed");      
}  
   
Mutex::~Mutex()
{      
#if defined _WIN32                                                
    CloseHandle(handle);                                          
#else
    if (pthread_mutex_destroy(&handle) != 0)                      
        CSSMATCH_PRINT("Error destroying the mutex");             
#endif // _WIN32                                                  
}      
                                                                  
void Mutex::lock()
{
#if defined _WIN32                                                
    if (WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0)   
#else                                                             
    if (pthread_mutex_lock(&handle) != 0)                         
#endif // _WIN32
        throw MutexException("Mutex lock failed");                
}      
   
void Mutex::unlock()                                              
{                                                                 
#if defined _WIN32
    if (! ReleaseMutex(handle))
#else
    if (pthread_mutex_unlock(&handle) != 0)
#endif // _WIN32
        throw MutexException("Mutex unlock failed");
}
