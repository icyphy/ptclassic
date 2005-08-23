static const char file_id[] = "CGCNOWamSend.pl";
// .cc file generated from CGCNOWamSend.pl by ptlang
/*
Copyright (c) 1995-1997 The Regents of the University of California
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCNOWamSend.h"

const char *star_nm_CGCNOWamSend = "CGCNOWamSend";

const char* CGCNOWamSend :: className() const {return star_nm_CGCNOWamSend;}

ISA_FUNC(CGCNOWamSend,CGCStar);

Block* CGCNOWamSend :: makeNew() const { LOG_NEW; return new CGCNOWamSend;}

CodeBlock CGCNOWamSend :: converttype (
# line 69 "CGCNOWamSend.pl"
"typedef union ints_or_double {\n"
"        int asInt[2];\n"
"        double asDouble;\n"
"} convert;\n");

CodeBlock CGCNOWamSend :: timeincludes (
# line 75 "CGCNOWamSend.pl"
"#ifdef TIME_INFO1\n"
"#include <sys/time.h>\n"
"#endif\n");

CodeBlock CGCNOWamSend :: replyHandler (
# line 80 "CGCNOWamSend.pl"
"void reply_handler(void *source_token, int d1, int d2, int d3, int d4)\n"
"{\n"
"}\n");

CodeBlock CGCNOWamSend :: errorHandler (
# line 85 "CGCNOWamSend.pl"
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

CodeBlock CGCNOWamSend :: amdecls (
# line 128 "CGCNOWamSend.pl"
"en_t global;\n"
"eb_t bundle;\n");

CodeBlock CGCNOWamSend :: timedecls (
# line 132 "CGCNOWamSend.pl"
"#ifdef TIME_INFO1\n"
"hrtime_t timeRun;\n"
"hrtime_t beginRun;\n"
"hrtime_t endRun;\n"
"#endif\n");

CodeBlock CGCNOWamSend :: stardecls (
# line 139 "CGCNOWamSend.pl"
"#ifdef TIME_INFO2\n"
"hrtime_t $starSymbol(timeSend);\n"
"hrtime_t $starSymbol(beginSend);\n"
"hrtime_t $starSymbol(endSend);\n"
"#endif\n"
"en_t *$starSymbol(endname);\n"
"ea_t $starSymbol(endpoint);\n"
"int $starSymbol(i);\n");

CodeBlock CGCNOWamSend :: aminit (
# line 149 "CGCNOWamSend.pl"
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

CodeBlock CGCNOWamSend :: timeinit (
# line 161 "CGCNOWamSend.pl"
"#ifdef TIME_INFO2\n"
"$starSymbol(timeSend) = 0.0;\n"
"#endif\n"
"#ifdef TIME_INFO1\n"
"beginRun = gethrtime();\n"
"#endif\n");

CodeBlock CGCNOWamSend :: starinit (
# line 169 "CGCNOWamSend.pl"
"#ifdef TIME_INFO\n"
"$starSymbol(timeSend) = 0.0;\n"
"#endif\n"
"if (AM_AllocateKnownEndpoint(bundle, &$starSymbol(endpoint), &$starSymbol(endname), HARDPORT + $val(pairNumber)) != AM_OK) {\n"
"\n"
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
"if (AM_SetHandler($starSymbol(endpoint), 1, reply_handler) != AM_OK) {\n"
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

CodeBlock CGCNOWamSend :: block (
# line 234 "CGCNOWamSend.pl"
"	int i, pos, check;\n"
"	convert myData;\n"
"	\n"
"#ifdef TIME_INFO2\n"
"        $starSymbol(beginSend) = gethrtime();\n"
"#endif\n"
"\n"
"	for (i = 0; i < $val(numData); i++) {\n"
"		pos = $val(numData) - 1 + i;\n"
"		myData.asDouble = $ref(input,pos);\n"
"                check = AM_Request4($starSymbol(endpoint), $val(hostAddr), 2, myData.asInt[0], myData.asInt[1], 0, 0);      \n"
"		if (check == -1) {\n"
"			printf(\"Error in sending data\\n\");\n"
"		}\n"
"	}\n"
"\n"
"#ifdef TIME_INFO2\n"
"        $starSymbol(endSend) = gethrtime();\n"
"        $starSymbol(timeSend) += $starSymbol(endSend) - $starSymbol(beginSend);\n"
"        printf(\"Cumulative time to send %lld usec\\n\", $starSymbol(timeSend) / 1000);\n"
"#endif\n"
"\n");

CodeBlock CGCNOWamSend :: runtime (
# line 262 "CGCNOWamSend.pl"
"#ifdef TIME_INFO1\n"
"endRun = gethrtime();\n"
"timeRun = endRun - beginRun;\n"
"printf(\"Time to run %lld usec\\n\", timeRun / 1000);\n"
"#endif\n");

CGCNOWamSend::CGCNOWamSend ()
{
	setDescriptor("Send star between NOWam processors.");
	addPort(input.setPort("input",this,FLOAT));
	addState(numData.setState("numData",this,"1","number of tokens to be transferred",
# line 32 "CGCNOWamSend.pl"
A_NONSETTABLE));
	addState(nodeIPs.setState("nodeIPs",this,"0 1 2 3","IP addresses of nodes in program.",
# line 39 "CGCNOWamSend.pl"
A_NONSETTABLE));
	addState(hostAddr.setState("hostAddr",this,"0","Host virtual node for server",
# line 46 "CGCNOWamSend.pl"
A_NONSETTABLE));
	addState(numNodes.setState("numNodes",this,"0","Number of nodes in program.",
# line 53 "CGCNOWamSend.pl"
A_NONSETTABLE));
	addState(pairNumber.setState("pairNumber",this,"0","Send Receive pair number for unique IP port.",
# line 60 "CGCNOWamSend.pl"
A_NONSETTABLE));


}

void CGCNOWamSend::wrapup() {
# line 271 "CGCNOWamSend.pl"
addCode(runtime, "mainClose", "runTime");
                addCode("AM_Terminate();\n", "mainClose", "amTerminate");
}

void CGCNOWamSend::initCode() {
# line 204 "CGCNOWamSend.pl"
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

void CGCNOWamSend::setup() {
# line 66 "CGCNOWamSend.pl"
numData = input.numXfer();
}

void CGCNOWamSend::go() {
# line 259 "CGCNOWamSend.pl"
addCode(block);
}

// prototype instance for known block list
static CGCNOWamSend proto;
static RegisterBlock registerBlock(proto,"NOWamSend");
