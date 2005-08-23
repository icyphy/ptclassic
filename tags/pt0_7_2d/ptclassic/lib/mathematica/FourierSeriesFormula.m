(* :Title:	Fourier Series Formulas *)

(* :Authors:	Brian L. Evans  *)

(* :Summary:
This package provides routines to compute the Fourier series as formulas.
 *)

(* :Context:	FourierSeriesFormula` *)

(* :PackageVersion:  @(#)FourierSeriesFormula.m	1.1	3/12/96  *)

(* :Copyright:
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 *)

(* :History:	*)

(* :Keywords:	Fourier series	*)

(* :Source:	*)

(* :Warning:	*)

(* :Mathematica Version:  2.1 *)

(* :Limitation: *)

(* :Discussion:
The FourierCosSeriesCoefficient, FourierSinSeriesCoefficient, and
FourierExpSeriesCoefficient routines in the Calculus standard packages
do return the Fourier series coefficients as a general formula, but
in an unsimplified form.  I cannot figure how to make Mathematica
simplify terms like Cos[n Pi] where n is symbolic but represents an
integer quantity.  Using

k/: IntegerQ[k] = True

did not work.
*)

(* :Functions:
FourierCosSeriesFormula
FourierSinSeriesFormula
FourierExpSeriesFormula
 *)


BeginPackage[ "FourierSeriesFormula`" ]


(*  U S A G E     I N F O R M A T I O N  *)

FourierCosSeriesFormula::usage =
"FourierCosSeriesFormula[expr, {x, xmin, xmax}, n] gives the formula
for the coefficients of the Fourier cosine series as a function of the integer
index n.
The definition follows electrical engineering convention, i.e.
expr = Sum[f[n] Cos[-2 Pi k x / T], {n, -Infinity, Infinity}],
where the period is T = x1 - x0.";

FourierExpSeriesFormula::usage =
"FourierExpSeriesFormula[expr, {x, xmin, xmax}, n] gives the formula
for the coefficients of the Fourier exponential series as a function of
the integer index n.
The definition follows electrical engineering convention, i.e.
expr = Sum[f[n] Exp[-2 I Pi k x / T], {n, -Infinity, Infinity}],
where the period is T = x1 - x0.";

FourierSinSeriesFormula::usage =
"FourierSinSeriesFormula[expr, {x, xmin, xmax}, n] gives the formula
for the coefficients of the Fourier cosine series as a function of the integer
index n.
The definition follows electrical engineering convention, i.e.
expr = Sum[f[n] Sin[-2 Pi k x / T], {n, -Infinity, Infinity}],
where the period is T = x1 - x0.";

(*  E N D     U S A G E     I N F O R M A T I O N  *)


Begin["`Private`"]


simplify[f_, n_] :=
Module[ {},
	f /.
	{ Cos[n Pi] -> -1,
	  Cos[_?OddQ n Pi] :> -1,
	  Cos[_?EvenQ n Pi] :> 1,
	  Cos[m_?IntegerQ n Pi] :> (-1)^m,
	  Sin[n Pi] -> 0,
	  Sin[_?IntegerQ n Pi] :> 0,
	  Sin[Rational[m_?OddQ, 2] n Pi] :> -(-1)^m,
	  Sin[Rational[_?EvenQ, 2] n Pi] :> 0,
	  Exp[Complex[0, _?OddQ] n Pi] :> -1,
	  Exp[Complex[0, _?EvenQ] n Pi] :> 1,
	  Exp[Complex[0, _?IntegerQ] n Pi] :> (-1)^m } ]

fourierSeries[f_, {x_, x0_, x1_}, k_, kernel_] :=
	Module[ {period},
		period = x1 - x0;
		2/period Integrate[f kernel[2 Pi k x / period], {x, x0, x1}] ]

fourierSeriesFormula[f_, {x_, x0_, x1_}, k_, kernel_] :=
	Module[ {fs, ktemp},
		fs = simplify[ fourierSeries[f, {x, x0, x1}, ktemp, kernel],
			       ktemp ];
		Simplify[fs, Trig -> False] /. ktemp -> k ]

FourierCosSeriesFormula[f_, {x_Symbol, x0_, x1_}, k_Integer] :=
	fourierSeries[f, {x, x0, x1}, k, Cos]

FourierCosSeriesFormula[f_, {x_Symbol, x0_, x1_}, k_] :=
	fourierSeriesFormula[f, {x, x0, x1}, k, Cos]

FourierExpSeriesFormula[f_, {x_Symbol, x0_, x1_}, k_Integer] :=
	fourierSeries[f, {x, x0, x1}, k, Exp[I #]&]

FourierExpSeriesFormula[f_, {x_Symbol, x0_, x1_}, k_] :=
	fourierSeriesFormula[f, {x, x0, x1}, k, Exp[I #]&]

FourierSinSeriesFormula[f_, {x_Symbol, x0_, x1_}, k_Integer] :=
	fourierSeries[f, {x, x0, x1}, k, Sin]

FourierSinSeriesFormula[f_, {x_Symbol, x0_, x1_}, k_] :=
	fourierSeriesFormula[f, {x, x0, x1}, k, Sin]


(*  E N D     P A C K A G E *)

End[]
EndPackage[]


(*  Restore the state of these four messages  *)

Protect [ FourierCosSeriesFormula,
	  FourierSinSeriesFormula,
	  FourierExpSeriesFormula ]

Null
