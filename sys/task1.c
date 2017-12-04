#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }

/*----------------------------------Task 1---------------------------*/
void writerl (char *msg, int lck)
{
	kprintf ("  %s: to acquire lock %d\n", msg, lck);
	lock (lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock %d, sleep 5s\n", msg, lck);
	sleep (3);
	kprintf("started logic\n");
	long i=0;
	for(i=0;i < 1000000; i++)
	{
		long j =0;
		for(j=0;j < 1500; j++);
	}
	kprintf ("  %s: to release lock %d\n", msg, lck);
	kprintf("Process %s is completed\n",msg);
	releaseall (1, lck);
}

void writers (char *msg, int sem)
{
	kprintf ("  %s: to acquire semaphore\n", msg);
	wait (sem);
	kprintf ("  %s: acquired semaphore\n", msg);
	sleep (3);
	long i=0;
	for(i=0;i < 1000000; i++)
	{
		long j =0;
		for(j=0;j < 1500;j++);
	}
	kprintf ("  %s: to release semaphore\n", msg);
	kprintf("Process %s is completed\n",msg);
	signal (sem);
	sleep(10);
}

void normalproc(char *msg)
{
	sleep(1);
	kprintf("Executing Medium priority %s\n",msg);
	long i=0;
	for(i=0;i < 1000000; i++)
	{
		long j =0;
		for(j=0;j < 1500; j++);
	}
	kprintf("Process %s is completed\n",msg);
}

void task1 ()
{
	int     lck,sem;
	int     wr1,wr2,np;

	kprintf("\nTask 1: Test the priority inheritence\n");
	lck  = lcreate ();
	assert (lck != SYSERR, "Task 1 failed");

	kprintf("Readers/writer implementation\n");

	wr1 = create(writerl, 2000, 10, "writer", 2, "writer A", lck);
	wr2 = create(writerl, 2000, 30, "writer", 3, "writer B", lck);
	np = create(normalproc, 2000, 20, "Process", 1, "process E");

	kprintf("-start writer A, then sleep 1s. lock granted to A (prio 10)\n");
	resume(wr1);
	sleep (1);

	kprintf("-start writer B, then sleep 1s. writer B(prio 30) blocked\n");
	resume(wr2);
	sleep (1);

	kprintf("-start normal process E(prio 20)\n");
	resume(np);

	sleep (30);

	kprintf("Semaphore implementation\n");

	sem = screate(1);
	wr1 = create(writers, 2000, 10, "writer", 2, "writer A", sem);
	wr2 = create(writers, 2000, 30, "writer", 3, "writer B", sem);
	np = create(normalproc, 2000, 20, "Process",1, "process E");

	kprintf("-start writer A, then sleep 1s. semaphore granted to A (prio 10)\n");
	resume(wr1);
	sleep (1);

	kprintf("-start writer B, then sleep 1s. writer B(prio 30) blocked on the semaphore\n");
	resume(wr2);
	sleep (1);

	kprintf("-start normal process E(prio 20)\n");
	resume(np);

	sleep(30);
	kprintf ("Task 1 OK\n");
}
