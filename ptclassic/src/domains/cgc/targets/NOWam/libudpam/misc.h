/* Misc. stuff to manage locks, FIFOS, lists for NOW */
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995-%Q%  The Regents of the University of California.
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
							COPYRIGHTENDKEY

 Programmer: The NOW Project, UC Berkeley
*******************************************************************/

#ifndef _MISC_H
#define _MISC_H   "$Id$"

#include <stdlib.h>
#include <udpam.h>

#ifdef SOLARIS
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
#endif /* SOLARIS */

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

/* Routine to handle timeouts and events */
void *TimeoutThread(void *bundle);

#define QUANTA                2    /* Initial Tout in secs */
#define UDPAM_MAX_RETRIES     3    /* Max Number of Retries */
#define MESSAGES_PER_ENDPOINT 5
/* #define DEBUG2  */

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




