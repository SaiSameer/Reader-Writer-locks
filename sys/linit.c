/* linit.c  -  linit */

#include <conf.h>
#include <kernel.h>
#include <lq.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * linit  --  initialize the locks
 *------------------------------------------------------------------------
 */
void linit()
{
	int i;
	struct lentry * lptr;
	for (i=0 ; i<NLOCKS ; i++) {	/* initialize locks */
		(lptr = &locktab[i])->lstate = LFREE;
		lptr->lprio = MININT;
		lptr->lhead = NULL;
		lptr->lqtail = 1 + (lptr->lqhead = newlqueue());
	}
}
