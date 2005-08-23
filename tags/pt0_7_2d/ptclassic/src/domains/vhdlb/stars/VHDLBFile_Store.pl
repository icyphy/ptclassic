defstar {
	name { File_Store }
	domain {VHDLB}
        desc {
Stores the incoming bits (integers) at constant intervals (controlled by the
clock)
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
A user-specified file is opened to store the incoming data at constant
intervals (at the rising edges of the synchronization clock signal).  All
values are stored on separate lines in the text file.  When the "stop" signal
goes high (from zero to non-zero), the file is closed and all following data
are ignored (This is based on the assumption that the incoming signal is no
longer of interest to us once the "stop" signal is asserted)
        }
        defstate {
                name { Out_File }
	        type { string }
	}
	input {
		name { clock }
		type { int }
	}
	input {
		name { input }
		type { int }
	}
	input {
		name { stop }
		type { int }
		desc {
		When this bit goes high, stop writing to the output file
		}
	}
	go {
	}
}
