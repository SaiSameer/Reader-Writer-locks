/* lq.h - firstid, firstkey, isempty, lastkey, nonempty */

#ifndef _LQUEUE_H_
#define _LQUEUE_H_

/* lq structure declarations, constants, and inline procedures		*/

#ifndef	NLQENT
#define	NLQENT		NPROC + NSEM + NSEM	/* for ready & sleep	*/
#endif

struct	lqent	{		/* one for each process plus two for	*/
				/* each list				*/
	int	lqkey;		/* key on which the queue is ordered	*/
	int	lqnext;		/* pointer to next process or tail	*/
	int	lqprev;		/* pointer to previous process or head	*/
	int lqtype;
	long ltime;
};

extern	struct	lqent lq[];
extern	int	nextlqueue;

/* inline list manipulation procedures */

#define	isemptylq(list)	(lq[(list)].lqnext >= NPROC)
#define	nonemptylq(list)	(lq[(list)].lqnext < NPROC)
#define	firstlkey(list)	(lq[lq[(list)].lqnext].lqkey)
#define lastlkey(tail)	(lq[lq[(tail)].lqprev].lqkey)
#define firstlid(list)	(lq[(list)].lqnext)


/* ANSI compliant function prototypes */

int lenqueue(int item, int type, int priority, int head);
int ldequeue(int item);
int newlqueue();
int getfirstl(int head);
int getlastl(int tail);
void updatewpinh( int lqhead, int priority);
int updatelprio(int lqhead);

#endif
