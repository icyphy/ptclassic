$Id$
/* bittrue.h -- version 1.18 (IMEC)		last updated: 5/9/89 */
/* C. Scheers */

#define IPBS 7              /* Ints Per Bit String */
#define BPI 14              /* Bits Per Int */

typedef enum {false, true} bool;

typedef int BitString[IPBS];

typedef struct {
	  BitString bs;
        } Sig_Type;

#define NegativeBits(x) \
      (((x[IPBS-1] >> (BPI-1)) == 1)?true:false)

#define CastBits(x, dold, dnew) \
      if (dold > dnew) ShiftRight (x, (dold)-(dnew)); \
		  else ShiftLeft (x, (dnew)-(dold));

/*extern double d_to_a (); */
int iter;
Sig_Type tmp;

#define Float2Fix(x, result, wx, dx) \
	  a_to_d ((double)(x), result.bs, wx, dx); \
	  co (result.bs, wx, dx);

#define Fix2Int(x, wx, dx, result) \
	  (result) = (int) d_to_a (x.bs, wx, dx);

/* removed because complained of too few args, added fix_to_int in show */
/*#define f2i(x, wx, dx) ((int) d_to_a (x.bs, wx, dx))  */

#define f2i(x, wx, dx) ((int) fix_2_int(x.bs, wx, dx))
#define f2f(x, wx, dx) ((float) d_to_a (x.bs, wx, dx))

#define FixDisplay(file, name, value, wx, dx) \
	  fprintf (file, "%s :	%f	", name, d_to_a (value.bs, wx, dx)); \
	  ShowBits (file, value.bs, wx, dx); fprintf (file, "\n");

#define IntDisplay(file, name, value) \
	  fprintf (file, "%s : 	%d\n", name, value);

#define BoolDisplay(file, name, value) \
	  fprintf (file, "%s :	%d\n", name, (value!=0)?1:0)

#define FixAssign(x, result) \
	  for (iter=0; iter<IPBS; iter++) result.bs[iter] = x.bs[iter];

#define FixBitSelect(x, wx, dx, b, w, result, wr, dr) \
	  SelectBits (x.bs, b, w, result.bs);

#define FixBitMerge(x, wx, dx, y, wy, dy, result, wr, dr) \
	  MergeBits (x.bs, wx, y.bs, wy, result.bs); \
	  co (result.bs, wr, dr);

#define FixPlus(x, wx, dx, y, wy, dy, result, wr, dr) \
	  AddBits (x.bs, y.bs, result.bs); \
	  if (dx != dr) CastBits (result.bs, dx, dr); \
	  co (result.bs, wr, dr);

/*if(typeof y is not fix then a_to_d) 
	  a_to_d(y,wy,dy); */

#define FixMinus(x, wx, dx, y, wy, dy, result, wr, dr) \
	  TwoComp (y.bs, tmp.bs); \
	  AddBits (x.bs, tmp.bs, result.bs); \
	  if (dx != dr) CastBits (result.bs, dx, dr); \
	  co (result.bs, wr, dr);
           
#define FixNegate(x, result, wx, dx) \
	  TwoComp (x.bs, result.bs);

#define FixCast(x, wx, dx, result, wr, dr) \
	  FixAssign (x, result); \
	  CastBits (result.bs, dx, dr); \
	  co (result.bs, wr, dr);

#define FixMult(x, wx, dx, y, wy, dy, result, wr, dr) \
	  MultBits (x.bs, y.bs, result.bs); \
	  CastBits (result.bs, dx+dy, dr); \
	  co (result.bs, wr, dr);

#define FixDiv(x, wx, dx, y, wy, dy, result, wr, dr) \
	  DivBits (x.bs, wx, dx, y.bs, wy, dy, result.bs, wr, dr); \
	  co (result.bs, wr, dr);

#define FixOr(x, wx, dx, y, wy, dy, result, wr, dr) \
	  OrBits (x.bs, y.bs, result.bs);

#define FixXor(x, wx, dx, y, wy, dy, result, wr, dr) \
	  XorBits (x.bs, y.bs, result.bs);

#define FixAnd(x, wx, dx, y, wy, dy, result, wr, dr) \
	  AndBits (x.bs, y.bs, result.bs);

#define FixNot(x, wx, dx, result, wr, dr) \
	  NotBits (x.bs, result.bs);

#define FixSR(x, result, wx, dx, n) \
	  FixAssign (x, result); \
	  ShiftRight (result.bs, n);

#define FixSL(x, result, wx, dx, n) \
	  FixAssign (x, result); \
	  ShiftLeft (result.bs, n); \
	  WrapBits (result.bs, wx, dx);

#define FixMin(x, wx, dx, y, wy, dy, result, wr, dr) \
	fprintf (file, "Bit true Min() not implemented\n")

#define FixMax(x, wx, dx, y, wy, dy, result, wr, dr) \
    	fprintf (file, "Bit true Max() not implemented\n")



#define FixGT(x, wx, dx, y, wy, dy, result) \
          ((result) = Greater (x.bs, y.bs))

#define FixEQ(x, wx, dx, y, wy, dy, result) \
          ((result) =  Equal (x.bs, y.bs))

#define FixGTE(x, wx, dx, y, wy, dy, result) \
          ((result) = GreaterEqual(x.bs, y.bs))

/* defined so in highlevel */
#define FixGE(x, wx, dx, y, wy, dy, result) \
          ((result) = GreaterEqual(x.bs, y.bs))

#define FixLT(x, wx, dx, y, wy, dy, result) \
          ((result) = Smaller (x.bs, y.bs))

#define FixLTE(x, wx, dx, y, wy, dy, result) \
          ((result) = SmallerEqual (x.bs, y.bs))

/* defined so in highlevel */
#define FixLE(x, wx, dx, y, wy, dy, result) \
          ((result) = SmallerEqual (x.bs, y.bs))

#define FixNTE(x, wx, dx, y, wy, dy, result) \
          ((result) = NotEqual (x.bs, y.bs))

/* defined so in highlevel */
#define FixNE(x, wx, dx, y, wy, dy, result) \
          ((result) = NotEqual (x.bs, y.bs))

/*
void co (x, wx, dx)
   BitString x;
   int wx, dx;
void co (BitString x, int wx, int dx)
*/
void co (BitString x, int wx, int dx)
{
#ifdef DETOV
   ChkOv (x, wx, dx);
#endif
}

#define FixSqrt(x, wx, dx, result, wr, dr) \
	  SqrtBits (x.bs, wx, dx, result.bs, wr, dr); \
	  co (result.bs, wr, dr);

#define FixCos(x, wx, dx, result, wr, dr) \
	  printf("FixCos is not implemented\n");

#define FixSin(x, wx, dx, result, wr, dr) \
	  printf("FixSin is not implemented\n");
/*
#define lookup(table, wt, dt, address, wa, da, result, wr, dr) { \
	  FixAssign (table[(int) d_to_a ((address)->bs, wa, da)], (result)[0]); \
	  CastBits ((result)->bs, dt, dr); \
	  co ((result)->bs, wr, dr); \
        }
*/

#define lookup2(table, wt, dt, address1, wa1, da1, address2, wa2, da2, result, wr, dr) { \
	  FixAssign (table[(int) d_to_a ((address1)->bs, wa1, da1)] \
			  [(int) d_to_a ((address2)->bs, wa2, da2)], (result)[0]); \
	  CastBits ((result)->bs, dt, dr); \
	  co ((result)->bs, wr, dr); \
        }

#define shiftleft(x, wx, dx, shiftvalue, ws, ds, result, wr, dr) { \
	  FixAssign ((x)[0], (result)[0]); \
	  ShiftLeft ((result)->bs, (int) d_to_a ((shiftvalue)->bs, ws, ds)); \
	  CastBits ((result)->bs, dx, dr); \
	  WrapBits ((result)->bs, wr, dr); \
        }

#define shiftright(x, wx, dx, shiftvalue, ws, ds, result, wr, dr) { \
	  FixAssign ((x)[0], (result)[0]); \
	  ShiftRight ((result)->bs, (int) d_to_a ((shiftvalue)->bs, ws, ds)); \
	  CastBits ((result)->bs, dx, dr); \
        }

/*
#define abs(x, wx, dx, result, wr, dr) \
	  if (NegativeBits ((x)->bs) == true) TwoComp ((x)->bs, (result)->bs); \
	  else FixAssign ((x)[0], (result)[0]); \
	  if (dx != dr) CastBits ((result)->bs, dx, dr); \
	  co ((result)->bs, wr, dr);
*/

#define addsaturate(x, wx, dx, y, wy, dy, result, wr, dr) \
	  AddBits ((x)->bs, (y)->bs, (result)->bs); \
	  if (dx != dr) CastBits ((result)->bs, dx, dr); \
	  SaturateBits ((result)->bs, wr, dr)

#define subsaturate(x, wx, dx, y, wy, dy, result, wr, dr) \
	  TwoComp ((y)->bs, tmp.bs); \
	  AddBits ((x)->bs, tmp.bs, (result)->bs); \
	  if (dx != dr) CastBits ((result)->bs, dx, dr); \
	  SaturateBits ((result)->bs, wr, dr)

#define multsaturate(x, wx, dx, y, wy, dy, result, wr, dr) \
	  MultBits ((x)->bs, (y)->bs, (result)->bs); \
	  CastBits ((result)->bs, dx+dy, dr); \
	  SaturateBits ((result)->bs, wr, dr)

#define addwrap(x, wx, dx, y, wy, dy, result, wr, dr) \
	  AddBits ((x)->bs, (y)->bs, (result)->bs); \
	  if (dx != dr) CastBits ((result)->bs, dx, dr); \
	  WrapBits ((result)->bs, wr, dr)

#define subwrap(x, wx, dx, y, wy, dy, result, wr, dr) \
	  TwoComp ((y)->bs, tmp.bs); \
	  AddBits ((x)->bs, tmp.bs, (result)->bs); \
	  if (dx != dr) CastBits ((result)->bs, dx, dr); \
	  WrapBits ((result)->bs, wr, dr)

#define multwrap(x, wx, dx, y, wy, dy, result, wr, dr) \
	  MultBits ((x)->bs, (y)->bs, (result)->bs); \
	  CastBits ((result)->bs, dx+dy, dr); \
	  WrapBits ((result)->bs, wr, dr)

#define printint(x, y) \
	   printf ("x :	%d\n", *(x));

#define printfix(x, wx, dx, y, wy, dy) \
	   printf ("x :	%f	", d_to_a ((x)->bs, wx, dx)); \
	   ShowBits (stdout, (x)->bs, wx, dx); printf ("\n");
