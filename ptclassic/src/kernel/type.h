#ifndef _type_h
#define _type_h 1


// SCCS version identification
// $Id$

typedef void* Pointer;

// Types of data stored in queues between stars
enum dataType { NOT_SET, INT, FLOAT, POINTER, STRING };

// The following is also defined in gnu stdio.h, which is included
// by stream.h
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif
