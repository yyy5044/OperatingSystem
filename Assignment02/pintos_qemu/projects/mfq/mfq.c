#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/interrupt.h"
#include "devices/timer.h"
#include "projects/mfq/mfq.h"

void test_loop(void *aux)
{
    int64_t start_time = timer_ticks();

    for(int i = 0; i < 10; i++)
    {
        struct thread *t = thread_current();
        printf("Thread %s is running at time %lld with priority %d and age %d\n", t->name, timer_ticks() - start_time, t->priority, t->age);
        thread_yield();
    }

    while (1) {
        timer_msleep(1000);
    }
    
}

void run_mfqtest(char **argv)
{   
    int cnt;
	char *token, *save_ptr;

    enum intr_level old_level;
    old_level = intr_disable ();

    /// TODO: make your own test
	cnt = 0;
	for (token = strtok_r (argv[1], ":", &save_ptr); token != NULL; 
		token = strtok_r (NULL, ":", &save_ptr)) {

        char *subtoken, *save_ptr2, *name; // 수정됨 name-> *name
        int priority;
        
        subtoken = strtok_r (token, ".", &save_ptr2);
        name = &subtoken[1];
        printf("thread name: %s\n", &subtoken[1]);
        subtoken = strtok_r (NULL, ".", &save_ptr2);
        priority = atoi(subtoken);
        printf("priority: %d\n", priority);

        // you can create threads here 
        thread_create(name, priority, test_loop, NULL);

		cnt++;
	}
    intr_set_level (old_level);
    
    while (1) {
        timer_msleep(1000);
    }
}