defstar {
	name { HostIn  }
	domain { CG56 }
	desc { Data Input }
	version { $Id$ }
	acknowledge { Gabriel version by Phil Lapsley }
	author { Chih-Tsung, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star accepts input from the DSP56000's host port and
makes this input available on its output.
.pp
The generated code polls the host port and waits until
a sample is available.
Interrupt-based code is not necessary, since the host port
isn't a real-time interface.
.pp
The parameter \fIcommand\fP specifies a command to be run after
the code has been loaded into the target architecture.
This command might, for example, read data from a file and
write it to the DSP56000's host port.
Note that there should probably only be one \fI56host_in\fP or
\fI56host_out\fP star per universe
with a non-empty \fIcommand\fP parameter.
.uh BUGS
There needs to be a flow control mechanism to allow other code
to access the host port.
The current implementation "hogs" the port, reading data from it
as fast as possible.
If, for example, a host interrupt handler wanted to read the host
port to get a parameter from the host, it would have no idea what
sample it was reading.
	}
	seealso { hostOut }
	execTime {
		return 2;
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {command}
		type {string}
		desc { }
		default {""}
	}
	codeblock (std) {
$label(l)
	jclr	#m_hrdf,x:m_hsr,$label(l)
	jclr	#1,x:m_pbddr,$label(l)
	movep	x:m_hrx,$ref(output)
	}
        initCode {
		addRunCmd(command,"\n");
	}
	go {
		gencode(std);
	}
}
