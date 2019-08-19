#ifndef __SYS_H__
#define __SYS_H__

/* Memory alignment for data structures */
#define __align(n)	__attribute__((__aligned__(n)))

/* Packing of data structure members */
#define __packed	__attribute__((__packed__))

#endif /* __SYS_H__ */
