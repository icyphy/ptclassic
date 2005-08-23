/******************************************************************
Version identification:
@(#)misc.c	1.15 04/13/98

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

#include <stdio.h>		/* define NULL, fprintf, and perror */
#include <stdlib.h>		/* define exit function */
#include <string.h>		/* define memcpy function */
#include <errno.h>		/* define errno variable */

#include <sys/time.h>		/* define gettimeofday */
#include <netinet/in.h>		/* define struct in_addr */
#include <arpa/inet.h>		/* define inet_ntoa function */

#include <udpam.h>
#include <am.h>
#include <misc.h>
#include <thread.h>

#ifdef SOLARIS 
/* For a complete discussion of gettimeofday under sol2.5,
 * see /usr/include/sys/time.h
 */
#if defined (PTSOL2_5) || (PTSOL2_6)
#define SOL2_5ORHIGHER
#else
int gettimeofday(struct timeval *);
#endif /* PTSOL2_5 || PTSOL2_6 */
#endif /* SOLARIS */

/*
 * Walk through endpoint's timeout list and resend messages that have 
 * timed out.  If a message has timed out too many times, invoke the error
 * handler.
 */
static
void ScanTimeoutList(ea_t ea)
{
  int                 num_elements, reply_endpoint;
  struct timeout_elem *timeout_elem, *next;
  struct timeval      curr_time;
  Token               token;     
  ArgBlock            argblock;
  UDPAM_Buf           *buf; 

  num_elements = ea->txtimeout.num_elements;
  timeout_elem = ea->txtimeout.head;
#ifdef SOL2_5ORHIGHER
  gettimeofday(&curr_time, NULL);
#elif SOLARIS
  gettimeofday(&curr_time);
#endif 

  while (num_elements-- > 0) {
    if ((curr_time.tv_sec - timeout_elem->timestamp.tv_sec) >
	(1 << (timeout_elem->num_tries - 1))*QUANTA) {
      if (timeout_elem->num_tries < UDPAM_MAX_RETRIES) {
	DPRINTF(("Timeout: Message={Type=0x%x, ReqBufID=%d ReqSeqNum=%d}\n", 
		 timeout_elem->message->type, timeout_elem->message->buf_id,
		 timeout_elem->message->seq_num));
	switch(timeout_elem->message->type) {
	case UDPAM_REQUEST_4:
	case UDPAM_REPLY_4:       
	  while (sendto(ea->socket,
  	      (const char *)&(timeout_elem->message->packet.udpam_short4_pkt),
              sizeof(timeout_elem->message->packet.udpam_short4_pkt), 0, 
              (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;
	case UDPAM_REQUEST_8:
	case UDPAM_REPLY_8:
	  while (sendto(ea->socket, 
              (const char *)&(timeout_elem->message->packet.udpam_short8_pkt),
              sizeof(timeout_elem->message->packet.udpam_short8_pkt), 0, 
	      (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;
	case UDPAM_REQUEST_MEDIUM_4:
	case UDPAM_REPLY_MEDIUM_4:
	  while (sendto(ea->socket, 
		(const char *)&(timeout_elem->message->packet.udpam_medium4_pkt),
		sizeof(timeout_elem->message->packet.udpam_medium4_pkt) - 
		(UDPAM_MAX_MEDIUM - 
		timeout_elem->message->packet.udpam_medium4_pkt.nbytes),
		0, (struct sockaddr *)&(timeout_elem->destination),
	        sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;
	case UDPAM_REQUEST_MEDIUM_8:
	case UDPAM_REPLY_MEDIUM_8:
	  while (sendto(ea->socket, 
              (const char *)&(timeout_elem->message->packet.udpam_medium8_pkt),
              sizeof(timeout_elem->message->packet.udpam_medium8_pkt) - 
	      (UDPAM_MAX_MEDIUM - 
	      timeout_elem->message->packet.udpam_medium8_pkt.nbytes),
	      0, (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;
	case UDPAM_REQUEST_LONG_4:
	case UDPAM_REPLY_LONG_4:
	  while (sendto(ea->socket, 
              (const char *)&(timeout_elem->message->packet.udpam_long4_pkt),
              sizeof(timeout_elem->message->packet.udpam_long4_pkt) -
   	      (UDPAM_MAX_LONG - 
              timeout_elem->message->packet.udpam_long4_pkt.nbytes),
  	      0, (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;
	case UDPAM_REQUEST_LONG_8:
	case UDPAM_REPLY_LONG_8:
	  while (sendto(ea->socket, 
              (const char *)&(timeout_elem->message->packet.udpam_long8_pkt),
              sizeof(timeout_elem->message->packet.udpam_long8_pkt) - 
   	      (UDPAM_MAX_LONG - 
              timeout_elem->message->packet.udpam_long8_pkt.nbytes),
	      0, (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;

        case UDPAM_GET_XFER_4:
	  while (sendto(ea->socket,
  	      (const char *)&(timeout_elem->message->packet.udpam_getxfer4_pkt),
              sizeof(timeout_elem->message->packet.udpam_getxfer4_pkt), 0, 
              (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }	  
	  break;	  

        case UDPAM_GET_XFER_8:
	  while (sendto(ea->socket,
  	      (const char *)&(timeout_elem->message->packet.udpam_getxfer8_pkt),
              sizeof(timeout_elem->message->packet.udpam_getxfer8_pkt), 0, 
              (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }	  
	  break;	
  
        case UDPAM_GET_XFER_REPLY_4:
	  while (sendto(ea->socket,
           (const char *)&(timeout_elem->message->packet.udpam_getxferreply4_pkt),
           sizeof(timeout_elem->message->packet.udpam_getxferreply4_pkt), 0, 
           (struct sockaddr *)&(timeout_elem->destination),
           sizeof(timeout_elem->destination)) < 0) {
	      if (errno != EAGAIN) {
	        perror("sendto error\n");
	        exit(-1);
	      }
	  }	  
	  break;	  

        case UDPAM_GET_XFER_REPLY_8:
	  while (sendto(ea->socket,
           (const char *)&(timeout_elem->message->packet.udpam_getxferreply8_pkt),
           sizeof(timeout_elem->message->packet.udpam_getxferreply8_pkt), 0, 
           (struct sockaddr *)&(timeout_elem->destination),
           sizeof(timeout_elem->destination)) < 0) {
	      if (errno != EAGAIN) {
	        perror("sendto error\n");
	        exit(-1);
	      }
	  }	  
	  break;	  

	case UDPAM_ERROR:
	  while (sendto(ea->socket, 
              (const char *)&(timeout_elem->message->packet.udpam_error_pkt),
   	      sizeof(timeout_elem->message->packet.udpam_error_pkt), 0, 
	      (struct sockaddr *)&(timeout_elem->destination),
	      sizeof(timeout_elem->destination)) < 0) {
	    if (errno != EAGAIN) {
	      perror("sendto error\n");
	      exit(-1);
	    }
	  }
	  break;	  
	default:
	  fprintf(stderr, "ScanTimeoutList: Bad Message Type 0x%x\n", 
		  timeout_elem->message->type);
	  abort();
	}
	next = timeout_elem->next;
	MoveToTailTimeout(ea, timeout_elem);
	timeout_elem = next;
      }
      else {   
	/*
	 * Message timed out too many times.  Bad news! 
	 */
	DPRINTF(("Timeout: Message: Timed Out %d Times.  Giving Up!\n",
		 UDPAM_MAX_RETRIES));
	DPRINTF(("Timeout: Message: {Type=0x%x BufID=%d SeqNum=%d TimesTried=%d}\n",
		timeout_elem->message->type, timeout_elem->req_buf_id,
		timeout_elem->req_seq_num, timeout_elem->num_tries));
	DPRINTF(("Timeout: Looks like Endpoint %s(Port %d) is dead\n",
		 inet_ntoa(timeout_elem->destination.sin_addr),
		 timeout_elem->destination.sin_port));

	switch(timeout_elem->message->type) {
	case UDPAM_REQUEST_4: {
	  UDPAM_Short4_Pkt *packetPtr;
	  /*
	   * Don't give back credit.  We don't want to send to an endpoint
	   * we think is dead.  If wsize goes to zero, no prob.
	   */
	  packetPtr = &(timeout_elem->message->packet.udpam_short4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_4, ea, reply_endpoint, &token, 
			packetPtr->handler, 0, 0, 0, 0, 0, packetPtr->arg0,
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REQUEST_4,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_8: {
	  UDPAM_Short8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_short8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_8, ea, reply_endpoint, &token, 
			packetPtr->handler, 0, 0, 0, 0, 0, packetPtr->arg0,
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REQUEST_8, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_medium4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_MEDIUM_4, ea, reply_endpoint, 
			&token, packetPtr->handler, packetPtr->source_addr, 
			0, packetPtr->nbytes, 0, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, 
						 AM_REQUEST_MEDIUM_4, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_medium8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_MEDIUM_8, ea, reply_endpoint, 
			&token, packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, 0, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, 
						 AM_REQUEST_MEDIUM_8, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_long4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_LONG_4, ea, reply_endpoint, 
			&token, packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, packetPtr->dest_offset, 0,
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, 0, 0, 0, 0, packetPtr->buf_id, 
			packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, 
						 AM_REQUEST_LONG_4,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_long8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_LONG_8, ea, reply_endpoint, 
			&token, packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, packetPtr->dest_offset, 0,
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			packetPtr->arg6, packetPtr->arg7, packetPtr->buf_id, 
			packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, 
						 AM_REQUEST_LONG_8,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_4: {
	  UDPAM_Short4_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_short4_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_4, 0, 0, &token, packetPtr->handler,
			0, 0, 0, 0, 0, packetPtr->arg0, packetPtr->arg1, 
			packetPtr->arg2, packetPtr->arg3, 0, 0, 0, 0,
			packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REPLY_4, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_8: {
	  UDPAM_Short8_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_short8_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_8, 0, 0, &token, packetPtr->handler,
			0, 0, 0, 0, 0, packetPtr->arg0, packetPtr->arg1, 
			packetPtr->arg2, packetPtr->arg3, packetPtr->arg4, 
			packetPtr->arg5, packetPtr->arg6, packetPtr->arg7,
			packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REPLY_8, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_medium4_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_MEDIUM_4, 0, 0, &token, 
			packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, 0, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REPLY_MEDIUM_4, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_medium8_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_MEDIUM_8, 0, 0, &token, 
			packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, 0, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REPLY_MEDIUM_8, 
						 (void *)&argblock);
	}
	  break;	  
        case UDPAM_REPLY_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_long4_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_LONG_4, 0, 0, &token, 
			packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, packetPtr->dest_offset, 0,
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, 0, 0, 0, 0, packetPtr->buf_id, 
			packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REPLY_LONG_4, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_long8_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_LONG_8, 0, 0, &token, 
			packetPtr->handler, packetPtr->source_addr,
			0, packetPtr->nbytes, packetPtr->dest_offset, 0,
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			packetPtr->arg6, packetPtr->arg7, packetPtr->buf_id, 
			packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_REPLY_LONG_8, 
						 (void *)&argblock);
	}
	  break; 
	/*
         * Gets (implict req)
	 */
	case UDPAM_GET_XFER_4: {
	  UDPAM_GetXfer4_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_getxfer4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));

	  BuildArgBlock(&argblock, AM_GET_XFER_8, ea, reply_endpoint, 
			&token, packetPtr->handler, 0, 0, 
			packetPtr->nbytes, packetPtr->dest_offset, 
			packetPtr->source_offset,
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, 0, 0, 0, 0, packetPtr->buf_id, 
			packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_GET_XFER_4,
						 (void *)&argblock);
	}
	  break;

	case UDPAM_GET_XFER_8: {
	  UDPAM_GetXfer8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_getxfer8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));

	  BuildArgBlock(&argblock, AM_GET_XFER_8, ea, reply_endpoint, 
			&token, packetPtr->handler, 0, 0,
			packetPtr->nbytes, packetPtr->dest_offset, 
			packetPtr->source_offset,
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			packetPtr->arg6, packetPtr->arg7, packetPtr->buf_id, 
			packetPtr->seq_num);
	  ((ErrorHandler)(ea->handler_table[0]))(EUNREACHABLE, AM_GET_XFER_8,
						 (void *)&argblock);
	}
	  break;

       /*	    
	* These are internally generated messages.  
	*/
	case UDPAM_GET_XFER_REPLY_4: 
	case UDPAM_GET_XFER_REPLY_8: 
        case UDPAM_ERROR: 
	  /* Throw it on the floor */
          break;

	default:
	  fprintf(stderr, "ScanTimeoutList: TxTimeout Corrupted?\n");
	  break;
	}
	/* 
	 * Following the error handler's execution, take the buffer 
	 * containing the message that timed out off TxTimeout and put it back 
	 * on TxFree.  Hopefully, the error handler did something intelligent 
 	 * with it.
	 */
	next = timeout_elem->next;   /* Save pointer before removing */
	/*
	 * Oh no!  There goes my original data for medium/long messages.  I
	 * hope someone has the original.
	 */ 
	buf = timeout_elem->message;               /* Save pointer to buffer */
	RemoveTimeout(ea, timeout_elem->message);  /* Pull off TxTimeout */ 
	Enqueue(ea, buf);                          /* Place on TxFree */
	timeout_elem = next;                       /* Check next message */
      }
    }
  }
}

/* 
 * Post semaphore and wake up a thread waiting on an event.
 */
static 
int AM_PostSvar(eb_t eb)
{
  ASSERT(eb != NULL);
  
  /*
   * If sema is 0, then post, else ignore.
   */
  if (eb->synch_var.count == 0) {
    if (sema_post(&(eb->synch_var)) != 0) {
      perror("sema_post error\n");
      exit(-1);
    }
    DPRINTF(("Posted Semaphore.  Count is now %ld\n", eb->synch_var.count));
  }
  return(AM_OK);
}

void MoveToTailTimeout(ea_t endpoint, struct timeout_elem *timeout_elem)
{
  UDPAM_TimeoutList *list;

  ASSERT(timeout_elem != NULL);

  list = &(endpoint->txtimeout);
  timeout_elem->num_tries++;  
  if ((list->num_elements == 1) || (timeout_elem == list->tail)) {
#ifdef SOL2_5ORHIGHER
    gettimeofday(&(timeout_elem->timestamp), NULL);
#elif SOLARIS
    gettimeofday(&(timeout_elem->timestamp));
#endif
    return;
  }
  if (timeout_elem == list->head) {
    timeout_elem->next->prev = (struct timeout_elem *)NULL;
    list->head = timeout_elem->next;    
  }
  else {
    timeout_elem->prev->next = timeout_elem->next;
    timeout_elem->next->prev = timeout_elem->prev;
  }
#ifdef SOL2_5ORHIGHER
  gettimeofday(&(timeout_elem->timestamp), NULL);
#elif SOLARIS
  gettimeofday(&(timeout_elem->timestamp));
#endif
  list->tail->next = timeout_elem;
  timeout_elem->prev = list->tail;
  timeout_elem->next = (struct timeout_elem *)NULL;
  list->tail = timeout_elem;
}

UDPAM_Buf *Dequeue(ea_t endpoint)
{
  UDPAM_Buf     *buf;
  UDPAM_BufFifo *fifo;

  fifo = &(endpoint->txfree);
  if (fifo->num_elements == 0) {
    return((UDPAM_Buf *)NULL);
  }
  else {   
    buf = fifo->pointers[fifo->head];  
    fifo->head = (fifo->head + 1) % endpoint->num_txbufs;
    fifo->num_elements--; 
    return(buf);
  }
}

void Enqueue(ea_t endpoint, UDPAM_Buf *buf)
{
  UDPAM_BufFifo *fifo;

  ASSERT(buf != NULL);
  fifo = &(endpoint->txfree);
  if (fifo->num_elements == 0) 
    fifo->head = fifo->tail = 0;  /* FIFO previously empty */
  else 
    fifo->tail = (fifo->tail + 1) % endpoint->num_txbufs;
  fifo->pointers[fifo->tail] = buf;
  fifo->num_elements++;
}

void AddTimeout(ea_t endpoint, UDPAM_Buf *buf, int req_buf_id, 
		int req_seq_num, struct sockaddr_in *destination)
{
  int                 i = 0;
  struct timeout_list *list;

  ASSERT(buf != NULL);
  list = &(endpoint->txtimeout);
  while (i < endpoint->num_txbufs) {   
    if (list->unackmessages[i].message == (UDPAM_Buf *)NULL)
      break;
    else 
      i++;
  }

  ASSERT(i != endpoint->num_txbufs);

  list->unackmessages[i].message = buf;
  list->unackmessages[i].req_buf_id = req_buf_id;    /* BufID of Request */
  list->unackmessages[i].req_seq_num = req_seq_num;  /* SeqNum of Request */
#ifdef SOL2_5ORHIGHER
  gettimeofday(&(list->unackmessages[i].timestamp), NULL);
#elif SOLARIS
  gettimeofday(&(list->unackmessages[i].timestamp));
#endif
  list->unackmessages[i].destination.sin_port = destination->sin_port;
  list->unackmessages[i].destination.sin_addr.s_addr = 
    destination->sin_addr.s_addr;
  list->unackmessages[i].num_tries = 1;   /* First try */
  if (list->num_elements == 0) {
    list->unackmessages[i].prev = (struct timeout_elem *)NULL;
    list->unackmessages[i].next = (struct timeout_elem *)NULL;
    list->head = &(list->unackmessages[i]);
    list->tail = &(list->unackmessages[i]);    
  }
  else {
    list->tail->next = &(list->unackmessages[i]);
    list->unackmessages[i].prev = list->tail;
    list->unackmessages[i].next = (struct timeout_elem *)NULL;
    list->tail = &(list->unackmessages[i]);
  }
  endpoint->txtimeout.num_elements++;     
  return;
}

void RemoveTimeout(ea_t endpoint, UDPAM_Buf *buf)
{
  int                 i = 0;
  struct timeout_list *list;
  
  ASSERT(buf != NULL);
  list = &(endpoint->txtimeout);

  while (i < endpoint->num_txbufs) {          
    if (list->unackmessages[i].message == buf)  /* Found buf */ 
      break;
    else 
      i++;
  }

  ASSERT(i != endpoint->num_txbufs);
  
  buf->seq_num++;    /* Increment sequence number to detect dups */

  if (list->num_elements == 1) 
    list->head = list->tail = (struct timeout_elem *)NULL;
  else if (&(list->unackmessages[i]) == list->head) {
    list->unackmessages[i].next->prev = (struct timeout_elem *)NULL;
    list->head = list->unackmessages[i].next;
  }
  else if (&(list->unackmessages[i]) == list->tail) {
    list->unackmessages[i].prev->next = (struct timeout_elem *)NULL;
    list->tail= list->unackmessages[i].prev;
  }
  else {
    list->unackmessages[i].prev->next = list->unackmessages[i].next;
    list->unackmessages[i].next->prev = list->unackmessages[i].prev;
  }
  list->num_elements--;
  list->unackmessages[i].message = (UDPAM_Buf *)NULL;
}

/* 
 * Given an endpoint name, return an index in the endpoint's translation
 * table (if any).
 */
int GlobalToIndex(ea_t endpoint, struct sockaddr_in *global)
{
  int i;     
  
  for (i = 0; i < endpoint->num_translations; i++) 
    if (endpoint->translation_table[i].inuse)
      if ((endpoint->translation_table[i].name.port == global->sin_port) &&
	  (endpoint->translation_table[i].name.ip_addr == 
	   global->sin_addr.s_addr))
	return(i);
  return(-1);  
}

void BuildToken(Token *token, struct sockaddr_in *sender, int buf_id, 
		int seq_num, tag_t tag, ea_t dest_ep)
{
  token->source_port = sender->sin_port;     
  token->source_ip_addr = sender->sin_addr.s_addr;
  token->buf_id = buf_id;
  token->seq_num = seq_num;
  token->tag = tag;
  token->dest_ep = dest_ep;
}

void BuildArgBlock(ArgBlock *argblock, int type, ea_t request_endpoint, 
		   int reply_endpoint, Token *token, handler_t handler, 
		   void *source_addr, void *dest_addr, int nbytes, 
		   int dest_offset, int source_offset,
		   int arg0, int arg1, int arg2, int arg3, int arg4,
		   int arg5, int arg6, int arg7, int buf_id, int seq_num)
{
  argblock->request_endpoint = request_endpoint;
  argblock->reply_endpoint   = reply_endpoint;

  if (type & REPLY)
    memcpy(&(argblock->token), token, sizeof(Token));
  argblock->handler       = handler;
  argblock->source_addr   = source_addr;
  argblock->nbytes        = nbytes;
  argblock->dest_offset   = dest_offset;
  argblock->source_offset = source_offset;
  argblock->arg0          = arg0;
  argblock->arg1          = arg1;
  argblock->arg2          = arg2;
  argblock->arg3          = arg3;
  argblock->arg4          = arg4;
  argblock->arg5          = arg5;
  argblock->arg6          = arg6;
  argblock->arg7          = arg7;
  argblock->buf_id        = buf_id;    
  argblock->seq_num       = seq_num;
}

/*
 * Function to handle timeouts and events.  Timeouts are a real pain,
 * so let's not do them that often. 
 */
void *TimeoutThread(void *voidbundle)
{
  int              num_eps, err, n, flag = 0;
  struct ep_elem   *ep_elem_ptr;
  eb_t             bundle;
  fd_set           temp_fdset;
  timestruc_t      ts_quanta;
  struct timeval   tv_quanta;
  
  err = 0;
  bundle = (eb_t)voidbundle;
  ts_quanta.tv_nsec = 0;
  tv_quanta.tv_sec = QUANTA;
  tv_quanta.tv_usec = 0;

  LockBundle(bundle);   

  while (1) {
     
     /* 
      * If we're not waiting on events, flag should be 0.
      */
     if (flag == 0) {
	ts_quanta.tv_sec = time(NULL) + QUANTA;
	/* 
	 * Give up bundle lock until the CV is posted or timeouts need to
	 * be done.
	 */
	err = cond_timedwait(&(bundle->event_cv), &(bundle->lock), &ts_quanta);
     }
     /*
      * If we're not waiting on an event yet and the timer has expired, 
      * clean up timeout lists.  Otherwise, block waiting for an event.
      */
     if ((flag == 0) && (err == ETIME)) {
	num_eps = bundle->num_eps;
	ep_elem_ptr = bundle->head;		
	while (num_eps-- > 0) {
	   LockEP(ep_elem_ptr->endpoint);
	   if (ep_elem_ptr->endpoint->txtimeout.num_elements > 0)
	     ScanTimeoutList(ep_elem_ptr->endpoint);
	   UnlockEP(ep_elem_ptr->endpoint); 
	   ep_elem_ptr = ep_elem_ptr->next; 
	}
	/* Cheap hack? */
	if (bundle->mask == AM_NOTEMPTY)
	  flag = 1;
     }   
     else if ((flag == 1) || (err == 0)) {  
	memcpy(&temp_fdset, &(bundle->fdset), sizeof(temp_fdset));
	flag = 1;
	
	UnlockBundle(bundle);   /* Hope CV sig doesn't get lost */
	if ((n = select(bundle->maxfd + 1, &temp_fdset, NULL, NULL, 
			&tv_quanta)) < 0) {
	   perror("select error\n");
	   exit(-1);
	}
	else if (n > 0) {
	   LockBundle(bundle);
	   flag = 0;
	   AM_PostSvar(bundle);
	   /* If CV was lost, fix it up */
	   if (bundle->mask == AM_NOTEMPTY)
	     flag = 1;
	}
	else {
	   LockBundle(bundle);
	   /* If CV was lost, fix it up */
	   if (bundle->mask == AM_NOTEMPTY)
	     flag = 1;
	   /* 
	    * err will still be 0 when we come back since we're not calling
	    * cond_timedwait() while we're waiting for a message.  Thread
	    * is waiting for a message, has timed out, so must handle
	    * timeout/retry.
	    */
	   num_eps = bundle->num_eps;
	   ep_elem_ptr = bundle->head;		
	   while (num_eps-- > 0) {
	      LockEP(ep_elem_ptr->endpoint);
	      if (ep_elem_ptr->endpoint->txtimeout.num_elements > 0)
		ScanTimeoutList(ep_elem_ptr->endpoint);
	      UnlockEP(ep_elem_ptr->endpoint); 
	      ep_elem_ptr = ep_elem_ptr->next; 
	   }      
	}
     }
     else if (err < 0) {
	perror("cond_timedwait error\n");
	exit(-1);
     }
  }
}
