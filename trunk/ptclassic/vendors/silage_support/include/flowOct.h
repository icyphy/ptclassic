/* $Id$ */

extern octStatus myLastStatus;

#define ASSERT(fct)                    \
    if ((myLastStatus = fct) < OCT_OK) \
        { octError(NULL);\
	  fprintf(stderr,"\n");\
	  return (0);}

#define ASSERT_WHY(fct, why)           \
    if ((myLastStatus = fct) < OCT_OK) \
        {fprintf(stderr,"\n%s\.\n", why); \
	 octError(NULL);\
	 fprintf(stderr,"\n");\
         return (0);}
