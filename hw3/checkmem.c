/* Dan Andresen based on http://blog.mbedded.ninja/programming/operating-systems/linux/monitor-linux-process-memory-usage-using-c-cpp */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "sys/types.h"
#include "sys/sysinfo.h"
#include "checkmem.h"

processMem_t mem;
int hasMem = 0;

int parseLine(char *line) {
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}

void GetProcessMemory(processMem_t* processMem) {
  if (hasMem == 1)
    return;
	FILE *file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmSize:", 7) == 0) {
			processMem->virtualMem = parseLine(line);
		}

		if (strncmp(line, "VmRSS:", 6) == 0) {
			processMem->physicalMem = parseLine(line);
		}
	}
	fclose(file);
  hasMem = 1;
}

uint32_t GetVirtualMem(){
  GetProcessMemory(&mem);
  
  return mem.virtualMem;
}

uint32_t GetPhysicalMem(){
  GetProcessMemory(&mem);
  
  return mem.physicalMem;
}
