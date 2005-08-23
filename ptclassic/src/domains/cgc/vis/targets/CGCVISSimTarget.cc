#ifdef lint
static const char file_id[] = "$Id$";
#endif /*lint*/
/******************************************************************
Version identification:
@(#)CGCVISTarget.cc	1.4 06/29/96
 
Copyright (c) 1995-1996  The Regents of the University of California.
All Rights Reserved.
 
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
 
 Programmer: William Chen
 
*******************************************************************/
#include "CGCTarget.h"
#include "CGCVISSimTarget.h"
#include "Error.h"
#include "KnownTarget.h"
#include "CGUtilities.h"
// -----------------------------------------------------------------------------	
CGCVISSimTarget::CGCVISSimTarget(const char* name,const char* starclass,const char* desc):CGCTarget(name,starclass,desc) {
        compileCommand.setState("compileCommand",this,"cc","command for compiling code.");
        compileOptions.setState("compileOptions",this,"-fast -xO4 -xdepend -xchip=ultra -xarch=v8plusa -V ${VSDKHOME}/util/vis.il","options to be specified for compiler.");
}

int CGCVISSimTarget::compileCode()
{
    // invoke the compiler
    StringList cmd, error, file, rtlib;
    if (onHostMachine(targetHost)) {
        StringList ptolemy = getenv("PTOLEMY");
	StringList vislib;
	vislib << "-I" << getenv("VSDKHOME") << "/include " ;
        rtlib << vislib << "-I" << ptolemy << "/src/domains/cgc/rtlib -L" 
              << ptolemy << "/lib." << getenv("PTARCH") << " -lCGCrtlib" ;
    }
    else {
        rtlib << "CGCrtlib.c";
    }
    file << filePrefix << ".c " << rtlib;
    cmd << compileLine(file) << " -o " << filePrefix;
    error << "Could not compile " << file;
    return (systemCall(cmd, error, targetHost) == 0);
}

// -----------------------------------------------------------------------------
Block* CGCVISSimTarget :: makeNew() const {
	LOG_NEW; return new CGCVISSimTarget(name(),starType(),descriptor());
}
// -----------------------------------------------------------------------------
ISA_FUNC(CGCVISSimTarget,CGCTarget);

static CGCVISSimTarget myTargetPrototype("CGCVIS","CGCStar","A VIS target for VIS code generation");

static KnownTarget entry(myTargetPrototype,"CGCVIS");



