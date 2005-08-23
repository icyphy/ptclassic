/****************************************************************************
 *                      Annapolis Micro Systems Inc.                        *
 *                         190 Admiral Cochrane                             *
 *                              Suite 130                                   *
 *                         Annapolis, MD  21401                             *
 ****************************************************************************/
                                                                            
/****************************************************************************
 *  File       :  wf4api.h
 *  Project    :  Wildforce
 *  Description:  Contains the API level prototypes for the functions used
 *                in WF4 _only_.
 *  Author     :  Al Walters                                              
 *  Copyright  :  Annapolis Micro Systems Inc., 1995-97
 *                                                                        
 *  Notes
 *   o Exporting to end user.
 *   o The following header file contains common APIs used by both WF4 and
 *     WF1 boards.  Anything specific to the board should be put into the
 *     wf4api.h or the wf1api.h files, respectively.
 *                                                                        
 ****************************************************************************/

#ifndef __WF4API_H__
#define __WF4API_H__

#include "wf4.h"


/****************************************************************************
 *                              WF4 General APIs                             *
 ****************************************************************************/

WFAPI const char *
WF4_APIVersion( void );

WFAPI WF4_RetCode
WF4_Close( WF4_BoardNum  board );

WFAPI const char *
WF4_ErrorString( WF4_RetCode err );

WFAPI void *
WF4_MemAlloc( DWORD  numBytes );

WFAPI void
WF4_MemFree( void  *pBuffer );

WFAPI WF4_RetCode
WF4_Open( WF4_BoardNum  board,
          DWORD         flags );


/****************************************************************************
 *                              WF4 Clock APIs                              *
 ****************************************************************************/


WFAPI WF4_RetCode
WF4_ClkFreeRun( WF4_BoardNum         board );

WFAPI WF4_RetCode
WF4_ClkGetBank( WF4_BoardNum    board,
                WF4_ClockBank  *pBank );

WFAPI WF4_RetCode
WF4_ClkGetCounterValue( WF4_BoardNum   board,
                        DWORD         *pCount );

WFAPI WF4_RetCode
WF4_ClkGetRunMode( WF4_BoardNum       board,
                   WF4_ClockRunMode  *pRunMode );

WFAPI WF4_RetCode
WF4_ClkGetSourceMode( WF4_BoardNum          board,
                      WF4_ClockSourceMode  *pSourceMode );

WFAPI WF4_RetCode
WF4_ClkGetSuspendSource( WF4_BoardNum             board,
                         WF4_ClockSuspendSource  *pSuspendSource );

WFAPI WF4_RetCode
WF4_ClkInitLocal( WF4_BoardNum  board,
                  BOOLEAN       enableFreeRun,
                  float         frequencyMhz );
WFAPI WF4_RetCode
WF4_ClkInitMaster( WF4_BoardNum   board,
                   WF4_ClockBank  bank,
                   BOOLEAN        enableFreeRun,
                   float          frequencyMhz );

WFAPI WF4_RetCode
WF4_ClkInitSlave( WF4_BoardNum  board,
                  WF4_BoardNum  masterBoard );

WFAPI WF4_RetCode
WF4_ClkHwSuspendEnable( WF4_BoardNum            board,
                        WF4_ClockSuspendSource  suspendSource,
                        BOOLEAN                 enable );

WFAPI WF4_RetCode
WF4_ClkSetFrequency( WF4_BoardNum   board,
                     float          frequencyMhz );

WFAPI WF4_RetCode
WF4_ClkSetEclkFrequency( WF4_BoardNum   board,
                         float          frequencyMhz );

WFAPI WF4_RetCode
WF4_ClkStep( WF4_BoardNum  board,
             DWORD         steps );

WFAPI WF4_RetCode
WF4_ClkSuspend( WF4_BoardNum  board,
                BOOLEAN       enable );


/****************************************************************************
 *                              WF4 FIFO APIs                               *
 ****************************************************************************/

WFAPI WF4_RetCode
WF4_FifoGetStatusFlags( WF4_BoardNum    board,
                        WF4_Fifo        fifoName,
                        WF4_FifoFlags  *pFlags );

WFAPI WF4_RetCode
WF4_FifoGetThresholdSettings( WF4_BoardNum         board,
                              WF4_Fifo             fifoName,
                              WF4_FifoThresholds  *pThresholds );

WFAPI WF4_RetCode
WF4_FifoInitialize( WF4_BoardNum        board,
                    WF4_Fifo            fifoName,
                    WF4_FifoThresholds  thresholds );

WFAPI WF4_RetCode
WF4_FifoRead( WF4_BoardNum   board,
              WF4_Fifo       fifoName,
              DWORD         *pBuffer,
              DWORD          dwordsToRead,
              DWORD         *pDwordsRead,
              DWORD          dTimeout ); 

WFAPI WF4_RetCode
WF4_FifoReadMailBox( WF4_BoardNum   board,
                     WF4_Fifo       fifoName,
                     DWORD         *pMail );

WFAPI WF4_RetCode
WF4_FifoReset( WF4_BoardNum  board,
               WF4_Fifo      fifoName );

WFAPI WF4_RetCode
WF4_FifoWrite( WF4_BoardNum  board,
               WF4_Fifo      fifoName,
               DWORD        *pBuffer,
               DWORD         dwordsToWrite,
               DWORD        *pDwordsWritten,
               DWORD         dTimeout );

WFAPI WF4_RetCode
WF4_FifoWriteMailBox( WF4_BoardNum  board,
                      WF4_Fifo      fifoName,
                      DWORD         mail );

WFAPI WF4_RetCode
WF4_FifoWriteTag( WF4_BoardNum  board,
                  WF4_Fifo      fifoName,
                  DWORD         tagVal );

/****************************************************************************
 *                              WF4 Memory APIs                             *
 ****************************************************************************/

WFAPI WF4_RetCode
WF4_MemBlockPe( WF4_BoardNum  board,
                DWORD         peMask,
                BOOLEAN       blockPe );

WFAPI WF4_RetCode
WF4_MemGetBlockedStatus( WF4_BoardNum   board,
                         DWORD          peNum,
                         BOOLEAN       *pIsBlocked );
WFAPI WF4_RetCode
WF4_MemFifoRead( WF4_BoardNum   board,
                 DWORD          peNum,
                 DWORD          dDwordOffset,
                 DWORD          numDwords,
                 DWORD         *pDestVirtAddr );
                     
WFAPI WF4_RetCode
WF4_MemFifoWrite( WF4_BoardNum   board,
                  DWORD          peNum,
                  DWORD          dDwordOffset,
                  DWORD          numDwords,
                  DWORD         *pSrcVirtAddr );

WFAPI WF4_RetCode
WF4_MemRead( WF4_BoardNum   board,
             DWORD          peNum,
             DWORD          dDwordOffset,
             DWORD          numDwords,
             DWORD         *pDestVirtAddr );
                     
WFAPI WF4_RetCode
WF4_MemWrite( WF4_BoardNum   board,
              DWORD          peNum,
              DWORD          dDwordOffset,
              DWORD          numDwords,
              DWORD         *pSrcVirtAddr );

WFAPI WF4_RetCode
WF4_MemFill( WF4_BoardNum     board,
             DWORD            peMask,
             DWORD            pattern );


/****************************************************************************
 *                              WF4 PE APIs                                 *
 ****************************************************************************/

WFAPI WF4_RetCode
WF4_CreatePeSymbolTable( const char          *pFilename,
                         WF4_PeSymbolTable   *pPeSymbolTable );

WFAPI WF4_RetCode
WF4_CreateSymbolBitVector( WF4_PeSymbolTable     peSymbolTable,
                           char                 *pSymbols[],
                           WF4_SymbolBitVector  *pSymbolBitVector );

WFAPI WF4_RetCode 
WF4_DestroyPeSymbolTable( WF4_PeSymbolTable  *pSymbolTable );


WFAPI WF4_RetCode
WF4_DestroySymbolBitVector( WF4_SymbolBitVector  *pSymbolBitVector );

WFAPI WF4_RetCode
WF4_PeDisableInterrupts( WF4_BoardNum  board,
                         DWORD         peMask );

WFAPI WF4_RetCode
WF4_PeEnableInterrupts( WF4_BoardNum  board,
                        DWORD         peMask );

WFAPI WF4_RetCode
WF4_PeGetConfiguration( WF4_BoardNum          board,
                        DWORD                 peNum,
                        WF4_PeConfiguration  *pConfiguration );

WFAPI WF4_RetCode
WF4_PeProgram( WF4_BoardNum   board,
               DWORD          peMask,
               const char    *pFilename );

WFAPI WF4_RetCode
WF4_PeProgramFromBuffer( WF4_BoardNum   board,
                         DWORD          peMask,
                         const DWORD   *pProgramBuffer,
                         DWORD          numBytes );

WFAPI WF4_RetCode
WF4_PeQueryInterruptStatus( WF4_BoardNum   board,
                            DWORD          peMask,
                            DWORD         *pStatusMask );

WFAPI WF4_RetCode
WF4_PeReset( WF4_BoardNum  board,
             DWORD         peMask,
             BOOLEAN       enable );

WFAPI WF4_RetCode
WF4_PeResetInterrupts( WF4_BoardNum  board,
                       DWORD         peMask );

WFAPI WF4_RetCode 
WF4_ReadPeState( WF4_BoardNum    board,
                 DWORD           peNum );

WFAPI WF4_RetCode
WF4_ReadSymbolBitVector( WF4_BoardNum         board,
                         DWORD                peNum,
                         WF4_SymbolBitVector  symbolBitVector,
                         DWORD                startIdx,
                         DWORD                dBitLen,
                         DWORD               *pDword );

WFAPI WF4_RetCode
WF4_PeRegisterRead( WF4_BoardNum        board,
                    DWORD               peNum,
				    DWORD               regIndex,
                    DWORD              *pValue );
WFAPI WF4_RetCode
WF4_PeRegisterWrite( WF4_BoardNum        board,
                     DWORD               peNum,
				     DWORD               regIndex,
                     DWORD               dValue );

/****************************************************************************
 *                              WF4 Crossbar APIs                           *
 ****************************************************************************/

WFAPI WF4_RetCode
WF4_XbarSetConfig( WF4_BoardNum   board,
                   const char    *pFilename);





#endif  /* sentry */
