defstar {
	name { WaveForm }
	domain { VHDL }
	desc {
Output a waveform as specified by the array state "value" (default "1 -1").
You can get periodic signals with any period, and can halt a simulation
at the end of the given waveform.  The following table summarizes the
capabilities:

 haltAtEnd   periodic   period    operation
 ------------------------------------------------------------------------
 NO          YES        0         The period is the length of the waveform
 NO          YES        N>0       The period is N
 NO          NO         anything  Output the waveform once, then zeros

*** NOTE:
 The following behavior is not implemented in the VHDL version of this star:
 YES         anything   anything  Stop after outputting the waveform once

The first line of the table gives the default settings.
	}
	htmldoc {
This star may be used to read a file by simply setting "value" to
something of the form "&lt; filename".
	}
	version { @(#)VHDLWaveForm.pl	1.4 10/01/96 }
	author { Michael C. Williamson, S. Ha }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	output {
		name { output }
		type { float }
	}
	state {
		name { value }
		type { floatarray }
		default { "1 -1" }
		desc { One period of the output waveform. }
	}
	state {
		name { haltAtEnd }
		type { int }
		default { "NO" }
		desc {	NOT IMPLEMENTED
		  	Halt the run at the end of the given data. }
	}
	state {
		name { periodic }
		type { int }
		default { "YES" }
		desc { Output is periodic if "YES" (nonzero). }
	}
	state {
		name { period }
		type { int }
		default { 0 }
		desc { If greater than zero, gives the period of the waveform. }
	}
	state {
		name { pos }
		type { int }
		default { 0 }
		attributes { A_NONSETTABLE }
	}
	state {
		name { size }
		type { int }
		default { 0 }
		attributes { A_NONSETTABLE }
	}
	setup {
		pos = 0;
		size = value.size();
	}
	codeblock(body) {
if ($ref(pos) >= $val(size))
  $ref(output) $assign(output) 0.0;
  $ref(pos) $assign(pos) $ref(pos) + 1;
else
  $ref(output) = $ref(value,pos);
  $ref(pos) $assign(pos) + 1;
end if;
if ($val(periodic) /= 0)
  if(($val(period) <= 0) and ($ref(pos) >= $val(size))) 
    $ref(pos) $assign(pos) 0;
  else if (($val(period) > 0) and ($ref(pos) >= $val(period))) 
    $ref(pos) $assign(pos) 0;
  end if;
end if;
}
	go {
		addCode(body);
	}
}
