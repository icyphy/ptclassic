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

Base class for the Ptolemy interface to Mathematica via MathLink.

Based on a Tcl/Tk interface to Mathematica by John M. Novak of
Wolfram Research Inc., but this file is an abstraction that does
not know what tools it will interface with.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "MathematicaIfc.h"

// count how many instances of this class have been created
static int mathematicaStarsCount = 0;

// Ptolemy-controlled Mathematica process
static MLINK gMathLink = 0;
static MLEnvironment env = 0;

// A flag to check the type of last packet on the link
static int lastOutputEmpty = TRUE;

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
MathematicaIfc::MathematicaIfc() {
    mathematicaStarsCount++;
    returnType = ILLEGALPKT;
}

// Destructor
MathematicaIfc::~MathematicaIfc() {
    mathematicaStarsCount--;

    if ( mathematicaStarsCount == 0 ) {
	KillMathematica();
    }
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

// get static data
MLINK MathematicaIfc::GetCurrentLink() {
    return gMathLink;
}

int MathematicaIfc::GetMathematicaIfcInstanceCount () {
    return mathematicaStarsCount;
}

// Methods to interface to the Mathematica process via MathLink

// A. low-level interfaces
int MathematicaIfc::OpenMathLink(int argc, char **argv) {
    int retval = TRUE;
    env = MLInitialize(NULL);
    if ( env == NULL ) {
	fprintf(stderr, "Problem initializing the link to Mathematica.\n");
	retval = FALSE;
    }
    gMathLink = MLOpen(argc, argv);
    if ( gMathLink == NULL ) {
	fprintf(stderr, "Problem opening the link to Mathematica.\n");
	retval = FALSE;
    } else {
	MLPutFunction(gMathLink, "EvaluatePacket", 1);
	MLPutFunction(gMathLink, "Get", 1);
	MLPutString(gMathLink, "Motif.m");      /* NeXT.m on a NeXT */
	MLEndPacket(gMathLink);
    }
    return retval;
}


// SendToMathLink
// Send a user entered Mathematica command which is sent as
// a string to the Mathematica kernel.
int MathematicaIfc::SendToMathLink(char* command) {
  int retval = TRUE;
  InfString completeReturnString, previousReturnString;

  // Put Mathematica commands on the link
  // FIXME: Check for errors
  MLPutFunction(gMathLink, "Enter", 1);
  MLPutString(gMathLink, command);

  // Check the previous packet
  if ( !lastOutputEmpty ) {
    // Discard packet if it is not an input packet and get a new packet
    if (returnType != INPUTNAMEPKT) {
      MLNewPacket(gMathLink);
      returnType = MLNextPacket(gMathLink);
    }
  }

  // Wait until the INPUTNAMEPKT appears on the link
  while ( returnType != INPUTNAMEPKT ) {
    MLNewPacket(gMathLink);
    returnType = MLNextPacket(gMathLink);
  }

  // Current packet is INPUTNAMEPKT.  Get the next packet.
  MLNewPacket(gMathLink);
  returnType = MLNextPacket(gMathLink);

  // Process packets until INPUTNAMEPK, OUTPUTNAMEPKT, SYNTAXPKT, or TEXTPKT
  while (returnType != INPUTNAMEPKT &&
         returnType != OUTPUTNAMEPKT &&
         returnType != SYNTAXPKT &&
         returnType != TEXTPKT) {
    char* returnString = 0;

    switch (returnType) {
      case MESSAGEPKT:
	MLNewPacket(gMathLink);
	returnType = MLNextPacket(gMathLink);
	MLGetString(gMathLink, &returnString);
	if ( previousReturnString.length() == 0 ) {
	  previousReturnString = returnString;
	} else {
	  previousReturnString << "\n" << returnString;
	}
	MLDisownString(gMathLink, returnString);
	MLNewPacket(gMathLink);
	returnType = MLNextPacket(gMathLink);
	break;

      case DISPLAYPKT:
      case DISPLAYENDPKT:
      default:
	MLNewPacket(gMathLink);
	returnType = MLNextPacket(gMathLink);
	break;      
    }
  }

  // If current packet is INPUTNAMEPKT. Done
  if (returnType == INPUTNAMEPKT) {
    lastOutputEmpty = (previousReturnString.length() == 0);
    completeReturnString = previousReturnString;
  }
  else if (returnType == SYNTAXPKT) {
    char* returnString = 0;
    // FIXME: We do not use the value of returnString
    MLGetString(gMathLink, &returnString);
    completeReturnString = previousReturnString;
    MLDisownString(gMathLink, returnString);
    lastOutputEmpty = FALSE;
  }
  else if (returnType == TEXTPKT) {
    char* returnString = 0;
    MLGetString(gMathLink, &returnString);
    completeReturnString = returnString;
    MLDisownString(gMathLink, returnString);
    lastOutputEmpty = FALSE;    
  }
  else if (returnType == OUTPUTNAMEPKT) {
    char* returnString = 0;
    MLNewPacket(gMathLink);
    returnType = MLNextPacket(gMathLink);
    lastOutputEmpty = 0;

    switch( returnType ) {
      case RETURNPKT:
	MLGetString(gMathLink, &returnString);
	outputBuffer << returnString;
	MLDisownString(gMathLink, returnString);
	break;

      case RETURNTEXTPKT:
	MLGetString(gMathLink, &returnString);
	if ( previousReturnString.length() == 0 ) {
	  completeReturnString = returnString;
	} else {
	  completeReturnString = previousReturnString;
	  completeReturnString << "\n" << returnString;
        }
	outputBuffer << completeReturnString;
	MLDisownString(gMathLink, returnString);
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

// CloseMathLink:
// FIXME: Check return values of MLClose and MLDeinitialize
int MathematicaIfc::CloseMathLink() {
  MLClose(gMathLink);
  MLDeinitialize(env);
  return TRUE;
}


// B. higher-level interfaces to the Mathematica process

// EvaluateOneCommand:
int MathematicaIfc::EvaluateOneCommand(char* command) {

    int result = 0;		// FIXME: What should the default value be?

    if ( MathematicaIsRunning() ) {
	result = SendToMathLink(command);
    }
    else {
	SetErrorString("Mathematica is not running!");
    }

    return result;
}



// C. highest-level interface to the Mathematica process

// StartMathematica
int MathematicaIfc::StartMathematica() {
    KillMathematica();

    // Set default link options
    int oargc = 6;
    char *oargv[] = {"-linkmode", "launch",
		     "-linkname",
		     "math -mathlink -linkprotocol tcp -linkmode parentconnect",
		     "-linkprotocol", "tcp",
		     0};

    // FIXME: Design a mechanism to allow the user to specify options
    OpenMathLink(oargc, oargv);

    if ( ! MathematicaIsRunning() ) {
	errorString = "Could not start Mathematica using the options ";
	for (int i = 0; i < oargc; i++) {
	  errorString << oargv[i];
	}
	return FALSE;
    }

    return TRUE;
}

// MathematicaIsRunning
int MathematicaIfc::MathematicaIsRunning() {
    return ( gMathLink != 0 );
}

/*****************************************************************************
 * EvaluateUserCommand:
 *
 ****************************************************************************/
int MathematicaIfc::EvaluateUserCommand(char* command) {
    return EvaluateOneCommand(command);
}

// KillMathematica:
int MathematicaIfc::KillMathematica() {
    if ( MathematicaIsRunning() ) {
	if ( ! CloseMathLink() ) {
	    errorString = "Error when terminating connection to the Mathematica kernel";
	    return FALSE;
	}
    }

    return TRUE;
}
