// $Id$
// A macro for generating isA functions in derived classes

#ifndef _isa_h
#define _isa_h 1
#ifdef __STDC__
#define quote(x) #x
#else
#define quote(x) "x"
#endif

#define ISA_FUNC(me,parent)\
int me :: isA (const char* cname) const {\
	if (strcmp(cname,quote(me)) == 0) return 1;\
	else return parent::isA(cname);\
}

// Alternate form for use within a class definition body
#define ISA_INLINE(me,parent)\
int isA (const char* cname) const {\
	if (strcmp(cname,quote(me)) == 0) return 1;\
	else return parent::isA(cname);\
}

#endif
