#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>

#include "sys/types.h"
#include "sys/sysinfo.h"


#define NUM_ITER 100000000
#define bigArraySize 100000

int bigArray[bigArraySize];
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

void *add(int myId) {
	int i, startPos, endPos;
	double localSum = 0.0, x = 0.0;
	double st = 1.0/((double) NUM_ITER);
	
	#pragma omp private(myId,x,i,localSum,startPos,endPos)
	{
		startPos = myId * (NUM_ITER / numThreads);
		endPos = startPos + (NUM_ITER / numThreads);
		printf("myId = %d startPos = %d endPos = %d \n", myId, startPos, endPos);
	
		for(i = startPos; i < endPos; i++)
		{
		  x = (i + 0.25)*st;
		  localSum += 4.0/(x*x+1);
		}
	}
	
	#pragma omp critical
	{
 //keep writing to variable one at a time
		sum += localSum;
	}
}

int main(int argc, char *argv[]) {
  struct timeval t1, t2;
	double elapsedTime;
 
	omp_set_num_threads(atoi(argv[1])); //sets number of threads
	
 	gettimeofday(&t1, NULL);
	#pragma omp parallel
	{
		add(omp_get_thread_num());
	}
  gettimeofday(&t2, NULL);
  
  printf("Finished with sum %f \n", sum);
  
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms
 
 	processMem_t myMem; 
	processMemory(&myMem);
 
	printf("DATA, %d, %s, %f, vMem %u KB, pMem %u KB\n", numThreads, getenv("SLURMD_NODENAME"),  elapsedTime, myMem.virtualMem, myMem.physicalMem);
	return 0;
}
