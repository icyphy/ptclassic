/* $Id$ */

/* ops.c -- version 1.7 (IMEC)		last updated: 9/30/88 */
static char *SccsId = "@(#)ops.c	1.7 (IMEC)	88/09/30";

#include <stdio.h>
#include "bits.h"

extern double d_to_a ();

int MSBpos (x)
   BitString x;
{
   int i, p, m, a;
#ifdef DEBUG
   if ((NegativeBits (x)) == true)
      Crash ("MSBpos");
#endif
   for (i=IPBS-1; (x[i] == 0) && (i>0); i--);
   a = x[i];
   p = (i+1) * BPI;
   m = 1<<(BPI-1);
   for (i=0; ((m & a) == 0) && (i<BPI); i++) {
      m = m>>1;
      p--;
   }
   return (p+1);
}

int MSBneg (x)
   BitString x;
{
   int i, p, m, a, tcm;
#ifdef DEBUG
   if ((NegativeBits (x)) == false)
      Crash ("MSBneg");
#endif
   tcm = ~((-1)<<BPI);
   for (i=IPBS-1; (((~x[i]) & tcm) == 0) && (i>0); i--);
   a = x[i];
   p = (i+1) * BPI;
   m = 1<<(BPI-1);
   for (i=0; ((m & a) != 0) && (i<BPI); i++) {
      m = m>>1;
      p--;
   }
   return (p+1);
}

void ChkOv (x, wx, dx)
   BitString x;
   int wx, dx;
{
   int MSB;
   if ((x[IPBS-1] >> (BPI-1)) == 0)
      MSB = MSBpos (x);
   else
      MSB = MSBneg (x);
/*   if (MSB > wx) {
      printf ("\n*** Overflow ***");
      printf ("\n*** fix<%d,%d> is too small for %f = ", wx, dx, d_to_a (x, MSB, dx));
      ShowBits (stdout, x, MSB, dx);
      printf ("\n\n");
      exit (1);
   }  */
}

void SaturateBits (x, wx, dx)
   BitString x;
   int wx, dx;
{
   int MSB, i;
   if ((x[IPBS-1] >> (BPI-1)) == 0) {
      MSB = MSBpos (x);
      if (MSB > wx) {
	 int tcm;
         tcm = ~((-1)<<BPI);
	 for (i=0; (wx-=BPI)>0; i++) x[i] = tcm;
	 x[i] = tcm>>(-wx+1);
         for (i++; i<IPBS; i++) x[i] = 0;
      }
   } else {
      MSB = MSBneg (x);
      if (MSB > wx) {
	 int tcm;
         tcm = ~((-1)<<BPI);
	 for (i=0; (wx-=BPI)>0; i++) x[i] = 0;
	 x[i] = tcm & (tcm<<(BPI+wx-1));
         for (i++; i<IPBS; i++) x[i] = tcm;
      }
   }
}

void WrapBits (x, wx, dx)
   BitString x;
   int wx, dx;
{
   int i, MSBmask;
   for (i=0; (wx-=BPI)>0; i++);
   MSBmask = 1<<(wx+BPI-1);
   if (MSBmask & x[i]) {
      int tcm;
      tcm = ~((-1)<<BPI);
      x[i] = (x[i] | ((-1)<<(wx+BPI))) & tcm;
      for (i++; i<IPBS; i++) x[i] = tcm;
   } else {
      int tcm;
      tcm = ~((-1)<<(wx+BPI));
      x[i] = x[i] & tcm;
      for (i++; i<IPBS; i++) x[i] = 0;
   }
}

void SelectBits (x, p, w, result)
   BitString x;
   int p, w;
   BitString result;
{
   int i, j, mask, bnr;
   void ShiftRight();

   CopyBits(x, result);
   ShiftRight(result, p);
   bnr = 0;
   for(i=0; i<IPBS; i++) {
      mask = 1;
      for (j=0; j<BPI; j++) {
	  if (bnr >= w)             /* must clear this bit */
	      result[i] = result[i]&(~(mask));
	  mask = mask<<1;
	  bnr++;
      }
   }
}

void MergeBits (x, wx, y, wy, result)
   BitString x, y, result;
   int wx, wy;
{
   int bs[MWL];
   int i, j, mask, bnr, end;

   /* copy x over to bs */
   bnr = MWL-1;
   for(i=0; i<IPBS; i++) {
      mask = 1;
      for (j=0; j<BPI; j++) {
	 if ((mask & x[i]) == 0)
	    bs[bnr]=0;
         else
	    bs[bnr]=1;
         bnr--;
	 mask = mask<<1;
      }
   }

   end = MWL-(wx+wy);
   CopyBits(y, result);
   bnr = MWL-1;
   for(i=0; i<IPBS; i++) {
      mask = 1;
      for (j=0; j<BPI; j++) {
	  if (bnr < MWL-wy && bnr >= end) {
	      if (bs[bnr+wy] == 0)        /* must clear result bit */
	          result[i] = result[i]&(~(mask));
              else
	          result[i] = result[i] | mask;  /* must set result bit */
          } else if (bnr < end)
	      result[i] = result[i]&(~(mask));  /* must clear result bit */
	  mask = mask<<1;
	  bnr--;
      }
   }
}

void AddBits (x, y, result)
   BitString x, y, result;
{
   int carry, cm, i, a;
   carry = 0;
   cm = ~(1<<BPI);
   for (i=0; i<IPBS; i++) {
      a = x[i] + y[i] + carry;
      carry = a>>BPI;
      result[i] = a & cm;
   }
}

void TwoComp (x, result)
   BitString x, result;
{
   BitString Comp;
   int carry, tcm, cm, i, a;
   tcm = ~((-1)<<BPI);
   cm = ~(1<<BPI);
   a = ((~x[0]) & tcm) + 1;
   carry = a>>BPI;
   result[0] = a & cm;
   for (i=1; i<IPBS; i++) {
      a = ((~x[i]) & tcm) + carry;
      carry = a>>BPI;
      result[i] = a & cm;
   }
}

void MultBits (x, y, result)
   BitString x, y, result;
{
   BitString px, py;
   int pr[2*IPBS], bx, by;
   bool ResNeg;
   int i, j, carry, tcm, a;
   if ((NegativeBits (x)) == true) {
      TwoComp (x, px);
      if ((NegativeBits (y)) == true) {
	 TwoComp (y, py);
	 ResNeg = false;
      } else {
	 CopyBits (y, py);
	 ResNeg = true;
      }
   } else {
      CopyBits (x, px);
      if ((NegativeBits (y)) == true) {
	 TwoComp (y, py);
	 ResNeg = true;
      } else {
	 CopyBits (y, py);
	 ResNeg = false;
      }
   }
   tcm = ~((-1)<<BPI);
   for (bx=IPBS-1; (px[bx] == 0) && (bx>0); bx--);
   for (by=IPBS-1; (py[by] == 0) && (by>0); by--);
   for (i=0; i<2*IPBS; i++)
      pr[i] = 0;
   for (i=0; i<=bx; i++) {
      carry = 0;
      for (j=0; j<=by; j++) {
	 a = px[i] * py[j] + pr[i+j] + carry;
         pr[i+j] = a & tcm;
	 carry = a>>BPI;
      }
      pr[i+by+1] = pr[i+by+1] + carry;
   }
   if (ResNeg == true)
      TwoComp (pr, result);
   else
      CopyBits (pr, result);
}

void ShiftLeftShort (x, n)
   BitString x;
   int n;
{
   int i, tcm, a, carry;
#ifdef DEBUG
   if ((n<0) || (n>BPI))
      Crash ("ShiftLeftShort");
#endif
   tcm = ~((-1)<<BPI);
   carry = 0;
   for (i=0; i<IPBS; i++) {
      a = x[i]>>(BPI-n);
      x[i] = ((x[i]<<n) & tcm) + carry;
      carry = a;
   }
}

void ShiftLeft (x, n)
   BitString x;
   int n;
{
   int s;
   for (s=0; (s + BPI) <= n; s+=BPI)
      ShiftLeftShort (x, BPI);
   if (s < n)
      ShiftLeftShort (x, n-s);
}

void ShiftRightShort (x, n)
   BitString x;
   int n;
{
   int i, carry, tcm, a;
#ifdef DEBUG
   if ((n<0) || (n>BPI))
      Crash ("ShiftLeftShort");
#endif
   tcm = ~((-1)<<n);
   if ((NegativeBits (x)) == true)
      carry = (~((-1)<<n))<<(BPI-n);
   else
      carry = 0;
   for (i=IPBS-1; i>=0; i--) {
      a = (x[i] & tcm)<<(BPI-n);
      x[i] = (x[i]>>n) + carry;
      carry = a;
   }
}

void ShiftRight (x, n)
   BitString x;
   int n;
{
   int s;
   for (s=0; (s + BPI) <= n; s+=BPI)
      ShiftRightShort (x, BPI);
   if (s < n)
      ShiftRightShort (x, n-s);
}

bool Greater (x, y)
   BitString x, y;
{
   BitString cx, diff;
   TwoComp (x, cx);
   AddBits (cx, y, diff);
   return (NegativeBits (diff));
}

bool Equal (x, y)
   BitString x, y;
{
   int i;
   for (i=0; i<IPBS; i++)
      if (x[i] != y[i]) return (false);
   return (true);
}

bool GreaterEqual (x, y)
   BitString x, y;
{
   int i;
   BitString cx, diff;
   TwoComp (x, cx);
   AddBits (cx, y, diff);
   if ((NegativeBits (diff)) == true)
      return (true);
   for (i=0; i<IPBS; i++)
      if (diff[i] != 0) return (false);
   return (true);
}

bool Smaller (x, y)
   BitString x, y;
{
   BitString cy, diff;
   TwoComp (y, cy);
   AddBits (x, cy, diff);
   return (NegativeBits (diff));
}

bool SmallerEqual (x, y)
   BitString x, y;
{
   int i;
   BitString cy, diff;
   TwoComp (y, cy);
   AddBits (x, cy, diff);
   if ((NegativeBits (diff)) == true)
      return (true);
   for (i=0; i<IPBS; i++)
      if (diff[i] != 0) return (false);
   return (true);
}

bool NotEqual (x, y)
   BitString x, y;
{
   int i;
   for (i=0; i<IPBS; i++)
      if (x[i] != y[i]) return (true);
   return (false);
}

void DivBits (x, wx, dx, y, wy, dy, result, wr, dr)
   BitString x, y, result;
   int wx, dx, wy, dy, wr, dr;
{
   BitString px, py, pr, bk, ak, ck;
   bool ResNeg;
   int i, niters, MSBpx, MSBpy;
   if ((NegativeBits (x)) == true) {
      TwoComp (x, px);
      if ((NegativeBits (y)) == true) {
	 TwoComp (y, py);
	 ResNeg = false;
      } else {
	 CopyBits (y, py);
	 ResNeg = true;
      }
   } else {
      CopyBits (x, px);
      if ((NegativeBits (y)) == true) {
	 TwoComp (y, py);
	 ResNeg = true;
      } else {
	 CopyBits (y, py);
	 ResNeg = false;
      }
   }
   MSBpx = MSBpos (px);
   MSBpy = MSBpos (py);
   if (MSBpy == 1) {
      CopyBits (py, result);
      printf ("\n*** warning : division by zero ***\n\n");
      return;
   }
   ShiftLeft (px, MWL-MSBpx-2);
   CopyBits (py, bk);
   ShiftLeft (bk, MWL-MSBpy-2);
   InitBits (ak);
   InitBits (pr);
   niters = dr - ((MSBpy - dy) - (MSBpx - dx + 1));
   for (i=0; i<niters; i++) {
      AddBits (ak, bk, ck);
      ShiftLeftShort (pr, 1);
      if (GreaterEqual (px, ck) == true) {
	 pr[0] = pr[0] + 1;
	 CopyBits (ck, ak);
      }
      ShiftRightShort (bk, 1);
   }
   if (ResNeg == true)
      TwoComp (pr, result);
   else
      CopyBits (pr, result);
}

void SqrtBits (x, wx, dx, result, wr, dr)
   BitString x, result;
   int wx, dx, wr, dr;
{
   BitString ak, bk, dk, tmp;
   int msb, msbmindx, n, dsqrt, sl, dak, i, dkpos, dksr;
   if ((NegativeBits (x)) == true) {
      printf ("\n*** square root of negative number ***\n\n");
      exit (1);
   }
   CopyBits (x, ak);
   msb = MSBpos (ak);
   InitBits (result);
   if (msb == 1)
      return;
   msbmindx = msb - dx - 1;
   if (msbmindx > 0) {
      n = msbmindx/2;
      if ((msbmindx%2) != 0) n++;
   } else
      n = 0;
   dsqrt = n + dx;
   sl = dr - dsqrt;
   if (sl > 0) {
      ShiftLeft (ak, sl);
      dsqrt = dsqrt + sl;
      dak = dx + 2*n + sl;
   } else
      dak = dx + 2*n;
   dksr = 1;
   InitBits (dk);
   dk[0] = 1; ShiftLeft (dk, dak-1);
   for (i=0; i<dr+n; i++) {
      for (dkpos=0; dk[dkpos] == 0; dkpos++);
      CopyBits (result, bk);
      ShiftLeftShort (bk, 1);
      bk[dkpos] += dk[dkpos];
      ShiftRight (bk, dksr);
      if (SmallerEqual (bk, ak) == true) {
	 result[dkpos] += dk[dkpos]; 
	 TwoComp (bk, tmp);
	 AddBits (ak, tmp, ak);
      }
      dksr++;
      ShiftRightShort (dk, 1);
   }
   if (sl < 0)
      ShiftRight (result, -sl);
}

void OrBits (x, y, result)
   BitString x, y, result;
{
   int i;
   for (i=0; i<IPBS; i++)
      result[i] = x[i] | y[i];
}

void XorBits (x, y, result)
   BitString x, y, result;
{
   int i;
   for (i=0; i<IPBS; i++)
      result[i] = x[i] ^ y[i];
}

void AndBits (x, y, result)
   BitString x, y, result;
{
   int i;
   for (i=0; i<IPBS; i++)
      result[i] = x[i] & y[i];
}

void NotBits (x, result)
   BitString x, result;
{
   int i, tcm;
   tcm = ~((-1)<<BPI);
   for (i=0; i<IPBS; i++)
      result[i] = (~x[i]) & tcm;
}
