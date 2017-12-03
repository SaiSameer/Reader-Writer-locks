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
	llist *rprocs = NULL;
	for(; numlocks >0;numlocks--)
	{
		int i=0;
		lock = (int)*((&locks) + i);
		kprintf("The lock being released is %d\n",lock);
		rprocs = release(lock,currpid);
		i++;
	}
	while(rprocs != NULL){
		ready(rprocs->item,RESCHYES);
		rprocs = removelist(rprocs,rprocs->item);
	}
	resched();
	restore(ps);
	return OK;
}

/*------------------------------------------------------------------------
 * release  --  Release lock
 *------------------------------------------------------------------------
 */
llist* release(int lock, int pid)
{
	struct lentry *lptr;
	struct pentry *pptr;
	pptr = &proctab[pid];

	if (isbadlock(lock) || (lptr= &locktab[lock])->lstate==LFREE) {
		return(SYSERR);
	}

	// Remove lock from process' lock list
	pptr->lhead = removelist(pptr->lhead,lock);

	kprintf("Removed lock from process' llist\n");

	// Update current process priority
	int plprio = getmaxprio(pptr->lhead);
	pptr->pinh =0;
	if(ppriority(pid)<plprio)
	{
		pptr->pinh = plprio;
	}
	kprintf("updated currpid pinh\n");

	llist *plist = lptr->lhead;
	while(plist != NULL)
	{
		kprintf("item in llist %d \n",plist->item);
		plist = plist->lnext;
	}
	//Remove process from lock's process list
	lptr->lhead = removelist(lptr->lhead,pid);
	kprintf("Remove process form lock's process list\n");

	llist *readyprocs = NULL;
	if(lptr->lhead == NULL)
	{
		lptr->lstate = LFREE; //DEqueue nextproc //Update lockid to -1
		int nextproc = lq[lptr->lqhead].lqnext;
		int dummy = nextproc;
		while(lq[dummy].lqnext != EMPTY){
			kprintf("dummhy is %d \n",dummy);
			dummy = lq[dummy].lqnext;
		}
		if(nextproc != lptr->lqtail){
			int nexttype = lq[nextproc].lqtype;
			ldequeue(nextproc);
			kprintf("%d is going to acquire\n",nextproc);
			acqlock(nextproc,lock,nexttype);
			proctab[nextproc].lockid = EMPTY;
			readyprocs = addlist(nextproc,readyprocs);
			//ready(nextproc, RESCHYES);
			if(nexttype == READ){
				kprintf("Read process acquired lcok \n");
				while(nexttype == READ && lq[nextproc].lqnext != lptr->lqtail){
					nextproc = lq[nextproc].lqnext;
					nexttype = lq[nextproc].lqtype;
					if(nexttype == READ){
						ldequeue(nextproc);
						acqlock(nextproc,lock,nexttype);
						proctab[nextproc].lockid = EMPTY;
						readyprocs = addlist(nextproc,readyprocs);
						//ready(nextproc, RESCHYES);
					}
				}
			}
		}
	}

	kprintf("New processes aquired lock\n");
	struct lqent *ptr = &lq[lptr->lqhead];
	while(ptr->lqnext != EMPTY)
	{
		kprintf("The value in lq is %d\n",ptr->lqnext);
		ptr = &lq[ptr->lqnext];
	}
	//Update lprio of the lock
	lptr->lprio = updatelprio(lptr->lqhead);

	kprintf("Updated lprio\n");
	//Update pinh of the holding processes
	updatepinh(lptr->lhead, lptr->lprio);

	//Remember the process once acquired the lock
	lptr->remprocs = addlist(lptr->remprocs,pid);

	kprintf("updated pinh and lockid\n");
	return readyprocs;
}

