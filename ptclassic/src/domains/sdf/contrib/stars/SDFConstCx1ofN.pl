defstar {
	name {ConstCx1ofN}
	domain {SDF}
	version {@(#)SDFConstCx1ofN.pl	1.0 9/1/96}
	desc {
Outputs a constant complex value from the array of ComplexValues selected by
the given ValueIndex.
	}
	author { A. Richter, U. Trautwein }
	copyright {
Copyright (c) 1996 Technical University of Ilmenau.
All rights reserved.
	}
	location { SDF main library }
	explanation {
	}
	ccinclude { "ComplexSubset.h" }
	output {
		name {output}
		type {complex}
	}
	defstate {
		name {ComplexValues}
		type{complexarray}
		default {"(0.0,0.0)"}
		desc {Complex DC value table.}
	}
	defstate {
		name {ValueIndex}
		type{int}
		default {"1"}
		desc {Index for value table.}
	}
	go {
		output%0 << ComplexValues[ValueIndex-1];
	}
}

