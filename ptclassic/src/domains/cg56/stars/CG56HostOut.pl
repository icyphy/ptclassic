defstar {
	name { HostOut }
	domain { CG56 }
	desc { Synchronous host port output. }
	version { $Id$ }
	acknowledge { Gabriel version by Phil Lapsley }
	author { Kennard White, Chih-Tsung Huang (ported from Gabriel) }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 io library }
	explanation {
Output data from DSP to host via host port synchronously.
.UH IMPLEMENTATION:
.pp
We distiguish between single word transfers and multiword transfers.
The single word case is simpler in that we can use the \fIref\fP macro to
access the value.  The multiword case requires a loop.  Currently
we distinguish the two cases by \fIsamplesConsumed\fP, but I think it should
be \fIsamplesOuput\fP.
.LP
In blocking mode we wait until the host is ready to read our samples.
In non-blocking mode, we completely skip the transfer if the host
is not ready.
.LP
This implementation currently uses the "unsafe" START/STOP host port DMA
flow control.
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
	setup {
	input.setSDFParams(int(samplesConsumed),int(samplesConsumed)-1);
	}
	initCode {
		const char* p=command;
		StringList cmd;
		cmd << command << "\n";
		if (p[0] != 0) addCode(command,"shellCmds");
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
				addCode(cbSingleBlocking);
			} else {
				addCode(cbSingleNonBlocking);
			}
		} else {
			if ( int(blockOnHost) ) {
				addCode(cbMultiBlocking);
			} else {
				addCode(cbMultiNonBlocking);
			}
		}
	}

	execTime { 
		return 2;
	}
}
