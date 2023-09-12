#ifndef __PROJECTS_PROJECT1_VEHICLE_H__
#define __PROJECTS_PROJECT1_VEHICLE_H__

#include "threads/thread.h"
#include "threads/synch.h"
#include "projects/crossroads/position.h"

#define VEHICLE_STATUS_READY 	0
#define VEHICLE_STATUS_RUNNING	1
#define VEHICLE_STATUS_FINISHED	2

struct vehicle_info {
	char id;
	char state;
	char start;
	char dest;
	struct position position;
	struct lock **map_locks;
	int required;
	bool released;
};

struct semaphore sema_crossroad;
struct semaphore sema_step;
int total_cnt;
int ready_cnt;

void vehicle_loop(void *vi);

#endif /* __PROJECTS_PROJECT1_VEHICLE_H__ */
