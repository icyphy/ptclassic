static const char file_id[] = "CGCNOWamTarget.cc";
/******************************************************************
@Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
 
Programmer: Patrick Warner
Version: $Id$
 
This file is modelled after CGCMultiTarget.cc.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

// Ptolemy kernel include files
#include "pt_fstream.h"         // define pt_ofstream class
#include "StringList.h"         // define StringList class
#include "Error.h"              // define Error class
#include "Block.h"              // needed for KnownTarget.h
#include "Target.h"             // needed for KnownTarget.h
#include "KnownTarget.h"        // define KnownTarget to register target

// Ptolemy domain include files
#include "CGUtilities.h"
#include "CGCStar.h"
#include "CGCNOWamTarget.h"
#include "CGCTarget.h"
#include "CGCSpread.h"
#include "CGCCollect.h"
#include "CGCNOWamSend.h"
#include "CGCNOWamRecv.h"

// Standard include files
#include <ctype.h>		// define isspace macro
#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>		// define gethostbyname and struct hostent
#include <arpa/inet.h>		// Sol2 needs this for inet_addr()

extern "C" {
#include <netinet/in.h>                 // for struct in_addr
char* inet_ntoa(struct in_addr);
}

// stream for logging information.  It is opened by the setup method.
static pt_ofstream feedback;

// Constructor
CGCNOWamTarget::CGCNOWamTarget(const char* name, const char* starclass,
			       const char* desc, const char* assocDomain) :
CGMultiTarget(name,starclass,desc,assocDomain) {

	// specify machine names
	addState(machineNames.setState("machineNames", this, "lucky, babbage",
		"machine names (separated by commas)"));
	addState(nameSuffix.setState("nameSuffix",this,"",
		"common suffix of machine names such as .berkeley.edu"));

        // override target parameter values
	childType.setInitValue("default-CGC");
	compileFlag.setInitValue("NO");
	runFlag.setInitValue("NO");
	displayFlag.setInitValue("YES");
	resources.setInitValue("");

	// Initialize data members
	machineInfo = 0;
	pairs = 0;
	baseNum = 0;
	mapArray = 0;
	replicateFlag = 0;
}

CGCNOWamTarget :: ~CGCNOWamTarget() {
//	if (inherited() == 0) {
		LOG_DEL; delete [] machineInfo;
//	}
}

DataFlowStar* CGCNOWamTarget :: createSend(int, int, int) {
	LOG_NEW; CGCNOWamSend* s = new CGCNOWamSend;
	return s;
}

DataFlowStar* CGCNOWamTarget :: createReceive(int, int, int) {
	LOG_NEW; CGCNOWamRecv* r =  new CGCNOWamRecv;
	return r;
}

DataFlowStar* CGCNOWamTarget :: createSpread() {
	LOG_NEW; return (new CGCSpread);
}

DataFlowStar* CGCNOWamTarget :: createCollect() {
	LOG_NEW; return (new CGCCollect);
}

void CGCNOWamTarget :: pairSendReceive(DataFlowStar* s, DataFlowStar* r) {
	feedback << "\tpairing " << s->fullName() << " --> " << r->fullName()
		 << "\n"; feedback.flush();
	CGCNOWamSend* cs = (CGCNOWamSend*) s;
	CGCNOWamRecv* cr = (CGCNOWamRecv*) r;
        int pnum = (int)nprocs;
 
        cs->numNodes.setInitValue(pnum);
        cr->numNodes.setInitValue(pnum);

        cs->pairNumber.setInitValue(pairs);
        cr->pairNumber.setInitValue(pairs);
	pairs++;

        StringList nodeAddrs = " ";
        for (int i = 0; i < pnum; i++) {
                nodeAddrs << (int)(machineInfo[i].inetAddr) << " ";
        }
        cs->nodeIPs.setInitValue(hashstring(nodeAddrs));
        cr->nodeIPs.setInitValue(hashstring(nodeAddrs));

	cs->partner = cr;
}

void CGCNOWamTarget :: setMachineAddr(CGStar* s, CGStar* r) {
	CGCNOWamSend* cs = (CGCNOWamSend*) s;
	CGCNOWamRecv* cr = (CGCNOWamRecv*) r;

	CGTarget* tg = cr->cgTarget();
	if (replicateFlag && mapArray) {
		int six = -1;
		int rix = -1;
		CGTarget* sg = cs->cgTarget();
		int numMatch = 0;
		for (int i = 0; i < mapArray->size(); i++) {
			CGTarget* temp = (CGTarget*) child(mapArray->elem(i));
			if (temp == sg) {
				six = i; numMatch++;
			}
			else if (temp == tg) {
				rix = i; numMatch++;
			}
			if (numMatch >= 2) break;
		}
		if ((six < 0) || (rix < 0) || (numMatch != 2)) {
			Error :: abortRun("setMachineAddr failed.");
			return;
		}
		int zz = six /baseNum;
		if ((rix / baseNum) != zz) {
			int newIx = zz * baseNum + (rix % baseNum);
			tg = (CGTarget*) child(mapArray->elem(newIx));
		}
	}
				
	// machine address
	int dix = machineId(tg);
	if (dix < 0) {
		Error :: abortRun(*cr, "no child target for this star.");
		return;
	}
	cs->hostAddr.setInitValue(machineInfo[dix].virtNode);
}

int CGCNOWamTarget :: machineId(Target* t) {
	for (int i = 0; i < nChildrenAlloc; i++) {
		if (child(i) == t) return i;
	}
	return -1;
}
	
// setup
void CGCNOWamTarget :: setup() {
//	if (inherited()) {
//		CGCNOWamTarget* orgT = (CGCNOWamTarget*) child(0)->parent();
//		machineInfo = orgT->getVirtualInfo();
//		CGMultiTarget :: setup();
//		return;
//	}

	// all runs will append to the same file.
	// FIXME: should not be done this way.
	if (!feedback) {
                // Put the log file in the destination directory
                StringList pathname = (const char*) destDirectory;
                pathname << "/" << "CGCNOWam_log";

                // Expand environment variables in pathname and open the file
                feedback.open(pathname);
	}
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

int CGCNOWamTarget :: identifyMachines() {
	// construct machine information table
	int pnum = int(nprocs);
	LOG_NEW; machineInfo = new VirtualInfo[pnum];

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
		machineInfo[i].inetAddr = inet_addr(hashstring(inet_ntoa(*ptr)));
		machineInfo[i].virtNode = i;

		// monitoring.
		feedback << "machine(" << i << ") = ";
		feedback << mname << ": ";
		feedback << machineInfo[i].virtNode << "\n";
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

// Return a copy of itself
Block* CGCNOWamTarget :: makeNew() const {
	LOG_NEW; return new CGCNOWamTarget(name(),starType(),descriptor(),
					   getAssociatedDomain());
}

// wormhole interface method
int CGCNOWamTarget :: receiveWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceSendData();
	return TRUE;
}

// wormhole interface method
int CGCNOWamTarget :: sendWormData(PortHole& p) {
	CGPortHole& cp = *(CGPortHole*)&p;
	cp.forceGrabData();
	return TRUE;
}

ISA_FUNC(CGCNOWamTarget,CGMultiTarget);

static CGCNOWamTarget targ("CGCNOWam","CGCStar",
"A NOW target for parallel C code generation");

static KnownTarget entry(targ,"CGCNOWam");
