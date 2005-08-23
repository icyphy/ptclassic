defstar {
	name { WaveFormCx }
	domain { VHDL }
	desc {
This star works the same way as the VHDLWaveForm star,
only using complex types instead of float types.
	}
	htmldoc {
This star may be used to read a file by simply setting "value" to
something of the form "&lt; filename".  The file will be read completely
and its contents stored in an array.
	}
	version { @(#)VHDLWaveFormCx.pl	1.4 10/01/96 }
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
		type { complex }
	}
	state {
		name { value }
		type { complexarray }
		default { "(1.0,0.0) (-1.0,0.0)" }
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
  $refCx(output,real) $assign(output) 0.0;
  $refCx(output,imag) $assign(output) 0.0;
  $ref(pos) $assign(pos) $ref(pos) + 1;
else
  $refCx(output,real) = $refCx(value,pos,real);
  $refCx(output,real) = $refCx(value,pos,real);
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
