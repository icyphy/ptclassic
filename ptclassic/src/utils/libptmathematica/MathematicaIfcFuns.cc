static const char file_id[] = "MathematicaIfcFuns.cc";

/**************************************************************************
Version identification:
@(#)MathematicaIfcFuns.cc	1.3	07/09/97

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
 Date of creation: 1/18/96
 Revisions:

 Default functions for the Ptolemy interface to Mathematica.

**************************************************************************/

// Ptolemy Error class
#include "Error.h"
#include "MathematicaIfcFuns.h"

//
// Mathematica MathLink Routines
//

inline void mmaPtError() {
	Error::abortRun( "The external interface to Mathematica has not been ",
			 "compiled into Ptolemy." );
}

#define PT_MMA_STUB(x) x { \
	mmaPtError(); \
	return 0; \
}

#define PT_MMA_VOID_STUB(x) x { \
	mmaPtError(); \
	return; \
}

//
// Extracted from "$PTOLEMY/src/compat/mathematica/Source/Includes/mathlink.h"
//
// converted ARGS (or P) macro in mathlink to PT_MMA_STUB (for functions
// that return a value) or PT_MMA_VOID_STUB (for void functions)
//

// Globals
int MLAbort, MLDone;

// Functions
extern "C" MLEnvironment MLInitialize
	PT_MMA_STUB((MLParametersPointer));
extern "C" void MLDeinitialize
	PT_MMA_VOID_STUB((MLEnvironment));

extern "C" MLEnvironment MLBegin
	PT_MMA_STUB((MLParametersPointer));
extern "C" void MLEnd
	PT_MMA_VOID_STUB((MLEnvironment));

extern "C" int   MLStringToArgv
	PT_MMA_STUB((kcharp_ct, ml_charp, ml_charpp, int));
extern "C" MLINK MLOpen
	PT_MMA_STUB((int, ml_charpp));
extern "C" MLINK MLOpenInEnv
	PT_MMA_STUB((MLEnvironment, int, ml_charpp, ml_longp));
extern "C" int   MLConnect
	PT_MMA_STUB((MLINK));
extern "C" int   MLFlush
	PT_MMA_STUB((MLINK));
extern "C" int   MLReady
	PT_MMA_STUB((MLINK));
extern "C" void  MLClose
	PT_MMA_VOID_STUB((MLINK));

extern "C" void  MLSetUserData
	PT_MMA_VOID_STUB((MLINK, MLPointer, MLUserFunctionType));
extern "C" MLPointer MLUserData
	PT_MMA_STUB((MLINK , MLUserFunctionType *));

extern "C" kcharp_ct MLName
	PT_MMA_STUB((MLINK));

extern "C" int   MLGetNext
	PT_MMA_STUB((MLINK));
extern "C" int   MLGetType
	PT_MMA_STUB((MLINK));
extern "C" int   MLGetData
	PT_MMA_STUB((MLINK, ml_charp, long, ml_longp));
extern "C" int   MLGetArgCount
	PT_MMA_STUB((MLINK, ml_longp));
extern "C" int   MLBytesToGet
	PT_MMA_STUB((MLINK, ml_longp));

extern "C" int   MLPutNext
	PT_MMA_STUB((MLINK, int));
extern "C" int   MLPutSize
	PT_MMA_STUB((MLINK, long));
extern "C" int   MLPutData
	PT_MMA_STUB((MLINK, kcharp_ct, long));
extern "C" int   MLPutArgCount
	PT_MMA_STUB((MLINK, long));
extern "C" int   MLBytesToPut
	PT_MMA_STUB((MLINK, ml_longp));

extern "C" int   MLGetShortInteger
	PT_MMA_STUB((MLINK, ml_shortp));
extern "C" int   MLGetInteger     
	PT_MMA_STUB((MLINK, ml_intp));
extern "C" int   MLGetLongInteger 
	PT_MMA_STUB((MLINK, ml_longp));
extern "C" int   MLGetFloat       
	PT_MMA_STUB((MLINK, ml_floatp));
extern "C" int   MLGetDouble      
	PT_MMA_STUB((MLINK, ml_doublep));

extern "C" int   MLGetString    
	PT_MMA_STUB((MLINK, kcharpp_ct));
extern "C" void  MLDisownString 
	PT_MMA_VOID_STUB((MLINK, kcharp_ct));
extern "C" int   MLGetSymbol    
	PT_MMA_STUB((MLINK, kcharpp_ct));
extern "C" int   MLGetFunction  
	PT_MMA_STUB((MLINK, kcharpp_ct, ml_longp));
extern "C" void  MLDisownSymbol 
	PT_MMA_VOID_STUB((MLINK, kcharp_ct));

extern "C" int   MLCheckFunction  
	PT_MMA_STUB((MLINK, kcharp_ct, ml_longp));
extern "C" int   MLCheckFunctionWithArgCount
	PT_MMA_STUB(( MLINK, kcharp_ct, ml_longp));

extern "C" int   MLGetIntegerList   
	PT_MMA_STUB((MLINK, ml_intpp, ml_longp));
extern "C" void  MLDisownIntegerList
	PT_MMA_VOID_STUB((MLINK, ml_intp, long ));
extern "C" int   MLGetRealList      
	PT_MMA_STUB((MLINK, ml_doublepp, ml_longp));
extern "C" void  MLDisownRealList   
	PT_MMA_VOID_STUB((MLINK, ml_doublep, long ));

extern "C" int   MLPutShortInteger  
	PT_MMA_STUB((MLINK, int));
extern "C" int   MLPutInteger    
	PT_MMA_STUB((MLINK, int));
extern "C" int   MLPutLongInteger
	PT_MMA_STUB((MLINK, long));

extern "C" int   MLPutFloat      
	PT_MMA_STUB((MLINK, double));
extern "C" int   MLPutDouble     
	PT_MMA_STUB((MLINK, double));

extern "C" int   MLPutString 
	PT_MMA_STUB((MLINK, kcharp_ct));
extern "C" int   MLPutSymbol 
	PT_MMA_STUB((MLINK, kcharp_ct));

extern "C" int   MLPutComposite
	PT_MMA_STUB((MLINK, long));
extern "C" int   MLPutFunction
	PT_MMA_STUB((MLINK, kcharp_ct, long));

extern "C" int   MLPutIntegerList
	PT_MMA_STUB((MLINK, ml_intp, long));
extern "C" int   MLPutRealList
	PT_MMA_STUB((MLINK, ml_doublep, long));

extern "C" int  MLNextPacket
	PT_MMA_STUB((MLINK));

extern "C" int  MLNewPacket
	PT_MMA_STUB((MLINK));
extern "C" int  MLEndPacket
	PT_MMA_STUB((MLINK));

extern "C" int	MLMain
	PT_MMA_STUB((int, ml_charpp));

extern "C" int	MLInstall
	PT_MMA_STUB((MLINK));
extern "C" int	MLAnswer
	PT_MMA_STUB((MLINK));
extern "C" int	MLDoCallPacket
	PT_MMA_STUB((MLINK));
extern "C" int	MLEvaluate
	PT_MMA_STUB((MLINK, ml_charp));
extern "C" void MLDefaultHandler
	PT_MMA_VOID_STUB((MLINK, unsigned long, unsigned long));
extern "C" PTMLLONG MLDefaultYielder
	PT_MMA_STUB((MLINK, MLYieldParameters));

extern "C" int	MLMainStep
	PT_MMA_STUB((MLINK));

extern "C" int   MLPutIntegerArray
	PT_MMA_STUB((MLINK, ml_intp, ml_longp, ml_charpp, long));

extern "C" int   MLPutShortIntegerArray
	PT_MMA_STUB((MLINK, ml_shortp, ml_longp, ml_charpp, long));

extern "C" int   MLPutLongIntegerArray
	PT_MMA_STUB((MLINK, ml_longp, ml_longp, ml_charpp, long));

extern "C" int   MLPutDoubleArray
	PT_MMA_STUB((MLINK, ml_doublep, ml_longp, ml_charpp, long));

extern "C" int   MLPutFloatArray
	PT_MMA_STUB((MLINK, ml_floatp, ml_longp, ml_charpp, long));

extern "C" int   MLGetIntegerArray
	PT_MMA_STUB((MLINK, ml_intpp, ml_longpp, ml_charppp, ml_longp));

extern "C" void  MLDisownIntegerArray
	PT_MMA_VOID_STUB((MLINK, ml_intp, ml_longp, ml_charpp, long));

extern "C" int   MLGetShortIntegerArray
	PT_MMA_STUB((MLINK, ml_shortpp, ml_longpp, ml_charppp, ml_longp));
extern "C" void  MLDisownShortIntegerArray
	PT_MMA_VOID_STUB((MLINK, ml_shortp, ml_longp, ml_charpp, long));

extern "C" int   MLGetLongIntegerArray
	PT_MMA_STUB((MLINK, ml_longpp, ml_longpp, ml_charppp, ml_longp));
extern "C" void  MLDisownLongIntegerArray
	PT_MMA_VOID_STUB((MLINK, ml_longp, ml_longp, ml_charpp, long));

extern "C" int   MLGetDoubleArray
	PT_MMA_STUB((MLINK, ml_doublepp, ml_longpp, ml_charppp, ml_longp));
extern "C" void  MLDisownDoubleArray
	PT_MMA_VOID_STUB((MLINK, ml_doublep, ml_longp, ml_charpp, long));

extern "C" int   MLGetFloatArray
	PT_MMA_STUB((MLINK, ml_floatpp, ml_longpp, ml_charppp, ml_longp));
extern "C" void  MLDisownFloatArray
	PT_MMA_VOID_STUB((MLINK, ml_floatp, ml_longp, ml_charpp, long));

extern "C" int   MLGetLongDouble
	PT_MMA_STUB((MLINK, ml_extendedp));
extern "C" int   MLPutLongDouble
	PT_MMA_STUB((MLINK, ml_extended));
extern "C" int   MLPutLongDoubleArray
	PT_MMA_STUB((MLINK, ml_extendedp, ml_longp, ml_charpp, long));
extern "C" int   MLGetLongDoubleArray
	PT_MMA_STUB((MLINK, ml_extendedpp, ml_longpp, ml_charppp, ml_longp));
extern "C" void  MLDisownLongDoubleArray
	PT_MMA_VOID_STUB((MLINK, ml_extendedp, ml_longp, ml_charpp, long));

extern "C" int MLTransferExpression
	PT_MMA_STUB((MLINK, MLINK));

extern "C" int   MLPutMessage
	PT_MMA_STUB((MLINK, unsigned long));
extern "C" int   MLMessageReady
	PT_MMA_STUB((MLINK));
extern "C" int   MLGetMessage
	PT_MMA_STUB((MLINK, ml_ulongp, ml_ulongp));

extern "C" MLMessageHandlerObject MLCreateMessageHandler
	PT_MMA_STUB((MLEnvironment, MLMessageHandlerType, MLPointer));
extern "C" MLMessageHandlerType MLDestroyMessageHandler
	PT_MMA_STUB((MLMessageHandlerObject));
extern "C" void MLCallMessageHandler
	PT_MMA_VOID_STUB((MLMessageHandlerObject, MLINK, unsigned long, unsigned long));

extern "C" MLMessageHandlerObject MLMessageHandler
	PT_MMA_STUB((MLINK));
extern "C" int   MLSetMessageHandler
	PT_MMA_STUB((MLINK, MLMessageHandlerObject));

extern "C" MLYieldFunctionObject MLCreateYieldFunction
	PT_MMA_STUB((MLEnvironment, MLYieldFunctionType, MLPointer));
extern "C" MLYieldFunctionType MLDestroyYieldFunction
	PT_MMA_STUB((MLYieldFunctionObject));
extern "C" PTMLLONG MLCallYieldFunction
	PT_MMA_STUB((MLYieldFunctionObject, MLINK, MLYieldParameters));

extern "C" MLYieldFunctionObject MLYieldFunction
	PT_MMA_STUB((MLINK));
extern "C" int   MLSetYieldFunction
	PT_MMA_STUB((MLINK, MLYieldFunctionObject));

extern "C" MLYieldFunctionObject MLDefaultYieldFunction
	PT_MMA_STUB((MLEnvironment));
extern "C" int MLSetDefaultYieldFunction
	PT_MMA_STUB((MLEnvironment, MLYieldFunctionObject));

extern "C" PTMLLONG MLError
	PT_MMA_STUB((MLINK));
extern "C" int   MLClearError
	PT_MMA_STUB((MLINK));
extern "C" int   MLSetError
	PT_MMA_STUB((MLINK, PTMLLONG));
extern "C" kcharp_ct MLErrorMessage
	PT_MMA_STUB((MLINK));


/* MLVERSION is defined by Mathematica 3.0 */

#ifdef MLVERSION

extern "C" mldlg_result MLAlert
	PT_MMA_STUB((MLEnvironment, kcharp_ct));
extern "C" mldlg_result MLRequest
	PT_MMA_STUB((MLEnvironment, kcharp_ct, ml_charp, long));

#else

extern "C" void MLAlert
	PT_MMA_VOID_STUB((MLEnvironment, kcharp_ct));
extern "C" char *MLRequest
	PT_MMA_STUB((MLEnvironment, kcharp_ct, ml_charp, long));

#endif


extern "C" mldlg_result MLConfirm
	PT_MMA_STUB((MLEnvironment, kcharp_ct, PTMLLONG));

extern "C" mldlg_result MLRequestArgv
	PT_MMA_STUB((MLEnvironment, ml_charpp, long, ml_charp, long));

extern "C" mldlg_result MLRequestToInteract
	PT_MMA_STUB((MLEnvironment, PTMLLONG));

extern "C" int MLSetDialogFunction
	PT_MMA_STUB((MLEnvironment, PTMLLONG, MLDialogFunctionType));

extern "C" MLINKMark  MLCreateMark
	PT_MMA_STUB((MLINK));
extern "C" MLINKMark  MLSeekToMark
	PT_MMA_STUB((MLINK, MLINKMark, long));
extern "C" void       MLDestroyMark
	PT_MMA_VOID_STUB((MLINK, MLINKMark));

extern "C" int  MLNewExpression
	PT_MMA_STUB((MLINK));
extern MLINK MLDuplicateLink
	PT_MMA_STUB((MLINK, ml_charp, ml_longp));

extern "C" ml_ucharp MLStringNextPosFun
	PT_MMA_STUB((MLStringPositionPointer));


/* MLVERSION is defined by Mathematica 3.0 */

#ifdef MLVERSION

extern "C" kcharp_ct MLStringFirstPosFun
	PT_MMA_STUB((kcharp_ct, MLStringPositionPointer));

extern "C" long MLPutCharToString
	PT_MMA_STUB((unsigned long, ml_charpp));

#else

extern "C" ml_ucharp MLStringFirstPosFun
	PT_MMA_STUB((kcharp_ct, MLStringPositionPointer));

extern "C" int MLPutCharToString
	PT_MMA_STUB((int, ml_charpp));

#endif

extern "C" int MLStringCharFun
	PT_MMA_STUB((MLStringPositionPointer));
