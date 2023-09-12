#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 0                  /* Default priority. */
#define PRI_MAX 3                      /* Highest priority. */

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    int age; // Aging 기법을 적용시키기 위해 추가
   
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
#endif

    /* For timer_sleep() */
    int64_t wakeup_tick;

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void); // 스레드 시스템을 초기화합니다. 특히 메인 스레드를 초기화합니다.
void thread_start (void); // 모든 스레드가 준비되면 이 함수를 호출하여 스케줄러를 시작합니다.

void thread_tick (void); // 각 타이머 인터럽트에 대해 호출되며, 현재 실행 중인 스레드의 정보를 업데이트합니다.
void thread_print_stats (void); // 스레드 통계를 출력합니다.

typedef void thread_func (void *aux); 
tid_t thread_create (const char *name, int priority, thread_func *, void *); // 새로운 스레드를 생성하고 시작합니다.

void thread_block (void); // 현재 스레드를 블록 상태로 만듭니다.
void thread_unblock (struct thread *); // 특정 스레드를 블록 상태에서 벗어나게 합니다.

int64_t get_next_tick_to_wakeup (void); 
void thread_sleep (int64_t);
void thread_wakeup (int64_t);

struct thread *thread_current (void); // 현재 실행중인 스레드를 반환합니다.
tid_t thread_tid (void); // 현재 스레드의 스레드ID를 반환합니다.
const char *thread_name (void); // 현재 스레드의 이름을 반환합니다.

void thread_exit (void) NO_RETURN; // 현재 스레드를 종료합니다.
void thread_yield (void); // 현재 스레드가 실행을 양보하게 만듭니다. 다른 스레드가 실행될 수 있습니다.

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *); // 모든 스레드에 대해 특정 작업을 수행합니다.

int thread_get_priority (void); // 현재 스레드의 우선순위를 가져옵니다.
void thread_set_priority (int); // 현재 스레드의 우선순위를 설정합니다.

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

#endif /* threads/thread.h */