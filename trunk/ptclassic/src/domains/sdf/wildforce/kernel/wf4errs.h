/**************************************************************************
 *                      Annapolis Micro Systems Inc.                      *
 *                         190 Admiral Cochrane                           *
 *                              Suite 130                                 *
 *                         Annapolis, MD  21401                           *
 **************************************************************************/

/**************************************************************************
 *  File       :  wf4errs.h                                               *
 *  Project    :  Wildforce                                               *
 *  Description:  Common error codes used in the Wildforce project        *
 *  Author     :  Al Walters                                              *
 *  Copyright  :  Annapolis Micro Systems Inc., 1996                      *
 *                                                                        *
 *  Notes                                                                 *
 **************************************************************************/

#ifndef __WF4ERRS_H__
#define __WF4ERRS_H__


#define WF4_SUCCESS                                 (0)

/*
 * Driver related errors
 */
#define WF4_ERR_SECTION_0_START                     (1)
#define WF4_ERR_BAD_DRIVER_HANDLE                   (WF4_ERR_SECTION_0_START + 0)
#define WF4_ERR_DRIVER_SHUTDOWN                     (WF4_ERR_SECTION_0_START + 1)
#define WF4_ERR_IOCTL_FAILURE                       (WF4_ERR_SECTION_0_START + 2)

/*
 * Board I/O related errors
 */
#define WF4_ERR_SECTION_1_START                     (20)
#define WF4_ERR_INCOMPLETE_READ                     (WF4_ERR_SECTION_1_START + 0)
#define WF4_ERR_INCOMPLETE_WRITE                    (WF4_ERR_SECTION_1_START + 1)
#define WF4_ERR_MEM_ALLOC                           (WF4_ERR_SECTION_1_START + 2)
#define WF4_ERR_FILE_IO                             (WF4_ERR_SECTION_1_START + 3)
#define WF4_ERR_INVALID_ACCESS                      (WF4_ERR_SECTION_1_START + 4)
#define WF4_ERR_FUNCTION_NOT_SUPPORTED              (WF4_ERR_SECTION_1_START + 5)
#define WF4_ERR_BUS_EXCEPTION_TARGET_ABORT          (WF4_ERR_SECTION_1_START + 6)
#define WF4_ERR_OPERATION_TIMEOUT                   (WF4_ERR_SECTION_1_START + 7)

/*
 * Programming/Readback related errors
 */
#define WF4_ERR_SECTION_2_START                     (40)
#define WF4_ERR_PROGRAMMING_FAILED                  (WF4_ERR_SECTION_2_START + 0)
#define WF4_ERR_BAD_PROGRAM_COUNT                   (WF4_ERR_SECTION_2_START + 1)
#define WF4_ERR_OSC_PROGRAMMING                     (WF4_ERR_SECTION_2_START + 2)
#define WF4_ERR_INVALID_SYMBOL                      (WF4_ERR_SECTION_2_START + 3)
#define WF4_ERR_PROGRAMMING_TIMEOUT                 (WF4_ERR_SECTION_2_START + 4)
#define WF4_ERR_READBACK_TIMEOUT                    (WF4_ERR_SECTION_2_START + 5)
#define WF4_ERR_READBACK_FORMAT                     (WF4_ERR_SECTION_2_START + 6)

/*
 * Argument related errors
 */
#define WF4_ERR_SECTION_3_START                     (60)
#define WF4_ERR_INVALID_PE_ARG                      (WF4_ERR_SECTION_3_START + 0)
#define WF4_ERR_INVALID_PORT_ARG                    (WF4_ERR_SECTION_3_START + 1)
#define WF4_ERR_INVALID_CONFIGARRAY_ARG             (WF4_ERR_SECTION_3_START + 2)
#define WF4_ERR_INVALID_MODE_ARG                    (WF4_ERR_SECTION_3_START + 3)
#define WF4_ERR_INVALID_BOARD_ARG                   (WF4_ERR_SECTION_3_START + 4)
#define WF4_ERR_INVALID_FILENAME_ARG                (WF4_ERR_SECTION_3_START + 5)
#define WF4_ERR_INVALID_FIFO_ARG                    (WF4_ERR_SECTION_3_START + 6)
#define WF4_ERR_INVALID_THRESHOLD_ARG               (WF4_ERR_SECTION_3_START + 7)
#define WF4_ERR_INVALID_LENGTH_ARG                  (WF4_ERR_SECTION_3_START + 8)
#define WF4_ERR_INVALID_ADDRESS_ARG                 (WF4_ERR_SECTION_3_START + 9)
#define WF4_ERR_INVALID_XILINX_ARG                  (WF4_ERR_SECTION_3_START + 10)
#define WF4_ERR_INVALID_MEMORY_ARG                  (WF4_ERR_SECTION_3_START + 11)
#define WF4_ERR_INVALID_TIMEOUT_ARG                 (WF4_ERR_SECTION_3_START + 12)
#define WF4_ERR_INVALID_FLAGS_ARG                   (WF4_ERR_SECTION_3_START + 13)
#define WF4_ERR_INVALID_SYMBOL_TABLE_ARG            (WF4_ERR_SECTION_3_START + 14)
#define WF4_ERR_INVALID_SYMBOL_LIST_ARG             (WF4_ERR_SECTION_3_START + 15)
#define WF4_ERR_INVALID_BIT_VECTOR_ARG              (WF4_ERR_SECTION_3_START + 16)
#define WF4_ERR_INVALID_BIT_VECTOR_LENGTH           (WF4_ERR_SECTION_3_START + 17)
#define WF4_ERR_BOARD_ALREADY_OPEN                  (WF4_ERR_SECTION_3_START + 18)
#define WF4_ERR_INVALID_PE_REGISTER_ARG             (WF4_ERR_SECTION_3_START + 19)
/*
 * Clock related errors
 */
#define WF4_ERR_SECTION_4_START                     (80)
#define WF4_ERR_OSC_PROG_NOT_DONE                   (WF4_ERR_SECTION_4_START + 0)
#define WF4_ERR_INVALID_CLOCKRUNMODE_ARG            (WF4_ERR_SECTION_4_START + 1)
#define WF4_ERR_INVALID_CLOCKSUSPENDSOURCE_ARG      (WF4_ERR_SECTION_4_START + 2)
#define WF4_ERR_INVALID_CLOCKTYPE_ARG               (WF4_ERR_SECTION_4_START + 3)
#define WF4_ERR_INVALID_OSC_DATA_ARG                (WF4_ERR_SECTION_4_START + 4)
#define WF4_ERR_INVALID_OSC_MUX_SEL_ARG             (WF4_ERR_SECTION_4_START + 5)
#define WF4_ERR_INVALID_OSC_PORT_ARG                (WF4_ERR_SECTION_4_START + 6)
#define WF4_ERR_INVALID_FREQUENCY_ARG               (WF4_ERR_SECTION_4_START + 7)
#define WF4_ERR_INVALID_STEP_ARG                    (WF4_ERR_SECTION_4_START + 8)
#define WF4_ERR_INVALID_CLOCK_STATE                 (WF4_ERR_SECTION_4_START + 9)

/*
 * Other clock Master/Slave related errors
 */
#define WF4_ERR_SECTION_5_START                     (100)
#define WF4_ERR_INVALID_CLOCKBANK_ARG               (WF4_ERR_SECTION_5_START + 0)
#define WF4_ERR_INVALID_CLOCKMASTERBOARD_ARG        (WF4_ERR_SECTION_5_START + 1)
#define WF4_ERR_CLOCK_MASTER_EXISTS                 (WF4_ERR_SECTION_5_START + 2)
#define WF4_ERR_CLOCKSOURCEMODE_SLAVE               (WF4_ERR_SECTION_5_START + 3)
#define WF4_ERR_CLOCK_BANK_USED                     (WF4_ERR_SECTION_5_START + 4)
#define WF4_ERR_CLOCK_CABLE                         (WF4_ERR_SECTION_5_START + 5)

/*
 * System level errors
 */
#define WF4_ERR_SECTION_6_START                     (120)
#define WF4_ERR_BAD_EVENT_HANDLE                    (WF4_ERR_SECTION_6_START + 0)
#define WF4_ERR_BAD_MUTEX_HANDLE                    (WF4_ERR_SECTION_6_START + 1)
#define WF4_ERR_BAD_THREAD_HANDLE                   (WF4_ERR_SECTION_6_START + 2)
#define WF4_ERR_EVENT_TIMEOUT                       (WF4_ERR_SECTION_6_START + 3)
#define WF4_ERR_THREAD_TERMINATION_FAILURE          (WF4_ERR_SECTION_6_START + 4)

/*
 * CrossBar related errors
 */
#define WF4_ERR_SECTION_7_START                     (140)
#define WF4_ERR_INVALID_XBAR_CONFIGURATION          (WF4_ERR_SECTION_7_START + 0)
#define WF4_ERR_INVALID_XBAR_PORT                   (WF4_ERR_SECTION_7_START + 1)
#define WF4_ERR_INVALID_XBAR_CFG_FILE_LINE          (WF4_ERR_SECTION_7_START + 2)

/*
 * FIFO specific related errors
 */
#define WF4_ERR_SECTION_8_START                     (160)
#define WF4_ERR_INVALID_AF1_THRESHOLD               (WF4_ERR_SECTION_8_START + 0)
#define WF4_ERR_INVALID_AE1_THRESHOLD               (WF4_ERR_SECTION_8_START + 1)
#define WF4_ERR_INVALID_AF2_THRESHOLD               (WF4_ERR_SECTION_8_START + 2)
#define WF4_ERR_INVALID_AE2_THRESHOLD               (WF4_ERR_SECTION_8_START + 3)
#define WF4_ERR_FIFO_MAILBOX_EMPTY                  (WF4_ERR_SECTION_8_START + 4)
#define WF4_ERR_FIFO_MAILBOX_FULL                   (WF4_ERR_SECTION_8_START + 5)
#define WF4_ERR_INVALID_FIFO_TAG_ARG                (WF4_ERR_SECTION_8_START + 6)
#define WF4_ERR_FIFO_TIMEOUT                        (WF4_ERR_SECTION_8_START + 7)

/*
 * Configuration related errors
 */
#define WF4_ERR_SECTION_9_START                     (180)
#define WF4_ERR_CONFIG_PE_XILINX_TYPE               (WF4_ERR_SECTION_9_START + 0)
#define WF4_ERR_CONFIG_PE_SPEED_GRADE               (WF4_ERR_SECTION_9_START + 1)
#define WF4_ERR_CONFIG_PE_MEMORY_SIZE               (WF4_ERR_SECTION_9_START + 2)
#define WF4_ERR_CONFIG_PE_MEMORY_SPEED_GRADE        (WF4_ERR_SECTION_9_START + 3)
#define WF4_ERR_CONFIG_XBAR_XILINX_TYPE             (WF4_ERR_SECTION_9_START + 4)
#define WF4_ERR_CONFIG_XBAR_SPEED_GRADE             (WF4_ERR_SECTION_9_START + 5)
#define WF4_ERR_CONFIG_DATA_NOT_AVAILABLE           (WF4_ERR_SECTION_9_START + 6)


 
/*
 * External I/O Device related errors
 */
#define WF4_ERR_SECTION_10_START		                (500)
#define WF4_ERR_EXTERNAL_IO_SYSIO		                (WF4_ERR_SECTION_10_START + 0)
#define WF4_ERR_EXTERNAL_IO_VID0	                  (WF4_ERR_SECTION_10_START + 20)
#define WF4_ERR_EXTERNAL_IO_E1  	                  (WF4_ERR_SECTION_10_START + 40)
#define WF4_ERR_EXTERNAL_IO_E3  	                  (WF4_ERR_SECTION_10_START + 60)
#define WF4_ERR_EXTERNAL_IO_RS422  	                (WF4_ERR_SECTION_10_START + 80)


/*
 * Video0 related errors
 */
#define WF4_ERR_VID0                                (WF4_ERR_EXTERNAL_IO_VID0)
#define WF4_ERR_VID0_SHUTTERMODE_ARG                (WF4_ERR_VID0 + 0)
#define WF4_ERR_VID0_CAMERAFREQ_ARG                 (WF4_ERR_VID0 + 1)
#define WF4_ERR_VID0_CAMERACLKSRC_ARG               (WF4_ERR_VID0 + 2)
#define WF4_ERR_VID0_READCAMERAREG_ARG              (WF4_ERR_VID0 + 3)
#define WF4_ERR_VID0_ENABLE_CONTROL_ARG             (WF4_ERR_VID0 + 4)
#define WF4_ERR_VID0_ENABLE_CLOCK_ARG               (WF4_ERR_VID0 + 5)
#define WF4_ERR_VID0_SHUTTERMODE_SETTING            (WF4_ERR_VID0 + 6)
#define WF4_ERR_VID0_CAMERAFREQ_SETTING             (WF4_ERR_VID0 + 7)
#define WF4_ERR_VID0_CAMERACLKSRC_SETTING           (WF4_ERR_VID0 + 8)
#define WF4_ERR_VID0_CLKFROMCAMERA_SETTING          (WF4_ERR_VID0 + 9)
#define WF4_ERR_VID0_CAMERA_CTRL_ENABLE_SETTING     (WF4_ERR_VID0 + 10)
#define WF4_ERR_VID0_INVALID_DEVICE                 (WF4_ERR_VID0 + 11)

/*
 * E1 External I/O Module Errors
 */
#define WF4_ERR_E1                                  (WF4_ERR_EXTERNAL_IO_E1)
#define WF4_ERR_E1_NULL_ARG                         (WF4_ERR_E1 + 0)
#define WF4_ERR_E1_MODE_ARG                         (WF4_ERR_E1 + 1)
#define WF4_ERR_E1_RESET_ARG                        (WF4_ERR_E1 + 2)
#define WF4_ERR_E1_DEVICE_ARG                       (WF4_ERR_E1 + 3)
#define WF4_ERR_E1_INTERRUPT_CHAIN                  (WF4_ERR_E1 + 4)
#define WF4_ERR_E1_CHANNEL_NUM                      (WF4_ERR_E1 + 5)
#define WF4_ERR_E1_LOOPBACK_MODE                    (WF4_ERR_E1 + 6)
#define WF4_ERR_E1_REG_NAME                         (WF4_ERR_E1 + 7)
#define WF4_ERR_E1_INVALID_DEVICE                   (WF4_ERR_E1 + 9)


/*
 * WF/RS422 External I/O Module Errors
 */
#define WF4_ERR_RS422                               (WF4_ERR_EXTERNAL_IO_RS422)
#define WF4_ERR_RS422_INVALID_ARG                   (WF4_ERR_RS422 + 0)
#define WF4_ERR_RS422_NOT_INITIALIZED               (WF4_ERR_RS422 + 1)
#define WF4_ERR_RS422_INVALID_DEVICE                (WF4_ERR_RS422 + 2)
#define WF4_ERR_RS422_UNABLE_TO_INIT                (WF4_ERR_RS422 + 3)

 
/*
 * Expansion I/O Device related errors
 */
#define WF4_ERR_SECTION_11_START                    (700)
#define WF4_ERR_EXPANSION_IO_DSP0                   (WF4_ERR_SECTION_11_START + 0)


/*
 * DSP related errors
 */
#define WF4_ERR_DSP0                                (WF4_ERR_EXPANSION_IO_DSP0)
#define WF4_ERR_DSP0_INVALID_COFF_IMAGE             (WF4_ERR_DSP0 + 0)
#define WF4_ERR_DSP0_TOO_MANY_SECTIONS              (WF4_ERR_DSP0 + 1)
#define WF4_ERR_DSP0_BAD_OPTION_HDR                 (WF4_ERR_DSP0 + 2)
#define WF4_ERR_DSP0_PROGRAMMING                    (WF4_ERR_DSP0 + 3)
#define WF4_ERR_DSP0_INVALID_DEVICE                 (WF4_ERR_DSP0 + 4)
#define WF4_ERR_DSP0_FAILED_SEMAPHORE_ACQUISITION   (WF4_ERR_DSP0 + 5)
#define WF4_ERR_DSP0_FAILED_SEMAPHORE_RELEASE       (WF4_ERR_DSP0 + 6)

/*
 * Start of user defined errors
 *
 *   The following section allows users to define their own set
 *   of error codes that will not conflict with core library values.
 *
 */
#define WF4_ERR_SECTION_USER_DEFINED_START          (50000)
#define WF4_ERR_SECTION_USER_DEFINED_END            (59999)

#endif  /* sentry */
