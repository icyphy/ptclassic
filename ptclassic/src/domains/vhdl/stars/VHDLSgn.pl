defstar {
	name { Sgn }
	domain { VHDL }
	desc {
This star computes the signum of its input.
The output is +/- 1.
Note that 0.0 maps to 1.
	}
	version { $Id$ }
	author { Michael C. Williamson, E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	htmldoc {
<a name="signum"></a>
	}
	input {
		name{ input }
		type{ float }
	}
	output {
		name{ output }
		type{ float }
	}
	codeblock (std) {
if $ref(input) >= 0.0 then
  $ref(output) $assign(output) 1.0;
else
  $ref(output) $assign(output) -1.0;
end if;
	}
	go {
	    addCode(std);
	}
}
