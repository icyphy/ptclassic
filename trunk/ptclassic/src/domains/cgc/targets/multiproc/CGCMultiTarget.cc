static const char file_id[] = "CGCMultiTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer: S. Ha

 A test multiTarget for CGC domain

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CodeBlock.h"
#include "CGUtilities.h"
#include "CGCStar.h"
#include "KnownTarget.h"
#include "CGCMultiTarget.h"
#include "CGCTarget.h"
#include "CGCSpread.h"
#include "CGCCollect.h"
#include "CGCUnixSend.h"
#include "CGCUnixReceive.h"
#include "CGCMacroStar.h"
#include "CGCDDFCode.h"
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>			// for struct hostent

extern "C" {
#include <netinet/in.h>			// for struct in_addr
	char* inet_ntoa(struct in_addr);
}

// stream for logging information.  It is opened by the setup method.

static pt_ofstream feedback;

// ---------------------------- code blocks --------------------------------

CodeBlock ipcAcceptHandler (
"\n"
"int ipcAcceptHandler(hostPort) \n"
"	int hostPort; {\n"
"	int len, sId, newSId;\n"
"	struct sockaddr_in addr, far_addr;\n"
"\n"
"	/* Open a TCP socket (an Internet stream socket) */\n"
"	if ((sId = socket(AF_INET, SOCK_STREAM, 0)) < 0) {\n"
"		printf(\"cannot open stream socket for %d.\\n\", hostPort);\n"
"		exit(1);\n"
"	}\n"
"\n"
"	/* Bind local address */\n"
"	bzero((char*) &addr, sizeof(addr));\n"
"	addr.sin_family = AF_INET;\n"
"	addr.sin_addr.s_addr = htonl(INADDR_ANY);\n"
"	addr.sin_port = htons(hostPort);\n"
"\n"
"	if (bind(sId,(struct sockaddr*) &addr,sizeof(addr)) < 0) {\n"
"		printf(\"bind on port $val(hostPort) failed.\\n\");\n"
"		exit(1);\n"
"	}\n"
"\n"
"	listen(sId, 5);\n"
"\n"
"	/* wait for connection from a sender */\n"
"	len = sizeof(far_addr);\n"
"	newSId = accept(sId, (struct sockaddr*) &far_addr, &len);\n"
"	if (newSId < 0) {\n"
"		printf(\"accept error on port %d.\\n\", hostPort);\n"
"		exit(1);\n"
"	}\n"
"	printf(\"accept connection on port %d.\\n\", hostPort);\n"
"	return newSId;\n"
"}	\n");


CodeBlock ipcConnectHandler (
"\n"
"int ipcConnectHandler(hostPort, hostAddr) \n"
"	int hostPort; \n"
"	char* hostAddr; {\n"
"	int sId, timeout, localLoop;\n"
"	struct sockaddr_in addr;\n"
"\n"
"	/* connect to the server */\n"
"	for (timeout = 0; ; timeout++) {\n"
"		/* Open a TCP socket (an Internet stream socket */\n"
"		if ((sId = socket(AF_INET, SOCK_STREAM, 0)) < 0) {\n"
"			printf(\"cannot open socket for %d.\\n\", hostPort);\n"
"			exit(1);\n"
"		}\n"
"\n"
"		/* Fill in the structure addr with the address of the server\n"
"	   	   that we want to connect with */\n"
"		bzero((char*) &addr, sizeof(addr));\n"
"		addr.sin_family = AF_INET;\n"
"		addr.sin_addr.s_addr = inet_addr(hostAddr);\n"
"		addr.sin_port = htons(hostPort);\n"
"\n"
"		if (!connect(sId, (struct sockaddr*) &addr, sizeof(addr))) \n"
"			break;\n"
"		sleep(1);\n"
"		if (timeout > 10000) {\n"
"			printf(\"cannot connect to server on port %d.\\n\", hostPort);\n"
"			exit(1);\n"
"		}\n"
"		close(sId);\n"
"	}\n"
"	return sId;"
"}	\n");

// -----------------------------------------------------------------------------	
CGCMultiTarget::CGCMultiTarget(const char* name,const char* starclass,
		   const char* desc) : CGSharedBus(name,starclass,desc) {

	// port_number
	addState(portNumber.setState("portNumber",this,"7654",
		"hopefully will not collide any port_numbers taken"));

	// specify machine names
	addState(machineNames.setState("machineNames",this,"herschel",
		"machine names (separated by a comma)"));
	addState(nameSuffix.setState("nameSuffix",this,"",
		"common suffix of machine names(e.g. .berkeley.edu)"));

	// make some states invisible
	childType.setInitValue("default-CGC");

	machineInfo = 0;
	currentPort = 0;
	baseNum = 0;
	mapArray = 0;
	replicateFlag = 0;
}

CGCMultiTarget :: ~CGCMultiTarget() {
	LOG_DEL; delete [] machineInfo;
}

void CGCMultiTarget :: installDDF() {
	LOG_NEW; ddfcode = new CGCDDFCode(this);
}

// -----------------------------------------------------------------------------
DataFlowStar* CGCMultiTarget :: createSend(int, int, int) {
	LOG_NEW; CGCUnixSend* s = new CGCUnixSend;
	return s;
}

DataFlowStar* CGCMultiTarget :: createReceive(int, int, int) {
	LOG_NEW; CGCUnixReceive* r =  new CGCUnixReceive;
	return r;
}

DataFlowStar* CGCMultiTarget :: createSpread() {
	LOG_NEW; return (new CGCSpread);
}

DataFlowStar* CGCMultiTarget :: createCollect() {
	LOG_NEW; return (new CGCCollect);
}

CGStar* CGCMultiTarget :: createMacro(CGStar* s, int pix, int invoc,
	int flag) {
	LOG_NEW; CGCMacroStar* newS = new CGCMacroStar;
	newS->setProp(s,pix,invoc,flag);
	return newS;
}

void CGCMultiTarget :: pairSendReceive(DataFlowStar* s, DataFlowStar* r) {
	feedback << "\tpairing " << s->fullName() << " --> " << r->fullName()
		<< "\n"; feedback.flush();
	CGCUnixSend* cs = (CGCUnixSend*) s;
	CGCUnixReceive* cr = (CGCUnixReceive*) r;

	// for each child_target
	CGCTarget* ts = (CGCTarget*) cs->cgTarget();
	CGCTarget* tr = (CGCTarget*) cr->cgTarget();

	// get Socket id
	int six = machineId(ts);
	int dix = machineId(tr);
	StringList sid = "sId_";
	if (six < dix) sid << six << "_" << dix;
	else	       sid << dix << "_" << six;

	// setup the state
	cs->socketId.setInitValue(hashstring(sid));
	cr->socketId.setInitValue(hashstring(sid));
}

int CGCMultiTarget :: machineId(Target* tg) {
	for (int i = 0; i < nChildrenAlloc; i++) {
		if (machineInfo[i].t == tg) return i;
	}
	return -1;
}
	
// -----------------------------------------------------------------------------
			///////////////////
			// setup
			///////////////////

void CGCMultiTarget :: setup() {
	currentPort = int(portNumber);

	// all runs will append to the same file.
	// FIXME: should not be done this way.
	if (!feedback) feedback.open("cgcMulti_log");
	if (!feedback) return;

	// machine idetifications
	if (identifyMachines() == FALSE) return;

	CGMultiTarget :: setup();

	// set up socket connection
	for (int i = 0; i < nChildrenAlloc; i++) {
		CGCTarget* t = (CGCTarget*) child(i);
		setupSocketConnection(t, i);
	}
	feedback.flush();
}

void CGCMultiTarget :: prepareChildren() {
	CGMultiTarget :: prepareChildren();

	// machine name setup
	for (int i = 0; i < nChildrenAlloc; i++) {
		CGCTarget* t = (CGCTarget*) child(i);
		machineInfo[i].t = t;
		t->setHostName(machineInfo[i].nm);
	}
}
	
int CGCMultiTarget :: identifyMachines() {
	// construct machine information table
	int pnum = int(nprocs);
	LOG_NEW; machineInfo = new MachineInfo[pnum];

	feedback << " ** machine identification ** \n";
	const char* p = machineNames;
	int i = 0;
	while (*p) {
		char buf[80], *b = buf;
		while (isspace(*p)) p++;
		while ((*p != ',') && (*p != 0)) {
			if (isspace(*p)) p++;
			else *b++ = *p++; 
		}
		if (*p == ',') p++;
		*b = 0;		// end of string.
		// record names
		StringList mname = (const char*) buf;
		mname << (const char*) nameSuffix;
		machineInfo[i].nm = hashstring((const char*) mname);
		// internet address calculation
		struct hostent* hp;
		if ((hp = gethostbyname((const char*) mname)) == NULL) {
			StringList errMsg;
			errMsg << "host name error: " << mname;
			Error :: abortRun(errMsg);
			return FALSE;
		}
		struct in_addr* ptr = (struct in_addr*) hp->h_addr_list[0];
		machineInfo[i].inetAddr = hashstring(inet_ntoa(*ptr));

		// monitoring.
		feedback << "machine(" << i << ") = ";
		feedback << mname << ": ";
		feedback << machineInfo[i].inetAddr << "\n";
		feedback.flush();

		i++;
	}
	
	// check if the number of processors and the machine names are matched.
	if (i != pnum) {
		Error :: abortRun(*this, "The number of processors and",
			" the number of machine names are not equal.");
		return FALSE;
	}
	return TRUE;
}

void CGCMultiTarget :: setupSocketConnection(CGCTarget* t, int i) {

	// add include files to the CGCTarget
	CodeStream* inc = t->getStream("include");
	inc->put("#include <stdio.h>\n", "<stdio.h>");
	inc->put("#include <sys/types.h>\n", "<sys/types.h>");
	inc->put("#include <sys/socket.h>\n", "<sys/socket.h>");
	inc->put("#include <netinet/in.h>\n", "<netinet/in.h>");
	inc->put("#include <arpa/inet.h>\n", "<arpa/inet.h>");

	// add socket connection procedure
	CodeStream* prCode = t->getStream("procedure");
	if (i) prCode->put(ipcConnectHandler, "ipcConnectHandler");
	if (i == nChildrenAlloc - 1) return;
	prCode->put(ipcAcceptHandler, "ipcAcceptHandler");

	// add socket connection with other machines
	for (int k = i+1; k < nChildrenAlloc; k++) {
		StringList sid = "sId_";
		sid << i << "_" << k;		// between i and k

		// add connection
		CodeStream* glob = t->getStream("globalDecls");
		StringList temp = "int ";
		temp << sid << ";\n";
		glob->put(temp, temp);

		glob = t->getStream("mainClose");
		StringList temp2 = "";
		temp2 << "\tclose(" << sid << ");\n";
		glob->put(temp2);

		CodeStream* com = t->getStream("commInit");
		temp2 = "";
		temp2 << "\t" << sid << " = " << "ipcAcceptHandler(";
		temp2 << currentPort << ");\n";
		com->put(temp2);

		// similar thing to the partner
		CGCTarget* par = (CGCTarget*) child(k);
		glob = par->getStream("globalDecls");
		glob->put(temp, temp);

		glob = par->getStream("mainClose");
		temp2 = "";
		temp2 << "\tclose(" << sid << ");\n";
		glob->put(temp2);

		com = par->getStream("commInit");
		temp2 = "";
		temp2 << "\t" << sid << " = " << "ipcConnectHandler(";
		temp2 << currentPort << ", \"";
		temp2 << machineInfo[i].inetAddr << "\");\n";
		com->put(temp2);
	}
}
		
// -----------------------------------------------------------------------------
Block* CGCMultiTarget :: makeNew() const {
	LOG_NEW; return new CGCMultiTarget(name(),starType(),descriptor());
}
// -----------------------------------------------------------------------------
			/////////////////////////////
			// wormhole interface method
			/////////////////////////////

int CGCMultiTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceSendData();
	return TRUE;
}
// -----------------------------------------------------------------------------
int CGCMultiTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceGrabData();
	return TRUE;
}


			///////////////////
			// CGDDF support
			///////////////////

void CGCMultiTarget :: prepCode(Profile* pf, int numP, int numChunk) {
	// call prepCode routine
	for (int i = 0; i < numP; i++) {
		int pId = pf->assignedTo(1, i);
		CGTarget* from = (CGTarget*) child(pId);
		CodeStream* foo = from->getStream("commInit");
		if (foo->length() == 0) continue;
		for (int j = 1; j < numChunk; j++) {
			int k = i + j * numP;
			int tId = pf->assignedTo(1, k);
			CGTarget* to = (CGTarget*) child(tId);
			to->getStream("commInit")->put(*foo);
		}
	}
	baseNum = numP;
	mapArray = pf->assignArray(1);
}

// ------------------------------------------------------------------------

ISA_FUNC(CGCMultiTarget,CGMultiTarget);

static CGCMultiTarget targ("unixMulti_C","CGCStar",
"A test target for parallel C code generation");

static KnownTarget entry(targ,"unixMulti_C");

