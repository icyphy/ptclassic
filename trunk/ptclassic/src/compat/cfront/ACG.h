// $Id$
#ifndef _ACG_h
#define _ACG_h 1
// This is a fake-out of the libg++ random-number interface.
// The difference is that there is only a single random-number
// stream, with seed set by srandom and values read by random.
// This works fine within Ptolemy.

#if defined(hppa) || defined(SVR4) || defined(SYSV)
#ifndef __GNUG__
extern "C" void srand48(long int);

class ACG {
public:
	ACG(unsigned seed = 1) {srand48(seed);}
};
#endif /* !__GNUG__ */
#else /* hppa */
extern "C" void srandom(unsigned);

class ACG {
public:
	ACG(unsigned seed = 1) {srandom(seed);}
};
#endif /* hppa */
#endif
