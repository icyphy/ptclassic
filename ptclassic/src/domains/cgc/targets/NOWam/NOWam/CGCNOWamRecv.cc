static const char file_id[] = "CGCNOWamRecv.pl";
// .cc file generated from CGCNOWamRecv.pl by ptlang
/*
Copyright(c) 1995-1997 The Regents of the University of California
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCNOWamRecv.h"

const char *star_nm_CGCNOWamRecv = "CGCNOWamRecv";

const char* CGCNOWamRecv :: className() const {return star_nm_CGCNOWamRecv;}

ISA_FUNC(CGCNOWamRecv,CGCStar);

Block* CGCNOWamRecv :: makeNew() const { LOG_NEW; return new CGCNOWamRecv;}

CodeBlock CGCNOWamRecv :: converttype (
# line 60 "CGCNOWamRecv.pl"
"typedef union ints_or_double {\n"
"        int asInt[2];\n"
"        double asDouble;\n"
"} convert;\n");

CodeBlock CGCNOWamRecv :: timeincludes (
# line 66 "CGCNOWamRecv.pl"
"#ifdef TIME_INFO1\n"
"#include <sys/time.h>\n"
"#endif\n");

CodeBlock CGCNOWamRecv :: ipcHandler (
# line 71 "CGCNOWamRecv.pl"
"void $starSymbol(ipc_handler)(void *source_token, int d1, int d2, int d3, int d4)\n"
"{\n"
"	convert temp;\n"
"\n"
"	temp.asInt[0] = d1;\n"
"	temp.asInt[1] = d2;\n"
"	$starSymbol(RecvData) = temp.asDouble;\n"
"}\n");

CodeBlock CGCNOWamRecv :: errorHandler (
# line 81 "CGCNOWamRecv.pl"
"void error_handler(int status, op_t opcode, void *argblock)\n"
"{\n"
"        switch (opcode) {\n"
"                case EBADARGS:\n"
"                        fprintf(stderr,\"Bad Args:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EBADENTRY:\n"
"                        fprintf(stderr,\"Bad Entry:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EBADTAG:\n"
"                        fprintf(stderr,\"Bad Tag:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EBADHANDLER:\n"
"                        fprintf(stderr,\"Bad Handler:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EBADSEGOFF:\n"
"                        fprintf(stderr,\"Bad Seg offset:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EBADLENGTH:\n"
"                        fprintf(stderr,\"Bad Length:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EBADENDPOINT:\n"
"                        fprintf(stderr,\"Bad Endpoint:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case ECONGESTION:\n"
"                        fprintf(stderr,\"Congestion:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"                case EUNREACHABLE:\n"
"                        fprintf(stderr,\"Unreachable:\");\n"
"                        fflush(stderr);\n"
"                        break;\n"
"        }\n"
"}\n");

CodeBlock CGCNOWamRecv :: amdecls (
# line 124 "CGCNOWamRecv.pl"
"en_t global;\n"
"eb_t bundle;\n");

CodeBlock CGCNOWamRecv :: timedecls (
# line 128 "CGCNOWamRecv.pl"
"#ifdef TIME_INFO1\n"
"hrtime_t timeRun;\n"
"hrtime_t beginRun;\n"
"hrtime_t endRun;\n"
"#endif\n");

CodeBlock CGCNOWamRecv :: stardecls (
# line 135 "CGCNOWamRecv.pl"
"#ifdef TIME_INFO3\n"
"hrtime_t $starSymbol(timeRecv);\n"
"hrtime_t $starSymbol(beginRecv);\n"
"hrtime_t $starSymbol(endRecv);\n"
"#endif\n"
"int $starSymbol(i);\n"
"en_t *$starSymbol(endname);\n"
"ea_t $starSymbol(endpoint);\n");

CodeBlock CGCNOWamRecv :: aminit (
# line 145 "CGCNOWamRecv.pl"
"AM_Init();\n"
"if (AM_AllocateBundle(AM_PAR, &bundle) != AM_OK) {\n"
"        fprintf(stderr, \"error: AM_AllocateBundle failed\\n\");\n"
"        exit(1);\n"
"}\n"
"if (AM_SetEventMask(bundle, AM_NOTEMPTY) != AM_OK) {\n"
"        fprintf(stderr, \"error: AM_SetEventMask error\\n\");\n"
"        exit(1);\n"
"}\n"
"\n");

CodeBlock CGCNOWamRecv :: timeinit (
# line 157 "CGCNOWamRecv.pl"
"#ifdef TIME_INFO3\n"
"$starSymbol(timeRecv) = 0.0;\n"
"#endif\n"
"#ifdef TIME_INFO1\n"
"beginRun = gethrtime();\n"
"#endif\n");

CodeBlock CGCNOWamRecv :: starinit (
# line 165 "CGCNOWamRecv.pl"
"#ifdef TIME_INFO3\n"
"$starSymbol(timeRecv) = 0.0;\n"
"#endif\n"
"$starSymbol(RecvData) = -0.001;\n"
"\n"
"if (AM_AllocateKnownEndpoint(bundle, &$starSymbol(endpoint), &$starSymbol(endname), HARDPORT + $val(pairNumber)) != AM_OK) {\n"
"        fprintf(stderr, \"error: AM_AllocateKnownEndpoint failed\\n\");\n"
"        exit(1);\n"
"}\n"
" \n"
"if (AM_SetTag($starSymbol(endpoint), 1234) != AM_OK) {\n"
"        fprintf(stderr, \"error: AM_SetTag failed\\n\");\n"
"        exit(1);\n"
"}\n"
"if (AM_SetHandler($starSymbol(endpoint), 0, error_handler) != AM_OK) {\n"
"        fprintf(stderr, \"error: AM_SetHandler failed\\n\");\n"
"        exit(1);\n"
"}\n"
"if (AM_SetHandler($starSymbol(endpoint), 2, $starSymbol(ipc_handler)) != AM_OK) {\n"
"        fprintf(stderr, \"error: AM_SetHandler failed\\n\");\n"
"        exit(1);\n"
"}\n"
"\n"
"for ($starSymbol(i) = 0; $starSymbol(i) < $val(numNodes); $starSymbol(i)++) {\n"
"        global.ip_addr = $ref(nodeIPs, $starSymbol(i));\n"
"        global.port = HARDPORT + $val(pairNumber);\n"
"        if (AM_Map($starSymbol(endpoint), $starSymbol(i), global, 1234) != AM_OK) {\n"
"                fprintf(stderr, \"AM_Map error\\n\");\n"
"                fflush(stderr);\n"
"                exit(-1);\n"
"        }\n"
"}\n");

CodeBlock CGCNOWamRecv :: block (
# line 225 "CGCNOWamRecv.pl"
"	int i, pos;\n"
"\n"
"\n"
"#ifdef TIME_INFO3\n"
"        $starSymbol(beginRecv) = gethrtime();\n"
"#endif\n"
"\n"
"	for (i = 0; i < $val(numData); i++) {\n"
"		while ($starSymbol(RecvData) == -0.001) {\n"
"			if (AM_Poll(bundle) != AM_OK) {\n"
"        			fprintf(stderr, \"error: AM_Poll failed\\n\");\n"
"        			exit(1);\n"
"			}\n"
"		}\n"
"		pos = $val(numData) - 1 + i;\n"
"		$ref(output,pos) = $starSymbol(RecvData);\n"
"                $starSymbol(RecvData) = -0.001;\n"
"	}\n"
"\n"
"#ifdef TIME_INFO3\n"
"        $starSymbol(endRecv) = gethrtime();\n"
"        $starSymbol(timeRecv) += $starSymbol(endRecv) - $starSymbol(beginRecv);\n"
"        printf(\"Cumulative time to receive %lld usec\\n\", $starSymbol(timeRecv) / 1000);\n"
"#endif\n");

CodeBlock CGCNOWamRecv :: runtime (
# line 255 "CGCNOWamRecv.pl"
"#ifdef TIME_INFO1\n"
"endRun = gethrtime();\n"
"timeRun = endRun - beginRun;\n"
"printf(\"Time to run %lld usec\\n\", timeRun / 1000);\n"
"#endif\n");

CGCNOWamRecv::CGCNOWamRecv ()
{
	setDescriptor("Receive star between NOW processors.");
	addPort(output.setPort("output",this,FLOAT));
	addState(numData.setState("numData",this,"1","number of tokens to be transferred",
# line 32 "CGCNOWamRecv.pl"
A_NONSETTABLE));
	addState(nodeIPs.setState("nodeIPs",this,"0 1 2 3","IP addresses of nodes in program.",
# line 39 "CGCNOWamRecv.pl"
A_NONSETTABLE));
	addState(numNodes.setState("numNodes",this,"0","Number of nodes in program.",
# line 46 "CGCNOWamRecv.pl"
A_NONSETTABLE));
	addState(pairNumber.setState("pairNumber",this,"0","Send Receive pair number for unique IP port.",
# line 53 "CGCNOWamRecv.pl"
A_NONSETTABLE));


}

void CGCNOWamRecv::wrapup() {
# line 263 "CGCNOWamRecv.pl"
addCode(runtime, "mainClose", "runTime");
		addCode("AM_Terminate();\n", "mainClose", "amTerminate");
}

void CGCNOWamRecv::initCode() {
# line 200 "CGCNOWamRecv.pl"
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

void CGCNOWamRecv::setup() {
# line 57 "CGCNOWamRecv.pl"
numData = output.numXfer();
}

void CGCNOWamRecv::go() {
# line 253 "CGCNOWamRecv.pl"
addCode(block);
}

// prototype instance for known block list
static CGCNOWamRecv proto;
static RegisterBlock registerBlock(proto,"NOWamRecv");
