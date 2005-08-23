#ifndef _ACSCGCCore_h
#define _ACSCGCCore_h

/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  J. A. Lundblad
 Date of creation: 2/29/98
 Version: @(#)ACSCGCCore.h	1.6 09/08/99

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "ACSCGCore.h"

class ACSCGCCore : public ACSCGCore {
public:

	// This is the default constructor for the Floating Point Simulation 
	// core category.
	ACSCGCCore(const char* category) : ACSCGCore(category) { }

   /* virtual */ StringList setargStates(Attribute a=ANY);	

protected:

	/* virtual */	int runIt();

	// Language specific code stream method for above.
	/* virtual */ StringList addMoveDataBetweenShared( int thereEmbedded, int startEmbedded, int stopEmbedded, const char* farName, const char* geoName, int numXfer );

	// Generate declarations for PortHoles and States.
	/* virtual */ StringList declareBuffer(const ACSPortHole*) ;
	/* virtual */ StringList declareOffset(const ACSPortHole*) ;
	/* virtual */ StringList declareState(const State*) ;

	// Generate declaration, initilization and function
	// codes for command-line settable states.
        /* virtual */ StringList cmdargState(const State*) ;
        /* virtual */ StringList cmdargStatesInit(const State*) ;
        /* virtual */ StringList setargState(const State*) ;
        /* virtual */ StringList setargStatesHelp(const State*) ;

	// Generate initialization code for PortHoles and States.
	/* virtual */ StringList initCodeBuffer(ACSPortHole*) ;
	/* virtual */ StringList initCodeOffset(const ACSPortHole*) ;
	/* virtual */ StringList initCodeState(const State*) ;

	/* virtual */ StringList portBufferIndex(const ACSPortHole*) ;
	/* virtual */ StringList portBufferIndex(const ACSPortHole*, const State*, const char*) ;

	/* virtual */ StringList arrayStateIndexRef(const State*, const char*) ;

	// add the fixed-point supporting routines
	void addFixedPointSupport();

	// After each firing, update the offset pointers
	virtual void updateOffsets();

	// include a module from a C library in the Ptolemy tree
	void addModuleFromLibrary(const char* basename,
		const char* subdirectory, const char* libraryname);

	// Add lines to be put at the beginning of the code file
	int addInclude(const char* decl);

	// Add options to be used when compiling a C program
	int addCompileOption(const char*);

	// Add options to be used when linking a C program
	int addLinkOption(const char*);

	// Add options to be used when linking a C program on the local machine
	int addLocalLinkOption(const char*);

	// Add options to be used when linking a C program on a remote machine
	int addRemoteLinkOption(const char*);

	// Add a file to be copied over the remote machine
	int addRemoteFile(const char*, int flag = FALSE);

	// JMS
	  /*virtual*/ int isA(const char*) const;

};




#endif // _ACSCGCCore_h

