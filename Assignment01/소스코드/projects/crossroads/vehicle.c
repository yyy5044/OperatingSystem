
#include <stdio.h>

#include "threads/thread.h"
#include "threads/synch.h"
#include "projects/crossroads/vehicle.h"
#include "projects/crossroads/map.h"
#include "projects/crossroads/ats.h"

int count_required(int start, int dest) {
	if (start == dest) {
		return 0;
	}

	int count = 0;
	int diff = ((start)-(dest));

	if (diff == (-1) || diff == 3) {
		count = 1;
	}
	if (diff == -2 || diff == 2) {
		count = 3;
	}
	if (diff == 1 || diff == (-3)) {
		count = 5;
	}

	return count;
}

/* path. A:0 B:1 C:2 D:3 */
const struct position vehicle_path[4][4][10] = {
	/* from A */ {
		/* to A */
		{{-1,-1},},
		/* to B */
		{{4,0},{4,1},{4,2},{5,2},{6,2},{-1,-1},},
		/* to C */
		{{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{-1,-1},},
		/* to D */
		{{4,0},{4,1},{4,2},{4,3},{4,4},{3,4},{2,4},{1,4},{0,4},{-1,-1}}
	},
	/* from B */ {
		/* to A */
		{{6,4},{5,4},{4,4},{3,4},{2,4},{2,3},{2,2},{2,1},{2,0},{-1,-1}},
		/* to B */
		{{-1,-1},},
		/* to C */
		{{6,4},{5,4},{4,4},{4,5},{4,6},{-1,-1},},
		/* to D */
		{{6,4},{5,4},{4,4},{3,4},{2,4},{1,4},{0,4},{-1,-1},}
	},
	/* from C */ {
		/* to A */
		{{2,6},{2,5},{2,4},{2,3},{2,2},{2,1},{2,0},{-1,-1},},
		/* to B */
		{{2,6},{2,5},{2,4},{2,3},{2,2},{3,2},{4,2},{5,2},{6,2},{-1,-1}},
		/* to C */
		{{-1,-1},},
		/* to D */
		{{2,6},{2,5},{2,4},{1,4},{0,4},{-1,-1},}
	},
	/* from D */ {
		/* to A */
		{{0,2},{1,2},{2,2},{2,1},{2,0},{-1,-1},},
		/* to B */
		{{0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{-1,-1},},
		/* to C */
		{{0,2},{1,2},{2,2},{3,2},{4,2},{4,3},{4,4},{4,5},{4,6},{-1,-1}},
		/* to D */
		{{-1,-1},}
	}
};

static int is_position_outside(struct position pos)
{
	return (pos.row == -1 || pos.col == -1);
}

/* return 0:termination, 1:success, -1:fail */
static int try_move(int start, int dest, int step, struct vehicle_info *vi)
{
	ready_cnt++;

	struct position pos_cur, pos_next;

	pos_next = vehicle_path[start][dest][step];
	pos_cur = vi->position;

	if (vi->state == VEHICLE_STATUS_RUNNING) {
		/* check termination */
		if (is_position_outside(pos_next)) {
			/* actual move */
			vi->position.row = vi->position.col = -1;
			/* release previous */
			lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
			return 0;
		}
	}

	if (step == 0) {
		/* lock next position */
		lock_acquire(&vi->map_locks[pos_next.row][pos_next.col]);
		if (vi->state == VEHICLE_STATUS_READY) {
			/* start this vehicle */
			vi->state = VEHICLE_STATUS_RUNNING;
		}
		/* update position */
		vi->position = pos_next;

		return 1;
	}
	if (step == 1) {
		lock_acquire(&vi->map_locks[pos_next.row][pos_next.col]);
		lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
		vi->position = pos_next;

		return 1;
	}
	if (step == 2) {
		sema_down(&sema_crossroad);
		
		vi->required = count_required(start, dest);
		for (int i = 0; i < vi->required; i++) {
			while (!lock_try_acquire(&vi->map_locks[vehicle_path[start][dest][step + i].row][vehicle_path[start][dest][step + i].col])) {
				continue; // busy waiting until lock acquired
			}
		}
	
		lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
		vi->position = pos_next;

		return 1;

	}
	if (step == 3) {
		vi->position = pos_next;
		sema_up(&sema_crossroad);
		return 1;
	}
	else {
		if ((pos_next.row >= 2 && pos_next.row <= 4) && (pos_next.col >= 2 && pos_next.col <= 4)) {
			vi->position = pos_next;
			return 1;
		}
		else {

			if (vi->released) {
				for (int i = 0; i < vi->required; i++) {
					lock_release(&vi->map_locks[vehicle_path[start][dest][2 + i].row][vehicle_path[start][dest][2 + i].col]);
				}
				lock_acquire(&vi->map_locks[pos_next.row][pos_next.col]);
				vi->position = pos_next;
				vi->released = false;
				return 1;
			}
			else {
				lock_release(&vi->map_locks[pos_cur.row][pos_cur.col]);
				lock_acquire(&vi->map_locks[pos_next.row][pos_next.col]);
				vi->position = pos_next;
				return 1;
			}
			
		}
	}

	
}

void init_on_mainthread(int thread_cnt){
	/* Called once before spawning threads */
	sema_init(&sema_crossroad, 3); // only 3 cars can exist in crossroad
	sema_init(&sema_step, 0);
	total_cnt = thread_cnt;
	ready_cnt = 0;
}

void vehicle_loop(void *_vi)
{
	int res;
	int start, dest, step;

	struct vehicle_info *vi = _vi;

	start = vi->start - 'A';
	dest = vi->dest - 'A';

	vi->position.row = vi->position.col = -1;
	vi->state = VEHICLE_STATUS_READY;

	vi->required = 0;
	vi->released = true;

	step = 0;
	while (1) {

		if (sema_try_down(&sema_step)) {
			
			res = try_move(start, dest, step, vi);
			if (res == 1) {
				step++;
				ready_cnt--;
				unitstep_changed();	
			}

			/* termination condition. */ 
			if (res == 0) {
				break;
			}

		}
		else {
			sema_init(&sema_step, (total_cnt - ready_cnt));
			crossroads_step++;
		}

	}	

	/* status transition must happen before sema_up */
	vi->state = VEHICLE_STATUS_FINISHED;
}
