defstar {
	name { NOWamRecv }
	domain { CGC }
	desc {
Receive star between NOW processors.
	}
	version { $Id$ }
	author { Patrick Warner }
	copyright {
Copyright(c) 1995-%Q% The Regents of the University of California
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location { CGC NOW Active Message target library }
	explanation {
Produce code for inter-process communication (receive-side).
	}
	private {
		friend class CGCNOWamTarget;
	}
	output {
		name {output}
		type {FLOAT}
	}

	state {
		name { numData }
		type { int }
		default { 1 }
		desc { number of tokens to be transferred }
		attributes { A_NONSETTABLE }
	}
	state {
		name { IPCHandlerName }
		type { STRING }
		default { "IPCHandler" }
		desc { Name of receiver's IPC handler function. }
		attributes { A_NONSETTABLE }
	}
	setup {
		numData = output.numXfer();
	}

	codeblock (timeincludes) {
#ifdef TIME_INFO
#include <sys/fcntl.h>
#include <sys/resource.h>
#include <sys/procfs.h>
#endif
	}
	codeblock (ipcHandler) {
void $val(IPCHandlerName)(int src, double arg, int dum1, int dum2)
{
        $starSymbol(RecvData) = arg;
}
	}
	codeblock (timedecls) {
#ifdef TIME_INFO
int fd;
char proc[BUFSIZ];
double timeRun;
prusage_t beginRun;
prusage_t endRun;
#endif
	}
	codeblock (stardecls) {
#ifdef TIME_INFO
double $starSymbol(timeRecv);
prusage_t $starSymbol(beginRecv);
prusage_t $starSymbol(endRecv);
#endif
	}
	codeblock (timeinit) {
#ifdef TIME_INFO
timeRun = 0.0;
#endif
	}
	codeblock (starinit) {
#ifdef TIME_INFO
$starSymbol(timeRecv) = 0.0;
$starSymbol(RecvData) = -0.001;
#endif
	}
	codeblock (openfd) {
#ifdef TIME_INFO
sprintf(proc,"/proc/%d", (int)getpid());
if ((fd = open(proc, O_RDONLY)) == -1)
	printf("error opening proc\n");
if (fd == -1)
       	printf("couldn't open proc\n");
else if (ioctl(fd, PIOCUSAGE, &beginRun) == -1)
       	printf("error getting time\n");
#endif
	}
	initCode {
		addGlobal("double $starSymbol(RecvData);\n");
		addInclude("<stdio.h>");
		addInclude("<am.h>");
		addCode(timeincludes, "include", "timeIncludes");
		addCode(timedecls, "mainDecls", "timeDecls");
		addCode(stardecls, "mainDecls");
                addCode(timeinit, "mainInit", "timeInit");
                addCode(starinit, "mainInit");
		addCode(openfd, "mainInit", "openFd");
		addCode("am_enable();\n", "mainInit", "amEnable");
	}
		
	codeblock (block) {
	int i, pos;


#ifdef TIME_INFO
        if (fd == -1) {
                printf("couldn't open proc\n");
        }
        else if (ioctl(fd, PIOCUSAGE, &$starSymbol(beginRecv)) == -1) {
                printf("error getting time\n");
        }
#endif

	for (i = 0; i < $val(numData); i++) {
		while ($starSymbol(RecvData) == -0.001) {
			am_poll();
		}
		pos = $val(numData) - 1 + i;
		$ref(output,pos) = $starSymbol(RecvData);
                $starSymbol(RecvData) = -0.001;
	}

#ifdef TIME_INFO
        if (fd == -1) {
                printf("couldn't open proc\n");
        }
        else if (ioctl(fd, PIOCUSAGE, &$starSymbol(endRecv)) == -1) {
                printf("error getting time\n");
        }
        $starSymbol(timeRecv) += (double)($starSymbol(endRecv).pr_rtime.tv_sec -
                             $starSymbol(beginRecv).pr_rtime.tv_sec) +
                    ((double)($starSymbol(endRecv).pr_rtime.tv_nsec -
                             $starSymbol(beginRecv).pr_rtime.tv_nsec)) /
                             1000000000.0;
        printf("Cumulative time to receive %lf seconds\n", $starSymbol(timeRecv));
#endif
	}

	go {
		addProcedure(ipcHandler, IPCHandlerName);
		addCode(block);
	}
	codeblock (runtime) {
#ifdef TIME_INFO
	if (fd == -1)
		printf("couldn't open proc\n");
	else if (ioctl(fd, PIOCUSAGE, &endRun) == -1)
		printf("error getting time\n");
	timeRun = (double)(endRun.pr_rtime.tv_sec - beginRun.pr_rtime.tv_sec) +
		  ((double)(endRun.pr_rtime.tv_nsec -
			    beginRun.pr_rtime.tv_nsec)) / 1000000000.0;
	printf("Time to run %lf seconds\n", timeRun);
#endif
	}
	wrapup {
		addCode(runtime, "mainClose", "runTime");
		addCode("am_disable();\n", "mainClose", "amDisable");
	}
}

