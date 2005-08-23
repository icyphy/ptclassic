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
Version: @(#)MathematicaIfc.cc	1.17	07/09/97

General base class for an interface to Mathematica via MathLink.

OpenStaticMathLink and CloseStaticMathLink methods are adapted from [1].

The SendToMathLink method was at one time based on a Tcl/Tk interface
to Mathematica written by John M. Novak of Wolfram Research Inc.  Unlike
the Tcl/Tk interface, this version is an abstraction that does not know
with which tool(s) it will interface, handles more packets, and is
more robust to user inputs.

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
static InfString inputNameString = "In[1]:= ";

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

    SetInputName("In[1]:= ");

    return retval;
}

// Get the last input name string read
const char* MathematicaIfc::GetInputName() {
    return inputNameString;
}

// Set the last input name string read
const char* MathematicaIfc::SetInputName(const char* name) {
    inputNameString = name;
    return inputNameString;
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
  const char* wordString = (currentFlag) ? "Current" : "Last";
  const char* packetString = 0;

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
MathematicaIfc::MathematicaIfc(
	const char* initstring, const char* name, int privateContextFlag,
	int echoInputFlag, int echoOutputNumFlag) :
			mathlink(0), environment(0) {
    instanceNumber = mathematicaStarsCount++;
    customInitCommand = initstring ? initstring : "";
    setContext(name, privateContextFlag);
    displayInputFlag = echoInputFlag;
    displayOutputNumFlag = echoOutputNumFlag;

    // Initialize the remaining InfString members
    outputBuffer = "";
    privateOutputBuffer = "";
    errorString = "";
    warningString = "";
}

void MathematicaIfc::setContext(const char* name, int flag) {
    context = name;
    context << instanceNumber << "`";

    // Define Mathematica variables to keep track of state:
    // context`Private`context`lastContext
    // context`Private`context`currContext
    StringList lastContextVariable = context;
    lastContextVariable << "Private`" << context << "lastContext";
    StringList currContextVariable = context;
    currContextVariable << "Private`" << context << "currContext";

    prolog = currContextVariable;
    prolog << " = $Context; $Context = " << lastContextVariable << ";";

    epilog = lastContextVariable;
    epilog << " = $Context; $Context = " << currContextVariable << ";";

    if (flag) {
	initCode = lastContextVariable;
	initCode << " = \"" << context << "\";";
    }
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
    return outputBuffer.length();
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

const char* MathematicaIfc::GetPrivateOutputBuffer() {
    return privateOutputBuffer;
}

int MathematicaIfc::GetPrivateOutputBufferLength() {
    return privateOutputBuffer.length();
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
int MathematicaIfc::EvaluatePrivateCommand(
		MLINK linkp, char* command, int storeResult) {
    int retval = TRUE;
    StringList result = "";
    if ( command && *command ) {
	MLPutFunction(linkp, "EvaluatePacket", 1);
	MLPutFunction(linkp, "ToString", 1);
	MLPutFunction(linkp, "ToExpression", 1);
	MLPutString(linkp, command);
	MLEndPacket(linkp);

	// Expect a Return Packet back
	int returnType = ILLEGALPKT;
    	retval = ExpectPacket(linkp, FALSE, returnType);
	while ( retval && (returnType != RETURNPKT) ) {
	    MLNewPacket(linkp);
	    retval = ExpectPacket(linkp, FALSE, returnType);
	}
	if (! retval) SetErrorString("Error reading results from Mathematica.");
	result = ReadPacketContents(linkp);
    }
    if (storeResult) privateOutputBuffer = result;
    return retval;
}

// Initialize Mathematica to use X windows for graphics
int MathematicaIfc::InitXWindows(MLINK linkp) {
    // Initialize X/Motif graphics by having Mathematica evaluate Motif.m
    return EvaluatePrivateCommand(linkp, "Get[\"Motif.m\"];");
}

StringList MathematicaIfc::ReadPacketContents(MLINK linkp) {
    kcharp_ct returnString = 0;
    MLGetString(linkp, &returnString);
    StringList contents = returnString ? returnString : "";
    if (returnString) {
        MLDisownString(linkp, returnString);
    }
    MLNewPacket(linkp);
    return contents;
}

// SendToMathLink
// Send a user entered Mathematica command which is sent as
// a string to the Mathematica kernel.
// MathLink can respond with a variety of packets in the following order:
//   InputName
//   InputName -> OutputName ------------> Text
//   InputName -> OutputName -> Message -> Text -> ReturnText
//   InputName -> OutputName --------------------> ReturnText
//   InputName ---------------> Message -> Text -> Syntax
// When the Display variables are not set for external display, MathLink
// can respond to graphics and sounds commands with display packets
// after the InputName packet.
//
int MathematicaIfc::SendToMathLink(MLINK linkp, char* command) {
    int returnType = ILLEGALPKT;
    InfString completeReturnString = "", outputString = "";

    // Set the output buffer to an empty string
    SetOutputBuffer("");

    // Return an error if Mathematica is not running
    if ( ! MathematicaIsRunning() ) {
      SetErrorString("Mathematica is not running!");
      return FALSE;
    }

    // Return on an empty command
    if ( command == 0 || *command == 0 ) {
      return FALSE;
    }

    // Make a string holding the input number and input command
    if ( displayInputFlag ) {
      completeReturnString = GetInputName();
      completeReturnString << command << "\n";
    }

    // Put the Mathematica command on the link
    MLPutFunction(linkp, "EnterTextPacket", 1);
    MLPutString(linkp, command);
    MLEndPacket(linkp);

    // While waiting until we receive an InputName, Return, Syntax, or Text
    // packet, process Message and OutputName packets but ignore Display
    // packets; this while is necessary because Mathematica outputs a lot
    // of illegal packets before it puts out valid packets
    ExpectPacket(linkp, FALSE, returnType);
    while (returnType != INPUTNAMEPKT &&
	   returnType != RETURNTEXTPKT &&
	   returnType != TEXTPKT &&
	   returnType != SYNTAXPKT) {

      switch(returnType) {
	case MESSAGEPKT:
	  // We ignore the contents of the message packet and instead
	  // use its printed form which is contained in the following
	  // packet, which is a text packet
	  MLNewPacket(linkp);
	  ExpectPacket(linkp, FALSE, returnType);
	  completeReturnString << ReadPacketContents(linkp) << "\n";
	  break;

	case OUTPUTNAMEPKT:
	  outputString = ReadPacketContents(linkp);
	  outputString << "\n";
	  break;

	default:
	  MLNewPacket(linkp);
	  break;
      }
      ExpectPacket(linkp, FALSE, returnType);
    }

    // Process the final packet
    switch ( returnType ) {
      case SYNTAXPKT:
	outputString.initialize();
	// fall through
      case RETURNTEXTPKT:
      case TEXTPKT:
	completeReturnString << ReadPacketContents(linkp);
    }

    // Look for the next input name packet
    if ( returnType != INPUTNAMEPKT ) {
      while ( MLNextPacket(linkp) != INPUTNAMEPKT ) MLNewPacket(linkp);
    }
    SetInputName(ReadPacketContents(linkp));

    if ( displayOutputNumFlag ) {
      completeReturnString << outputString;
    }
    SetOutputBuffer(completeReturnString);

    return TRUE;
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

    EvaluateUnrecordedCommand(customInitCommand, FALSE);
    EvaluateUnrecordedCommand(initCode, FALSE);

    return TRUE;
}

// MathematicaIsRunning
int MathematicaIfc::MathematicaIsRunning() {
    return ( environment && mathlink );
}

// EvaluateUserCommand
int MathematicaIfc::mathematicaCommand(char* command, int preprocessFlag) {
    EvaluateUnrecordedCommand(prolog, FALSE);
    int retval = preprocessFlag ? EvaluateOneCommand(command) :
				  EvaluateUnrecordedCommand(command, TRUE);
    EvaluateUnrecordedCommand(epilog, FALSE);
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
