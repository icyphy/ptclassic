defstar {
	name {ConstCx1ofN}
	domain {SDF}
	version { $Id$ }
	desc {
Outputs a constant complex value from the array of ComplexValues selected by
the given ValueIndex.
	}
	author { A. Richter and U. Trautwein }
	copyright {
Copyright (c) 1996-1997 Technical University of Ilmenau.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF contribution library }
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

