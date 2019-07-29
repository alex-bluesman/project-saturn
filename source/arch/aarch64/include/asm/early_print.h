#ifndef __EARLY_PRINT_H__
#define __EARLY_PRINT_H__

#ifdef CONFIG_EARLY_PRINT

  #define early_print(_s)		\
	adr	x0, 98f;		\
	bl	early_uart_print;	\
	b	99f;			\
  98:					\
	.asciz _s;			\
	.align 2;			\
  99:

  #define early_print_hex()		\
	bl	early_uart_print_hex;

  #define early_uart_init()		\
	bl	early_uart_init

#else

  #define early_print(_s)
  #define early_print_hex()
  #define early_uart_init()

#endif

#endif /* __EARLY_PRINT_H__ */
