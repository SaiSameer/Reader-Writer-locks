/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lq.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}

	kprintf("THe proces being killed is %d\n",pid);

	llist *pllist = pptr->lhead;
	llist *readyprocs = NULL;
	while(pllist != NULL)
	{
		llist *rprocs = release(pllist->item,pid);
		locktab[pllist->item].remprocs = removelist(locktab[pllist->item].remprocs, pid);
		while(rprocs != NULL){
			//ready(rprocs->item,RESCHYES);
			readyprocs = addlist(readyprocs,rprocs->item);
			rprocs = removelist(rprocs,rprocs->item);
		}
	}
	if(pptr->lockid != -1)
	{
		ldequeue(pid);
		locktab[pptr->lockid].lprio =0;
		//updatepinh(locktab[pptr->lockid].lqhead,0);
		struct lqent *ptr = &lq[locktab[pptr->lockid].lqhead];
		while(ptr->lqnext != EMPTY)
		{
			kprintf("The value in lq is %d\n",ptr->lqnext);
			ptr = &lq[ptr->lqnext];
		}
		kprintf("The lqhead is %d and lqtail is %d\n",locktab[pptr->lockid].lqhead,locktab[pptr->lockid].lqtail);
		updatewpinh(pptr->lockid,0);
		chprioupdates(pid);
	}
	while(readyprocs != NULL)
	{
		ready(readyprocs->item,RESCHYES);
		readyprocs = removelist(readyprocs,readyprocs->item);
	}
	/*removeprocess(pptr->lhead,pid);
	if(pptr->lockid != -1)
	{
		pptr->lockid = -1;
		ldequeue(pid);
		struct lentry *lptr;
		lptr = &locktab[pptr->lockid];
		if(lptr->lprio <= ppriority(pid)){
			lptr->lprio = updatelprio(lptr->lhead);
			updatepinh(lptr->lhead,lptr->lprio);
		}
	}*/

	restore(ps);
	return(OK);
}
