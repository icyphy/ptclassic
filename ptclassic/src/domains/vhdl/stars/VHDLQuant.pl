defstar {
	name { Quant }
	domain { VHDL }
	desc {
	}
	version { $Id$ }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	codeblock (std) {
if $ref(input) > 127 then
  $ref(output) $assign(output) 255;
else
  $ref(output) $assign(output) 0;
end if;
	}
	go {
	  addCode(std);
	}
	exectime {
		return 2;
	}
}
