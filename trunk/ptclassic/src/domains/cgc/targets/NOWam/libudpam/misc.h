/* misc.h:  Header for misc UDPAM stuff  */
/******************************************************************
Version identification:
@(#)misc.h	1.6 7/23/96

Copyright (c) 1995-1996  The Regents of the University of California.
All Rights Reserved.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer: Brent Chun 
 Creation Date: Sat Nov  4 17:25:01 PST 1995
*/

#ifndef _MISC_H
#define _MISC_H   "$Id$"

#include <stdlib.h>
#include <thread.h>
#include <udpam.h>

#define LockEP(endpoint)  \
      if (mutex_lock(&endpoint->lock) != 0) { \
          perror("mutex_lock error\n"); \
          exit(-1); \
      }    
#define UnlockEP(endpoint)  \
      if (mutex_unlock(&endpoint->lock) != 0) { \
          perror("mutex_lock error\n"); \
          exit(-1); \
      }    
#define LockBundle(bundle)  \
      if (mutex_lock(&bundle->lock) != 0) { \
          perror("mutex_lock error\n"); \
          exit(-1); \
      }    
#define UnlockBundle(bundle)  \
      if (mutex_unlock(&bundle->lock) != 0) { \
          perror("mutex_lock error\n"); \
          exit(-1); \
      }    

/*
 * TxFree FIFO management functions:
 *   Dequeue(endpoint,type): Dequeues a free buffer from TxFree
 *   Enqueue(endpoint,buf,type): Enqueues a free buffer onto TxFree
 */
extern void Enqueue(ea_t endpoint, UDPAM_Buf *buf);
extern UDPAM_Buf *Dequeue(ea_t endpoint);

/*
 * TxTimeout Management Functions:
 *   AddTimeout(endpoint,buf,req_buf_id,destination):  Add buf to timeout list
 *   RemoveTimeout(endpoint,buf): Remove buf from timeout list
 *   MoveToTailTimeout(endpoint,buf): Move buf to tail of timeout list
 */
extern void AddTimeout(ea_t endpoint, UDPAM_Buf *buf, int req_buf_id,
		       int req_seq_num, struct sockaddr_in *destination);
extern void RemoveTimeout(ea_t endpoint, UDPAM_Buf *buf);
extern void MoveToTailTimeout(ea_t endpoint, 
			      struct timeout_elem *timeout_elem);

/*
 * Miscellaneous Functions:
 *   GlobalToIndex(endpoint,global): Global to index in translation table
 *   BuildToken(token,....):  Builds a token 
 *   BuildArgBlock(argblock,...): Constructs an argblock to pass to err handler
 */
extern int  GlobalToIndex(ea_t endpoint, struct sockaddr_in *global);
extern void BuildToken(Token *token, struct sockaddr_in *sender, int buf_id, 
		       int seq_num, tag_t tag, ea_t dest_ep);
extern void BuildArgBlock(ArgBlock *argblock, int type, ea_t request_endpoint, 
			  int reply_endpoint, Token *token, handler_t handler, 
			  void *source_addr, void *dest_addr, int nbytes, 
			  int dest_offset, int source_offset, int arg0, 
			  int arg1, int arg2, int arg3, int arg4, int arg5, 
			  int arg6, int arg7, int buf_id,int seq_num);

/* Function to handle timeouts and events */
void *TimeoutThread(void *bundle);

#define QUANTA                2    /* Initial Timeout Value in Seconds */
#define UDPAM_MAX_RETRIES     3    /* Max Number of Retries */

typedef enum {FALSE, TRUE} Bool;

/* Misc macros */
 
#ifdef  DEBUG
#ifndef DPRINTF
#define DPRINTF(string)        printf string;  \
                               fflush(stdout);

#define ASSERT(n)   \
  if (!(n)) { \
    fprintf(stderr, "\nAssertion Failed: %s, Line %d\n", __FILE__, __LINE__); \
    fflush(stderr); \
    abort(); \
  }

#endif 
#else
#ifndef DPRINTF
#define DPRINTF(string)        /**/
#endif 

#define ASSERT(n)              /**/
#endif /* DEBUG */

#endif /* _MISC_H */




