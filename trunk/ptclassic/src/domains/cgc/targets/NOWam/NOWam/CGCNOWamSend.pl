defstar {
	name { NOWamSend }
	domain { CGC }
	desc {
Send star between NOWam processors.
	}
	version { $Id$ }
	author { Patrick O. Warner }
	copyright { 1994 The Regents of the University of California }
	location { CGC target library }
	explanation {
Produce code for inter-process communication (send-side)
	}
	private {
		friend class CGCNOWamTarget;
		CGStar* partner;
	}
	input {
		name {input}
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
		desc { Name of sender's IPC handler function. }
		attributes { A_NONSETTABLE }
	}
	state {
		name { hostAddr }
		type { int }
		default { 0 }
		desc { Host virtual node for server }
		attributes { A_NONSETTABLE }
	}
	ccinclude { "StringList.h" }
	hinclude { "CGCNOWamTarget.h" }

	setup {
		numData = input.numXfer();
	}

        codeblock (timeincludes) {
#ifdef TIME_INFO
#include <sys/fcntl.h>
#include <sys/resource.h>
#include <sys/procfs.h>
#endif
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
double $starSymbol(timeSend);
prusage_t $starSymbol(beginSend);
prusage_t $starSymbol(endSend);
#endif
        }
        codeblock (timeinit) {
#ifdef TIME_INFO
timeRun = 0.0;
#endif
        }
        codeblock (starinit) {
#ifdef TIME_INFO
$starSymbol(timeSend) = 0.0;
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
		// obtain the hostAddr state from parent MultiTarget.
		// Note that this routine should be placed here.
		CGCNOWamTarget* t = (CGCNOWamTarget*) cgTarget()->parent();
		t->setMachineAddr(this, partner);
		hostAddr.initialize();

		// code generation.
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
	int i, pos, check;
	double myData;
	
	#ifdef TIME_INFO
	if (fd == -1) {
		printf("couldn't open proc\n");
	}
	else if (ioctl(fd, PIOCUSAGE, &$starSymbol(beginSend)) == -1) {
		printf("error getting time\n");
	}
	#endif

	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		myData = $ref(input,pos);
		check = am_request_fd($val(hostAddr), $val(IPCHandlerName), myData, 0, 0);	
		if (check == -1) {
			printf("Error in sending data\n");
		}
	}

	#ifdef TIME_INFO
	if (fd == -1) {
		printf("couldn't open proc\n");
	}
	else if (ioctl(fd, PIOCUSAGE, &$starSymbol(endSend)) == -1) {
		printf("error getting time\n");
	}
        $starSymbol(timeSend) += (double)($starSymbol(endSend).pr_rtime.tv_sec -
                             $starSymbol(beginSend).pr_rtime.tv_sec) +
                    ((double)($starSymbol(endSend).pr_rtime.tv_nsec -
                             $starSymbol(beginSend).pr_rtime.tv_nsec)) /
                             1000000000.0; 
	printf("Cumulative time to send %lf seconds\n", $starSymbol(timeSend));
	#endif

	}
	go {
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

