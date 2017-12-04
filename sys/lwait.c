/* lwait.c - lwait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lq.h>
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
	int pprio = ppriority(currpid);
	lenqueue(currpid,type,priority,lptr->lqhead);
	if(lptr->lprio < pprio){
		lptr->lprio = pprio;
		llist * list = lptr->lhead;
		kprintf("lptr->head item is %d\n", lptr->lhead->item);
		while(list != NULL)
		{
			int lprio = ppriority(list->item);
			if(lprio < lptr->lprio)
			{
				proctab[list->item].pinh = lptr->lprio;
			}
			chprioupdates(list->item);
			list= list->lnext;
		}
	}
	pptr->pwaitret = OK;
	resched();
	restore(ps);
	return pptr->pwaitret;
}
