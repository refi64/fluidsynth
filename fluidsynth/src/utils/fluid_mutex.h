/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */


/**

   This header contains an implementation of atomic operations.

 */

#ifndef _FLUID_MUTEX_H
#define _FLUID_MUTEX_H

#if HAVE_WINDOWS_H

/* Regular mutex */

typedef HANDLE fluid_mutex_t;
#define fluid_mwinapi_call(_f, _r, ...) FLUID_STMT_START { \
                                          if ((__VA_ARGS__) == (_r)) \
                                            FLUID_LOG(FLUID_ERR, #_f " failed"); \
                                        } FLUID_STMT_END

#define FLUID_MUTEX_INIT NULL
#define fluid_mutex_init(_m)      fluid_mwinapi_call(CreateMutex, NULL, \
                                    ((_m) = CreateMutex(NULL, FALSE, NULL)))
#define fluid_mutex_destroy(_m)   fluid_mwinapi_call(CloseHandle, 0, \
                                    CloseHandle((_m)))
#define fluid_mutex_lock(_m)      fluid_win_mutex_lock(&(_m))
#define fluid_mutex_unlock(_m)    fluid_win_mutex_unlock(&(_m))

static void
fluid_win_mutex_lock (fluid_mutex_t* mutex)
{
  if (*mutex == NULL)
    fluid_mutex_init(*mutex);
  fluid_mwinapi_call(WaitForSingleObject, WAIT_FAILED, \
    WaitForSingleObject(*mutex, INFINITE));
}

static void
fluid_win_mutex_unlock (fluid_mutex_t* mutex)
{
  if (*mutex == NULL)
    return;
  fluid_mwinapi_call(ReleaseMutex, 0, ReleaseMutex(*mutex));
}

/* On Windows, mutexes are already recursive. */

typedef fluid_mutex_t fluid_rec_mutex_t;

#define fluid_rec_mutex_init(_m)      fluid_mutex_init(_m)
#define fluid_rec_mutex_destroy(_m)   fluid_mutex_destroy(_m)
#define fluid_rec_mutex_lock(_m)      fluid_mutex_lock(_m)
#define fluid_rec_mutex_unlock(_m)    fluid_mutex_unlock(_m)

/* Thread condition signaling */

typedef ;

#elif HAVE_PTHREAD_H

#include <pthread.h>

/* Regular mutex */

typedef pthread_mutex_t fluid_mutex_t;
#define fluid_pthread_call(_f, ...) FLUID_STMT_START { \
                                    if (_f(__VA_ARGS__) != 0) \
                                      FLUID_LOG(FLUID_ERR, #_f " failed"); \
                                  } FLUID_STMT_END

#define FLUID_MUTEX_INIT          PTHREAD_MUTEX_INITIALIZER
#define fluid_mutex_init(_m)      fluid_pthread_call(pthread_mutex_init, &(_m), NULL)
#define fluid_mutex_destroy(_m)   fluid_pthread_call(pthread_mutex_destroy, &(_m))
#define fluid_mutex_lock(_m)      fluid_pthread_call(pthread_mutex_lock, &(_m))
#define fluid_mutex_unlock(_m)    fluid_pthread_call(pthread_mutex_unlock, &(_m))

/* Recursive lock capable mutex */

typedef struct _fluid_rec_mutex_t {
  pthread_mutex_t mutex;
  pthread_mutexattr_t attr;
} fluid_rec_mutex_t;

#define fluid_rec_mutex_init(_m)      FLUID_STMT_START { \
                                        fluid_rec_mutex_t* _mp = &(_m); \
                                        fluid_pthread_call(pthread_mutexattr_init, \
                                                           &_mp->attr); \
                                        fluid_pthread_call(pthread_mutexattr_settype, \
                                                           &_mp->attr, \
                                                           PTHREAD_MUTEX_RECURSIVE); \
                                        fluid_pthread_call(pthread_mutex_init, \
                                                           &_mp->mutex, &_mp->attr); \
                                      } FLUID_STMT_END
#define fluid_rec_mutex_destroy(_m)   FLUID_STMT_START { \
                                        fluid_rec_mutex_t* _mp = &(_m); \
                                        fluid_pthread_call(pthread_mutexattr_destroy, \
                                                           &_mp->attr); \
                                        fluid_mutex_destroy(_mp->mutex); \
                                      } FLUID_STMT_END
#define fluid_rec_mutex_lock(_m)      fluid_mutex_lock((_m).mutex)
#define fluid_rec_mutex_unlock(_m)    fluid_mutex_unlock((_m).mutex)

/* Thread condition signaling */

// TODO: This will explode into a violent flame.

typedef int fluid_cond_t;
#define fluid_cond_mutex_lock(m)        ((void)0)
#define fluid_cond_mutex_unlock(m)      ((void)0)

static FLUID_INLINE fluid_cond_t *
new_fluid_cond (void)
{
  return FLUID_NEW(int);
}

static FLUID_INLINE void
delete_fluid_cond (fluid_cond_t *cond)
{
  free(cond);
}

#else

#error Unsupported platform

#endif // HAVE_WINDOWS_H, HAVE_PTHREAD_H

/* Dynamically allocated mutex suitable for fluid_cond_t use */
typedef fluid_mutex_t fluid_cond_mutex_t;
#define fluid_cond_mutex_lock(m)        fluid_mutex_lock(*m)
#define fluid_cond_mutex_unlock(m)      fluid_mutex_unlock(*m)

static FLUID_INLINE fluid_cond_mutex_t *
new_fluid_cond_mutex (void)
{
  fluid_cond_mutex_t* mutex = FLUID_NEW(fluid_cond_mutex_t);
  fluid_mutex_init(*mutex);
  return (mutex);
}

static FLUID_INLINE void
delete_fluid_cond_mutex (fluid_cond_mutex_t *m)
{
  fluid_mutex_destroy(*m);
  free(m);
}

#endif
