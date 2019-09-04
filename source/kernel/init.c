#include <asm/sys_ctrl.h>
#include <console.h>
#include <mmu.h>
#include <sys.h>

unsigned int boot_stack[STACK_SIZE] __align(PAGE_SIZE);

void saturn_init(void)
{
	mmu_init();
	console_init();

	console_msg("\r\n > console enabled\r\n");

	for (;;);
}
