/* $Id$ */
/* bitfast.h -- version 1.8 (IMEC)		last updated: 5/9/89 */
/* C. Scheers */

#define Sig_Type int

#define CastBits(x, dold, dnew) \
      (dold>dnew)?((x)>>((dold)-(dnew))):((x)<<((dnew)-(dold)))

extern double f_d_to_a ();
extern int f_a_to_d ();

#define Float2Fix(x, result, wx, dx) \
	  result = f_a_to_d ((double)(x), wx, dx); \
	  co (result, wx, dx);

#define Fix2Int(x, wx, dx, result) \
	  (result) = (int) f_d_to_a (x, wx, dx);

#define FixDisplay(file, name, value, wx, dx) \
	  fprintf (file, "%s :	%f	", name, f_d_to_a (value, wx, dx)); \
	  f_ShowBits (file, value, wx, dx); fprintf (file, "\n");

#define IntDisplay(file, name, value) \
	  fprintf (file, "%s : 	%d\n", name, value);

#define BoolDisplay(file, name, value) \
	  fprintf (file, "%s :	%d\n", name, (value!=0)?1:0)

#define FixAssign(x, result) \
	  (result) = (x);

#define FixPlus(x, wx, dx, y, wy, dy, result, wr, dr) \
	  (result) = (x) + (y); \
	  if (dx != dr) result = CastBits (result, dx, dr); \
	  co (result, wr, dr);

#define FixMinus(x, wx, dx, y, wy, dy, result, wr, dr) \
	  (result) = (x) - (y); \
	  if (dx != dr) result = CastBits (result, dx, dr); \
	  co (result, wr, dr);

#define FixNegate(x, result, wx, dx) \
	  (result) = -(x);

#define FixCast(x, wx, dx, result, wr, dr) \
	  result = CastBits (x, dx, dr); \
	  co (result, wr, dr);

#define FixMult(x, wx, dx, y, wy, dy, result, wr, dr) { \
          int xl, xh, yl, yh, ll, lh, hl, hh, sl, sm, sh, s; \
	  if (wx+wy<=32) { \
	     (result) = (x) * (y); \
	     (result) = CastBits (result, dx+dy, dr); \
	  } else { \
	  xl = (x)&32767; yl = (y)&32767; \
	  xh = (x)>>15;   yh = (y)>>15; \
	  ll = yl*xl; lh = yl*xh; hl = yh*xl; hh = yh*xh; \
	  sl = ll&32767; \
	  sm = lh + hl + (ll>>15); \
	  sh = hh + (sm>>15); \
	  sm = sm&32767; \
	  s = (dx)+(dy)-(dr); \
	  if (s>0) { \
	     if (s<15) \
	        (result) = (sl>>s)|(sm<<(15-s))|(sh<<(30-s)); \
	     else \
                (result) = (sm>>(s-15))|(sh<<(30-s)); \
	  } else { \
	  s = -s; \
	  if (s == 0) \
	     (result) = (sl)|(sm<<15)|(sh<<30); \
          else \
	     (result) = (sl<<s)|(sm<<(15+s))|(sh<<(30+s)); \
	  } \
	  } \
	  co (result, wr, dr); }

#define FixDiv(x, wx, dx, y, wy, dy, result, wr, dr) \
	  (result) = ((x)<<(dy))/(y); \
	  (result) = CastBits (result, dx, dr); \
	  co (result, wr, dr);

#define FixOr(x, wx, dx, y, wy, dy, result, wr, dr) \
	  (result) = (x) | (y);

#define FixXor(x, wx, dx, y, wy, dy, result, wr, dr) \
	  (result) = (x) ^ (y);

#define FixAnd(x, wx, dx, y, wy, dy, result, wr, dr) \
	  (result) = (x) & (y);

#define FixNot(x, wx, dx, result, wr, dr) \
	  (result) = ~ (x);

#define FixBitSelect (x, wx, dx, b, w, result, wr, dr) { \
   printf ("No equivalent bitfast operation for bitselect.\n"); exit (1);}

#define FixBitMerge(x, wx, dx, y, wy, dy, result, wr, dr) \
   printf ("No equivalent bitfast operation for bitmerge.\n"); exit (1);}

#define FixSR(x, result, wx, dx, n) \
          (result) = (x) >> (n);

#define FixSL(x, result, wx, dx, n) \
          (result) = (x) << (n);


#define FixGT(x, wx, dx, y, wy, dy, result, wr, dr) \
	  ((result) = (x) > (y))

#define FixEQ(x, wx, dx, y, wy, dy, result, wr, dr) \
	  ((result) = (x) == (y))

#define FixGTE(x, wx, dx, y, wy, dy, result, wr, dr) \
	  ((result) = (x) >= (y))

#define FixLT(x, wx, dx, y, wy, dy, result, wr, dr) \
	  ((result) = (x) < (y))

#define FixLTE(x, wx, dx, y, wy, dy, result, wr, dr) \
	  ((result) = (x) <= (y))

#define FixNTE(x, wx, dx, y, wy, dy, result, wr, dr) \
	  ((result) = (x) != (y))

void co (x, wx, dx)
   int x;
   int wx, dx;
{
}

#define lookup(table, wt, dt, address, wa, da, result, wr, dr) { \
	  FixAssign (table[(*(address)) >> (da)], *(result)); \
	  *(result) = CastBits (*(result), dt, dr); \
	  co (*(result), wr, dr); \
        }

#define lookup2(table, wt, dt, address1, wa1, da1, address2, wa2, da2, result, wr, dr) { \
	  FixAssign (table[(*(address1)) >> (da1)] \
			  [(*(address2)) >> (da2)], *(result)); \
	  *(result) = CastBits (*(result), dt, dr); \
	  co (*(result), wr, dr); \
        }

#define shiftleft(x, wx, dx, shiftvalue, ws, ds, result, wr, dr) { \
	  *(result) = (*(x))<<((*(shiftvalue))>>(ds)); \
	  *(result) = CastBits (*(result), dx, dr); \
	  co (*(result), wr, dr); \
        }

#define shiftright(x, wx, dx, shiftvalue, ws, ds, result, wr, dr) { \
	  *(result) = (*(x))>>((*(shiftvalue))>>(ds)); \
	  *(result) = CastBits (*(result), dx, dr); \
	  co (*(result), wr, dr); \
        }

#define abs(x, wx, dx, result, wr, dr) \
	  *(result) = (*(x))<0?(-(*(x))):(*(x)); \
	  if (dx != dr) *(result) = CastBits (*(result), dx, dr); \
	  co (*(result), wr, dr);

/* not implemented

#define FixSqrt(x, wx, dx, result, wr, dr) \
	  SqrtBits (x.bs, wx, dx, result.bs, wr, dr); \
	  co (result.bs, wr, dr);

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

*/
