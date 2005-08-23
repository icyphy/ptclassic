/*
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
						COPYRIGHTENDKEY

Author:  Steve X. Gu and Brian L. Evans
Created: 01/13/96
Version: @(#)MathematicaIfc.h	1.11	03/18/96

General base class to define an interface to Mathematica via MathLink.

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
    MathematicaIfc(const char* initstring = 0,
		   const char* name = 0,
		   int privateContextFlag = 0,
		   int echoInputFlag = 0,
		   int echoOutputNumFlag = 0);

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
    const char* GetPrivateOutputBuffer();
    int GetPrivateOutputBufferLength();

    // get static members
    MLINK GetCurrentLink();
    int GetMathematicaIfcInstanceCount();
    const char* GetHandle();

    // Methods to interface to the Mathematica process via MathLink

    // A. low-level interfaces
    int OpenMathLink(int argc, char** argv);
    int SendToMathLink(MLINK link, char* command);
    int CloseMathLink();

    int ExpectPacket(MLINK link, int discardFlag, int& returnType);
    int LoopUntilPacket(MLINK link, int packetType, int& returnType);
    int EvaluatePrivateCommand(MLINK link, char* command,
			       int storeResult = FALSE);
    int InitXWindows(MLINK link);

    // B. higher-level interfaces to the Mathematica process
    inline int EvaluateOneCommand(char* command) {
	return SendToMathLink(mathlink, command);
    }

    inline int EvaluateUnrecordedCommand(char* command, int storeResult) {
	return EvaluatePrivateCommand(mathlink, command, storeResult);
    }

    // C. highest-level interface to the Mathematica process
    int StartMathematica();
    int StartMathematica(int oargc, char** oargv);
    int MathematicaIsRunning();
    inline int EvaluateUserCommand(char* command) {
	return mathematicaCommand(command, TRUE);
    }
    inline int EvaluateUnrecordedUserCommand(char* command) {
	return mathematicaCommand(command, FALSE);
    }
    int CloseMathematicaFigures();
    int KillMathematica();

protected:
    // place to store Mathematica output returned by EvaluateUserCommand
    InfString outputBuffer;

    // place to store Mathematica output returned by EvaluateUnrecordedCommand
    InfString privateOutputBuffer;

    // Mathematica error string
    InfString errorString;

    // add an error message
    void AddErrorMessage(const char *msg);

    // initialize error messages
    void InitErrorMessages();

    // Mathematica warning string
    InfString warningString;

    // instance number
    int instanceNumber;

    // package name attached to Mathematica interface instance
    InfString context;
    InfString prolog;
    InfString epilog;
    InfString initCode;
    InfString customInitCommand;

    // method to set the string states context, prolog, epilog, and initCode
    void setContext(const char* name, int flag);

    // current link to Mathematica
    MLINK mathlink;
    MLEnvironment environment;

    // read the contents of a packet
    StringList ReadPacketContents(MLINK linkp);

    // methods to manage the static connection
    int OpenStaticMathLink(int argc, char** argv);
    int CloseStaticMathLink();
    const char* SetInputName(const char* name);
    const char* GetInputName();

    // whether to display the input prompt and expression
    int displayInputFlag;
    int displayOutputNumFlag;

    // abstraction of highest-level evaluate methods
    int mathematicaCommand(char* command, int preprocessedFlag);
};

#endif
