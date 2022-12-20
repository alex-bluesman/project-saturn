#include <types.h>
#include <asm/sys_ctrl.h>
#include <sys.h>

// TBD: should be globally defined!
//  - Size of the core stack
//  - Size of the page
static const size_t stack_size = 1024;
static const size_t page_size = 4096;

unsigned int boot_stack[stack_size] __align(page_size);

// External functions
extern "C" 
{
	void console_init(void);
	void console_msg(const char *fmt, ...);
}

static void Main(void)
{
	console_init();
	console_msg("\r\n > console enabled\r\n");

	for (;;);
}

extern "C" void saturn_init()
{
	// Let's switch to C++ world!
	Main();
}

