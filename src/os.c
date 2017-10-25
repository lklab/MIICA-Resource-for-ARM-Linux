#include "os.h"

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000ULL

static struct timespec read_time;
static struct timespec release_time;

static os_func_t registered_handler = NULL;

static void sigint_handler(int sig);
static void timespec_chk_overflow(struct timespec *ts);

int os_task_init(task_t* task, os_proc_t proc, unsigned long long period)
{
	task -> proc = proc;
	task -> period = period;
	task -> alive = 1;

	return 0;
}

int os_task_start(task_t* task)
{
	clock_gettime(CLOCK_REALTIME, &read_time);

	while(task -> alive)
	{
		task -> proc();

		read_time.tv_nsec += task -> period;
		timespec_chk_overflow(&read_time);
		release_time.tv_sec = read_time.tv_sec;
		release_time.tv_nsec = read_time.tv_nsec;
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &release_time, NULL);
	}

	return 0;
}

int os_task_stop(task_t* task)
{
	task -> alive = 0;
	return 0;
}

int os_register_interrupt_handler(os_func_t handler)
{
	registered_handler = handler;
	signal(SIGINT, sigint_handler);
	return 0;
}

void os_exit_process(int value)
{
	exit(value);
}

void* os_memcpy(void* s1, const void* s2, unsigned int n)
{
	return memcpy(s1, s2, (size_t)n);
}

void* os_malloc(unsigned int size)
{
	return malloc((size_t)size);
}

void os_free(void *ptr)
{
	return free(ptr);
}

static void sigint_handler(int sig)
{
	if(registered_handler != NULL)
		registered_handler();
}

static void timespec_chk_overflow(struct timespec *ts)
{
	while (ts -> tv_nsec >= NSEC_PER_SEC)
	{
		ts -> tv_nsec -= NSEC_PER_SEC;
		ts -> tv_sec++;
	}
}
