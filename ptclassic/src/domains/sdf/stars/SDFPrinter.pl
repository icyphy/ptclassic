ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt
 Date of creation: 1/16/90
 Revisions:  Redone to support ANYTYPE, J. Buck
	Added StringState -- file Name, D.Messerschmitt
 Converted to use preprocessor, 10/3/90 JTB

 Printer prints out its input, which may be any supported type.
 There may be multiple inputs: all inputs are printed together on
 the same line.

**************************************************************************/
}
defstar {
	name { Printer }
	domain { SDF }
	desc {	"Prints out one sample from each input port per line\n"
		"If 'fileName' is not equal to 'cout' (the default), it\n"
		"specifies the filename to print to."
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	defstate {
		name { fileName }
		type { string }
		default { "cout" }
		desc { "filename for output" }
	}
	protected {
		UserOutput output;
	}
	start {
		output.fileName(fileName);
		input.reset();
	}

	go {
		for(int i=input.numberPorts(); i>0; i--)
		 	output << (input()%0).print() << "\t";
		output << "\n";
	}
}

