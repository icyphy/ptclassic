$Id$
/* highlevel.h -- version 1.12 (IMEC)		last updated: 5/9/89 */
/* C. Scheers */

#define Sig_Type double

#define f2i(x, wx, dx) ((int) x)

#define Float2Fix(x, result, wx, dx)  ((result) = (x))
#define Fix2Int(x, wx, dx, result)    ((result) = (int) (x))

#define FixDisplay(file, name, value, wx, dx)  fprintf (file, "%s :	%f\n", name, value)
#define IntDisplay(file, name, value)  fprintf (file, "%s :	%d\n", name, value)
#define BoolDisplay(file, name, value) fprintf (file, "%s :	%d\n", name, (value!=0)?1:0)

#define FixAssign(x, result)       	               ((result) = (x))
#define FixPlus(x, wx, dx, y, wy, dy, result, wr, dr)  ((result) = (x) + (y))
#define FixMinus(x, wx, dx, y, wy, dy, result, wr, dr) ((result) = (x) - (y))
#define FixNegate(x, result, wx, dx)   		       ((result) = -(x))
#define FixMult(x, wx, dx, y, wy, dy, result, wr, dr)  ((result) = (x) * (y))
#define FixDiv(x, wx, dx, y, wy, dy, result, wr, dr)   ((result) = (x) / (y))
#define FixOr(x, wx, dx, y, wy, dy, result, wr, dr) { \
	   printf ("No equivalent high level operation for | (or).\n"); exit (1);}
#define FixAnd(x, wx, dx, y, wy, dy, result, wr, dr) { \
	   printf ("No equivalent high level operation for & (and).\n"); exit (1);}
#define FixNot(x, wx, dx, result, wr, dr) { \
	   printf ("No equivalent high level operation for ! (not).\n"); exit (1);}

#define FixSR(x, result, wx, dx, n) { \
	  int qqsr; \
	  result = x; \
	  for (qqsr=0; qqsr<(n); qqsr++) \
	     result = result/2; \
        }

#define FixSL(x, result, wx, dx, n) { \
	  int qqsl; \
	  result = x; \
	  for (qqsl=0; qqsl<(n); qqsl++) \
	     result = result*2; \
        }

#define FixGT(x, wx, dx, y, wy, dy,result)  \
		if( (x) > (y)) result = 1; else result=0;
#define FixEQ(x, wx, dx, y, wy, dy,result)  \
		if( (x) == (y)) result = 1; else result=0;
/* defined so in bittrue */
#define FixGTE(x, wx, dx, y, wy, dy,result)  \
		if( (x) >= (y)) result = 1; else result=0;
#define FixGE(x, wx, dx, y, wy, dy,result)  \
		if( (x) >= (y)) result = 1; else result=0;
#define FixLT(x, wx, dx, y, wy, dy,result)  {\
		if( (x) < (y)) result = 1; else result=0;}
/* defined so in bittrue */
#define FixLTE(x, wx, dx, y, wy, dy,result)  {\
		if( (x) <= (y)) result = 1; else result=0;}
#define FixLE(x, wx, dx, y, wy, dy,result)  {\
		if( (x) <= (y)) result = 1; else result=0;}
/* defined so in bittrue */
#define FixNTE(x, wx, dx, y, wy, dy,result)  {\
		if( (x) != (y)) result = 1; else result=0;}
#define FixNE(x, wx, dx, y, wy, dy,result)  {\
		if( (x) != (y)) result = 1; else result=0;}

#define FixCast(x, wx, dx, result, wr, dr)  ((result) = (x))


#define FixSqrt(x, wx, dx, result, wr, dr)             ((result) = (Sig_Type) sqrt ((double)x))

/*#define lookup(table, wt, dt, address, wa, da, result, wr, dr) \
					    (*(result) = table[(int) *(address)])
*/

#define lookup2(table, wt, dt, address1, wa1, da1, address2, wa2, da2, result, wr, dr) \
			    (*(result) = table[(int) *(address1)][(int) *(address2)])

#define shiftleft(x, wx, dx, shiftvalue, ws, ds, result, wr, dr) { \
	  int qqsl; \
	  *(result) = *(x); \
	  for (qqsl=0; qqsl<(int)*(shiftvalue); qqsl++) \
	     *(result) = *(result)*2; \
        }

#define shiftright(x, wx, dx, shiftvalue, ws, ds, result, wr, dr) { \
	  int qqsr; \
	  *(result) = *(x); \
	  for (qqsr=0; qqsr<(int)*(shiftvalue); qqsr++) \
	     *(result) = *(result)/2; \
        }

/*#define abs(x, wx, dx, result, wr, dr)    *(result) = (*(x)<0)?-(*(x)):*(x)*/

#define addsaturate(x, wx, dw, y, wy, dy, result, wr, dr)  \
	   *(result) = *(x) + *(y)

#define subsaturate(x, wx, dw, y, wy, dy, result, wr, dr)  \
	   *(result) = *(x) - *(y)

#define multsaturate(x, wx, dw, y, wy, dy, result, wr, dr)  \
	   *(result) = *(x) * *(y)

#define addwrap(x, wx, dw, y, wy, dy, result, wr, dr)  \
	   *(result) = *(x) + *(y)

#define subwrap(x, wx, dw, y, wy, dy, result, wr, dr)  \
	   *(result) = *(x) - *(y)

#define multwrap(x, wx, dw, y, wy, dy, result, wr, dr)  \
	   *(result) = *(x) * *(y)

#define printint(x, y) \
	   printf ("x :	%d\n", *(x));

#define printfix(x, wx, dx, y, wy, dy) \
	   printf ("x :	%f\n", *(x));
