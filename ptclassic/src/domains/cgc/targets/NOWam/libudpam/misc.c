/* Misc. stuff to manage locks, FIFOS, lists) */
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

 Programmer: Brent Chun 
 Creation Date: Sat Nov  4 17:25:01 PST 1995
*******************************************************************/

static char rcsid[] = "@(#)$Id$";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <udpam.h>
#include <am.h>
#include <misc.h>
#ifdef SOLARIS 
#include <thread.h>
#endif /* SOLARIS */

extern int  GlobalToIndex(ea_t endpoint, struct sockaddr_in *global);
extern void BuildToken(Token *token, struct sockaddr_in *sender, int buf_id, 
		       int seq_num, tag_t tag, ea_t dest_ep);
extern void BuildArgBlock(ArgBlock *argblock, int type, ea_t request_endpoint, 
			  int reply_endpoint, Token *token, int handler, 
			  void *source_addr, int nbytes, int dest_offset, 
			  int arg0, int arg1, int arg2, int arg3, int arg4,
			  int arg5, int arg6, int arg7, char *data);

/* 
 * NOTE: This code executes with ea LOCKED
 */
static
void ScanTimeoutList(ea_t ea)
{
  int                 num_elements, reply_endpoint;
  struct timeout_elem *timeout_elem, *next;
  struct timeval      curr_time;
  Token               token;     
  ArgBlock            argblock;

  num_elements = ea->txtimeout.num_elements;
  timeout_elem = ea->txtimeout.head;
  gettimeofday(&curr_time);
  while (num_elements-- > 0) {
    if ((curr_time.tv_sec - timeout_elem->timestamp.tv_sec) >
	(1 << (timeout_elem->num_tries - 1))*QUANTA) {
      if (timeout_elem->num_tries < UDPAM_MAX_RETRIES) {
	DPRINTF(("Timeout: Resending Message = {Type=0x%x, ReqBufID=%d ReqSeqNum=%d}\n", 
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
		sizeof(timeout_elem->message->packet.udpam_medium4_pkt), 0, 
 	        (struct sockaddr *)&(timeout_elem->destination),
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
              sizeof(timeout_elem->message->packet.udpam_medium8_pkt), 0, 
	      (struct sockaddr *)&(timeout_elem->destination),
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
              sizeof(timeout_elem->message->packet.udpam_long4_pkt), 0, 
	      (struct sockaddr *)&(timeout_elem->destination),
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
              sizeof(timeout_elem->message->packet.udpam_long8_pkt), 0, 
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
	 * Message timed out too many times.  Bad news!  =)  The only 
	 * member of the argblock that cannot be retrieved is the source_addr
	 * parameter.  However, given that we have a pointer to nbytes 
	 * of that data, do we really need it?
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

	  packetPtr = &(timeout_elem->message->packet.udpam_short4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST, ea, reply_endpoint, &token, 
			packetPtr->handler, 0, 0, 0, packetPtr->arg0,
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			0, 0, 0, 0, NULL);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REQUEST, EUNREACHABLE,
						 (void *)&argblock);
	  /* 
	   * Don't give back credit.  We *don't* want to send to the
	   * dead endpoint.  If wsize goes to zero, great!  Unmap()
	   * and Map a new endpoint.
	   */
	}
	  break;
        case UDPAM_REQUEST_8: {
	  UDPAM_Short8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_short8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST, ea, reply_endpoint, &token, 
			packetPtr->handler, 0, 0, 0, packetPtr->arg0,
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			packetPtr->arg7, NULL);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REQUEST, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_medium4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_MEDIUM, ea, reply_endpoint, 
			&token, packetPtr->handler, 0,
			packetPtr->nbytes, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			0, 0, 0, 0, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REQUEST_MEDIUM, 
						 EUNREACHABLE, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_medium8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_MEDIUM, ea, reply_endpoint, 
			&token, packetPtr->handler, 0,
			packetPtr->nbytes, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			packetPtr->arg7, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REQUEST_MEDIUM, 
						 EUNREACHABLE, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_long4_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_LONG, ea, reply_endpoint, 
			&token, packetPtr->handler, 0,
			packetPtr->nbytes, packetPtr->dest_offset, 
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, 0, 0, 0, 0, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REQUEST_LONG,
						 EUNREACHABLE, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REQUEST_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;

	  packetPtr = &(timeout_elem->message->packet.udpam_long8_pkt);
	  reply_endpoint = GlobalToIndex(ea, &(timeout_elem->destination));
	  BuildArgBlock(&argblock, AM_REQUEST_LONG, ea, reply_endpoint, 
			&token, packetPtr->handler, 0,
			packetPtr->nbytes, packetPtr->dest_offset, 
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			packetPtr->arg6, packetPtr->arg7, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REQUEST_LONG,
						 EUNREACHABLE, 
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_4: {
	  UDPAM_Short4_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_short4_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  /* timeout_elem->message->packet.udpam_short4_pkt.tag, ea); */
	  BuildArgBlock(&argblock, AM_REPLY, 0, 0, &token, packetPtr->handler,
			0, 0, 0, packetPtr->arg0, packetPtr->arg1, 
			packetPtr->arg2, packetPtr->arg3, 0, 0, 0, 0, NULL);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REPLY, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_8: {
	  UDPAM_Short8_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_short8_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY, 0, 0, &token, packetPtr->handler,
			0, 0, 0, packetPtr->arg0, packetPtr->arg1, 
			packetPtr->arg2, packetPtr->arg3, packetPtr->arg4, 
			packetPtr->arg5, packetPtr->arg6, packetPtr->arg7, 
			NULL);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REPLY, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_medium4_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_MEDIUM, 0, 0, &token, 
			packetPtr->handler, 0,
			packetPtr->nbytes, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			0, 0, 0, 0, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REPLY_MEDIUM, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_medium8_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_MEDIUM, 0, 0, &token, 
			packetPtr->handler, 0,
			packetPtr->nbytes, 0, packetPtr->arg0, 
			packetPtr->arg1, packetPtr->arg2, packetPtr->arg3,
			packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			packetPtr->arg7, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REPLY_MEDIUM, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;	  
        case UDPAM_REPLY_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_long4_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_LONG, 0, 0, &token, 
			packetPtr->handler, 0,
			packetPtr->nbytes, packetPtr->dest_offset, 
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, 0, 0, 0, 0, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REPLY_LONG, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;
        case UDPAM_REPLY_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;
	  
	  packetPtr = &(timeout_elem->message->packet.udpam_long8_pkt);
	  BuildToken(&token, &(ea->sockaddr), timeout_elem->req_buf_id,
		     timeout_elem->req_seq_num, packetPtr->tag, ea);
	  BuildArgBlock(&argblock, AM_REPLY_LONG, 0, 0, &token, 
			packetPtr->handler, 0,
			packetPtr->nbytes, packetPtr->dest_offset, 
			packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			packetPtr->arg6, packetPtr->arg7, packetPtr->data);
	  ((ErrorHandler)(ea->handler_table[0]))(AM_REPLY_LONG, EUNREACHABLE,
						 (void *)&argblock);
	}
	  break;
	default:
	  fprintf(stderr, "ScanTimeoutList: TxTimeout Corrupted?\n");
	  break;
	}
	/* 
	 * After the error handler executes, take the message that timed out
	 * off txtimeout and put it back on txfree.  Hopefully, the
	 * error handler did something intelligent with it.  We have to
	 * do this here since we're using a pointer to bulk data, and
	 * not a copy in argblocks. 
	 */
	next = timeout_elem->next;   /* Save pointer before removing */

	RemoveTimeout(ea, timeout_elem->message);  /* Pull off TxTimeout */ 
	Enqueue(ea, timeout_elem->message);        /* Place on TxFree */

	timeout_elem = next;         /* Try another message */
      }
    }
    else
      break; /* Other messages still have time left */
  }
}

void MoveToTailTimeout(ea_t endpoint, struct timeout_elem *timeout_elem)
{
  UDPAM_TimeoutList *list;

  list = &(endpoint->txtimeout);
  timeout_elem->num_tries++;  
  if ((list->num_elements == 1) || (timeout_elem == list->tail)) {
    gettimeofday(&(timeout_elem->timestamp));
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
  gettimeofday(&(timeout_elem->timestamp));
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

  list = &(endpoint->txtimeout);
  while (i < endpoint->num_txbufs) {   /* Linear Search, Ack! */
    if (list->unackmessages[i].message == (UDPAM_Buf *)NULL)
      break;
    else 
      i++;
  }
  list->unackmessages[i].message = buf;
  list->unackmessages[i].req_buf_id = req_buf_id;    /* BufID of Request */
  list->unackmessages[i].req_seq_num = req_seq_num;  /* SeqNum of Request */
  gettimeofday(&(list->unackmessages[i].timestamp));
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
  
  list = &(endpoint->txtimeout);

  while (i < endpoint->num_txbufs) {          
    if (list->unackmessages[i].message == buf)  /* Found buf */ 
      break;
    else 
      i++;
  }
  buf->seq_num++;  /* Increment sequence number to detect dups */
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
 * Thread alternates between sleeping for QUANTA seconds and
 * scanning and retransmitting messages that have timed out.  
 * FIX ME: Get Event Model Right.
 */
void *TimeoutThread(void *voidbundle)
{
  int              num_eps;
  struct ep_elem   *ep_elem_ptr;
  eb_t             bundle;
  
  bundle = (eb_t)voidbundle;
  while (1) {
    sleep(QUANTA);
    LockBundle(bundle);
    num_eps = bundle->num_eps;
    ep_elem_ptr = bundle->head;		
    while (num_eps-- > 0) {
      LockEP(ep_elem_ptr->endpoint);
      if (ep_elem_ptr->endpoint->txtimeout.num_elements > 0)
	ScanTimeoutList(ep_elem_ptr->endpoint);
      UnlockEP(ep_elem_ptr->endpoint); 
      ep_elem_ptr = ep_elem_ptr->next; 
    }
    UnlockBundle(bundle);
  }
}
