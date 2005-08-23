/****************************************************************************
 *                      Annapolis Micro Systems Inc.                        *
 *                         190 Admiral Cochrane                             *
 *                              Suite 130                                   *
 *                         Annapolis, MD  21401                             *
 ****************************************************************************/
                                                                            
/****************************************************************************
 *  File       :  wf4.h
 *  Project    :  Wildforce
 *  Description:  Contains headers, macros, types, and consts that are
 *                specific only to WILDFORCE(tm)
 *  Author     :  Al Walters                                              
 *  Copyright  :  Annapolis Micro Systems Inc., 1996                      
 *                                                                        
 *  Notes
 *   o Exporting to end user.
 *                                                                        
 ****************************************************************************/

#ifndef __WF4_H__
#define __WF4_H__

#include <wfoss.h>
#include <wf4errs.h>

#define WF4_MAX_BOARDS                      (4)


/* 
 * Default FIFO Thresholds and other FIFO constants and macros 
 * shared between the API and RTL libraries
 */
#define WF4_FIFO_MAX_ALMOST_EMPTY_THRESHOLD   (508)
#define WF4_FIFO_MIN_ALMOST_EMPTY_THRESHOLD    (1)
#define WF4_FIFO_MAX_ALMOST_FULL_THRESHOLD    (508)
#define WF4_FIFO_MIN_ALMOST_FULL_THRESHOLD     (1)

#define WF4_MAX_PEX                           (4)
#define WF4_MAX_PES                           (WF4_MAX_PEX + 1)

#define WF4_MAX_PE_REGISTERS                  (8)

/*
 * The following define the bit patterns and values assigned to distinguish
 * the PEs on the board.
 */
#define WF4_PE0                             (1 << 0)
#define WF4_PE1                             (1 << 1)
#define WF4_PE2                             (1 << 2)
#define WF4_PE3                             (1 << 3)
#define WF4_PE4                             (1 << 4)
#define WF4_PE( x )                         (1 << (x))
#define WF4_ALL_PES                         (WF4_PE0 | WF4_PE1 | WF4_PE2 | WF4_PE3 | WF4_PE4)
#define WF4_ALL_PEX                         (WF4_PE1 | WF4_PE2 | WF4_PE3 | WF4_PE4)

/*
 * Defines for PE attributes
 */
#define WF4_MAX_ID_DESC_CHARS           (1024)

#define WF4_XILINX_NUM_TYPES            (15)
#define WF4_XILINX_4025                 (0)
#define WF4_XILINX_4025E                (1)
#define WF4_XILINX_4020E                (2)
#define WF4_XILINX_4013E                (3)
#define WF4_XILINX_4006E                (4)
#define WF4_XILINX_5206E                (5)
#define WF4_XILINX_4028EX               (6)
#define WF4_XILINX_4036EX               (7)
#define WF4_XILINX_4044EX               (8)
#define WF4_XILINX_4052EX               (9)
#define WF4_XILINX_4062EX               (10)
#define WF4_XILINX_4010EX               (11)
#define WF4_XILINX_4085EX               (12)
#define WF4_XILINX_4013EX               (13)
#define WF4_XILINX_4020EX               (14)


#define WF4_XILINX_NUM_PACKAGES         (3)
#define WF4_XILINX_HQ240                (0)
#define WF4_XILINX_HQ304                (1)
#define WF4_XILINX_BG432                (2)

typedef struct _WF4_PeConfiguration_
{
  DWORD  PeXilinxType;
  float  PeXilinxSpeedNanoseconds;
  DWORD  PeMemoryDwords;
  float  PeMemorySpeedNanoseconds;

  /* new field in V2.0 */ 
  DWORD  PePackageType;
} WF4_PeConfiguration;

#define WF4_PeXilinxType( p )              ( (p).PeXilinxType  )
#define WF4_PeXilinxSpeedNanoseconds( p )  ( (p).PeXilinxSpeedNanoseconds  )
#define WF4_PeMemoryDwords( p )            ( (p).PeMemoryDwords  )
#define WF4_PeMemorySpeedNanoseconds( p )  ( (p).PeMemorySpeedNanoseconds  )
#define WF4_PePackageType( p )             ( (p).PePackageType  )



/*
 * Return type for most function calls.  See the errors header file for the
 * possible return codes.
 */
typedef LONG    WF4_RetCode;

/*
 * Board number parameter passed in for most function calls.
 */
typedef DWORD   WF4_BoardNum;

/*
 * The following directory stores FIFO flag information for a single
 * FIFO.  Use the macros found in wf4defs.h to access these flags.
 * To fill the data structure, call the WF4_FifoGetStatusFlags( ) function.
 * Shared between the API and RTL.
 */
typedef struct _WF4_FifoFlags_
{
  UCHAR  PeToHostEmpty_n;
  UCHAR  HostToPeFull_n;
  UCHAR  PeToHostAlmostEmpty_n;
  UCHAR  HostToPeAlmostFull_n;
  UCHAR  HostToPeAlmostEmpty_n;
} WF4_FifoFlags;

/* Flag structure access */
#define WF4_FifoFlags_PeToHostEmpty_n( f )            ( (f).PeToHostEmpty_n  )
#define WF4_FifoFlags_HostToPeFull_n( f )             ( (f).HostToPeFull_n  )
#define WF4_FifoFlags_PeToHostAlmostEmpty_n( f )      ( (f).PeToHostAlmostEmpty_n )
#define WF4_FifoFlags_HostToPeAlmostFull_n( f )       ( (f).HostToPeAlmostFull_n )
#define WF4_FifoFlags_HostToPeAlmostEmpty_n( f )      ( (f).HostToPeAlmostEmpty_n )


/*
 * The following data structure holds the threshold values for
 * a FIFO.  Shared between the API and RTL.  Use the macros in wf4defs.h
 * to access this function.
 */
typedef struct _WF4_FifoThresholds_
{
  DWORD  HostToPeAlmostFull;
  DWORD  HostToPeAlmostEmpty;
  DWORD  PeToHostAlmostFull;
  DWORD  PeToHostAlmostEmpty;
} WF4_FifoThresholds;

/* Threshold structure access */
#define WF4_FifoThresholds_HostToPeAlmostFull( t )    ( (t).HostToPeAlmostFull )    
#define WF4_FifoThresholds_HostToPeAlmostEmpty( t )   ( (t).HostToPeAlmostEmpty )
#define WF4_FifoThresholds_PeToHostAlmostFull( t )    ( (t).PeToHostAlmostFull )
#define WF4_FifoThresholds_PeToHostAlmostEmpty( t )   ( (t).PeToHostAlmostEmpty )

/* 
 * FIFO enumerated data type in order to specify which FIFO to perform
 * operation on.  
 */
typedef enum _WF4_Fifo_
{
  WF4_Fifo_Pe0=0,
  WF4_Fifo_Pe1,
  WF4_Fifo_Pe4,
  WF4_Fifo_Max
} WF4_Fifo;

/* 
 * Interrupt bit masks for PE FIFOs -
 *   Each PE FIFO has (4) associated interrupts.  When calling some of the
 *   FIFO interrupt functions, use the following bits to identify which
 *   interrupt to perform the operation on.  Logical bit operations can
 *   be performed to specify multiple interrupts.  Use the 'x' macro
 *   to loop through, if necessary.
 */
#define WF4_FifoInterrupt_HostToPeFull_n          (1 << 0)
#define WF4_FifoInterrupt_PeToHostEmpty_n         (1 << 1)
#define WF4_FifoInterrupt_HostToPeMailBoxEmpty    (1 << 2)
#define WF4_FifoInterrupt_PeToHostMailBoxFull     (1 << 3)
#define WF4_FifoInterrupt( x )                    (1 << (x))

/*
 * Clock frequency range
 */
#define WF4_MIN_PCLK_FREQUENCY                    (1.0f)
#define WF4_MAX_PCLK_FREQUENCY                    (50.0f)
 
/*
 * Clock Source Mode 
 */
typedef enum _WF4_ClockSourceMode_
{
  WF4_ClockSourceMode_Slave=0,
  WF4_ClockSourceMode_Master,
  WF4_ClockSourceMode_Local,
  WF4_ClockSourceMode_Max
} WF4_ClockSourceMode;

/*
 * Clock Bank Mode .
 */
typedef enum _WF4_ClockBank_
{
  WF4_ClockBank_A=0,
  WF4_ClockBank_B,
  WF4_ClockBank_Max
} WF4_ClockBank;

/*
 * Clock Suspend Sources
 */
typedef enum _WF4_ClockSuspendSource_
{
  WF4_ClockSuspendSource_Pe0=0,
  WF4_ClockSuspendSource_ExtIo,
  WF4_ClockSuspendSource_Simd,
  WF4_ClockSuspendSource_Software,
  WF4_ClockSuspendSource_TermCount,
  WF4_ClockSuspendSource_None,
  WF4_ClockSuspendSource_Max
} WF4_ClockSuspendSource;

/*
 * Clock Run Modes
 */
typedef enum _WF4_ClockRunMode_
{
  WF4_ClockRunMode_Step=0,
  WF4_ClockRunMode_FreeRun,
  WF4_ClockRunMode_Max
} WF4_ClockRunMode;



typedef void * WF4_PeSymbolTable;


#define WF4_MAX_BIT_VECTOR_BITS        (128)

typedef void * WF4_SymbolBitVector;

#endif  /* sentry */
