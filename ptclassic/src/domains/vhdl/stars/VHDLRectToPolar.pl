defstar {
	name { RectToPolar }
	domain { VHDL }
	desc { CORDIC algorithm performs Cartesian to polar conversion }
	version { %W $Date$ }
	author { M. C. Williamson, A. P. Kalavade }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	acknowledge { C. Sheers, IMEC }
	location { VHDL main library }
	explanation {
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
		default { "< $PTOLEMY/src/domains/vhdl/demo/CorAngles" }
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
