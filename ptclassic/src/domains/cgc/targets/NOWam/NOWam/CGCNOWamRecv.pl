defstar {
	name { NOWamRecv }
	domain { CGC }
	desc {
Receive star between NOW processors.
	}
	version { $Id$ }
	author { Patrick Warner }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California
        }
	location { CGC NOW target library }
	explanation {
Produce code for inter-process communication (receive-side)
	}
	private {
		friend class CGCNOWamTarget;
	}
	output {
		name {output}
		type {FLOAT}
	}
	ccinclude { "StringList.h" }

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

	codeblock (ipcHandler) {
void $val(IPCHandlerName)(int src, double arg, int dum1, int dum2)
{
        $starSymbol(RecvData) = arg;
}
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
		addGlobal("double $starSymbol(RecvData);\n");
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
                addDeclaration("double $starSymbol(timeRecv);\n");
                addDeclaration("prusage_t $starSymbol(beginRecv);\n");
                addDeclaration("prusage_t $starSymbol(endRecv);\n");
                addCode("timeRun = 0.0;\n", "mainInit", "timeRun");
                addCode("$starSymbol(timeRecv) = 0.0;\n");
                addCode("$starSymbol(RecvData) = -0.001;\n");
		addCode(openfd, "mainInit", "openFd");
		addCode("am_enable();\n", "mainInit", "amEnable");
	}
		
	codeblock (block) {
	int i, pos;


        if (fd == -1) {
                printf("couldn't open proc\n");
        }
        else if (ioctl(fd, PIOCUSAGE, &$starSymbol(beginRecv)) == -1) {
                printf("error getting time\n");
        }

	for (i = 0; i < $val(numData); i++) {
		while ($starSymbol(RecvData) == -0.001) {
			am_poll();
		}
		pos = $val(numData) - 1 + i;
		$ref(output,pos) = $starSymbol(RecvData);
                $starSymbol(RecvData) = -0.001;
	}

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

	}

	go {
		addProcedure(ipcHandler, IPCHandlerName);
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

