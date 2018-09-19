#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>

#include "sys/types.h"
#include "sys/sysinfo.h"


#define NUM_ITER 100000000
#define bigArraySize 100000

pthread_mutex_t mutexsum;

int bigArray[bigArraySize];
int numThreads;
double sum;

typedef struct {
	uint32_t virtualMem;
	uint32_t physicalMem;
} processMem_t;

int parseLine(char *line) {
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}

void processMemory(processMem_t* processMem) {
	FILE *file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		//printf("%s", line);
		if (strncmp(line, "VmSize:", 7) == 0) {
			processMem->virtualMem = parseLine(line);
		}

		if (strncmp(line, "VmRSS:", 6) == 0) {
			processMem->physicalMem = parseLine(line);
		}
	}
	fclose(file);
}

void *add(void *myId) {
	int i;
	double localSum = 0.0, x = 0.0;
	double st = 1.0/((double) NUM_ITER);
	
	int startPos = ((intptr_t) myId) * (NUM_ITER / numThreads);
	int endPos = startPos + (NUM_ITER / numThreads);
	printf("myId = %d startPos = %d endPos = %d \n", myId, startPos, endPos);

	for(i = startPos; i < endPos; i++)
	{
	  x = (i + 0.25)*st;
	  localSum += 4.0/(x*x+1);
	}
	
	pthread_mutex_lock (&mutexsum);
	{
		sum += localSum;
	}
	pthread_mutex_unlock (&mutexsum);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int i, rc;
	struct timeval t1, t2;
	double elapsedTime;
  void *status;
 
	pthread_t threads[atoi(argv[1])];
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_mutex_init(&mutexsum, NULL);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
 	gettimeofday(&t1, NULL);
	for(i=0; i<numThreads; i++)
	{
		rc = pthread_create(&threads[i], &attr, add, (void *)(intptr_t)i);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(- 1);
		}
	}
		
  	pthread_attr_destroy(&attr);
  	for(i=0; i<numThreads; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}
  gettimeofday(&t2, NULL);
  printf("Finished with sum %f \n", sum);
  
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
 
 	processMem_t myMem; 
	processMemory(&myMem);
 
	pthread_mutex_destroy(&mutexsum);
	printf("DATA, %d, %s, %f, vMem %u KB, pMem %u KB\n", numThreads, getenv("SLURMD_NODENAME"),  elapsedTime, myMem.virtualMem, myMem.physicalMem);
	pthread_exit(NULL);
	return 0;
}
