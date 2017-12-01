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
	struct lentry *lptr;
	struct pentry *pptr;
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
LOCAL int acqlock(int ldesc1, int type)
{
	struct lentry *lptr = &locktab[ldesc1];
	lptr->lstate = LUSED;
	lptr->ltype = type;
	//priority = lptr->lprio > priority ? lptr->lprio : priority;
	addlist(currpid,lptr->phead);
	pptr->pinh = (pptr=&proctab[currpid])->pprio > lptr->lprio ? 0 : lptr->lprio; //TODO
}
