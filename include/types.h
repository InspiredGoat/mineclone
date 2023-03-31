#ifndef TYPES_H
#define TYPES_H

typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;

#ifndef bool

typedef unsigned char bool;
#define true 1
#define false 0

#endif


typedef struct {
	byte* data;
	ulong size;
} Buffer;

#endif
