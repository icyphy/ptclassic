defstar {
    name { HostSldr }
    derivedFrom { HostASrc }
    domain { CG56 }
    desc { Graphical host slider for asynchronous input source. }
    version { $Id$ }
    author { Kennard White }
    acknowledge { Based on CG56HostSldrGX }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
    hinclude { <minmax.h> }
    htmldoc {
<a name="slider bar"></a>
<a name="scale"></a>
This is an asynchronous source star (like the Const star) with
a particular X windows graphical interface on the host.
It is indended that this star be used with a graphical target monitor
(.e.g., qdm_s56x).
<p>
The <b>slider</b> widget presents the user with a "ruler" along which
the user may slide a marker.  The value output corresponds to the
current location of the slider.
The <i>dspMin</i> and <i>dspMax</i> states control the range of values
that will be output on the DSP.
The <i>hostMin</i> and <i>hostMax</i> states control the range of values
the user sees (the limits of the "ruler"),
and <i>hostInitVal</i> is the initial value in the host coordinate set.
The host values are <b>always</b> linearly mapped onto the dsp values.
The <i>scale</i> parameter controls how host values are displayed to
the user (linear,logrithmic,db10,db20).  It is important to note that
this controls only the values displayed and not the mapping from host
values to dsp values.
<h3>IMPLEMENTATION</h3>
<p>
This is a Ptolemy replacement for Gabriel host slider system.
Unlike the Gabriel system, there is no need to specify a "command" to execute
to bring up the slider bars, since Ptolemy targets automatically arrange for
the asynchronous input/output to be handled using a qdm derivative.
Also, the host value may be floating point.
    }
    state {
	    name { hostMin }
	    type { FLOAT }
	    desc { Host minimum value. }
	    default { "-1" }
    }
    state {
	    name { hostMax }
	    type { FLOAT }
	    desc { Host maximum value. }
	    default { 1 }
    }
    state {
	    name { hostInitVal }
	    type { FLOAT }
	    desc { Initial value. }
	    default { 0 }
    }
    state {
	    name { dspMin }
	    type { FIX }
	    desc { DSP minimum value. }
	    default { "-1" }
    }
    state {
	    name { dspMax }
	    type { FIX }
	    desc { DSP maximum value. }
	    default { 1 }
    }
    state {
	    name { scale }
	    type { STRING }
	    desc { "type of scale(mapping)". }
	    default { "linear" }
    }
    codeblock(cbSliderAio) {
aio_slider $ref(value) $fullname() "$val(label)" $val(hostMin) $val(hostMax) $val(hostInitVal) $val(dspMin) $val(dspMax) "$val(scale)"
    }
    setup {
	double hlo = double(hostMin), hhi = double(hostMax);
	double dlo = dspMin.asDouble(),  dhi = dspMax.asDouble();
	double hinit = double(hostInitVal), dinit;
	if ( hlo == hhi ) {
	    Error::abortRun(*this,"Host range most be nonempty.");
	    return;
	}
	if ( hinit < min(hlo,hhi) || hinit > max(hlo,hhi) ) {
	    Error::abortRun(*this,"Host initial value is out-of-range");
	    return;
	}
	double fRatio = (dhi-dlo)/(hhi-hlo);
	dinit = (hinit-hlo)*fRatio + dlo;
	if ( dinit < -1.0 || dinit > 1.0 ) {
	    Error::abortRun(*this,"Dsp initial value is out-of-range");
	    return;
	}
	if ( dinit > CG56_ONE )
	    dinit = CG56_ONE;
	value = dinit;
    }
    initCode {
    	addCode(cbSliderAio,"aioCmds");
    }
}
