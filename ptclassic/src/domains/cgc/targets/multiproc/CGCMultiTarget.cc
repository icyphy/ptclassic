static const char file_id[] = "CGCMultiTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: S. Ha

 A test multiTarget for CGC domain

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include "Error.h"
#include "CGDisplay.h"
#include "CGUtilities.h"
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

pt_ofstream feedback("cgcMulti_log");

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
	feedback << "\tpairing" << s->fullName() << " --> " << r->fullName()
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

	// machine idetifications
	if (identifyMachines() == FALSE) return;

	CGMultiTarget :: setup();
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
		machineInfo[i].localFlag = 0;
		// internet address calculation
		struct hostent* hp;
		if ((hp = gethostbyname(machineInfo[i].nm)) == NULL) {
			StringList errMsg;
			errMsg << "host name error: " << mname;
			Error :: abortRun(errMsg);
			return FALSE;
		}
		struct in_addr* ptr = (struct in_addr*) hp->h_addr_list[0];
		machineInfo[i].inetAddr = hashstring(inet_ntoa(*ptr));

		// monitoring.
		feedback << "machine(" << i << ") = ";
		feedback << machineInfo[i].nm << ": ";
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

	// host machine identify
	FILE* fp = popen("/bin/hostname", "r");
	if (fp == NULL) {
		Error::warn("popen error");
	} else {
		char line[40];
		if (fgets(line, 40, fp) != NULL) {
			char* myHost = makeLower(line);
			for (i = 0; i < pnum; i++) {
				char* temp = makeLower(machineInfo[i].nm);
				if (strncmp(myHost, temp, strlen(temp)) == 0) {
					machineInfo[i].localFlag = TRUE;
				} 
				LOG_DEL; delete temp;
			}
			LOG_DEL; delete myHost;
		}
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
	fileName << i << (const char*) filePrefix;
	fileName << i << ".c";
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
		int flag = machineInfo[i].localFlag;
		StringList fName;
		fName << i << (const char*) filePrefix;
		fName << i << ".c";
		char* tempName = writeFileName((const char*) fName);
		
		// remove file in the remove machine if any
		StringList cmd;
		if (!flag) { 
			cmd << "rsh " << machineInfo[i].nm << " ";
		}
		cmd << "rm -f " << fName;

		if (system(cmd)) {
			reportError(i,"can not remove file", fName);
			return FALSE;
		}
	
		// move the file
		cmd.initialize();
		cmd << "/bin/cat " << tempName;
		if (!flag) {
			cmd << " | rsh " << machineInfo[i].nm << " /bin/cat -";
		}
		cmd << " > " << fName;

		if (system(cmd)) {
			reportError(i,"can not write file", fName);
			return FALSE;
		}

		// compile code
		cmd.initialize();
		StringList oName;
		oName << i << (const char*) filePrefix << i;
		if (!flag) {
			cmd << "rsh " << machineInfo[i].nm << " ";
		}
		CGCTarget* t = (CGCTarget*) child(i);
		cmd << (const char*) t->compileLine((const char*) fName);
		cmd << " -o " << oName;

		if (system(cmd)) {
			reportError(i,"can not compile file", fName);
			return FALSE;
		}

		feedback << "compile " << machineInfo[i].nm << ": "
		         << fName << " is done!\n";
		feedback.flush();
	}
	return TRUE;
}

// -----------------------------------------------------------------------------
int CGCMultiTarget :: runCode() {
	for (int i = 0; i < nChildrenAlloc; i++) {
		StringList fName;
		fName << i << (const char*) filePrefix << i << " &";
		
		// run the code
		StringList cmd;
		if (machineInfo[i].localFlag == 0) {
			// use rshSystem method to preserve x environ.
			if (rshSystem(machineInfo[i].nm, (const char*) fName, 
			    0)) {
				reportError(i,"can not run file", fName);
				return FALSE;
			}
		} else {
			cmd << fName;
			if (system(cmd)) {
				reportError(i,"can not run file", fName);
				return FALSE;
			}
		}

		feedback << "run " << machineInfo[i].nm << ": "
		         << fName << " is done!\n";
		feedback.flush();
	}
	return TRUE;
}

void CGCMultiTarget :: reportError(int i, const char* msg, const char* fName) {
	StringList errMsg = machineInfo[i].nm;
	errMsg << ": " << msg << " " << fName;
	Error :: abortRun(errMsg);
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
"A test targets for parallel c-code generation");

static KnownTarget entry(targ,"unixMulti_C");

