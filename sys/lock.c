/* lock.c - lock, acqlock, addlist */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>


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
	if (isbadlock(ldesc1)) {
		return(SYSERR);
	}
	if(checkdllist(lock,currpid) == SYSERR)
	{
		restore(ps);
		return SYSERR;
	}
	if(locktab[ldesc1].lstate == LFREE)
	{
		acqlock(currpid,ldesc1, type);
		restore(ps);
		return OK;
	}
	else
	{
		if(locktab[ldesc1].ltype == READ && type == READ)
		{
			int nextproc = lq[locktab[ldesc1].lqhead].lqnext;
			if(lq[nextproc].lqkey <= priority)
			{
				acqlock(currpid,ldesc1,type);
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
int acqlock(int pid, int lock, int type)
{
	struct lentry *lptr;
	struct pentry *pptr;
	lptr = &locktab[lock];
	pptr = &proctab[pid];

	lptr->lstate = LUSED;
	lptr->ltype = type;

	pptr->lhead = addlist(lock, pptr->lhead);
	lptr->lhead = addlist(pid, lptr->lhead);

	if(ppriority(pid) < lptr->lprio)
		pptr->pinh = lptr->lprio;
	return OK;
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

/*------------------------------------------------------------------------
 * removelist  --  Remove item from llist
 *------------------------------------------------------------------------
 */
llist* removelist(int item, llist* lhead)
{
	llist * p = lhead;
	if(p->item == item)
	{
		return p->lnext;
	}
	llist *q = p->lnext;
	while(q != NULL)
	{
		if(q->item == item)
		{
			p->lnext = q->lnext;
			break;
		}
	}

	return lhead;
}

/*------------------------------------------------------------------------
 * checkdllist  --  Check if the lock is already deleted
 *------------------------------------------------------------------------
 */
int checkdllist(int lock, int pid)
{
	struct pentry *pptr;
	pptr = &proctab[pid];
	llist * list = pptr->dlhead;
	while(list != NULL)
	{
		if(list->item == lock)
		{
			return SYSERR;
		}
		list = list->lnext;
	}
	return OK;
}

/*------------------------------------------------------------------------
 * ppriority  --  Get priority of a process
 *------------------------------------------------------------------------
 */
int ppriority(int pid)
{
	return proctab[pid].pinh > 0 ? proctab[pid].pinh:proctab[pid].pprio;
}

/*------------------------------------------------------------------------
 * getmaxprio  --  Maximum lprio of all locks the process holds
 *------------------------------------------------------------------------
 */
int getmaxprio(llist* lhead)
{
	llist * list = lhead;
	int priority = -1;
	while(list != NULL)
	{
		if(locktab[list->item].lprio >priority)
		{
			priority = locktab[list->item].lprio;
		}
		list = list->lnext;
	}
	return priority;
}

/*------------------------------------------------------------------------
 * updatelprio  --  Update lprio of the lock
 *------------------------------------------------------------------------
 */
int updatelprio(llist* lhead)
{
	llist *list = lhead;
	int priority = -1;
	while(list != NULL)
	{
		int pprio = ppriority[list->item];
		priority = pprio > priority ? pprio : priority;
		list = list->lnext;
	}
	return priority;
}

/*------------------------------------------------------------------------
 * updatepinh  --  Update pinh of the holding processes
 *------------------------------------------------------------------------
 */
void updatepinh(llist* lhead, int priority)
{
	llist *list = lhead;
	while(list != NULL)
	{
		int pprio = ppriority[list->item];
		if(priority > pprio)
		{
			proctab[list->item].pinh = priority;
 		}
		proctab[list->item].lockid = EMPTY;
		list = list->lnext;
	}
}
