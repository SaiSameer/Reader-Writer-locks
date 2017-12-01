/* lwait.c - lwait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * lwait  --  make current process wait on a lock
 *------------------------------------------------------------------------
 */
SYSCALL	lwait(int lock, int type, int priority)
{
	STATWORD ps;
	struct	lentry	*lptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadlock(lock) || (lptr= &locktab[lock])->lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}

	(pptr = &proctab[currpid])->pstate = PRWAIT;
	pptr->lockid = lock;
	lenqueue(currpid,type,priority,lptr->lqhead); // TODO define lenqueue
	pptr->pwaitret = OK;
	resched();
	restore(ps);
	return pptr->pwaitret;
}
