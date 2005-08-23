defstar {
	name { WaveForm }
	domain { CGC }
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
 YES         anything   anything  Stop after outputting the waveform once

The first line of the table gives the default settings.
	}
	htmldoc {
This star may be used to read a file by simply setting "value" to
something of the form "&lt; filename".  The file will be read completely
and its contents stored in an array.  The size of the array is currently
limited to 20,000 samples.  To read longer files, use the 
<tt>ReadFile</tt>
star.  This latter star reads one sample at a time, and hence also
uses less storage.
<a name="file read"></a>
<a name="waveform from file"></a>
<a name="reading from a file"></a>
<a name="halting a simulation"></a>
<a name="simulation, halting"></a>
	}
	version { @(#)CGCWaveForm.pl	1.10	01 Oct 1996 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
		desc { Halt the run at the end of the given data. }
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
	if ($val(haltAtEnd) && ($ref(pos) >= $val(size)))
		break;
	else if ($ref(pos) >= $val(size)) {
		$ref(output) = 0.0;
		$ref(pos)++;
	} else {
		$ref(output) = $ref(value)[$ref(pos)++];
	}
	if ($val(periodic))
	    if($val(period) <= 0 && $ref(pos) >= $val(size)) 
		$ref(pos) = 0;
	    else if ($val(period) > 0 && $ref(pos) >= $val(period)) 
		$ref(pos) = 0;
	}
	go {
		addCode(body);
	}
	exectime {
		return 11;  /* worst case number */
	}
}
