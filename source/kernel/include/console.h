#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdarg.h>
#include <types.h>

/* Console API: */
void console_msg(const char *fmt, ...);
void console_init(void);

/* Generic UART API: */
void uart_init(void);
void uart_tx(uint8_t *buff, size_t len);

#ifdef DEBUG
  #define console_dbg(msg, ...)		console_msg("(debug) "msg)
  #define console_dbg_va(msg, ...)	console_msg("(debug) "msg, __VA_ARGS__)
#else
  #define console_dbg(msg, ...)
  #define console_dbg_va(msg, ...)
#endif /* DEBUG */

#endif /* __CONSOLE_H__ */
