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
                name { nodeIPs }
                type { intarray }
                default { "0 1 2 3" }
                desc { IP addresses of nodes in program. }
                attributes { A_NONSETTABLE }
        }
        state {
                name { numNodes }
                type { int }
                default { 0 }
                desc { Number of nodes in program. }
                attributes { A_NONSETTABLE }
        }
        state {
                name { pairNumber }
                type { int }
                default { 0 }
                desc { Send Receive pair number for unique IP port. }
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
void $starSymbol(ipc_handler)(void *source_token, int d1, int d2, int d3, int d4)
{
        $starSymbol(RecvData) = d1;
}
	}
        codeblock (errorHandler) {
void error_handler(int status, op_t opcode, void *argblock)
{
        switch (opcode) {
                case EBADARGS:
                        fprintf(stderr,"Bad Args:");
                        fflush(stderr);
                        break;
                case EBADENTRY:
                        fprintf(stderr,"Bad Entry:");
                        fflush(stderr);
                        break;
                case EBADTAG:
                        fprintf(stderr,"Bad Tag:");
                        fflush(stderr);
                        break;
                case EBADHANDLER:
                        fprintf(stderr,"Bad Handler:");
                        fflush(stderr);
                        break;
                case EBADSEGOFF:
                        fprintf(stderr,"Bad Seg offset:");
                        fflush(stderr);
                        break;
                case EBADLENGTH:
                        fprintf(stderr,"Bad Length:");
                        fflush(stderr);
                        break;
                case EBADENDPOINT:
                        fprintf(stderr,"Bad Endpoint:");
                        fflush(stderr);
                        break;
                case ECONGESTION:
                        fprintf(stderr,"Congestion:");
                        fflush(stderr);
                        break;
                case EUNREACHABLE:
                        fprintf(stderr,"Unreachable:");
                        fflush(stderr);
                        break;
        }
}
        }
        codeblock (amdecls) {
en_t global;
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
int $starSymbol(i);
eb_t $starSymbol(bundle);
ea_t $starSymbol(endpoint);
	}
        codeblock (aminit) {
AM_Init();
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
if (AM_AllocateBundle(AM_PAR, &$starSymbol(bundle)) != AM_OK) {
        fprintf(stderr, "error: AM_AllocateBundle failed\n");
        exit(1);
}
if (AM_SetEventMask($starSymbol(bundle), AM_EMPTYTONOT ) != AM_OK) {
        fprintf(stderr, "error: AM_SetEventMask error\n");
        exit(1);
}

if (AM_AllocateKnownEndpoint($starSymbol(bundle), &$starSymbol(endpoint), HARDPORT + $val(pairNumber)) != AM_OK) {
        fprintf(stderr, "error: AM_AllocateKnownEndpoint failed\n");
        exit(1);
}
 
if (AM_SetTag($starSymbol(endpoint), 1234) != AM_OK) {
        fprintf(stderr, "error: AM_SetTag failed\n");
        exit(1);
}
if (AM_SetHandler($starSymbol(endpoint), 0, error_handler) != AM_OK) {
        fprintf(stderr, "error: AM_SetHandler failed\n");
        exit(1);
}
if (AM_SetHandler($starSymbol(endpoint), 2, $starSymbol(ipc_handler)) != AM_OK) {
        fprintf(stderr, "error: AM_SetHandler failed\n");
        exit(1);
}

for ($starSymbol(i) = 0; $starSymbol(i) < $val(numNodes); $starSymbol(i)++) {
        global.ip_addr = $ref(nodeIPs, $starSymbol(i));
        global.port = HARDPORT + $val(pairNumber);
        if (AM_Map($starSymbol(endpoint), $starSymbol(i), global, 1234) != AM_OK) {
                fprintf(stderr, "AM_Map error\n");
                fflush(stderr);
                exit(-1);
        }
}
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
		addGlobal("#define HARDPORT 61114\n", "hardPort");
		addGlobal("double $starSymbol(RecvData);\n");
		addInclude("<stdio.h>");
		addInclude("<stdlib.h>");
		addInclude("<thread.h>");
		addInclude("<udpam.h>");
		addInclude("<am.h>");
		addCompileOption(
			"-I$(PTOLEMY)/src/domains/cgc/targets/NOWam/libudpam");
                addLinkOption(
			"-L$(PTOLEMY)/lib.$(PTARCH) -ludpam -lnsl -lsocket -lthread");

		addCode(timeincludes, "include", "timeIncludes");
                addCode(amdecls, "mainDecls", "amDecls");
		addCode(timedecls, "mainDecls", "timeDecls");
		addCode(stardecls, "mainDecls");
                addCode(timeinit, "mainInit", "timeInit");
		addCode(openfd, "mainInit", "openFd");
                addCode(aminit, "mainInit", "amInit");
                addCode(starinit, "mainInit");
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
			AM_Poll($starSymbol(bundle));
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
		addProcedure(ipcHandler);
                addProcedure(errorHandler, "ErrorHandler");
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
		addCode("AM_Terminate();\n", "mainClose", "amTerminate");
	}
}

