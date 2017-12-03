/* lqueue.c - ldequeue, lenqueue */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lq.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * enqueue  --	insert an item at the tail of a list
 *------------------------------------------------------------------------
 */
int lenqueue(int item, int type, int priority, int head)
/*	int	item;			- item to enqueue on a list	*/
/*	int	type;			- type of the item on a list*/
/*	int	priority;		- item priority on a list	*/
/*	int	head;			- index in q of list head	*/
{
	struct	lqent	*hptr;		/* points to head entry		*/
	struct	lqent	*mptr;		/* points to item entry		*/

	hptr = &lq[lq[head].lqnext];
	mptr = &lq[item];
	mptr->lqtype = type;
	mptr->lqkey = priority;
	while(hptr->lqnext != EMPTY)
	{
		if(hptr->lqkey >priority){
			hptr = &lq[hptr->lqnext];
		}
		else if(hptr->lqkey == priority){
			int ltype = hptr->lqtype;
			if(ltype == READ){
				break;
			}
			hptr = &lq[hptr->lqnext];
		}
		else{
			break;
		}
	}
	//mptr->lqprev = lq[hptr->lqnext].lqprev;
	mptr->lqprev = hptr->lqprev;
	mptr->lqnext = lq[hptr->lqprev].lqnext;
	lq[hptr->lqprev].lqnext = item;
	hptr->lqprev = item;
	//kprintf("%d\t%d\t%d\n",mptr->lqprev,mptr->lqnext, item);
	return(item);
}


/*------------------------------------------------------------------------
 *  ldequeue  --  remove an item from the head of a list and return it
 *------------------------------------------------------------------------
 */
int ldequeue(int item)
{
	struct	lqent	*mptr;		/* pointer to lq entry for item	*/

	mptr = &lq[item];
	lq[mptr->lqprev].lqnext = mptr->lqnext;
	lq[mptr->lqnext].lqprev = mptr->lqprev;
	return(item);
}

/*------------------------------------------------------------------------
 * getfirstl  --	 remove and return the first process on a list
 *------------------------------------------------------------------------
 */
int getfirstl(int head)
{
	int	proc;			/* first process on the list	*/

	if ((proc=lq[head].lqnext) < NPROC)
		return( ldequeue(proc) );
	else
		return(EMPTY);
}



/*------------------------------------------------------------------------
 * getlastl  --  remove and return the last process from a list
 *------------------------------------------------------------------------
 */
int getlastl(int tail)
{
	int	proc;			/* last process on the list	*/

	if ((proc=lq[tail].lqprev) < NPROC)
		return( ldequeue(proc) );
	else
		return(EMPTY);
}


/*------------------------------------------------------------------------
 * updatewpinh  --  Update pinhs of the waiting processes
 *------------------------------------------------------------------------
 */
void updatewpinh( int lock, int priority)
{
	struct lqent *ptr;
	ptr = &lq[locktab[lock].lqhead];
	kprintf("head is %d and tail is %d\n", locktab[lock].lqhead, locktab[lock].lqtail);
	while(ptr->lqnext != locktab[lock].lqtail)
	{
		struct pentry *pptr = &proctab[ptr->lqnext];
		int maxlprio = getmaxprio(pptr->lhead);
		pptr->pinh = pptr->pprio > maxlprio ? 0 : maxlprio;
		//kprintf("updated priority of pid %d is %d\n",ptr->lqnext,ppriority(ptr->lqnext));
		chprioupdates(ptr->lqnext);
		ptr = &lq[ptr->lqnext];
	}
}

/*------------------------------------------------------------------------
 * updatelprio  --  Update lprio of the lock
 *------------------------------------------------------------------------
 */
int updatelprio(int lqhead)
{
	int priority = -1;
	struct lqent *ptr;
	ptr = &lq[lqhead];
	kprintf("lqnext is %d \n", ptr->lqnext);
	while(ptr->lqnext != EMPTY)
	{
		int pprio = ppriority(ptr->lqnext);
		if(pprio > priority)
		{
			priority = pprio;
		}
		ptr = &lq[ptr->lqnext];
	}
	return priority;
}
