/************************************************************************
 *                      Annapolis Micro Systems Inc.                    *
 *                         190 Admiral Cochrane                         *
 *                              Suite 130                               *
 *                         Annapolis, MD  21401                         *
 ************************************************************************/
                                                                                                                                                    
/************************************************************************
 *  File       :  WF4ULIB.H
 *  Project    :  Wildforce                                               
 *  Description:  include file for name support
 *  Author     :  Mike Gray                                         
 *  Copyright  :  Annapolis Micro Systems Inc., 1996, 1997                      
 *                                                                        
 *                                                                        
 ************************************************************************/
#ifndef __WF4ULIB_H__
#define __WF4ULIB_H__

#include <stdio.h>
#include <string.h>
#include <wf4api.h>

/************************************************************************
 *                                Macros                                *
 ************************************************************************/

 /************************************************************************
 *                              Constants                               *
 ************************************************************************/

/************************************************************************
 *                              Prototypes                              *
 ************************************************************************/

char * 
WF4_XilinxString( DWORD xilinx );

char * 
WF4_RunModeString( WF4_ClockRunMode runMode );

char *
WF4_SuspendSourceString( WF4_ClockSuspendSource suspendSource );

char * 
WF4_FifoString( WF4_Fifo fifo );

char *
WF4_SourceModeString( WF4_ClockSourceMode sourceMode );

char * 
WF4_ClockBankString( WF4_ClockBank clockBank );

WF4_RetCode
WF4_SymbolDeleteList( char  ***pList );

WF4_RetCode
WF4_SymbolCreateList( WF4_PeSymbolTable  peSymbolTable,
                            char                    *pNetName,
                            char                   **pList[],
                            DWORD                    dListlen,
                            DWORD                   *pFound,
                            BOOL                     bIncrement,
                            BOOL                     bCaseSensitive );

#endif /* sentry */

