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
#include "CGDisplay.h"
#include "CGCStar.h"
#include "KnownTarget.h"
#include "CGCMultiTarget.h"
#include "CGCTarget.h"
#include "CGCSpread.h"
#include "CGCCollect.h"
#include "CGCUnixSend.h"
#include "CGCUnixReceive.h"
#include <ctype.h>
#include <stdio.h>
#include <netdb.h>			// for struct hostent

extern "C" {
	#include <netinet/in.h>			// for struct in_addr
	char* inet_ntoa(struct in_addr);
}

// stream for logging information.  It is opened by the setup method.

static pt_ofstream feedback;

// -----------------------------------------------------------------------------	
CGCMultiTarget::CGCMultiTarget(const char* name,const char* starclass,
		   const char* desc) : CGSharedBus(name,starclass,desc) {

	addState(doCompile.setState("doCompile",this,"NO",
		"disallow compiling during development stage"));

	// port_number
	addState(portNumber.setState("portNumber",this,"7654",
		"hopefully will not collide any port_numbers taken"));

	// specify machine names
	addState(machineNames.setState("machineNames",this,"herschel",
		"machine names (separated by a comma)"));
	addState(nameSuffix.setState("nameSuffix",this,".berkeley.edu",
		"common suffix of machine names(e.g. .berkeley.edu)"));

	// make some states invisible
	childType.setAttributes(A_NONSETTABLE);
}

CGCMultiTarget :: ~CGCMultiTarget() {
	LOG_DEL; delete [] machineInfo;
}

// -----------------------------------------------------------------------------
Target* CGCMultiTarget :: createChild() {
	LOG_NEW; return new CGCTarget("default-C", "CGCStar",
	"C code target for a test CGC multi Target.");
}

// -----------------------------------------------------------------------------
DataFlowStar* CGCMultiTarget :: createSend(int from, int to, int num) {
	LOG_NEW; CGCUnixSend* s = new CGCUnixSend;
	return s;
}

DataFlowStar* CGCMultiTarget :: createReceive(int from, int to, int num) {
	LOG_NEW; CGCUnixReceive* r =  new CGCUnixReceive;
	return r;
}

DataFlowStar* CGCMultiTarget :: createSpread() {
	LOG_NEW; return (new CGCSpread);
}

DataFlowStar* CGCMultiTarget :: createCollect() {
	LOG_NEW; return (new CGCCollect);
}

void CGCMultiTarget :: pairSendReceive(DataFlowStar* s, DataFlowStar* r) {
	feedback << "\tpairing " << s->fullName() << " --> " << r->fullName()
		<< "\n"; feedback.flush();
	CGCUnixSend* cs = (CGCUnixSend*) s;
	CGCUnixReceive* cr = (CGCUnixReceive*) r;

	// for each child_target
	CGCTarget* ts = (CGCTarget*) cs->myTarget();
	CGCTarget* tr = (CGCTarget*) cr->myTarget();

	// set the IPC handler names
	// for sender
	StringList handlerName = "ipcHandler";
	handlerName << '_' << cs->name();
	cs->IPCHandlerName.setInitValue(hashstring(handlerName));
	StringList callHandler;
	callHandler << handlerName << "();\n";
	ts->getStream("mainInit")->put((const char*) callHandler);
	// for receiver
	handlerName = "ipcHandler";
	handlerName << '_' << cr->name();
	cr->IPCHandlerName.setInitValue(hashstring(handlerName));
	callHandler.initialize();
	callHandler << handlerName << "();\n";
	tr->getStream("mainInit")->put((const char*) callHandler);

	// Set the port_number and machine address.
	// port_number
	cs->hostPort.setInitValue(currentPort);
	cr->hostPort.setInitValue(currentPort++);
	// machine address
	int dix = machineId(cr->myTarget());
	if (dix < 0) {
		Error :: abortRun(*cr, "no child target for this star.");
		return;
	}
	cs->hostAddr.setInitValue(machineInfo[dix].inetAddr);
	cr->hostAddr.setInitValue(machineInfo[dix].inetAddr);
}

int CGCMultiTarget :: machineId(Target* t) {
	for (int i = 0; i < nChildrenAlloc; i++) {
		if (child(i) == t) return i;
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

	// machine name setup
	for (int i = 0; i < nChildrenAlloc; i++) {
		CGCTarget* t = (CGCTarget*) child(i);
		t->setHostName(machineInfo[i].nm);
	}
	feedback.flush();
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

			///////////////////
			// wrapup
			///////////////////

void CGCMultiTarget :: wrapup() {
	if (galaxy()->parent() == 0)	 wormLoadCode();
}

void CGCMultiTarget :: addProcessorCode(int i, const char* s) {
	StringList code = s;
	StringList fileName;
	Target* t = child(i);
	fileName << t->name() << ".c";
	char* codeFileName = writeFileName((const char*) fileName);
	display(code,codeFileName);
}

// -----------------------------------------------------------------------------
			///////////////////
			// wormLoadCode
			///////////////////

int CGCMultiTarget::wormLoadCode() {

	if (int(doCompile) == 0) return TRUE;
	if (compileCode()) runCode();

	// done
	if(Scheduler::haltRequested()) return FALSE;
	return TRUE;
}
// -----------------------------------------------------------------------------
			///////////////////
			// compile and run
			///////////////////
// ship the codes and run them.
int CGCMultiTarget :: compileCode() {
	for (int i = 0; i < nChildrenAlloc; i++) {
		CGCTarget* t = (CGCTarget*) child(i);
		if (t->compileCode() == FALSE) return FALSE;
		feedback << "compile " << t->hostName() << ": "
		         << t->name() << ".c is done!\n";
		feedback.flush();
	}
	return TRUE;
}

// -----------------------------------------------------------------------------
int CGCMultiTarget :: runCode() {
	for (int i = 0; i < nChildrenAlloc; i++) {
		CGCTarget* t = (CGCTarget*) child(i);
		if (t->runCode() == FALSE) return FALSE;

		feedback << "run " << t->hostName() << ": "
		         << t->name() << " is done!\n";
		feedback.flush();
	}
	return TRUE;
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
// -----------------------------------------------------------------------------
ISA_FUNC(CGCMultiTarget,CGMultiTarget);

static CGCMultiTarget targ("unixMulti_C","CGCStar",
"A test target for parallel C code generation");

static KnownTarget entry(targ,"unixMulti_C");

