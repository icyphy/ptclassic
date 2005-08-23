#ifndef _MatlabPtIfc_h
#define _MatlabPtIfc_h 1

/**************************************************************************
Version identification:
@(#)MatlabPtIfc.h	1.2	07/03/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  Brian L. Evans
 Date of creation: 5/21/96
 Revisions:

 Base class for a Ptolemy interface to Matlab.
 Only one Matlab process is run and is shared by all of Ptolemy.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "Matrix.h"
#include "MatlabIfc.h"
#include "Particle.h"

class MatlabPtIfc : public MatlabIfc {
public:
	// convert Ptolemy particles to Matlab matrices
	mxArray* PtolemyToMatlab(Particle& particle, DataType portType,
				int* errflag);

	// convert Matlab matrices to Ptolemy particles
	int MatlabToPtolemy(Particle& particle, DataType portType,
			    mxArray* matlabMatrix, int* warnflag,
			    int* errflag);
};

#endif
