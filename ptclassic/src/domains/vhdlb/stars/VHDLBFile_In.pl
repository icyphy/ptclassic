defstar {
	name { File_In }
	domain {VHDLB}
	desc {
Read in a series of bits (integers) from a file and output them.
	}
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Reads integers from a user-specified file.  The file is opened and the data
(one on each line of the text file) are read and then sent to the output port
at the rising edges of the synchronization clock.  When the EOF is reached,
the "stop" signal is asserted (it changes from zero to a non-zero value).

Also see "File_Store" star.
        }
        defstate {
                name { In_File }
	        type { string }
		desc { This file contains a series of bits, one on each line }
	}
	input {
		name { clock }
		type { int }
	}
	output {
		name { done }
		type { int }
		desc { This bit goes high for 1 cycle when the EOF is reached }
	}
	output {
		name { output }
		type { int }
		desc { A new output bit is sent on each rising clock edge }
	}
	go {
	}
}
