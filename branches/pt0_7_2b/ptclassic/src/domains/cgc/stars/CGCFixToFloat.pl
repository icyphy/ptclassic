defstar {
	name { FixToFloat }
	domain { CGC }
	derivedFrom { Fix }
	version { @(#)CGCFixToFloat.pl	1.5	7/10/96 }
	author { J. Weiss }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	desc { Convert a fixed-point input to a floating-point output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { float }
		desc { Output float type }
	}
	// a setup method is inherited from CGCFix
	// if you define your own, you should call CGCFix::setup()
	// an initCode method is inherited from CGCFix
	// if you define your own, you should call CGCFix::initCode()
	go {
@	$ref(output) = FIX_Fix2Double($ref(input));
	}
	// a wrap-up method is inherited from CGCFix
	// if you define your own, you should call CGCFix::wrapup()
}
