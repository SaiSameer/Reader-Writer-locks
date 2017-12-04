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
	lptr->lprio =0;
	updatepinh(lptr->lhead,lptr->lprio);
	llist * list = lptr->lhead;
	while(list != NULL)
	{
		//proctab[list->item].pwaitret = DELETED;
		pid = list->item;
		proctab[pid].dlhead = addlist(lock,proctab[pid].dlhead);
		llist * freenode = list;
		list = list->lnext;
		freemem(freenode,sizeof(llist));
	}
	list = lptr->remprocs;
	while(list != NULL)
	{
		pid = list->item;
		//kprintf("Notify deletion to pid %d\n",pid);
		proctab[pid].dlhead = addlist(lock,proctab[pid].dlhead);
		llist * freenode = list;
		list = list->lnext;
		freemem(freenode,sizeof(llist));
	}
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
