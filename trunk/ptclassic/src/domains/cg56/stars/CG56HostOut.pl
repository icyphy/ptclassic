defstar {
	name { HostOut }
	domain { CG56 }
	desc { Syncronous Host Output. }
	version { $Id$ }
	acknowledge { Gabriel version by Phil Lapsley }
	author { Kennard, Chih-Tsung Huang (ported from Gabriel) }
	copyright { 1992 The Regents of the University of California }
	location { CG56 library }
	explanation {
Output data from DSP to host via host port.
	}
	input	{
		name { input }
		type { fix }
		}
	state {
		name { samplesConsumed }
		type { int }
		desc { Number of samples consumed. }
		default { 1 }
	}
	state {
		name { samplesOutput }
		type { int }
		desc { Number of samples transfered to host.  }
		default { 1 }
	}
 	state  {
		name { blockOnHost }
		type { int }
		desc { Boolean: wait until host ready? }
		default { "YES" }
	}
 	state  {
		name { command }
		type { string }
		desc {  }
		default { "" }
	}
	start {
	input.setSDFParams(int(samplesConsumed),int(samplesConsumed)-1);
	}
	initCode {
		const char* p=command;
		if (p[0] != 0) addRunCmd(command,"\n");
	}
	codeblock(cbSingleBlocking) {
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait)
	jclr	#0,x:m_pbddr,$label(wait)
	movep	$ref(input),x:m_htx
	}
	codeblock(cbSingleNonBlocking) {
	jclr	#m_htde,x:m_hsr,$label(skip)
	jclr	#0,x:m_pbddr,$label(slip)
	movep	$ref(input),x:m_htx
$label(skip)
	}
	codeblock(cbMultiNonBlocking) {
	jclr	#m_htde,x:m_hsr,$label(skip)
	jclr	#0,x:m_pbddr,$label(skip)
	move	#$addr(input),r0
	.LOOP	#$val(samplesOutput)
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait)
	jclr	#0,x:m_pbddr,$label(wait)
	movep	x:(r0)+,x:m_htx
	.ENDL
	nop
$label(skip)
	}
	codeblock(cbMultiBlocking) {
	move	#$addr(input),r0
	.LOOP	#$val(samplesOutput)
$label(wait)
	jclr	#m_htde,x:m_hsr,$label(wait)
	jclr	#0,x:m_pbddr,$label(wait)
	movep	x:(r0)+,x:m_htx
	.ENDL
	nop
	}
	go {
		if (int(samplesConsumed)==1) {
			if ( int(blockOnHost) ) {
				gencode(cbSingleBlocking);
			} else {
				gencode(cbSingleNonBlocking);
			}
		} else {
			if ( int(blockOnHost) ) {
				gencode(cbMultiBlocking);
			} else {
				gencode(cbMultiNonBlocking);
			}
		}
	}

	execTime { 
		return 2;
	}
}
