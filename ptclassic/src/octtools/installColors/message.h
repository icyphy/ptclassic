/*
 * Message Managment Package Header
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains declarations required for using the VEM
 * message display facility.  This include file requires:
 *  - general.h
 */

#ifndef MESSAGE
#define MESSAGE

#include "general.h"

#define MSG_NOLOG	0x01	/* Don't log message to file */
#define MSG_DISP	0x02	/* Display message to user   */
#define MSG_MOREMD	0x04	/* Use more mode             */

extern char *errMsgArea;
  /* Area for building error messages */

extern vemStatus vemMsgSetCons();
  /* Sets the current console window */

extern vemStatus vemMsgFile();
  /* Sets the current log file */

extern vemStatus vemMessage();
  /* Writes out a message      */

#endif
