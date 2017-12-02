/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lq.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lock)
{
	STATWORD ps;
	int	pid;
	struct lentry	*lptr;

	disable(ps);
	if (isbadlock(lock) || locktab[lock].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	lptr = &locktab[lock];
	lptr->lstate = LFREE;
	if (nonemptylq(lptr->lqhead)) {
		while( (pid=getfirstl(lptr->lqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    proctab[pid].dlhead = addlist(lock, proctab[pid].dlhead);
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	return(OK);
}
