/*************************************************************************

Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Stub file for mathlink.h provided for inclusion when Mathematica is
not installed on the local machine.

The commercial version of mathlink.h is a Mathematica source file,
Copyright 1986 through 1993 by Wolfram Research Inc.

The file defines several C language definitions to define the
interface between Mathematica and other programs.  These definitions
are "dummy" definitions and do not reflect the actual implementation
of the MathLink interface.  Instead, these definitions are provided
only to prevent compilation errors when the true version of this
file is not available.  Definitions for non-Unix machines have been
removed.

*************************************************************************/

#ifndef _MATHLINK_H
#define _MATHLINK_H

#ifndef ARGS
#if defined(__STDC__) || defined(__cplusplus)
#define ARGS(args)      args
#else
#define ARGS(args)      ()
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ml_extended long double

typedef void* MLEnvironment;
typedef void* MLPointer;
typedef char* MLBuffer;
typedef char** MLBufferArray;

typedef unsigned char * ml_ucharp;
typedef char  *   ml_charp;
typedef char  **  ml_charpp;
typedef char  *** ml_charppp;
typedef short *   ml_shortp;
typedef short **  ml_shortpp;
typedef int   *   ml_intp;
typedef int   **  ml_intpp;
typedef long  *   ml_longp;
typedef long  **  ml_longpp;
typedef unsigned long * ml_ulongp;

typedef float  *  ml_floatp;
typedef float  ** ml_floatpp;
typedef double *  ml_doublep;
typedef double ** ml_doublepp;
typedef ml_extended *  ml_extendedp;
typedef ml_extended ** ml_extendedpp;

typedef MLPointer (*MLallocator) ARGS((unsigned long));
typedef void (*MLdeallocator) ARGS((MLPointer));
typedef struct MLYieldParams *MLYieldParameters;

/*************** Starting MathLink ***************/
typedef struct MLParameters {
	short version;
	MLallocator allocator;
	MLdeallocator deallocator;
} MLParameters;

typedef MLParameters *MLParametersPointer;

extern MLEnvironment MLInitialize ARGS((MLParametersPointer));
extern void MLDeinitialize ARGS((MLEnvironment));

extern MLEnvironment MLBegin ARGS((MLParametersPointer));
extern void MLEnd ARGS((MLEnvironment));


/*************** Connection interface ***************/

typedef struct MLink *MLINK;

extern int   MLStringToArgv ARGS((ml_charp, ml_charp, ml_charpp, int));
extern MLINK MLOpen ARGS((int, ml_charpp));
extern MLINK MLOpenInEnv ARGS((MLEnvironment, int, ml_charpp, ml_longp));
extern int   MLConnect ARGS((MLINK));
extern int   MLFlush ARGS((MLINK));
extern int   MLReady ARGS((MLINK));
extern void  MLClose ARGS((MLINK));

typedef void (*MLUserFunctionType) ARGS((MLINK));
extern void  MLSetUserData ARGS((MLINK, MLPointer, MLUserFunctionType));
extern MLPointer MLUserData ARGS((MLINK , MLUserFunctionType *));

extern ml_charp MLName ARGS((MLINK));

/*************** Textual interface ***************/

#define	MLTKSYM		'Y'
#define	MLTKSTR		'S'
#define	MLTKINT		'I'
#define	MLTKREAL	'R'
#define	MLTKFUNC	'F'
#define	MLTKPCTEND	']'
#define	MLTKERROR	(0)

extern int   MLGetNext ARGS((MLINK));
extern int   MLGetType ARGS((MLINK));
extern int   MLGetData ARGS((MLINK, ml_charp, long, ml_longp));
extern int   MLGetArgCount ARGS((MLINK, ml_longp));
extern int   MLBytesToGet ARGS((MLINK, ml_longp));

extern int   MLPutNext ARGS((MLINK, int));
extern int   MLPutSize ARGS((MLINK, long));
extern int   MLPutData ARGS((MLINK, ml_charp, long));
extern int   MLPutArgCount ARGS((MLINK, long));
extern int   MLBytesToPut ARGS((MLINK, ml_longp));

/*************** Native C types interface ***************/

extern int   MLGetShortInteger ARGS((MLINK, ml_shortp));
extern int   MLGetInteger      ARGS((MLINK, ml_intp));
extern int   MLGetLongInteger  ARGS((MLINK, ml_longp));
extern int   MLGetFloat        ARGS((MLINK, ml_floatp));
extern int   MLGetDouble       ARGS((MLINK, ml_doublep));
#define MLGetReal(mlp,xp) MLGetDouble(mlp,xp)

extern int   MLGetString     ARGS((MLINK, ml_charpp));
extern void  MLDisownString  ARGS((MLINK, ml_charp));
extern int   MLGetSymbol     ARGS((MLINK, ml_charpp));
extern int   MLGetFunction   ARGS((MLINK, ml_charpp, ml_longp));
extern void  MLDisownSymbol  ARGS((MLINK, ml_charp));

extern int   MLCheckFunction   ARGS((MLINK, ml_charp, ml_longp));
extern int   MLCheckFunctionWithArgCount ARGS(( MLINK, ml_charp, ml_longp));

extern int   MLGetIntegerList    ARGS((MLINK, ml_intpp, ml_longp));
extern void  MLDisownIntegerList ARGS((MLINK, ml_intp, long ));
extern int   MLGetRealList       ARGS((MLINK, ml_doublepp, ml_longp));
extern void  MLDisownRealList    ARGS((MLINK, ml_doublep, long ));

extern int   MLPutShortInteger   ARGS((MLINK, int));
extern int   MLPutInteger     ARGS((MLINK, int));
extern int   MLPutLongInteger ARGS((MLINK, long));

extern int   MLPutFloat       ARGS((MLINK, double));
extern int   MLPutDouble      ARGS((MLINK, double));
#define MLPutReal(mlp,x) MLPutDouble(mlp,x)

extern int   MLPutString  ARGS((MLINK, ml_charp));
extern int   MLPutSymbol  ARGS((MLINK, ml_charp));

#define	MLPutExpression(mlp,argn) 

extern int   MLPutComposite ARGS((MLINK, long));
extern int   MLPutFunction ARGS((MLINK, ml_charp, long));

extern int   MLPutIntegerList ARGS((MLINK, ml_intp, long));
extern int   MLPutRealList ARGS((MLINK, ml_doublep, long));


/*************** Mathematica packet interface ***************/

extern int  MLNextPacket ARGS((MLINK));

#define ILLEGALPKT      0

#define CALLPKT         7
#define EVALUATEPKT    13
#define RETURNPKT       3

#define INPUTNAMEPKT    8
#define ENTERTEXTPKT   14
#define ENTEREXPRPKT   15
#define OUTPUTNAMEPKT   9
#define RETURNTEXTPKT   4
#define RETURNEXPRPKT  16

#define DISPLAYPKT     11
#define DISPLAYENDPKT  12

#define MESSAGEPKT      5
#define TEXTPKT         2

#define INPUTPKT        1
#define INPUTSTRPKT    21
#define MENUPKT         6
#define SYNTAXPKT      10

#define SUSPENDPKT     17
#define RESUMEPKT      18

#define BEGINDLGPKT    19
#define ENDDLGPKT      20

#define FIRSTUSERPKT  128
#define LASTUSERPKT   255

extern int  MLNewPacket ARGS((MLINK));
extern int  MLEndPacket ARGS((MLINK));

/*************** Template interface ***************/

extern MLINK	stdlink;
extern int	MLMain ARGS((int, ml_charpp));

extern int	MLInstall ARGS((MLINK));
extern int	MLAnswer ARGS((MLINK));
extern int	MLDoCallPacket ARGS((MLINK));
extern int	MLEvaluate ARGS((MLINK, ml_charp));
extern void MLDefaultHandler ARGS((MLINK, unsigned long, unsigned long));
extern int  MLDefaultYielder ARGS((MLINK, MLYieldParameters));

extern int	MLAbort, MLDone;

#ifdef ML_OBSOLETE
extern int	MLMainStep ARGS((MLINK));
#endif

/*************** Experimental enhancements ***************/

extern int   MLPutIntegerArray
	     ARGS((MLINK, ml_intp, ml_longp, ml_charpp, long));

extern int   MLPutShortIntegerArray
	     ARGS((MLINK, ml_shortp, ml_longp, ml_charpp, long));

extern int   MLPutLongIntegerArray
	     ARGS((MLINK, ml_longp, ml_longp, ml_charpp, long));

extern int   MLPutDoubleArray
	     ARGS((MLINK, ml_doublep, ml_longp, ml_charpp, long));

extern int   MLPutFloatArray
	     ARGS((MLINK, ml_floatp, ml_longp, ml_charpp, long));

extern int   MLGetIntegerArray
	     ARGS((MLINK, ml_intpp, ml_longpp, ml_charppp, ml_longp));

extern void  MLDisownIntegerArray
	     ARGS((MLINK, ml_intp, ml_longp, ml_charpp, long));

extern int   MLGetShortIntegerArray
	     ARGS((MLINK, ml_shortpp, ml_longpp, ml_charppp, ml_longp));
extern void  MLDisownShortIntegerArray
	     ARGS((MLINK, ml_shortp, ml_longp, ml_charpp, long));

extern int   MLGetLongIntegerArray
	     ARGS((MLINK, ml_longpp, ml_longpp, ml_charppp, ml_longp));
extern void  MLDisownLongIntegerArray
	     ARGS((MLINK, ml_longp, ml_longp, ml_charpp, long));

extern int   MLGetDoubleArray
	     ARGS((MLINK, ml_doublepp, ml_longpp, ml_charppp, ml_longp));
extern void  MLDisownDoubleArray
	     ARGS((MLINK, ml_doublep, ml_longp, ml_charpp, long));

extern int   MLGetFloatArray
	     ARGS((MLINK, ml_floatpp, ml_longpp, ml_charppp, ml_longp));
extern void  MLDisownFloatArray
	     ARGS((MLINK, ml_floatp, ml_longp, ml_charpp, long));

extern int   MLGetLongDouble  ARGS((MLINK, ml_extendedp));
extern int   MLPutLongDouble  ARGS((MLINK, ml_extended));
extern int   MLPutLongDoubleArray
	     ARGS((MLINK, ml_extendedp, ml_longp, ml_charpp, long));
extern int   MLGetLongDoubleArray
	     ARGS((MLINK, ml_extendedpp, ml_longpp, ml_charppp, ml_longp));
extern void  MLDisownLongDoubleArray
	     ARGS((MLINK, ml_extendedp, ml_longp, ml_charpp, long));

#ifdef ML_OBSOLETE
#define MLEchoExpression MLTransferExpression
#endif

extern int MLTransferExpression ARGS((MLINK, MLINK));

/*************** Message interface ***************/
typedef void (*MLMessageHandlerType) ARGS((MLINK, unsigned long, unsigned long));

typedef MLPointer MLMessageHandlerObject;
enum {	MLTerminateMessage = 1, MLInterruptMessage, MLAbortMessage,
	MLEndPacketMessage, MLSynchronizeMessage, MLImDyingMessage,
	MLWaitingAcknowledgment,
	MLFirstUserMessage = 128, MLLastUserMessage = 255 };

extern int   MLPutMessage ARGS((MLINK, unsigned long));
extern int   MLMessageReady ARGS((MLINK));
extern int   MLGetMessage ARGS((MLINK, ml_ulongp, ml_ulongp));

extern MLMessageHandlerObject MLCreateMessageHandler
	ARGS((MLEnvironment, MLMessageHandlerType, MLPointer));
extern MLMessageHandlerType MLDestroyMessageHandler
	ARGS((MLMessageHandlerObject));
extern void MLCallMessageHandler
	ARGS((MLMessageHandlerObject, MLINK, unsigned long, unsigned long));

extern MLMessageHandlerObject MLMessageHandler ARGS((MLINK));
extern int   MLSetMessageHandler ARGS((MLINK, MLMessageHandlerObject));


/*************** Threads interface ***************/
typedef int (*MLYieldFunctionType) ARGS((MLINK, MLYieldParameters));

typedef MLPointer MLYieldFunctionObject;

extern MLYieldFunctionObject MLCreateYieldFunction
	ARGS((MLEnvironment, MLYieldFunctionType, MLPointer));
extern MLYieldFunctionType MLDestroyYieldFunction
	ARGS((MLYieldFunctionObject));
extern int MLCallYieldFunction
	ARGS((MLYieldFunctionObject, MLINK, MLYieldParameters));

extern MLYieldFunctionObject MLYieldFunction ARGS((MLINK));
extern int   MLSetYieldFunction ARGS((MLINK, MLYieldFunctionObject));

extern MLYieldFunctionObject MLDefaultYieldFunction
	ARGS((MLEnvironment));
extern int MLSetDefaultYieldFunction
	ARGS((MLEnvironment, MLYieldFunctionObject));

/*************** MathLink errors ***************/
extern int   MLError ARGS((MLINK));
extern int   MLClearError ARGS((MLINK));
extern int   MLSetError ARGS((MLINK, int));
extern ml_charp MLErrorMessage ARGS((MLINK));

#define	MLEUNKNOWN	-1
#define	MLEOK		 0
#define	MLEDEAD		 1
#define	MLEGBAD		 2
#define	MLEGSEQ		 3
#define	MLEPBTK		 4
#define	MLEPSEQ		 5
#define	MLEPBIG		 6
#define	MLEOVFL		 7
#define	MLEMEM		 8
#define	MLEACCEPT	 9
#define	MLECONNECT	 10
#define	MLECLOSED	 11
#define	MLEPUTENDPACKET	 21
#define	MLENEXTPACKET	 22
#define	MLEUNKNOWNPACKET 23
#define	MLEGETENDPACKET  24
#define MLEABORT         25

#define	MLEINIT		32	
#define	MLEARGV		33	
#define	MLEPROTOCOL	34	
#define	MLEMODE		35	
#define	MLELAUNCH	36	
#define	MLELAUNCHAGAIN	37	
#define	MLELAUNCHSPACE	38	
#define	MLENOPARENT	39	
#define	MLENAMETAKEN	40	
#define	MLENOLISTEN	41	
#define	MLEBADNAME	42	
#define	MLEBADHOST	43	

#define	MLEUSER        1000


/*************** User interaction--for internal use only ***************/

typedef void (*MLAlertFunctionType) ARGS((MLEnvironment, MLBuffer));
typedef MLBuffer (*MLRequestFunctionType)
	ARGS((MLEnvironment, MLBuffer, MLBuffer, long));
typedef int (*MLConfirmFunctionType)
	ARGS((MLEnvironment, MLBuffer, int));
typedef int (*MLRequestArgvFunctionType)
	ARGS((MLEnvironment, MLBufferArray, long, MLBuffer, long));
typedef int (*MLRequestToInteractFunctionType)
	ARGS((MLEnvironment, int));
typedef void (*MLDialogFunctionType) ARGS((MLEnvironment));

enum {	MLAlertFunction = 1, MLRequestFunction, MLConfirmFunction,
	MLRequestArgvFunction, MLRequestToInteractFunction };

extern void MLAlert ARGS((MLEnvironment, ml_charp));

extern ml_charp MLRequest
	ARGS((MLEnvironment, ml_charp, ml_charp, long));
extern int MLConfirm
	ARGS((MLEnvironment, ml_charp, int));

extern int MLRequestArgv
	ARGS((MLEnvironment, ml_charpp, long, ml_charp, long));

extern int MLRequestToInteract
	ARGS((MLEnvironment, int));

#define ML_DEFAULT_DIALOG ((MLDialogFunctionType) 1)
#define ML_IGNORE_DIALOG ((MLDialogFunctionType) 0)

extern int MLSetDialogFunction
	ARGS((MLEnvironment, int, MLDialogFunctionType));


/*************** 2.2 experimental enhancements ***************/

typedef struct MLinkMark* MLINKMark;
extern MLINKMark  MLCreateMark ARGS((MLINK));
extern MLINKMark  MLSeekToMark ARGS((MLINK, MLINKMark, long));
extern void       MLDestroyMark ARGS((MLINK, MLINKMark));

extern int  MLNewExpression ARGS((MLINK));
extern MLINK MLDuplicateLink ARGS((MLINK, ml_charp, ml_longp));

/* values returned by selector DEVICE_TYPE */
#define UNREGISTERED_TYPE	0
#define UNIXPIPE_TYPE	1
#define UNIXSOCKET_TYPE 2
#define PPC_TYPE	3
#define MACTCP_TYPE	4
#define LOOPBACK_TYPE	5
#define COMMTB_TYPE	6
#define ADSP_TYPE	7
#define LOCAL_TYPE	8
#define WINLOCAL_TYPE	9

/* selectors */
#define DEVICE_TYPE 0
#define DEVICE_NAME 1
#define PIPE_FD                (UNIXPIPE_TYPE * 256 + 0)
#define PIPE_CHILD_PID         (UNIXPIPE_TYPE * 256 + 1)
#define SOCKET_FD              (UNIXSOCKET_TYPE * 256 + 0)
#define SOCKET_PARTNER_ADDR    (UNIXSOCKET_TYPE * 256 + 1)
#define SOCKET_PARTNER_PORT    (UNIXSOCKET_TYPE * 256 + 2)
#define PPC_SESS_REF_NUM       (PPC_TYPE * 256 + 0)
#define PPC_PARTNER_PSN        (PPC_TYPE * 256 + 1)
#define PPC_PARTNER_LOCATION   (PPC_TYPE * 256 + 2)
#define PPC_PARTNER_PORT       (PPC_TYPE * 256 + 3)
#define MACTCP_STREAM          (MACTCP_TYPE * 256 + 0)
#define MACTCP_PARTNER_ADDR    (MACTCP_TYPE * 256 + 1)
#define MACTCP_PARTNER_PORT    (MACTCP_TYPE * 256 + 2)
#define MACTCP_IPDRIVER        (MACTCP_TYPE * 256 + 3)
#define COMMTB_CONNHANDLE      (COMMTB_TYPE * 256 + 0)
#define ADSP_CCBREFNUM         (ADSP_TYPE * 256 + 0)
#define ADSP_IOCREFNUM         (ADSP_TYPE * 256 + 3)

int MLDeviceInformation
	ARGS((MLINK, unsigned long, MLPointer, ml_longp));

/*************** MathLink non-ASCII string interface ***************/
typedef struct {
	ml_ucharp cc;
	int  mode;
	int  more;
	ml_ucharp head;
} MLStringPosition;

typedef MLStringPosition *MLStringPositionPointer;

extern int MLPutCharToString ARGS((int, ml_charpp));

#define MLforString(s,pos) \
  for ( MLStringFirstPos(s, pos); (pos).more; MLStringNextPos(pos))

#define MLStringChar(pos) \
  (((pos).mode <= 1) ? ((int)*(ml_ucharp)((pos).cc)) : MLStringCharFun(&pos))

extern ml_ucharp MLStringNextPosFun ARGS((MLStringPositionPointer));
extern ml_ucharp MLStringFirstPosFun ARGS((ml_charp, MLStringPositionPointer));
extern int MLStringCharFun ARGS((MLStringPositionPointer));

#ifdef __cplusplus
}
#endif

#endif /* _MATHLINK_H */
