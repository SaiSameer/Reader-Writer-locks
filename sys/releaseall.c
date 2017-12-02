/* releaseall.c - releaseall */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lq.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * releaseall  --  Release multiple locks at once
 *------------------------------------------------------------------------
 */
SYSCALL releaseall(int numlocks, long locks, ...)
{
	STATWORD ps;
	int	lock;
	disable(ps);
	for(; numlocks >0;numlocks--)
	{
		int i=0;
		lock = (int*)((&locks) + i);
		if(release(lock,currpid) == SYSERR){
			return SYSERR;
		}
		i++;
	}
	resched();
	restore(ps);
	return OK;
}

/*------------------------------------------------------------------------
 * release  --  Release lock
 *------------------------------------------------------------------------
 */
int release(int lock, int pid)
{
	struct lentry *lptr;
	struct pentry *pptr;
	pptr = &proctab[pid];

	if (isbadlock(lock) || (lptr= &locktab[lock])->lstate==LFREE) {
		return(SYSERR);
	}

	// Remove lock from process' lock list
	pptr->lhead = removelist(pptr->lhead,lock);

	// Update current process priority
	int plprio = getmaxprio(pptr->lhead);
	pptr->pinh =0;
	if(ppriority(pid)<plprio)
	{
		pptr->pinh = plprio;
	}

	//Remove process from lock's process list
	lptr->lhead = removelist(lptr->lhead,pid);

	if(lptr->lhead == NULL)
	{
		lptr->lstate = LFREE; //DEqueue nextproc //Update lockid to -1
		int nextproc = lq[lptr->lqhead].lqnext;
		int nexttype = lq[nextproc].lqtype;
		ldequeue(nextproc);
		acqlock(nextproc,lock,nexttype);
		proctab[nextproc].lockid = EMPTY;
		ready(nextproc, RESCHYES);
		if(nexttype == READ){
			while(nexttype == READ){
				nextproc = lq[nextproc].lqnext;
				nexttype = lq[nextproc].lqtype;
				if(nexttype == READ){
					ldequeue(nextproc);
					acqlock(nextproc,lock,nexttype);
					proctab[nextproc].lockid = EMPTY;
					ready(nextproc, RESCHYES);
				}
			}
		}
	}

	//Update lprio of the lock
	lptr->lprio = updatelprio(lptr->lhead);

	//Update pinh and lockid of the holding processes
	updatepinhl(lptr->lhead, lptr->lprio);

	return OK;
}

