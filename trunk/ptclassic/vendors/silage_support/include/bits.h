/* $Id$ */

/* bits.h -- version 1.5 (IMEC)		last updated: 10/3/88 */
/* C. Scheers */

#define IPBS 7              /* Ints Per Bit String,
			       remember this value occurs
			       also in bittrue.h */
#define BPI 14              /* Bits Per Int,
			       remember this value occurs
			       also in bittrue.h */
#define MWL IPBS*BPI        /* Maximum Word Length */

typedef int BitString[IPBS];

typedef enum {false, true} bool;

#define CopyBits(x, result) { \
      int i; \
      for (i=0; i<IPBS; i++) result[i] = x[i]; \
   }

#define InitBits(x) { \
      int i; \
      for (i=0; i<IPBS; i++) x[i] = 0; \
   }

#define NegativeBits(x) \
      (((x[IPBS-1] >> (BPI-1)) == 1)?true:false)
