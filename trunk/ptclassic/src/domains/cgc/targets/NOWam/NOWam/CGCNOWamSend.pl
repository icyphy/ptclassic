defstar {
	name { NOWamSend }
	domain { CGC }
	desc {
Send star between NOWam processors.
	}
	version { $Id$ }
	author { Patrick O. Warner }
	copyright {
Copyright (c) 1995-%Q% The Regents of the University of California
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { CGC NOW Active Message target library }
	htmldoc {
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
                name { nodeIPs }
                type { intarray }
                default { "0 1 2 3" }
                desc { IP addresses of nodes in program. }
                attributes { A_NONSETTABLE }
        }
	state {
		name { hostAddr }
		type { int }
		default { 0 }
		desc { Host virtual node for server }
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

	hinclude { "CGCNOWamTarget.h" }

	setup {
		numData = input.numXfer();
	}

        codeblock (converttype) {
typedef union ints_or_double {
        int asInt[2];
        double asDouble;
} convert;
        } 
        codeblock (timeincludes) {
#ifdef TIME_INFO1
#include <sys/time.h>
#endif
        }
        codeblock (replyHandler) {
void reply_handler(void *source_token, int d1, int d2, int d3, int d4)
{
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
eb_t bundle;
        }
        codeblock (timedecls) {
#ifdef TIME_INFO1
hrtime_t timeRun;
hrtime_t beginRun;
hrtime_t endRun;
#endif
        }
        codeblock (stardecls) {
#ifdef TIME_INFO2
hrtime_t $starSymbol(timeSend);
hrtime_t $starSymbol(beginSend);
hrtime_t $starSymbol(endSend);
#endif
en_t *$starSymbol(endname);
ea_t $starSymbol(endpoint);
int $starSymbol(i);
        }
        codeblock (aminit) {
AM_Init();
if (AM_AllocateBundle(AM_PAR, &bundle) != AM_OK) {
        fprintf(stderr, "error: AM_AllocateBundle failed\n");
        exit(1);
}
if (AM_SetEventMask(bundle, AM_NOTEMPTY) != AM_OK) {
        fprintf(stderr, "error: AM_SetEventMask error\n");
        exit(1);
}

        }
        codeblock (timeinit) {
#ifdef TIME_INFO2
$starSymbol(timeSend) = 0.0;
#endif
#ifdef TIME_INFO1
beginRun = gethrtime();
#endif
        }
        codeblock (starinit) {
#ifdef TIME_INFO
$starSymbol(timeSend) = 0.0;
#endif
if (AM_AllocateKnownEndpoint(bundle, &$starSymbol(endpoint), &$starSymbol(endname), HARDPORT + $val(pairNumber)) != AM_OK) {

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
if (AM_SetHandler($starSymbol(endpoint), 1, reply_handler) != AM_OK) {
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

	initCode {
		// obtain the hostAddr state from parent MultiTarget.
		// Note that this routine should be placed here.
		CGCNOWamTarget* t = (CGCNOWamTarget*) cgTarget()->parent();
		t->setMachineAddr(this, partner);
		hostAddr.initialize();

		// code generation.
		addCode(converttype, "globalDecls", "convert");
		addGlobal("#define HARDPORT 61114\n", "hardPort");
		addInclude("<stdio.h>");
		addInclude("<stdlib.h>");
		addInclude("<thread.h>");
		addInclude("<udpam.h>");
		addInclude("<am.h>");
		addCompileOption(
                      "-I$PTOLEMY/src/domains/cgc/targets/NOWam/libudpam");
		addLinkOption(
		     "-L$PTOLEMY/lib.$PTARCH -ludpam -lnsl -lsocket -lthread");

                addCode(timeincludes, "include", "timeIncludes");
                addProcedure(replyHandler, "CGCNOWam_ReplyHandler");
                addProcedure(errorHandler, "CGCNOWam_ErrorHandler");
                addCode(amdecls, "mainDecls", "amDecls");
                addCode(timedecls, "mainDecls", "timeDecls");
                addCode(stardecls, "mainDecls");
                addCode(timeinit, "mainInit", "timeInit");
                addCode(aminit, "mainInit", "amInit");
                addCode(starinit, "mainInit");
	}
		
	codeblock (block) {
	int i, pos, check;
	convert myData;
	
#ifdef TIME_INFO2
        $starSymbol(beginSend) = gethrtime();
#endif

	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		myData.asDouble = $ref(input,pos);
                check = AM_Request4($starSymbol(endpoint), $val(hostAddr), 2, myData.asInt[0], myData.asInt[1], 0, 0);      
		if (check == -1) {
			printf("Error in sending data\n");
		}
	}

#ifdef TIME_INFO2
        $starSymbol(endSend) = gethrtime();
        $starSymbol(timeSend) += $starSymbol(endSend) - $starSymbol(beginSend);
        printf("Cumulative time to send %lld usec\n", $starSymbol(timeSend) / 1000);
#endif

	}
	go {
		addCode(block);
	}

        codeblock (runtime) {
#ifdef TIME_INFO1
endRun = gethrtime();
timeRun = endRun - beginRun;
printf("Time to run %lld usec\n", timeRun / 1000);
#endif
        }

	wrapup {
                addCode(runtime, "mainClose", "runTime");
                addCode("AM_Terminate();\n", "mainClose", "amTerminate");
	}
}

