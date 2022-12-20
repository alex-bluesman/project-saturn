#ifndef __IO_H__
#define __IO_H__

#include <types.h>

static inline void mem_write8(uint64_t addr, uint8_t data)
{
	volatile uint8_t *paddr = (uint8_t *)addr;
	*paddr = data;
}

#endif /* __IO_H__ */
