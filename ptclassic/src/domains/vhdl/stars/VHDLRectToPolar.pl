defstar {
	name { RectToPolar }
	domain { VHDL }
	desc { CORDIC algorithm performs Cartesian to polar conversion }
	version { @(#)VHDLRectToPolar.pl	1.6 10/01/96 }
	author { M. C. Williamson, A. P. Kalavade }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	acknowledge { C. Sheers, IMEC }
	location { VHDL main library }
	htmldoc {
The CORDIC algorithm is an efficient implementation for Cartesian to
polar coordinate conversion.
	}
	input {
		name { x }
		type { float }
	}
	input {
		name { y }
		type { float }
	}
	output {
		name { magnitude }
		type { float }
	}
	output {
		name { phase }
		type { float }
	}
	defstate {
		name { CorAngles }
		type { floatarray }
		default { "0.25 \
0.147583618 0.077979130 0.039583424 0.019868524 \
0.009943948 0.004973187 0.002486745 0.001243392 \
0.000621698 0.000310849 0.000155425 0.000077712 \
0.000038856 0.000019428 0.000009714 0.000004857 \
0.000002429 0.000001214 0.000000607" }
		desc { Angles for the CORDIC algorithm. }
	}

	codeblock(init, "const char* X, const char* Y, const char* Phi")
	{
	  if $ref(y) >= 0.0 then
	    $temp(@X_0,float) := $ref(y);
	    $temp(@Y_0,float) := -$ref(x);
	    $temp(@Phi_0,float) := $define(Angle90, float, 0.5);
	  else
	    $temp(@X_0,float) := -$ref(y);
  	    $temp(@Y_0,float) := $ref(x);
	    $temp(@Phi_0,float) := -$define(Angle90, float, 0.5);
	  end if;
	  
	}

	codeblock(body, "const char* i, const char* i1, const char* X, const char* Y, const char* Phi")
	{
	  if $temp(@Y_@i1,float) >= 0.0 then
	    $temp(@X_@i,float) := $temp(@X_@i1,float) + ($temp(@Y_@i1,float) / (2.0**@i1));
	    $temp(@Y_@i,float) := $temp(@Y_@i1,float) - ($temp(@X_@i1,float) / (2.0**@i1));
	    $temp(@Phi_@i,float) := $temp(@Phi_@i1,float) + $ref(CorAngles,@i1);
	  else
	    $temp(@X_@i,float) := $temp(@X_@i1,float) - ($temp(@Y_@i1,float) / (2.0**@i1));
	    $temp(@Y_@i,float) := $temp(@Y_@i1,float) + ($temp(@X_@i1,float) / (2.0**@i1));
	    $temp(@Phi_@i,float) := $temp(@Phi_@i1,float) - $ref(CorAngles,@i1);
	  end if;
	  
	}

	codeblock(equate, "const char* N1, const char* X, const char* /*Y*/, const char* Phi")
	{
	  $ref(magnitude) $assign(magnitude) $temp(@X_@N1,float) * $define(Corrector, float, 0.60725285);
	  $ref(phase) $assign(phase) $temp(@Phi_@N1,float) * 3.141592654;
	}

	go 
	{
		StringList X,Y,Phi,N1,p;
//		X = "$starSymbol(X)";
//		Y = "$starSymbol(Y)";
//		Phi = "$starSymbol(Phi)";
		X = "X";
		Y = "Y";
		Phi = "Phi";
		p = CorAngles.size() - 1;
		N1 = hashstring(p);
		addCode(init((const char*)X, (const char*)Y,
			     (const char*)Phi));
		
		for(int j=1; j< CorAngles.size(); j++)
		{
			StringList i,i1;	
			i = j; i1 = j-1;
			addCode(body((const char*)i, (const char*)i1,
				     (const char*)X, (const char*)Y,
				     (const char*)Phi));
		}

		addCode(equate((const char*)N1, (const char*)X, (const char*)Y,
			       (const char*)Phi));
	}
}
