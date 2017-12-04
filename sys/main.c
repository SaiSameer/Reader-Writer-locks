#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
int mystrncmp(char* des, char* target, int n) {
	int i;
	for (i = 0; i < n; i++) {
		if (target[i] == '.')
			continue;
		if (des[i] != target[i])
			return 1;
	}
	return 0;
}



void reader1(char *msg, int lck) {
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock, sleep 2s\n", msg);
	sleep(4);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}
void reader4(char *msg, int lck) {
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock, sleep 2s\n", msg);
	sleep(2);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
	sleep(10);
	int x = lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("status %d \n", x);
}

void test5() {
	int lck;
	int pid1;
	int pid2;

	kprintf("\nTest 1: readers can share the rwlock\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 1 failed");

	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader4, 2000, 20, "reader b", 2, "reader b", lck);

	resume(pid1);
	resume(pid2);

	sleep(5);
	ldelete(lck);
	int i;
	for (i = 0; i < NLOCKS; i++) {
		lck = lcreate();
		kprintf("created %d\n", lck);
	}
	sleep(10);
	for (i = 0; i < NLOCKS; i++) {
		ldelete(i);
	}
	kprintf("Test 1 ok\n");
}

void test1() {
	int lck;
	int pid1;
	int pid2;

	kprintf("\nTest 1: readers can share the rwlock\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 1 failed");

	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);

	resume(pid1);
	resume(pid2);

	sleep(5);
	ldelete(lck);

	kprintf("Test 1 ok\n");
}



void writer7(char msg, int lck, int lprio) {
	kprintf("  %c: to acquire lock %d \n", msg,lck);
	lock(lck, WRITE, lprio);
	kprintf("%c acquired lock %d sleep 10\n",msg,lck);
	sleep(10);
	releaseall(1,lck);
}

/*--------------------------------Test 1--------------------------------*/

void reader7(char msg, int lck,int lck2) {
	kprintf("  %c: trying to acquire lock1 %d \n", msg,lck);
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("  %c: acquired lock1 %d, sleep 3s\n", msg,lck);
	sleep(3);
	kprintf("  %c: trying to acquire lock2 %d \n", msg,lck2);
	lock(lck2, WRITE, DEFAULT_LOCK_PRIO);
	kprintf("  %c: acquired lock2 %d \n", msg, lck2);
	releaseall(2,lck,lck2);
}



void test8() {
	int lck1,lck2;
	int pid1;
	int pid2;
	int pid3;

	kprintf("\nTest 7: readers can share the rwlock\n");
	lck1 = lcreate();
	lck2 = lcreate();
	assert(lck1 != SYSERR, "Test 7 failed");

	pid1 = create(reader7, 2000, 10, "reader a", 3, 'R', lck1,lck2);
	pid2 = create(writer7, 2000, 20, "writer2", 3, 'X', lck2, DEFAULT_LOCK_PRIO);
	pid3 = create(writer7, 2000, 30, "writer2", 3, 'Y', lck1, DEFAULT_LOCK_PRIO);

	resume(pid1);
	sleep(1);

	resume(pid2);

	sleep(3);
	resume(pid3);
	sleep(1);

	kprintf("priority of rd1 is %d \n",getprio(pid1));
	kprintf("priority of rd2 is %d \n",getprio(pid2));
	kprintf("priority of rd3 is %d \n",getprio(pid3));




	sleep(30);

	ldelete(lck1);
	ldelete(lck2);



	kprintf("Test 8 ok\n");
}

/*----------------------------------Test 2---------------------------*/
char output2[10];
int count2;
void reader2(char msg, int lck, int lprio) {
	int ret;

	kprintf("  %c: to acquire lock\n", msg);
	lock(lck, READ, lprio);
	output2[count2++] = msg;
	kprintf("  %c: acquired lock, sleep 3s\n", msg);
	sleep(3);
	output2[count2++] = msg;
	kprintf("  %c: to release lock\n", msg);
	releaseall(1, lck);
}

void writer2(char msg, int lck, int lprio) {
	kprintf("  %c: to acquire lock\n", msg);
	lock(lck, WRITE, lprio);
	output2[count2++] = msg;
	kprintf("  %c: acquired lock, sleep 3s\n", msg);
	sleep(3);
	output2[count2++] = msg;
	kprintf("  %c: to release lock\n", msg);
	releaseall(1, lck);
}



void test2() {
	count2 = 0;
	int lck;
	int rd1, rd2, rd3, rd4;
	int wr1;

	kprintf(
			"\nTest 2: wait on locks with priority. Expected order of"
					" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 2 failed");

	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 20);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 30);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'D', lck, 25);
	rd4 = create(reader2, 2000, 20, "reader2", 3, 'E', lck, 20);
	wr1 = create(writer2, 2000, 20, "writer2", 3, 'C', lck, 25);

	kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
	resume(rd1);
	sleep(1);

	kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
	resume(wr1);
	sleep(1);

	kprintf("-start reader B, D, E. reader B is granted lock.\n");
	resume(rd2);
	resume(rd3);
	resume(rd4);

	sleep(15);
	kprintf("output=%s\n", output2);
	assert(mystrncmp(output2, "ABABDDCCEE", 10) == 0, "Test 2 FAILED\n");
	kprintf("Test 2 OK\n");

}

/*----------------------------------Test 3---------------------------*/
void reader3(char *msg, int lck) {
	int ret;

	kprintf("  %s: to acquire lock\n", msg);
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock\n", msg);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}

void reader6(char *msg, int lck) {
	int ret;

	kprintf("  %s: to acquire lock\n", msg);
	lock(lck, READ, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock\n", msg);
	sleep(5);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}

void writer3(char *msg, int lck) {
	kprintf("  %s: to acquire lock\n", msg);
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock, sleep 10s\n", msg);
	sleep(10);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}

void writer6(char *msg, int lck) {
	kprintf("  %s: to acquire lock\n", msg);
	lock(lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf("  %s: acquired lock, sleep 10s\n", msg);
	sleep(1);
	kprintf("  %s: to release lock\n", msg);
	releaseall(1, lck);
}

void test3() {
	int lck;
	int rd1, rd2;
	int wr1;

	kprintf("\nTest 3: test the basic priority inheritence\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 3 failed");

	rd1 = create(reader3, 2000, 25, "reader3", 2, "reader A", lck);
	rd2 = create(reader3, 2000, 30, "reader3", 2, "reader B", lck);
	wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);

	kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
	resume(wr1);
	sleep(1);

	kprintf(
			"-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
	resume(rd1);
	sleep(1);
	assert(getprio(wr1) == 25, "Test 3 failed");

	kprintf(
			"-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
	resume(rd2);
	sleep(1);
	assert(getprio(wr1) == 30, "Test 3 failed");

	kprintf("-kill reader B, then sleep 1s\n");
	kill(rd2);
	sleep(1);
	assert(getprio(wr1) == 25, "Test 3 failed");

	kprintf("-kill reader A, then sleep 1s\n");
	kill(rd1);
	sleep(1);
	assert(getprio(wr1) == 20, "Test 3 failed");

	sleep(8);
	kprintf("Test 3 OK\n");
}



void test6() {
	int lck;
	int rd1, rd2;
	int wr1,wr2;

	kprintf("\nTest 6: test the basic priority inheritence\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 6 failed");

	rd1 = create(reader6, 2000, 20, "reader3", 2, "reader A", lck);
	wr1 = create(writer6, 2000, 30, "writer3", 2, "writer A", lck);
	wr2 = create(writer6, 2000, 25, "writer3", 2, "writer B", lck);

	kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
	resume(rd1);
	sleep(1);
	resume(wr1);
	resume(wr2);

	kprintf("priority of rd1 is %d \n",getprio(rd1));
	assert(getprio(rd1) == 30, "Test 6 failed");

	chprio(wr1,20);
	kprintf("priority of rd1 is %d \n",getprio(rd1));

	assert(getprio(rd1) == 25, "Test 6 failed");


	sleep(60);
	kprintf("Test 3 OK\n");
}




//test 1 second rule
void test4() {
	count2 = 0;
	int lck;
	int rd1, rd2, rd3;
	int wr1, wr2;

	kprintf(
			"\nTest 2: wait on locks with priority. Expected order of"
					" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 4 failed");

	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 25);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 25);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'C', lck, 20);
	wr1 = create(writer2, 2000, 20, "writer2", 3, 'X', lck, 25);
	//wr2 = create(writer2, 2000, 20, "writer2", 3, 'Y', lck, 25);
	wr2 = create(writer2, 2000, 20, "writer2", 3, 'Y', lck, 20);

	kprintf("process id %d %d %d %d \n", rd1, rd2, rd3, wr1);

	kprintf("start X A\n");
	resume(wr1);
	sleep(1);

	kprintf("Start B\n");
	resume(rd1);
	resume(rd2);
	resume(wr2);
	resume(rd3);

	sleep(60);
	kprintf("Waiting\n");
	output2[count2++] = 0;
	kprintf("output=%s\n", output2);
	assert(mystrncmp(output2, "XXABCCBAYY", 10) == 0, "Test 4 FAILED\n");
	kprintf("Test 4 OK\n");
}

//test 1 second rule
void test4_2() {
	count2 = 0;
	int lck;
	int rd1, rd2, rd3;
	int wr1, wr2;

	kprintf(
			"\nTest 2: wait on locks with priority. Expected order of"
					" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
	lck = lcreate();
	assert(lck != SYSERR, "Test 4 failed");

	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 25);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 25);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'C', lck, 20);
	wr1 = create(writer2, 2000, 20, "writer2", 3, 'X', lck, 25);
	wr2 = create(writer2, 2000, 20, "writer2", 3, 'Y', lck, 25);
	//wr2 = create(writer2, 2000, 20, "writer2", 3, 'Y', lck, 20);

	kprintf("process id %d %d %d %d \n", rd1, rd2, rd3, wr1);

	kprintf("start X A\n");
	resume(wr1);
	sleep(1);

	kprintf("Start B\n");
	resume(rd1);
	resume(rd2);
	resume(wr2);
	resume(rd3);

	sleep(60);
	kprintf("Waiting\n");
	output2[count2++] = 0;
	kprintf("output=%s\n", output2);
	assert(mystrncmp(output2, "XXYYABCCBA", 10) == 0, "Test 4 FAILED\n");
	kprintf("Test 4 OK\n");
}

int main() {
	/* These test cases are only used for test purpose.
	 * The provided results do not guarantee your correctness.
	 * You need to read the PA2 instruction carefully.
	 */
	//test1();
	//test2();
	//test3();
	//test4();
	//test4_2();
	test8();

	/* The hook to shutdown QEMU for process-like execution of XINU.
	 * This API call exists the QEMU process.
	 */
	shutdown();
}
