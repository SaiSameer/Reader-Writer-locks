/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int acqlock();

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
	if(locktab[ldesc1].lstate == LFREE)
	{
		acqlock(ldesc1, type);
		restore(ps);
		return OK;
	}
	else
	{
		if(locktab[ldesc1].ltype == READ && type == READ)
		{
			if(locktab[ldesc1].lprio < priority)
			{
				acqlock(ldesc1,type);
				restore(ps);
				return OK;
			}
		}
	}
	int waitret = lwait(ldesc1,type,priority);
	restore(ps);
	return waitret;
}

/*------------------------------------------------------------------------
 * acqlock  --  Acquire lock
 *------------------------------------------------------------------------
 */
LOCAL int acqlock(int lock, int type)
{
	struct lentry *lptr;
	struct pentry *pptr;
	lptr = &locktab[lock];
	pptr = &proctab[currpid];

	lptr->lstate = LUSED;
	lptr->ltype = type;

	pptr->lhead = addlist(lock, pptr->lhead);
	lptr->lhead = addlist(currpid, lptr->lhead);

	pptr->pinh = pptr->pprio > lptr->lprio ? 0 : lptr->lprio;
}

/*------------------------------------------------------------------------
 * addlist  --  Add to llist
 *------------------------------------------------------------------------
 */
llist* addlist(int item, llist* lhead)
{
	llist *plock = (llist *)getmem(sizeof(llist));
	plock->lnext = lhead != NULL ? lhead->lnext :NULL;
	plock->item = lock;
	return plock;
}
