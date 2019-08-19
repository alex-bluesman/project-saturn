#include <asm/sys_ctrl.h>
#include <console.h>
#include <mmu.h>
#include <sys.h>

unsigned int boot_stack[STACK_SIZE] __align(1 << 12);

void saturn_init(void)
{
	console_init();

	console_msg("\r\nWelcome to the C world!\r\n");

	mmu_init();

	for (;;);
}
