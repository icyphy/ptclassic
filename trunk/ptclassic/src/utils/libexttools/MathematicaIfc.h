/*
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

Programmer: Steve X. Gu and Brian L. Evans
Date of creation: 01/13/96

Base class for the Ptolemy interface to Mathematica via MathLink.

 */

#ifndef _MathematicaIfc_h
#define _MathematicaIfc_h 

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "InfString.h"
#include "MathematicaIfcFuns.h"

#define MATHEMATICA_IFC_DEBUG 0

class MathematicaIfc {

public:
    // Constructor
    MathematicaIfc();

    // Destructor
    ~MathematicaIfc();

    // Methods to access data members
    
    // set protected data members
    const char* SetOutputBuffer(const char* string);
    const char* SetErrorString(const char* string);
    const char* SetWarningString(const char* string);

    // get protected data members
    const char* GetOutputBuffer();
    int GetOutputBufferLength();
    const char* GetErrorString();
    const char* GetWarningString();

    // get static members
    MLINK GetCurrentLink();
    int GetMathematicaIfcInstanceCount();

    // Methods to interface to the Mathematica process via MathLink

    // A. low-level interfaces
    int OpenMathLink(int argc, char **argv);
    int SendToMathLink(char* command);
    int CloseMathLink();

    // B. higher-level interfaces to the Mathematica process
    int EvaluateOneCommand(char* command);

    // C. highest-level interface to the Mathematica process
    int StartMathematica();
    int MathematicaIsRunning();
    int EvaluateUserCommand(char* command);
    int KillMathematica();

protected:
    // place to store output from Mathematica
    InfString outputBuffer;

    // Mathematica error string
    InfString errorString;

    // Mathematica warning string
    InfString warningString;

private:
    // type of the output packet
    int returnType;
};

#endif
