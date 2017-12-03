/* lock.c - lock, acqlock, addlist */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lq.h>
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
	llist *list = proctab[currpid].dlhead;
	while(list != NULL)
	{
		kprintf("Item in dllist is %d\n",list->item);
		list = list->lnext;
	}
	if(checkdllist(ldesc1,currpid) == SYSERR)
	{
		restore(ps);
		return SYSERR;
	}
	if(locktab[ldesc1].lstate == LUSED && locktab[ldesc1].lhead== NULL)
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
			kprintf("The highest wait priority is %d with head at %d\n",lq[nextproc].lqkey, locktab[ldesc1].lqhead);
			if(lq[nextproc].lqkey <= priority )
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
	kprintf("lptr->lhead in acqlock is  %d\n", lptr->lhead);
	lptr->lhead = addlist(pid, lptr->lhead);
	kprintf("lptr->lhead item in acqlock is  %d\n", lptr->lhead->item);

	if(ppriority(pid) < lptr->lprio)
		pptr->pinh = lptr->lprio;

	lptr->remprocs = removelist(lptr->remprocs,pid);
	return OK;
}

/*------------------------------------------------------------------------
 * addlist  --  Add to llist
 *------------------------------------------------------------------------
 */
llist* addlist(int item, llist* lhead)
{
	llist *plock = (llist *)getmem(sizeof(llist));
	plock->item = item;
	plock->lnext = lhead;
	return plock;
}

/*------------------------------------------------------------------------
 * removelist  --  Remove item from llist
 *------------------------------------------------------------------------
 */
llist* removelist( llist* lhead,int item)
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
			freemem(q,sizeof(llist));
			break;
		}
		p=q;
		q = q->lnext;
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
	kprintf("Current pid is %d and lock in dllist is %d\n", pid, lock);
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

int updatelprio(llist* lhead)
{
	llist *list = lhead;
	int priority = -1;
	while(list != NULL)
	{
		int pprio = ppriority(list->item);
		priority = pprio > priority ? pprio : priority;
		list = list->lnext;
	}
	return priority;
}*/

/*
------------------------------------------------------------------------
 * updatepinhl  --  Update pinh and lockid of the holding processes
 *------------------------------------------------------------------------

void updatepinhl(llist* lhead, int priority)
{
	llist *list = lhead;
	while(list != NULL)
	{
		int pprio = ppriority(list->item);
		if(priority > pprio)
		{
			proctab[list->item].pinh = priority;
 		}
		proctab[list->item].lockid = EMPTY;
		list = list->lnext;
	}
}
*/

/*
------------------------------------------------------------------------
 * removeprocess  --  Remove process from all locks' holding lists
 *------------------------------------------------------------------------

void removeprocess(llist* lhead, int pid)
{
	llist *list = lhead;
	while(list != NULL)
	{
		llist *p = locktab[list->item].lhead;
		if(p != NULL){
			if(p->item != pid)
			{
				llist *q = p->lnext;
				while(q != NULL)
				{
					if(q->item == pid)
					{
						p->lnext = q->lnext;
						freemem(q,sizeof(llist));
						break;
					}
					p = q;
					q = q->lnext;
				}
			}
			else{
				locktab[list->item].lhead = p->lnext;
				if(p->lnext == NULL){
					locktab[list->item].lstate = LFREE;
				}
				freemem(p,sizeof(llist));
			}
		}
		list = list->lnext;
	}
}
*/


/*------------------------------------------------------------------------
 * updatepinh  --  Update pinh of the holding processes
 *------------------------------------------------------------------------

void updatepinh(llist* lhead, int priority)
{
	llist *list = lhead;
	while(list != NULL)
	{
		//int pprio = ppriority(list->item);
		int pprio = proctab[list->item].pprio;
		if(priority > pprio)
		{
			struct pentry *pptr = &proctab[list->item];
			pptr->pinh = priority;
			if(pptr->lockid != -1)
			{
				pptr->lockid = -1;
				ldequeue(list->item);
				struct lentry *lptr;
				lptr = &locktab[pptr->lockid];
				if(lptr->lprio <= ppriority(list->item)){
					lptr->lprio = updatelprio(lptr->lhead);
					updatepinh(lptr->lhead,lptr->lprio);
				}
			}
 		}
		list = list->lnext;
	}
}*/

/*------------------------------------------------------------------------
 * chprioupdates  --  Update pinhs due to chprio
 *------------------------------------------------------------------------
 */
void chprioupdates(int pid)
{
	kprintf("Chprio to pid %d\n",pid);
	struct pentry *pptr;
	pptr =&proctab[pid];
	if(pptr->lockid != -1)
	{
		//Update lprio of the waiting lock
		int lprio = locktab[pptr->lockid].lprio;
		kprintf("THe lprio prior is %d\n", lprio);
		locktab[pptr->lockid].lprio = updatelprio(locktab[pptr->lockid].lqhead);
		kprintf("THe lprio after is %d\n", locktab[pptr->lockid].lprio);

		if(locktab[pptr->lockid].lprio != lprio || locktab[pptr->lockid].lprio==0){
			//Update pinh of all dependent processes
			updatepinh(locktab[pptr->lockid].lhead,pptr->lockid);
		}
	}
}

/*------------------------------------------------------------------------
 * updatepinh  --  Update pinhs for the given lprio
 *------------------------------------------------------------------------
 */
void updatepinh(llist * lhead, int priority)
{
	llist *p = lhead;
	while(p != NULL)
	{
		struct pentry *pptr = &proctab[p->item];
		int maxlprio = getmaxprio(pptr->lhead);
		pptr->pinh = pptr->pprio > maxlprio ? 0 : maxlprio;
		kprintf("updated priority of pid %d is %d\n",p->item,ppriority(p->item));
		chprioupdates(p->item);
		p = p->lnext;
	}
}

/*------------------------------------------------------------------------
 * getnextproc  --  Get next process in wait queue with req type
 *------------------------------------------------------------------------
 */
int getnextproc(int pid, int type)
{
	int nextproc = pid;
	while(lq[nextproc].lqkey == lq[lq[nextproc].lqnext].lqkey){
		int nextp = lq[nextproc].lqnext;
		kprintf("nexproc is %d and nextp is %d\n", nextproc, nextp);
		if(lq[nextproc].ltime - lq[nextp].ltime < 1000)
		{
			if(lq[nextp].lqtype == type){
				pid = nextp;
				break;
			}
		}
		nextproc = nextp;
	}
	kprintf("pid returned is %d\n", pid);
	return pid;
}

/*------------------------------------------------------------------------
 * searchlock  --  Search if a lock is being held by a process
 *------------------------------------------------------------------------
 */
int searchlock(int lock, int pid)
{
	llist * list = proctab[pid].lhead;
	while(list != NULL){
		if(list->item == lock){
			return OK;
		}
	}
	return SYSERR;
}

