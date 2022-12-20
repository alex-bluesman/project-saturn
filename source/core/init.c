#include <asm/sys_ctrl.h>
#include <console.h>
#include <sys.h>

#define PAGE_SIZE	4096
unsigned int boot_stack[STACK_SIZE] __align(PAGE_SIZE);

void saturn_init(void)
{
	console_init();

	console_msg("\r\n > console enabled\r\n");


	heap_debug();

	for (;;);
}
