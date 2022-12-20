#pragma once

// Memory alignment for data structures
#define __align(n)	__attribute__((__aligned__(n)))

// Packing of data structure members
#define __packed	__attribute__((__packed__))

// Assign object to specific section
#define __section(s)	__attribute__((section(s)))
