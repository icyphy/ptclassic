defstar {
	name { VISBase }
	domain { CGC }
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	initCode{
	// environment variables are automatically expanded
	  addCompileOption("-I$VSDKHOME/include"); 
	}
}
