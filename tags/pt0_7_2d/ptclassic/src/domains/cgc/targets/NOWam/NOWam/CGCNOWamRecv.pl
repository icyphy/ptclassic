defstar {
	name { NOWamRecv }
	domain { CGC }
	desc {
Receive star between NOW processors.
	}
	version { @(#)CGCNOWamRecv.pl	1.30 12/08/96 }
	author { Patrick Warner }
	copyright {
Copyright(c) 1995-1997 The Regents of the University of California
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location { CGC NOW Active Message target library }
	htmldoc {
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
	codeblock (ipcHandler) {
void $starSymbol(ipc_handler)(void *source_token, int d1, int d2, int d3, int d4)
{
	convert temp;

	temp.asInt[0] = d1;
	temp.asInt[1] = d2;
	$starSymbol(RecvData) = temp.asDouble;
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
#ifdef TIME_INFO3
hrtime_t $starSymbol(timeRecv);
hrtime_t $starSymbol(beginRecv);
hrtime_t $starSymbol(endRecv);
#endif
int $starSymbol(i);
en_t *$starSymbol(endname);
ea_t $starSymbol(endpoint);
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
#ifdef TIME_INFO3
$starSymbol(timeRecv) = 0.0;
#endif
#ifdef TIME_INFO1
beginRun = gethrtime();
#endif
	}
	codeblock (starinit) {
#ifdef TIME_INFO3
$starSymbol(timeRecv) = 0.0;
#endif
$starSymbol(RecvData) = -0.001;

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
	initCode {
		addCode(converttype, "globalDecls", "convert");
		addGlobal("#define HARDPORT 61114\n", "hardPort");
		addGlobal("double $starSymbol(RecvData);\n");
		addInclude("<stdio.h>");
		addInclude("<stdlib.h>");
		addInclude("<string.h>");
		addInclude("<thread.h>");
		addInclude("<udpam.h>");
		addInclude("<am.h>");
		addCompileOption(
			"-I$PTOLEMY/src/domains/cgc/targets/NOWam/libudpam");
                addLinkOption(
		"-L$PTOLEMY/lib.$PTARCH -ludpam -lnsl -lsocket -lthread");

		addCode(timeincludes, "include", "timeIncludes");
		addProcedure(ipcHandler);
                addProcedure(errorHandler, "CGCNOWam_ErrorHandler");
                addCode(amdecls, "mainDecls", "amDecls");
		addCode(timedecls, "mainDecls", "timeDecls");
		addCode(stardecls, "mainDecls");
                addCode(timeinit, "mainInit", "timeInit");
                addCode(aminit, "mainInit", "amInit");
                addCode(starinit, "mainInit");
	}
		
	codeblock (block) {
	int i, pos;


#ifdef TIME_INFO3
        $starSymbol(beginRecv) = gethrtime();
#endif

	for (i = 0; i < $val(numData); i++) {
		while ($starSymbol(RecvData) == -0.001) {
			if (AM_Poll(bundle) != AM_OK) {
        			fprintf(stderr, "error: AM_Poll failed\n");
        			exit(1);
			}
		}
		pos = $val(numData) - 1 + i;
		$ref(output,pos) = $starSymbol(RecvData);
                $starSymbol(RecvData) = -0.001;
	}

#ifdef TIME_INFO3
        $starSymbol(endRecv) = gethrtime();
        $starSymbol(timeRecv) += $starSymbol(endRecv) - $starSymbol(beginRecv);
        printf("Cumulative time to receive %lld usec\n", $starSymbol(timeRecv) / 1000);
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

