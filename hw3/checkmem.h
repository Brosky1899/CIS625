#ifndef CHECKMEM_H_
#define CHECKMEM_H_
#include <stdint.h>

typedef struct {
	uint32_t virtualMem;
	uint32_t physicalMem;
} processMem_t;

uint32_t GetVirtualMem();
uint32_t GetPhysicalMem();
#endif