defstar {
	name {MuLaw}
	domain {SDF}
	desc { This star compresses its input as per the Mu law. 
		using the conversion formula
	|v2| = log (1+ mu*|v1|) / log( 1+mu)  }
	author { Asawaree Kalavade }
	copyright { 1991 The Regents of the University of California }
	version {$Id$}
	explanation {
.Id "Mu law compression"
.Id "compression, Mu law"
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}

	defstate {
		name{ compress}
		type{ int}
		default{ 1}
		desc{ 0 to turn off compression, otherwises compresses with Mu law. }
		}

	defstate{
		name{ mu }
		type{int}
		default{255}
		desc{mu parameter}
		}
	
	ccinclude { <math.h> }
	go {

	float comp_value;
	float f=1.0;
	float pre_comp = input%0;

	if(compress==0)
	output%0 << pre_comp;
	else
		{
	if(pre_comp < 0) {
		pre_comp= pre_comp*(-1.0);
		f= (-1.0);
			}
	float num = log(1+ (int) mu *pre_comp);
	float den = log(1.0 + (int) mu);
	comp_value= f*num/den;	
		output%0 << comp_value;
		}
	}
}
