/* $Id$ */

/* show.c -- version 1.7 (IMEC)		last updated: 4/21/89 */
static char *SccsId = "@(#)show.c	1.7 (IMEC)	89/04/21";

#include <stdio.h>

#include "bits.h"

bool ValidWD (w, d)
   int w, d;
{
   if ((w > MWL) ||
       (w < 1) || (d < 0) ||
       (d >= w))
      return (false);
   else
      return (true);
}

void ShowBits (FD, x, wx, dx)
   FILE *FD;
   BitString x;
   int wx, dx;
{
   char bs[MWL+1];
   int i, j, mask, bnr;
#ifdef DEBUG
   if (ValidWD (wx, dx) == false)
      Crash ("ShowBits");
#endif
   bs[MWL] = '\0';
   bnr = MWL-1;
   for (i=0; i<IPBS; i++) {
      mask = 1;
      for (j=0; j<BPI; j++) {
	 if ((mask & x[i]) == 0)
	    bs[bnr] = '0';
         else
	    bs[bnr] = '1';
         bnr--;
	 mask = mask<<1;
      }
   }
   for (i=MWL-wx; i<MWL-dx; i++)
      fprintf (FD, "%c", bs[i]);
   if (dx > 0)
      fprintf (FD, ".");
   for (i=MWL-dx; i<MWL; i++)
      fprintf (FD, "%c", bs[i]);
}

void f_ShowBits (FD, x, wx, dx)
   FILE *FD;
   int x, wx, dx;
{
   int i, mask;
   char bs[33];
   for (i=0, mask=1; i<32; i++, mask<<=1)
      bs[31-i] = (mask&x)?'1':'0';
   for (i=32-wx; i<32-dx; i++)
      fprintf (FD, "%c", bs[i]);
   if (dx > 0)
      fprintf (FD, ".");
   for (i=32-dx; i<32; i++)
      fprintf (FD, "%c", bs[i]);
}

void ShowLogBits (x, wx)
   BitString x;
   int wx;
{
   char bs[MWL+1];
   int i, j, mask, bnr;
#ifdef DEBUG
   if (ValidWD (wx, 0) == false)
      Crash ("ShowLogBits");
#endif
   bs[MWL] = '\0';
   bnr = MWL-1;
   for (i=0; i<IPBS; i++) {
      mask = 1;
      for (j=0; j<BPI; j++) {
	 if ((mask & x[i]) == 0)
	    bs[bnr] = '0';
         else
	    bs[bnr] = '1';
         bnr--;
	 mask = mask<<1;
      }
   }
   for (i=MWL-wx; i<MWL; i++)
      printf ("%c", bs[i]);
}

void a_to_d (a, x, wx, dx)
   double a;
   BitString x;
   int wx, dx;
{
   int ip, i;
   bool ResNeg;
   double f;
   BitString xc;
#ifdef DEBUG
   if (ValidWD (wx, dx) == false)
      Crash ("a_to_d");
#endif
   f = a;
   if (f < 0) {
      ResNeg = true;
      f = -f;
   } else
      ResNeg = false;
   ip = f;
   f = f - (double) ip;
   InitBits (x);
/* from bits.h */
   for (i=1; i<32; i++) {
      ShiftLeftShort (x, 1);
      ip = ip<<1;
      if (ip<0)
	 x[0] = x[0] + 1;
   }
   for (i=0; i<dx; i++) {
      ShiftLeftShort (x, 1);
      f *= 2;
      if (f>=1.0) {
	 f -= 1.0;
	 x[0] = x[0] + 1;
      }
   }
   if (ResNeg == true) {
      CopyBits (x, xc);
      TwoComp (xc, x);
   }
}

void int_to_fix (a, x, wx, dx)
   int a;
   BitString x;
   int wx, dx;
{
   int ip, i;
   bool ResNeg;
   int f;
   BitString xc;
#ifdef DEBUG
   if (ValidWD (wx, dx) == false)
      Crash ("a_to_d");
#endif
   f = a;
   if (f < 0) {
      ResNeg = true;
      f = -f;
   } else
      ResNeg = false;
   ip = f;
   f = f - ip;
   InitBits (x);
/* from bits.h */
   for (i=1; i<32; i++) {
      ShiftLeftShort (x, 1);
      ip = ip<<1;
      if (ip<0)
	 x[0] = x[0] + 1;
   }
   for (i=0; i<dx; i++) {
      ShiftLeftShort (x, 1);
      f *= 2;
      if (f>=1.0) {
	 f -= 1.0;
	 x[0] = x[0] + 1;
      }
   }
   if (ResNeg == true) {
      CopyBits (x, xc);
      TwoComp (xc, x);
   }
}

int f_a_to_d (a, wx, dx)
   double a;
   int wx, dx;
{
   bool ResNeg;
   double f;
   int i, x;
   f = a;
   if (f < 0) {
      ResNeg = true;
      f = -f;
   } else
      ResNeg = false;
   x = f;
   f = f - (double) x;
   for (i=0; i<dx; i++) {
      x <<= 1;
      f *= 2;
      if (f>=1.0) {
	 f -= 1.0;
	 x += 1;
      }
   }
   if (ResNeg == true)
      return (-x);
   else
      return (x);
}

double d_to_a (x, wx, dx)
   BitString x;
   int wx, dx;
{
   BitString xc, xi, xf;
   bool ResNeg;
   double fi, ff;
   int bx, i;
#ifdef DEBUG
   if (ValidWD (wx, dx) == false)
      Crash ("d_to_a");
#endif

   CopyBits (x, xc);
   ShiftLeft (xc, MWL - wx);
   ShiftRight (xc, MWL - wx);
   if ((NegativeBits (xc)) == true) {
      ResNeg = true;
      TwoComp (xc, xi);
   } else {
      ResNeg = false;
      CopyBits (xc, xi);
   }
   CopyBits (xi, xf);
   for (bx=IPBS-1; (xi[bx] == 0) && (bx>0); bx--);
   fi = 0.0;
   for (i=0; i<((bx+1)*BPI-dx); i++) {
      fi *= 2;
      if (((1<<(BPI-1)) & xi[bx]) != 0)
	 fi += 1.0;
      ShiftLeftShort (xi, 1);
   }
   ff = 0.0;
   for (i=0; i<dx; i++) {
      if ((1 & xf[0]) != 0)
	 ff += 1.0;
      ff /= 2;
      ShiftRightShort (xf, 1);
   }

   if (ResNeg == true)
      return (-(fi+ff));
   else
      return (fi+ff);
}

/* ******************* */
float fix_2_float (x, wx, dx, out)
   BitString x;
   int wx, dx;
   float* out;
{
   BitString xc, xi, xf;
   bool ResNeg;
   double fi, ff;
   int bx, i;
float rslt;
#ifdef DEBUG
   if (ValidWD (wx, dx) == false)
      Crash ("d_to_a");
#endif

/*
fprintf(stderr,"1 \t ");
ShowBits(stderr,x,wx,dx);
fprintf(stderr,"\n");
*/
   CopyBits (x, xc);
   ShiftLeft (xc, MWL - wx);
   ShiftRight (xc, MWL - wx);
   if ((NegativeBits (xc)) == true) {
      ResNeg = true;
      TwoComp (xc, xi);
   } else {
      ResNeg = false;
      CopyBits (xc, xi);
   }
   CopyBits (xi, xf);
   for (bx=IPBS-1; (xi[bx] == 0) && (bx>0); bx--);
   fi = 0.0;
   for (i=0; i<((bx+1)*BPI-dx); i++) {
      fi *= 2;
      if (((1<<(BPI-1)) & xi[bx]) != 0)
	 fi += 1.0;
      ShiftLeftShort (xi, 1);
   }
   ff = 0.0;
   for (i=0; i<dx; i++) {
      if ((1 & xf[0]) != 0)
	 ff += 1.0;
      ff /= 2;
      ShiftRightShort (xf, 1);
   }

/*
fprintf(stderr,"2 \t ");
fprintf(stderr,"%f \n",(fi+ff));
*/
   if (ResNeg == true)
      rslt = -(fi+ff);
   else
      rslt = (fi+ff);

*out = rslt;
return (rslt);
}
/* ******************* */

/* ******************* */
int fix_2_int (x, wx, dx)
   BitString x;
   int wx, dx;
{
   BitString xc, xi, xf;
   bool ResNeg;
   double fi, ff;
   int bx, i;
int rslt;
#ifdef DEBUG
   if (ValidWD (wx, dx) == false)
      Crash ("d_to_a");
#endif

   CopyBits (x, xc);
   ShiftLeft (xc, MWL - wx);
   ShiftRight (xc, MWL - wx);
   if ((NegativeBits (xc)) == true) {
      ResNeg = true;
      TwoComp (xc, xi);
   } else {
      ResNeg = false;
      CopyBits (xc, xi);
   }
   CopyBits (xi, xf);
   for (bx=IPBS-1; (xi[bx] == 0) && (bx>0); bx--);
   fi = 0.0;
   for (i=0; i<((bx+1)*BPI-dx); i++) {
      fi *= 2;
      if (((1<<(BPI-1)) & xi[bx]) != 0)
	 fi += 1.0;
      ShiftLeftShort (xi, 1);
   }
   ff = 0.0;
   for (i=0; i<dx; i++) {
      if ((1 & xf[0]) != 0)
	 ff += 1.0;
      ff /= 2;
      ShiftRightShort (xf, 1);
   }

   if (ResNeg == true)
      rslt = -(fi+ff);
   else
      rslt = (fi+ff);

return (rslt);
}
/* ******************* */

double f_d_to_a (x, wx, dx)
   int x, wx, dx;
{
   double f;
   int i;
   f = x;
   for (i=0; i<dx; f /= 2, i++);
   return f;
}

void ShowInt (w)
   int w;
{
   int i, mask;
   char bits[33];
   bits[32] = '\0';
   for (i=0, mask=1; i<32; i++, mask<<=1)
      bits[31-i] = (mask&w)?'1':'0';
   printf ("int : %d bits : %s\n", w, bits);
}
