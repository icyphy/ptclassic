defstar {
	name { NOWamSend }
	domain { CGC }
	desc {
Send star between NOWam processors.
	}
	version { $Id$ }
	author { Patrick Warner }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California
        }
	location { CGC NOW target library }
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

        codeblock (openfd) {
sprintf(proc,"/proc/%d", (int)getpid());
if ((fd = open(proc, O_RDONLY)) == -1)
        printf("error opening proc\n");
if (fd == -1)
        printf("couldn't open proc\n");
else if (ioctl(fd, PIOCUSAGE, &beginRun) == -1)
        printf("error getting time\n");
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
		addInclude("<sys/fcntl.h>");
		addInclude("<sys/resource.h>");
                addInclude("<sys/procfs.h>");
                addDeclaration("int fd;\n");
                addDeclaration("char proc[BUFSIZ];\n");
		addDeclaration("double timeRun;\n");
		addDeclaration("prusage_t beginRun;\n");
		addDeclaration("prusage_t endRun;\n");
		addDeclaration("double $starSymbol(timeSend);\n");
		addDeclaration("prusage_t $starSymbol(beginSend);\n");
		addDeclaration("prusage_t $starSymbol(endSend);\n");
                addCode("timeRun = 0.0;\n", "mainInit", "timeRun");
		addCode("$starSymbol(timeSend) = 0.0;\n");
                addCode(openfd, "mainInit", "openFd");
                addCode("am_enable();\n", "mainInit", "amEnable");
	}
		
	codeblock (block) {
	int i, pos, check;
	double myData;
	
	if (fd == -1) {
		printf("couldn't open proc\n");
	}
	else if (ioctl(fd, PIOCUSAGE, &$starSymbol(beginSend)) == -1) {
		printf("error getting time\n");
	}

	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		myData = $ref(input,pos);
		check = am_request_fd($val(hostAddr), $val(IPCHandlerName), myData, 0, 0);	
		if (check == -1) {
			printf("Error in sending data\n");
		}
	}
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

	}
	go {
		addCode(block);
	}

        codeblock (runtime) {
if (fd == -1)
       printf("couldn't open proc\n");
else if (ioctl(fd, PIOCUSAGE, &endRun) == -1)
       printf("error getting time\n");
timeRun = (double)(endRun.pr_rtime.tv_sec - beginRun.pr_rtime.tv_sec) +
           ((double)(endRun.pr_rtime.tv_nsec -
                     beginRun.pr_rtime.tv_nsec)) / 1000000000.0;
printf("Time to run %lf seconds\n", timeRun);
        }

	wrapup {
                addCode(runtime, "mainClose", "runTime");
                addCode("am_disable();\n", "mainClose", "amDisable");
	}
}

