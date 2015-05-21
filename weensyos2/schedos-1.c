#include "schedos-app.h"
#include "x86sync.h"

/*****************************************************************************
 * schedos-1
 *
 *   This tiny application prints red "1"s to the console.
 *   It yields the CPU to the kernel after each "1" using the sys_yield()
 *   system call.  This lets the kernel (schedos-kern.c) pick another
 *   application to run, if it wants.
 *
 *   The other schedos-* processes simply #include this file after defining
 *   PRINTCHAR appropriately.
 *
 *****************************************************************************/

#ifndef PRINTCHAR
#define PRINTCHAR	('1' | 0x0C00)
#endif


#ifndef PRIORITYCHECK
#define PRIORITYCHECK	2
#endif

void
start(void)
{
	int i;

	proc_priority(PRIORITYCHECK);
	sys_yield();

	for (i = 0; i < RUNCOUNT; i++) {
#ifdef USE_SYSTEM_SYNC
		
#else
		// use atomic_swap to get a lock
		while (atomic_swap(&lock, 1) !=0 ) {
			continue;
		}
		// Write characters to the console, yielding after each one.
		*cursorpos++ = PRINTCHAR;

		// use atomic_swap to release lock
		atomic_swap(&lock, 0);
#endif
		sys_yield();
	}

	// Yield forever.
	// while (1)
	// 	sys_yield();
	sys_exit(0);
}
