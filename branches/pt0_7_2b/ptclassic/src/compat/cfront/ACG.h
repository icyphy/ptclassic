// $Id$
#ifndef _ACG_h
#define _ACG_h 1
// This is a fake-out of the libg++ random-number interface.
// The difference is that there is only a single random-number
// stream, with seed set by srandom and values read by random.
// This works fine within Ptolemy.

extern "C" void srandom(unsigned);

class ACG {
public:
	ACG(unsigned seed = 1) {srandom(seed);}
};
#endif
