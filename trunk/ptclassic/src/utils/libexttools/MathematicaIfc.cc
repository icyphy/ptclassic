/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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

Programmer: Steve X. Gu and Brian L. Evans
Date of creation: 01/13/96

General base class for an interface to Mathematica via MathLink.

OpenStaticMathLink and CloseStaticMathLink methods are adapted from [1].

The SendToMathLink method is based on a Tcl/Tk interface to Mathematica
written by John M. Novak of Wolfram Research Inc., but this version
is an abstraction that does not know with which tool(s) it will interface.

References:

1. Todd Gayley, {A MathLink Tutorial}, Wolfram Research Inc.,
   available from http://www.wri.com/.

2. Wolfram Research Inc., {MathLink Reference Guide: Mathematica
   Version 2.2}, 1993.  Comes with the Mathematica software package.

 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "MathematicaIfc.h"

// count how many instances of this class have been created
static int mathematicaStarsCount = 0;


// Begin: STATIC CONTROL OF THE MATHEMATICA PROCESS

// Status of static connection
static MLINK gMathLink = 0;
static MLEnvironment env = 0;

// Open the static connection
int MathematicaIfc::OpenStaticMathLink(int argc, char **argv) {
    // If already open, return TRUE; otherwise, initialize static data
    if ( env && gMathLink ) return TRUE;
    CloseStaticMathLink();

    InfString errMsg;
    int retval = FALSE;

    // Initialize the MathLink library, open a link to Mathematica,
    // verify the link, and initialize Mathematica
    if ( (env = MLInitialize(NULL)) == NULL ) {
        errMsg = "Cannot initialize MathLink.";
    }
    else if ( (gMathLink = MLOpen(argc, argv)) == NULL ) {
	errMsg = "Cannot open a MathLink connection to Mathematica.";
    }
    else if ( ! MLConnect(gMathLink) ) {
	errMsg = "Cannot verify the MathLink connection to Mathematica. ";
    }
    else if ( ! InitXWindows(gMathLink) ) {
	errMsg = "Cannot initialize X windows graphics in Mathematica.";
    }
    else {
	retval = TRUE;
    }

    // If an error occurred, then store the error message and close the link
    if (! retval) {
	errMsg << " Mathematica may not be licensed to run on this machine.";
	SetErrorString(errMsg);
	CloseStaticMathLink();
    }

    return retval;
}

// Close the static connection
int MathematicaIfc::CloseStaticMathLink() {
    if (gMathLink) {
	MLClose(gMathLink);
	gMathLink = 0;
    }
    if (env) {
	MLDeinitialize(env);
	env = 0;
    }
    return TRUE;
}

// End: STATIC CONTROL OF THE MATHEMATICA PROCESS


// Debugging functions

#if MATHEMATICA_IFC_DEBUG
// printPacketType
// prints out the type of packet stored in packetType
static void printPacketType(int packetType, int currentFlag) {
  char *wordString = (currentFlag) ? "Current" : "Last";
  char *packetString = 0;

  switch (packetType) {
    case ILLEGALPKT:
	packetString = "Illegal";
	break;
    case INPUTPKT:
	packetString = "Input";
	break;
    case TEXTPKT:
	packetString = "Text";
	break;
    case RETURNPKT:
	packetString = "Return";
	break;
    case RETURNTEXTPKT:
	packetString = "ReturnText";
	break;
    case MESSAGEPKT:
	packetString = "Message";
	break;
    case MENUPKT:
	packetString = "Menu";
	break;
    case CALLPKT:
	packetString = "Call";
	break;
    case INPUTNAMEPKT:
	packetString = "InputName";
	break;
    case OUTPUTNAMEPKT:
	packetString = "OutputName";
	break;
    case SYNTAXPKT:
	packetString = "Syntax";
	break;
    case DISPLAYPKT:
	packetString = "Display";
	break;
    case DISPLAYENDPKT:
	packetString = "DisplayEnd";
	break;
    default:
	packetString = "Unrecognized";
	fprintf(stderr, "%s% packet of type %d% is not recognizable",
		wordString, packetType);
  }

  if (packetString) {
     printf("%s packet is a %s% packet\n", wordString, packetString);
  }
}
#endif

// Constructor
MathematicaIfc::MathematicaIfc(const char* name, int restrictContextFlag) :
	mathlink(0), environment(0) {
    instanceNumber = mathematicaStarsCount++;
    contextFlag = restrictContextFlag;
    context = name;
    context << instanceNumber << "`";
    prolog = "Begin[\"";
    prolog << context << "\"]";
    epilog = "End[]";
}

// Destructor
MathematicaIfc::~MathematicaIfc() {
    mathematicaStarsCount--;
    KillMathematica();
}

// Methods to access data members

const char* MathematicaIfc::SetOutputBuffer(const char* string) {
    outputBuffer = string;
    return outputBuffer;
}

const char* MathematicaIfc::SetErrorString(const char* command) {
    errorString = command;
    return errorString;
}

const char* MathematicaIfc::SetWarningString(const char* command) {
    warningString = command;
    return warningString;
}

const char* MathematicaIfc::GetOutputBuffer() {
    return outputBuffer;
}

int MathematicaIfc::GetOutputBufferLength() {
    return strlen(outputBuffer);
}

const char* MathematicaIfc::GetErrorString() {
    return errorString;
}

const char* MathematicaIfc::GetWarningString() {
    return warningString;
}

const char* MathematicaIfc::GetHandle() {
    return context;
}

// get static data
MLINK MathematicaIfc::GetCurrentLink() {
    return mathlink;
}

int MathematicaIfc::GetMathematicaIfcInstanceCount () {
    return mathematicaStarsCount;
}

// Methods to interface to the Mathematica process via MathLink

// A. low-level interfaces

int MathematicaIfc::OpenMathLink(int argc, char **argv) {
    if ( environment && mathlink ) return TRUE;
    CloseMathLink();

    // Open static MathLink connection and set static variables env & gMathLink
    int retval = OpenStaticMathLink(argc, argv);
    if ( retval ) {
	environment = env;
	mathlink = gMathLink;
    }
    return retval;
}

// CloseMathLink:
int MathematicaIfc::CloseMathLink() {
    int retval = TRUE;
    environment = 0;
    mathlink = 0;
    if (mathematicaStarsCount == 0) retval = CloseStaticMathLink();
    return retval;
}

// Expect a specific return packet from MathLink
// -- set the data member returnType as a side effect
// -- assumes that data exists on the link
int MathematicaIfc::ExpectPacket(
		MLINK linkp, int discardFlag, int& returnType) {
    returnType = ILLEGALPKT;
    if ( MLError(linkp) == MLEOK ) {
	MLFlush(linkp);
	returnType = MLNextPacket(linkp);
	if ( discardFlag ) MLNewPacket(linkp);
    }
    return ( MLError(linkp) == MLEOK );
}

// LoopUntilPacket loops until an error occurs or until a packet of
// of type packetType is found (sets returnType)
int MathematicaIfc::LoopUntilPacket(
		MLINK linkp, int packetType, int& returnType) {
    int retval = TRUE;
    do {
	retval = ExpectPacket(linkp, TRUE, returnType);  // discard packet
    } while ( retval && ( returnType != packetType ) );
    if (! retval) SetErrorString("Error reading results from Mathematica.");
    return retval;
}

// Evaluate a command that does not go into the user history by
// bypassing the Mathematica parser
int MathematicaIfc::EvaluatePrivateCommand(MLINK linkp, char* command) {
    int retval = TRUE;
    if ( command && *command ) {
	MLPutFunction(linkp, "EvaluatePacket", 1);
	MLPutFunction(linkp, "ToExpression", 1);
	MLPutString(linkp, command);
	MLEndPacket(linkp);

	// Expect a Return Packet back
	int returnType = ILLEGALPKT;
	retval = LoopUntilPacket(linkp, RETURNPKT, returnType);
    }
    return retval;
}

// Initialize Mathematica to use X windows for graphics
int MathematicaIfc::InitXWindows(MLINK linkp) {
    // Initialize X/Motif graphics by having Mathematica evaluate Motif.m
    return EvaluatePrivateCommand(linkp, "Get[\"Motif.m\"]");
}

// SendToMathLink
// Send a user entered Mathematica command which is sent as
// a string to the Mathematica kernel.
int MathematicaIfc::SendToMathLink(MLINK linkp, char* command) {
  int retval = TRUE;
  int returnType = ILLEGALPKT;
  InfString completeReturnString, previousReturnString;

  // Return an error if Mathematica is not running
  if ( ! MathematicaIsRunning() ) {
    SetErrorString("Mathematica is not running!");
    return FALSE;
  }

  if ( command == 0 || *command == 0 ) {
    SetErrorString("empty Mathematica command.");
    return FALSE;
  }

  // Check to see if the link has something to read from it.
  // If so, skip up to and including the first Input Name Packet we receive
  if ( MLReady(linkp) ) {
    if ( ! LoopUntilPacket(linkp, INPUTNAMEPKT, returnType) ) return FALSE;
  }

  // Put the Mathematica command on the link
  MLPutFunction(linkp, "EnterTextPacket", 1);
  MLPutString(linkp, command);
  MLEndPacket(linkp);

  // Process packets until INPUTNAMEPK, OUTPUTNAMEPKT, SYNTAXPKT, or TEXTPKT
  ExpectPacket(linkp, FALSE, returnType);
  while (returnType != INPUTNAMEPKT &&
         returnType != OUTPUTNAMEPKT &&
         returnType != SYNTAXPKT &&
         returnType != TEXTPKT) {
    char* returnString = 0;

    switch (returnType) {
      case MESSAGEPKT:
	MLNewPacket(linkp);
	MLGetString(linkp, &returnString);
	if ( previousReturnString.length() != 0 ) {
	  previousReturnString << "\n";
	}
	previousReturnString << returnString;
	MLDisownString(linkp, returnString);
	returnString = 0;
	ExpectPacket(linkp, TRUE, returnType);
	ExpectPacket(linkp, FALSE, returnType);
	break;

      case DISPLAYPKT:
      case DISPLAYENDPKT:
      default:
	MLNewPacket(linkp);
	ExpectPacket(linkp, FALSE, returnType);
	break;
    }
  }

  // If current packet is INPUTNAMEPKT. Done
  if (returnType == INPUTNAMEPKT) {
    completeReturnString = previousReturnString;
  }
  else if (returnType == SYNTAXPKT) {
    char* returnString = 0;
    // FIXME: shouldn't we use the value of returnString somewhere?
    MLGetString(linkp, &returnString);
    completeReturnString = previousReturnString;
    MLDisownString(linkp, returnString);
  }
  else if (returnType == TEXTPKT) {
    char* returnString = 0;
    MLGetString(linkp, &returnString);
    completeReturnString = returnString;
    MLDisownString(linkp, returnString);
  }
  else if (returnType == OUTPUTNAMEPKT) {
    char* returnString = 0;
    MLNewPacket(linkp);
    returnType = MLNextPacket(linkp);

    switch( returnType ) {
      case RETURNPKT:
	MLGetString(linkp, &returnString);
	outputBuffer << returnString;
	MLDisownString(linkp, returnString);
	break;

      case RETURNTEXTPKT:
	MLGetString(linkp, &returnString);
	if ( previousReturnString.length() == 0 ) {
	  completeReturnString = returnString;
	}
	else {
	  completeReturnString = previousReturnString;
	  completeReturnString << "\n" << returnString;
        }
	outputBuffer << completeReturnString;
	MLDisownString(linkp, returnString);
	break;

      case CALLPKT:
      case MENUPKT:
      case INPUTPKT:
	break;

      default:
	SetErrorString("Expected Mathematica packets after an OutputName packet were not found.");
	retval = FALSE;
	break;
    }
  }

  SetOutputBuffer(completeReturnString);
  return retval;
}


// C. highest-level interface to the Mathematica process

// StartMathematica: set default link options for typical Unix MathLink session
// [Gayley, Section 2.2.1, MLOpen]
int MathematicaIfc::StartMathematica() {
    int oargc = 4;
    char* oargv[] = {"-linkname", "math -mathlink", "-linkmode", "launch", 0};

    return StartMathematica(oargc, oargv);
}

// StartMathematica: accept generic link options
int MathematicaIfc::StartMathematica(int oargc, char** oargv) {
    // Initialize the error string to a null string and accumulate errors
    InitErrorMessages();

    // Kill the current connection to Mathematica if one exists
    KillMathematica();

    if ( ! OpenMathLink(oargc, oargv) ) {
	AddErrorMessage("Could not start Mathematica using MathLink with the options");
	for (int i = 0; i < oargc; i++) {
	    AddErrorMessage(oargv[i]);
	}
	AddErrorMessage(".");
	return FALSE;
    }

    return TRUE;
}

// MathematicaIsRunning
int MathematicaIfc::MathematicaIsRunning() {
    return ( environment && mathlink );
}

// EvaluateUserCommand
int MathematicaIfc::EvaluateUserCommand(char* command) {
    if ( contextFlag ) EvaluateUnrecordedCommand(prolog);
    int retval = EvaluateOneCommand(command);
    if ( contextFlag ) EvaluateUnrecordedCommand(epilog);
    return retval;
}

// CloseMathematicaFigures:  Mathematica does not have a mechanism to tag
// the figures it creates, so we cannot systematically delete them
int MathematicaIfc::CloseMathematicaFigures() {
    return FALSE;
}

// KillMathematica:
int MathematicaIfc::KillMathematica() {
    if ( MathematicaIsRunning() ) {
	if ( ! CloseMathLink() ) {
	    errorString << "Error when terminating connection to the Mathematica kernel\n";
	    return FALSE;
	}
    }

    return TRUE;
}

// Method to add to the error messages
void MathematicaIfc::AddErrorMessage(const char *msg) {
    if ( errorString.length() > 0 ) errorString << " ";
    errorString << msg;
}

void MathematicaIfc::InitErrorMessages() {
    errorString.initialize();
}
