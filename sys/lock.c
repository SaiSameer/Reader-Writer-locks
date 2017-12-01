/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int freelock();

/*------------------------------------------------------------------------
 * lock  --  Acquire a lock for read/write
 *------------------------------------------------------------------------
 */
SYSCALL lock(int ldesc1, int type, int priority)
{
	STATWORD ps;
	int	lock;

	disable(ps);
	int	i;
	struct lentry lptr;
	if(locktab[ldesc1].lstate == LFREE)
	{
		lptr->lstate = LUSED;
		lptr->ltype = type;
	}
	else
	{
		if(locktab[ldesc1].ltype == READ)
		{

		}
	}

	restore(ps);
	return(lock);
}

/*------------------------------------------------------------------------
 * freelock  --  check if the lock is free
 *------------------------------------------------------------------------
 */
LOCAL int freelock(int lock)
{
	int	i;
	struct lentry lptr;
	if(getfirst((lptr=&locktab[lock])->lqhead) != EMPTY)
	{

	}
	else{
		lptr->lstate = LUSED;
		lptr->ltype = type;
	}

	for (i=0 ; i<NLOCKS ; i++) {
		lock=nextlock--;
		if (nextlock < 0)
			nextlock = NLOCKS-1;
		if (locktab[lock].lstate==LFREE) {
			locktab[lock].lstate = LUSED;
			return(lock);
		}
	}
	return(SYSERR);
}
