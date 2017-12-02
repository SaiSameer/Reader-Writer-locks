/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	if(pptr->pinh != 0)
	{
		pptr->pinh = pptr->pinh > pptr->pprio ? pptr->pinh : pptr->pprio;
	}
	if(pptr->lockid != -1)
	{
		struct lentry *lptr;
		lptr = &locktab[pptr->lockid];
		lptr->lprio = updatelprio(lptr->lhead);
		updatepinh(lptr->lhead,lptr->lprio);
	}

	restore(ps);
	return(newprio);
}
