/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <sys/timeb.h>
#include <semaphore.h>

#include "../../libs/neutron/neutron.h"

typedef struct _semaphore_t {
  sem_t semp;
  uint32_t count;
} semaphore_t;

result_t semaphore_create(handle_t *semp)
	{
  semaphore_t *semaphore = (semaphore_t *)malloc(sizeof(semaphore_t));
  sem_init(&semaphore->semp, 0, 0);
  semaphore->count = 0;
	*semp = semaphore;

	return s_ok;
	}

result_t semaphore_close(handle_t h)
	{
  semaphore_t *sem = ((semaphore_t *)h);
  sem_destroy(&sem->semp);
  free(sem);

  return s_ok;
	}

__thread handle_t tls_mutex;

handle_t bsp_thread_mutex()
  {
  if(tls_mutex == 0)
    // create our thread specific handle
    semaphore_create(&tls_mutex);

  return tls_mutex;
  }

result_t semaphore_wait(handle_t h, uint32_t timeout_ms)
  {
  semaphore_t *sem = ((semaphore_t *)h);

  if((long) timeout_ms == INDEFINITE_WAIT)
    {
    sem->count ++;
    int rsl = sem_wait(&sem->semp);
    sem->count --;
    return rsl == 0 ? s_ok : s_false;
    }

  struct timespec ts;
  if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    return false;

  timeout_ms += ts.tv_nsec / 1000000;

  ts.tv_sec += timeout_ms / 1000;
  ts.tv_nsec = (timeout_ms % 1000) * 1000000;

  sem->count++;
  int rsl = sem_timedwait(&sem->semp, &ts);
  sem->count--;
  return rsl == 0 ? s_ok : s_false;
  }

uint32_t ticks()
  {
  struct timespec ts;
  if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
    return 0;

  uint32_t tick = (uint32_t)(ts.tv_nsec / 1000000);
  return tick;
  }

result_t semaphore_signal(handle_t h)
  {
  semaphore_t *sem = ((semaphore_t *)h);
  sem_post(&sem->semp);

  return s_ok;
  }

result_t has_wait_tasks(handle_t hndl)
  {
  semaphore_t *sem = ((semaphore_t *)hndl);

  return sem->count == 0 ? s_false : s_ok;
  }

typedef enum {
  creating,
  suspended,
  running,
  terminated
} thread_status;
typedef struct _thread_handle_t
	{
	pthread_t thread;
	pthread_mutex_t suspend_mutex;
	pthread_cond_t suspend_cond;
	thread_status status;
	task_callback thread_func;
	void *param;
	} thread_handle_t;


static void *thread_handle_run(void *p)
  {
  thread_handle_t *pthis = (thread_handle_t *)(p);

  // started suspended
  pthread_mutex_lock(&(pthis->suspend_mutex));
  if(pthis->status == creating)
    {
    while(pthis->status == creating)
      pthread_cond_wait(&(pthis->suspend_cond), &(pthis->suspend_mutex));
    }
  else
    pthread_mutex_unlock(&(pthis->suspend_mutex));

  pthis->status = running;
  // run the thread.
  pthis->thread_func(pthis->param);

  pthis->status = terminated;

  return 0;
  }

static void create_thread_handle(thread_handle_t *pthis, task_callback pfn, size_t stack_size, void *param)
  {
  pthis->thread_func = pfn;
  pthis->param = param;

  pthis->status = creating;

  pthread_mutex_init(&pthis->suspend_mutex, NULL);
  pthread_mutex_lock(&pthis->suspend_mutex);

  pthread_condattr_t condattr;
  pthread_condattr_init(&condattr);
  pthread_cond_init(&pthis->suspend_cond, &condattr);
  pthread_condattr_destroy(&condattr);

  pthread_attr_t threadattr;

  pthread_attr_init(&threadattr);
  pthread_attr_setstacksize(&threadattr, stack_size);
  pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
  pthread_create(&pthis->thread, &threadattr, thread_handle_run, pthis);
  pthread_attr_destroy(&threadattr);
  }

static void destroy_thread_handle(thread_handle_t *pthis)
  {
  pthread_cancel(pthis->thread);
  free(pthis);
  }

static void thread_handle_suspend(thread_handle_t *pthis)
  {
  pthis->status = suspended;
  }

static void thread_handle_resume(thread_handle_t *pthis)
  {
  // see if starting
  if(pthis->status == creating ||
      pthis->status == suspended)
    {
    pthis->status = running;
    pthread_cond_signal(&pthis->suspend_cond);
    }

  pthread_mutex_unlock(&pthis->suspend_mutex);
  }

static void thread_handle_set_priority(thread_handle_t *pthis, unsigned char pri)
  {
  }

static unsigned char thread_handle_get_priority(thread_handle_t *pthis)
  {
  return 0;
  }

static unsigned int thread_handle_thread_id(thread_handle_t *pthis)
  {
  return 0;
  }

result_t scheduler_init()
  {
  return s_ok;
  }

// this is the main thread loop.  We just block.
void neutron_run()
  {
  handle_t semp;
  semaphore_create(&semp);
  semaphore_wait(semp, INDEFINITE_WAIT);                // and block indefinately
  }

 result_t task_create(const char *name,
                           uint16_t stack_size,
                           task_callback pfn,
                           void *param,
                           uint8_t priority,
                           handle_t *task)
  {
  thread_handle_t *thread = (thread_handle_t *) malloc(sizeof(thread_handle_t));
  if(thread == 0)
    return e_not_enough_memory;

  create_thread_handle(thread, pfn, stack_size, param);

  task_resume(thread);

  *task = thread;
  return s_ok;
  }

result_t set_task_priority(handle_t h, uint8_t p)
  {
  thread_handle_set_priority(((thread_handle_t *)h), p);

  return s_ok;
  }

result_t get_task_priority(handle_t h, uint8_t *p)
  {
  *p = thread_handle_get_priority(((thread_handle_t *)h));

  return s_ok;
  }

result_t close_task(handle_t h)
  {
  destroy_thread_handle((thread_handle_t *)h);

  return s_ok;
  }

result_t task_suspend(handle_t h)
  {
  thread_handle_suspend(((thread_handle_t *)h));

  return s_ok;
  }

result_t task_resume(handle_t h)
  {
  thread_handle_resume(((thread_handle_t *)h));

  return s_ok;
  }

result_t task_sleep(unsigned long n)
  {
  usleep(n * 1000);

  return s_ok;
  }

result_t task_close(handle_t h)
  {
  destroy_thread_handle((thread_handle_t *)h);

  return s_ok;
  }

void thread_terminate(handle_t h, unsigned long term_code)
  {
  }

unsigned int thread_current_id(handle_t h)
  {
  return thread_handle_thread_id(((thread_handle_t *)h));
  }
