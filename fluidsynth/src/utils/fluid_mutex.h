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

/* glib 2.32 and newer */

/* Regular mutex */
typedef GMutex fluid_mutex_t;
#define FLUID_MUTEX_INIT          { 0 }
#define fluid_mutex_init(_m)      g_mutex_init (&(_m))
#define fluid_mutex_destroy(_m)   g_mutex_clear (&(_m))
#define fluid_mutex_lock(_m)      g_mutex_lock(&(_m))
#define fluid_mutex_unlock(_m)    g_mutex_unlock(&(_m))

/* Recursive lock capable mutex */
typedef GRecMutex fluid_rec_mutex_t;
#define fluid_rec_mutex_init(_m)      g_rec_mutex_init(&(_m))
#define fluid_rec_mutex_destroy(_m)   g_rec_mutex_clear(&(_m))
#define fluid_rec_mutex_lock(_m)      g_rec_mutex_lock(&(_m))
#define fluid_rec_mutex_unlock(_m)    g_rec_mutex_unlock(&(_m))

/* Dynamically allocated mutex suitable for fluid_cond_t use */
typedef GMutex    fluid_cond_mutex_t;
#define fluid_cond_mutex_lock(m)        g_mutex_lock(m)
#define fluid_cond_mutex_unlock(m)      g_mutex_unlock(m)

static FLUID_INLINE fluid_cond_mutex_t *
new_fluid_cond_mutex (void)
{
  GMutex *mutex;
  mutex = g_new (GMutex, 1);
  g_mutex_init (mutex);
  return (mutex);
}

static FLUID_INLINE void
delete_fluid_cond_mutex (fluid_cond_mutex_t *m)
{
  g_mutex_clear (m);
  g_free (m);
}

/* Thread condition signaling */
typedef GCond fluid_cond_t;
#define fluid_cond_signal(cond)         g_cond_signal(cond)
#define fluid_cond_broadcast(cond)      g_cond_broadcast(cond)
#define fluid_cond_wait(cond, mutex)    g_cond_wait(cond, mutex)

static FLUID_INLINE fluid_cond_t *
new_fluid_cond (void)
{
  GCond *cond;
  cond = g_new (GCond, 1);
  g_cond_init (cond);
  return (cond);
}

static FLUID_INLINE void
delete_fluid_cond (fluid_cond_t *cond)
{
  g_cond_clear (cond);
  g_free (cond);
}

#endif
