static const char file_id[] = "MotorolaTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Joseph T. Buck, Jose Luis Pino, Thomas M. Parks

 Base target for Motorola DSP assembly code generation.
 Tracking memory usage in generated assembly code by Asawaree Kalavade
 and Brian L. Evans.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <ctype.h>

#include "StringList.h"
#include "Error.h"
#include "StringState.h"
#include "IntState.h"

#include "MotorolaTarget.h"
#include "MotorolaAttributes.h"
#include "CGUtilities.h"

const Attribute ANY = {0,0};

// a MotorolaMemory represents the X and Y memories of a 56000 or 96000.  
// It is derived from DualMemory.
MotorolaMemory :: MotorolaMemory(const char* x_map, const char* y_map) :
	DualMemory("l","x",A_XMEM,ANY,x_map,"y",A_YMEM,ANY,y_map)
{}

MotorolaTarget :: MotorolaTarget (const char* nam, const char* desc,
	const char* stype): AsmTarget(nam,desc,stype)
{
	initStates();
}

void MotorolaTarget :: initStates() {
	// Target parameters
	addState(xMemMap.setState("xMemMap", this, "0-4095", "X memory map"));
	addState(yMemMap.setState("yMemMap", this, "0-4095", "Y memory map"));
	addState(subFire.setState("subroutines?", this, "-1",
	    "Write star firings as subroutine calls."));
	addState(reportMemoryUsage.setState(
	    "show memory usage?", this, "NO",
	    "Report program and data memory usage" ) );

	// Initialize other data members
	inProgSection = 0;
 	mem = 0;
	assemblerOptions = "-A -B -L";
	resetMemoryUsage();
}

void MotorolaTarget :: resetMemoryUsage() {
	// Memory usage data members
	ProgramDataMemory = 0;
	XDataMemory = 0;
	YDataMemory = 0;
}

void MotorolaTarget :: setup() {
	int haltFlag = SimControl::haltRequested();
	if (haltFlag || ! galaxy()) {
	    Error::abortRun("Motorola Target has no galaxy defined.");
	    return;
	}

	LOG_DEL; delete mem;
	LOG_NEW; mem = new MotorolaMemory(xMemMap, yMemMap);

	// The Motorola 56000 assembler (Version 3.04) does not allow '.' in
	// the base file name, so we call ptSanitize to replace them with '_'
	// The Motorola 56000 simulator (Version 3.4) does not allow upper
	// case letters in file names, so we call makeLower
	if (filePrefix.null()) {
	    char* sanitizedFileName = makeLower(ptSanitize(galaxy()->name()));
	    filePrefix = sanitizedFileName;
	    delete [] sanitizedFileName;
	}

	AsmTarget::setup();
}

MotorolaTarget :: ~MotorolaTarget () {
	LOG_DEL; delete mem; mem = 0;
}

// copy constructor
MotorolaTarget :: MotorolaTarget (const MotorolaTarget& src) :
AsmTarget(src.name(),src.descriptor(),src.starType())
{
	initStates();
	copyStates(src);
}

// makeNew
Block* MotorolaTarget :: makeNew () const {
	LOG_NEW; return new MotorolaTarget(*this);
}

void MotorolaTarget::beginIteration(int repetitions, int) {
    if (repetitions == -1)		// iterate infinitely
	*defaultStream << targetNestedSymbol.push("LOOP") << "\n";
    else				// iterate finitely
	*defaultStream << "\tdo\t#" << repetitions << "," 
	       << targetNestedSymbol.push("LOOP") << "\n";
}

void MotorolaTarget::endIteration(int repetitions, int) {
	if (repetitions == -1)		// iterate infinitely
		*defaultStream << "\tjmp\t"<< targetNestedSymbol.pop() << "\n";
	else 				// iterate finitely
		*defaultStream << "\tnop\n; prevent two endloops in a row\n"
		       << targetNestedSymbol.pop() << "\n";
}

void MotorolaTarget::codeSection() {
	if (!inProgSection) {
		*defaultStream << "\torg p:\n";
		inProgSection = 1;
	}
}

void MotorolaTarget::disableInterrupts() {
	codeSection();
	*defaultStream << "	ori	#03,mr	;disable interrupts\n";
}

void MotorolaTarget::enableInterrupts() {
	codeSection();
	*defaultStream << "	andi	#$fc,mr	;enable interrupts\n";
}

void MotorolaTarget::saveProgramCounter() {
	codeSection();
	*defaultStream << targetNestedSymbol.push("SAVEPC") << "\tequ	*\n";
}

void MotorolaTarget::restoreProgramCounter() {
	codeSection();
	*defaultStream << "\torg	p:" << targetNestedSymbol.pop() << "\n";
}

void MotorolaTarget::orgDirective(const char* memName, unsigned addr) {
	*defaultStream << "\torg\t" << memName << ":" << int(addr) << "\n";
	inProgSection = 0;
}

void MotorolaTarget::writeInt(int val) {
	*defaultStream << "\tdc\t" << val << "\n";
}

void MotorolaTarget::writeFix(double val) {
	*defaultStream << "\tdc\t" << limitFix(val) << "\n";
}

void MotorolaTarget::writeFloat(double val) {
	*defaultStream << "\tdc\t" << val << "\n";
}

double MotorolaTarget::limitFix(double val) { 
	const double limit = 1.0 - 1.0/double(1<<23);
	if (val >= limit) return limit;
	else if (val <= -1.0) return -1.0;
	else return val;
}

StringList MotorolaTarget::comment(const char* msg, const char* begin,
const char* end, const char* cont) {
	if (begin==NULL) return CGTarget::comment(msg,"; ");
	else return CGTarget::comment(msg,begin,end,cont);
}

// Determine whether or not the star firing can be implemented with
// static code which could be enclosed in a loop or subroutine.
int staticCode(CGStar& star)
{
    BlockPortIter nextPort(star);
    CGPortHole* port;

    // Test the parameters of all ports.
    while ( (port = (CGPortHole*)nextPort++) != NULL)
    {
	// If the buffer size is not the same as the number of
	// particles transferred in one firing, then each firing must
	// read from a different location.
	if (port->numXfer() != port->bufSize())
	{
	    if ((port->attributes() & PB_CIRC) == 0) return FALSE;
	}
    }
    return TRUE;
}

void MotorolaTarget::writeFiring(Star& s, int level)
{
    CGStar& star = (CGStar&)s;
    int threshold = int(subFire);

    if (threshold >= 0 && star.reps() > threshold && staticCode(star))
    {
	if (star.index() < 0 && galaxy()) setStarIndices(*galaxy());

	StringList label = star.className();
	label << separator << star.index();

	// Generate procedure definition.
	if (procedures.put("",label))
	{
	    procedures << label << '\n';

	    CodeStream* previous = defaultStream;
	    defaultStream = &procedures;
	    AsmTarget::writeFiring(star, level);
	    defaultStream = previous;

	    // Some instructions are not allowed to precede rts
	    // so insert a nop to be safe.
	    procedures << "	nop\n";
	    procedures << "	rts\n";
	}

	// Invoke procedure.
	mainLoop << "	jsr " << label << '\n';
    }
    else
    {
	AsmTarget::writeFiring(s,level);
    }
}

#define PMEMORY_INDEX           0
#define XMEMORY_INDEX           1
#define YMEMORY_INDEX           2
#define NOT_MEMORY_INDEX        -1

#define MAXLINE	128

// memoryRequirements
// Parse the .lod file created by the Motorola assembler -A and -B options
// from an assembly language file to compute the program and data memory usage
static int memoryRequirements(const char* filename, int* words) {
	int memTypeIndex = -1;
	char buffer[MAXLINE];

	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return(FALSE);

	while (! feof(fp) && fgets(buffer, MAXLINE, fp) ) {
	    buffer[MAXLINE - 1] = 0;		    /* null terminate string */
	    if (buffer[0] == '_') {
		if ( strncmp(buffer, "_DATA ", 6) == 0 ) {
		    switch(buffer[6]) {
		      case 'P':
			memTypeIndex = PMEMORY_INDEX;
			break;
		      case 'X':
			memTypeIndex = XMEMORY_INDEX;
			break;
		      case 'Y':
			memTypeIndex = YMEMORY_INDEX;
			break;
		      default:
			memTypeIndex = NOT_MEMORY_INDEX;
		    }
		}
	    }
	    else if (memTypeIndex != NOT_MEMORY_INDEX) {
		int i = 0, dataflag = TRUE;
		char* str = buffer;
		while ( dataflag && *str && i < 6 ) {
		    dataflag = isalnum(*str);
		    i++;
		    str++;
		}
		if ( dataflag ) {
		    int length = strlen(buffer);
		    words[memTypeIndex] += (length+1)/7;
		}
	    }
	    buffer[0] = 0;
	}

	fclose(fp);
	return(TRUE);
}

int MotorolaTarget::computeMemoryUsage() {
        int words[3] = {0, 0, 0};
	resetMemoryUsage();

	// Figure out where the .lod file is
	// If it is on a remote machine, then copy it over
	int deleteFlag = FALSE;
	StringList loadfilename;
	loadfilename << filePrefix << ".lod";
	StringList loadpathname =
		findLocalFileName(targetHost, destDirectory,
				  loadfilename, deleteFlag);
	if ( loadpathname.length() == 0 ) return FALSE;

	// Compute the memory requirements from the .lod file
	int valid = memoryRequirements(loadpathname, words);
	if (valid) {
	    ProgramDataMemory = words[PMEMORY_INDEX];
	    XDataMemory = words[XMEMORY_INDEX];
	    YDataMemory = words[YMEMORY_INDEX];
	}
	else {
	    Error::warn("Could not open ", loadpathname, " for reading");
	}

	if ( deleteFlag ) {
	    StringList removeCommand = "rm ";
	    removeCommand << loadpathname;
	    StringList errmsg = "Could not remove ";
	    errmsg << loadpathname;
	    systemCall(removeCommand, errmsg);
	}

	return valid;
}

const char* MotorolaTarget::memoryUsageString() {
	MemoryUsageString = "Motorola 56000 memory usage in words";
	MemoryUsageString << ": program = " << programMemoryUsage()
			  << ", x data = " << xdataMemoryUsage()
			  << ", y data = " << ydataMemoryUsage();
	return MemoryUsageString;
}
