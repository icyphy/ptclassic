/* Active Message Transport Functions */
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

/*
 * $Log$
 * Revision 1.13  1995/12/04  14:42:42  bnc
 * Moved locking to transport.c.  The only function outside of this file
 * that locks anything is the timeout thread's routine.
 *
 * Revision 1.12  1995/12/01  10:49:01  bnc
 * Fixed error checking for the most part.  Added simple static window based
 * flow control.  Reply functions grab everything they need from the token.
 *
 * Revision 1.11  1995/11/30 00:20:54  bnc
 * *** empty log message ***
 *
 * TODO: Check seg/length checks.
 */
static char rcsid[] = "@(#)$Id$";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <udpam.h>
#include <am.h>
#include <misc.h>

#define FindReply(replyEndpoint,bufID,seqNum,sender,replyElem) \
          SeenRequest(replyEndpoint, bufID, seqNum, &sender, &replyElem)

#define NO_ERRORS             0x0
#define MAX_MESSAGES_PER_POLL 5

static Bool initialized = FALSE;  /* AM layer initialized? */

/* Send replying endpoint an unreliable hint(ack) to free up reply storage */
static 
void ReplyACK(int sender, struct sockaddr_in receiver, int buf_id, int seq_num)
{
  UDPAM_ReplyACK_Pkt packet;
  
  packet.type = UDPAM_REPLYACK;
  packet.buf_id = buf_id;    /* Buf ID of Request */
  packet.seq_num = seq_num;  /* Seq Num of Request Buf */
  while (sendto(sender, (const char *)&packet, sizeof(packet), 0,  
		(struct sockaddr *)&receiver, sizeof(receiver)) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    /* No need to poll here */
  }
}

/*
 * If a REPLY on TxTimeout is found that has a matching 
 * req_address/buf_id/seq_num, the request is a duplicate.  Otherwise, this 
 * is the first time we've seen it.  If a match is found, set the pointer 
 * that we passed in and  return 1, else return 0.  
 */
static
int SeenRequest(ea_t receiver, int req_buf_id, int req_seq_num, 
		struct sockaddr_in *sender, struct timeout_elem **reply_elem)
{
  struct timeout_elem *elem;
  
  elem = receiver->txtimeout.head;
  while (elem != (struct timeout_elem *)NULL) {
    if ((elem->destination.sin_port == sender->sin_port) &&
	(elem->destination.sin_addr.s_addr == sender->sin_addr.s_addr))
      if ((elem->req_buf_id == req_buf_id) && 
	  (elem->req_seq_num == req_seq_num) &&
	  (elem->message->type & REPLY)) {
	*reply_elem = elem;
	return(1);
      }
    elem = elem->next;     
  }
  return(0);
}

/*
 * If a reply acknowlegment was received and a reply handler executed,
 * the sequence number (32-bit int) for the buffer that previously 
 * contained the request should be different.  If it's not, this is the
 * first time.
 */
static
int SeenReply(ea_t request_endpoint, int buf_id, int seq_num)
{
  return(request_endpoint->txpool[buf_id].seq_num != seq_num ? 1 : 0);
}

/*
 * If a duplicate request was seen, resend the appropriate reply.  *sigh*..
 * such long names...
 */
static
void ResendReply(ea_t reply_endpoint, struct timeout_elem *dup_elem)
{
  DPRINTF(("ResendReply: Message = {Type=0x%x, ReqBufID=%d ReqSeqNum=%d}\n",
	   dup_elem->message->type, dup_elem->message->buf_id, 
	   dup_elem->message->seq_num));
  switch(dup_elem->message->type) {
  case UDPAM_REPLY_4:       
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_short4_pkt),
		  sizeof(dup_elem->message->packet.udpam_short4_pkt), 0, 
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
      AM_Poll(reply_endpoint->bundle);
    }
    break;
  case UDPAM_REPLY_8:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_short8_pkt),
		  sizeof(dup_elem->message->packet.udpam_short8_pkt), 0, 
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
      AM_Poll(reply_endpoint->bundle);
    }
    break;
  case UDPAM_REPLY_MEDIUM_4:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_medium4_pkt),
		  sizeof(dup_elem->message->packet.udpam_medium4_pkt), 0, 
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
      AM_Poll(reply_endpoint->bundle);
    }
    break;
  case UDPAM_REPLY_MEDIUM_8:
    while (sendto(reply_endpoint->socket, 
	       (const char *)&(dup_elem->message->packet.udpam_medium8_pkt),
	       sizeof(dup_elem->message->packet.udpam_medium8_pkt), 0, 
	       (struct sockaddr *)&(dup_elem->destination),
	       sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
      AM_Poll(reply_endpoint->bundle);
    }
    break;
  case UDPAM_REPLY_LONG_4:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_long4_pkt),
		  sizeof(dup_elem->message->packet.udpam_long4_pkt), 0, 
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
      AM_Poll(reply_endpoint->bundle);
    }
    break;
  case UDPAM_REPLY_LONG_8:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_long8_pkt),
		  sizeof(dup_elem->message->packet.udpam_long8_pkt), 0, 
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0){
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
      AM_Poll(reply_endpoint->bundle);
    }
    break;
  default:
    fprintf(stderr, "ResendReply: Bad Message Type.  TxTimeout Probably Corrupted\n");
    abort();
  }
  MoveToTailTimeout(reply_endpoint, dup_elem);   /* Resent, num_tries++ */
}

/*
 * Checks EBADTAG, EBADHANDLER, ECONGESTION, EBADSEGOFF, EBADLENGTH
 */
static
op_t RequestErrorCheck(ea_t replyEndpoint, int type, tag_t pktTag, 
		       handler_t pktHandler, int destOffset, int nbytes)
{
  if (pktTag != replyEndpoint->tag)
    return(EBADTAG);
  else if ((pktHandler < 0) || (pktHandler >= replyEndpoint->num_handlers) ||
	   (replyEndpoint->handler_table[pktHandler] == abort)) 
    return(EBADHANDLER);
  else if (replyEndpoint->txfree.num_elements == 0) 
    return(ECONGESTION);
  else if ((type & (UDPAM_REQUEST_LONG_4 | UDPAM_REQUEST_LONG_8)) && 
	   ((destOffset < 0) || (destOffset > 
 	   ((int)(replyEndpoint->start_addr)) + replyEndpoint->length)))
    return(EBADSEGOFF);
  else if ((type & (UDPAM_REQUEST_LONG_4 | UDPAM_REQUEST_LONG_8)) && 
	   ((destOffset + nbytes) > ((int)(replyEndpoint->start_addr)) + 
				     replyEndpoint->length))
    return(EBADLENGTH);
  else 
    return(NO_ERRORS);
}

/*
 * Checks EBADTAG, EBADHANDLER, EBADSEGOFF, EBADLENGTH
 */
static
op_t ReplyErrorCheck(ea_t requestEndpoint, int type, tag_t pktTag,
		     handler_t pktHandler, int destOffset, int nbytes)
{
  if (pktTag != requestEndpoint->tag)
    return(EBADTAG);
  else if ((pktHandler < 0) || (pktHandler >= requestEndpoint->num_handlers) ||
	   (requestEndpoint->handler_table[pktHandler] == abort)) 
    return(EBADHANDLER);
  else if ((type & (UDPAM_REPLY_LONG_4 | UDPAM_REPLY_LONG_8)) && 
	   ((destOffset < 0) || (destOffset > 
	   ((int)(requestEndpoint->start_addr)) + requestEndpoint->length)))
    return(EBADSEGOFF);
  else if ((type & (UDPAM_REPLY_LONG_4 | UDPAM_REPLY_LONG_8)) && 
	   ((destOffset + nbytes) > ((int)(requestEndpoint->start_addr)) +
				     requestEndpoint->length))
    return(EBADLENGTH);
  else 
    return(NO_ERRORS);
}

/* Return Index In Translation Table */
int GlobalToIndex(ea_t endpoint, struct sockaddr_in *global)
{
  int i;     
  
  for (i = 0; i < endpoint->num_translations; i++) 
    if (endpoint->translation_table[i].inuse)
      if ((endpoint->translation_table[i].name.port == global->sin_port) &&
	  (endpoint->translation_table[i].name.ip_addr == 
	   global->sin_addr.s_addr))
	return(i);
  return(-1);  /* Never! */
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

/*
 * Save arguments to active messages functions in case an error is detected.
 * I think this handles all cases:
 */ 
void BuildArgBlock(ArgBlock *argblock, int type, ea_t request_endpoint, 
		   int reply_endpoint, Token *token, int handler, 
		   void *source_addr, int nbytes, int dest_offset, 
		   int arg0, int arg1, int arg2, int arg3, int arg4,
		   int arg5, int arg6, int arg7, char *data)
{
  argblock->request_endpoint = request_endpoint;
  argblock->reply_endpoint = reply_endpoint;
  if (type & (AM_REPLY | AM_REPLY_MEDIUM | AM_REPLY_LONG))  /* Works? */
    memcpy(&(argblock->token), token, sizeof(Token));
  argblock->handler = handler;
  argblock->source_addr = source_addr;
  argblock->nbytes = nbytes;
  argblock->dest_offset = dest_offset;
  argblock->arg0 = arg0;
  argblock->arg1 = arg1;
  argblock->arg2 = arg2;
  argblock->arg3 = arg3;
  argblock->arg4 = arg4;
  argblock->arg5 = arg5;
  argblock->arg6 = arg6;
  argblock->arg7 = arg7;
  argblock->data = data;  /* Set pointer.  Shorts don't care */
}

/*
 * Active Message API 
 */
int AM_Init()
{
  initialized = TRUE;
  return(AM_OK);
}
 
int AM_Terminate()
{
  initialized = FALSE;  /* AM disabled */
  return(AM_OK);
}

int AM_Request4(ea_t request_endpoint, int reply_endpoint, handler_t handler, 
		int arg0, int arg1, int arg2, int arg3)
{
  UDPAM_Buf          *buf;
  UDPAM_Short4_Pkt   *packet; 
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  while (request_endpoint->translation_table[reply_endpoint].wsize == 0)
    AM_Poll(request_endpoint->bundle);
  LockEP(request_endpoint);
  while ((buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  }
  UnlockEP(request_endpoint);
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = 
    request_endpoint->translation_table[reply_endpoint].name.port;
  dest_sockaddr.sin_addr.s_addr = 
    request_endpoint->translation_table[reply_endpoint].name.ip_addr;
  buf->type = UDPAM_REQUEST_4;    
  packet = &(buf->packet.udpam_short4_pkt);
  packet->type = UDPAM_REQUEST_4;
  packet->tag = request_endpoint->translation_table[reply_endpoint].tag;
  packet->buf_id = buf->buf_id;    /* Set Request Buf ID */
  packet->seq_num = buf->seq_num;  /* Set Request Buf SeqNum */
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet), 0, (struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(request_endpoint->bundle);
  }
  /* Decrement window size */
  request_endpoint->translation_table[reply_endpoint].wsize--;
  LockEP(request_endpoint);
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);          
}

int AM_Request8(ea_t request_endpoint, int reply_endpoint, handler_t handler, 
		int arg0, int arg1, int arg2, int arg3,
		int arg4, int arg5, int arg6, int arg7)
{
  UDPAM_Buf          *buf;
  UDPAM_Short8_Pkt   *packet; 
  struct sockaddr_in dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);

  while (request_endpoint->translation_table[reply_endpoint].wsize == 0)
    AM_Poll(request_endpoint->bundle);
  LockEP(request_endpoint);
  while ((buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  }
  UnlockEP(request_endpoint);
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = 
    request_endpoint->translation_table[reply_endpoint].name.port;
  dest_sockaddr.sin_addr.s_addr = 
    request_endpoint->translation_table[reply_endpoint].name.ip_addr;
  buf->type = UDPAM_REQUEST_8;     
  packet = &(buf->packet.udpam_short8_pkt);
  packet->type = UDPAM_REQUEST_8;
  packet->tag = request_endpoint->translation_table[reply_endpoint].tag;
  packet->buf_id = buf->buf_id;    
  packet->seq_num = buf->seq_num;  
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->arg4 = arg4;
  packet->arg5 = arg5;
  packet->arg6 = arg6;
  packet->arg7 = arg7;
  while (sendto(request_endpoint->socket, (const char *)packet, 
		sizeof(*packet), 0, (struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(request_endpoint->bundle);
  }
  request_endpoint->translation_table[reply_endpoint].wsize--;
  LockEP(request_endpoint);
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);          
}

int AM_RequestMedium4(ea_t request_endpoint, int reply_endpoint,
		      handler_t handler, void *source_addr, int nbytes,
		      int arg0, int arg1, int arg2, int arg3)
{
  UDPAM_Buf           *buf;
  UDPAM_Medium4_Pkt   *packet;
  struct sockaddr_in  dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);

  while (request_endpoint->translation_table[reply_endpoint].wsize == 0)
    AM_Poll(request_endpoint->bundle);
  LockEP(request_endpoint);
  while ((buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);    
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  }
  UnlockEP(request_endpoint);    
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = 
    request_endpoint->translation_table[reply_endpoint].name.port;
  dest_sockaddr.sin_addr.s_addr = 
    request_endpoint->translation_table[reply_endpoint].name.ip_addr;
  buf->type = UDPAM_REQUEST_MEDIUM_4;     
  packet = &(buf->packet.udpam_medium4_pkt);
  packet->type = UDPAM_REQUEST_MEDIUM_4;
  packet->tag = request_endpoint->translation_table[reply_endpoint].tag;
  packet->buf_id = buf->buf_id;    
  packet->seq_num = buf->seq_num;    
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);     
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_MEDIUM - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(request_endpoint->bundle);
  }
  request_endpoint->translation_table[reply_endpoint].wsize--;
  LockEP(request_endpoint);
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);    
  return(AM_OK);            
}

int AM_RequestMedium8(ea_t request_endpoint, int reply_endpoint,
		      handler_t handler, void *source_addr, int nbytes,
		      int arg0, int arg1, int arg2, int arg3,
		      int arg4, int arg5, int arg6, int arg7)
{
  UDPAM_Buf           *buf;
  UDPAM_Medium8_Pkt   *packet;
  struct sockaddr_in  dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);

  while (request_endpoint->translation_table[reply_endpoint].wsize == 0)
    AM_Poll(request_endpoint->bundle);
  LockEP(request_endpoint);
  while ((buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  }
  UnlockEP(request_endpoint);
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = 
    request_endpoint->translation_table[reply_endpoint].name.port;
  dest_sockaddr.sin_addr.s_addr = 
    request_endpoint->translation_table[reply_endpoint].name.ip_addr;
  buf->type = UDPAM_REQUEST_MEDIUM_8;     
  packet = &(buf->packet.udpam_medium8_pkt);
  packet->type = UDPAM_REQUEST_MEDIUM_8;
  packet->tag = request_endpoint->translation_table[reply_endpoint].tag;
  packet->buf_id = buf->buf_id;    
  packet->seq_num = buf->seq_num;    
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->arg4 = arg4;
  packet->arg5 = arg5;
  packet->arg6 = arg6;
  packet->arg7 = arg7;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);     
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_MEDIUM - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(request_endpoint->bundle);
  }
  request_endpoint->translation_table[reply_endpoint].wsize--;
  LockEP(request_endpoint);
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num,
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);            
}

int AM_RequestMediumAsync4(ea_t request_endpoint, int reply_endpoint,
			   handler_t handler, void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3)
{
  return(AM_RequestMedium4(request_endpoint, reply_endpoint, handler, 
			   source_addr, nbytes, arg0, arg1, arg2, arg3));
}

int AM_RequestMediumAsync8(ea_t request_endpoint, int reply_endpoint,
			   handler_t handler, void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3,
			   int arg4, int arg5, int arg6, int arg7)
{
  return(AM_RequestMedium8(request_endpoint, reply_endpoint, handler, 
			   source_addr, nbytes, arg0, arg1, arg2, arg3, arg4, 
			   arg5, arg6, arg7));
}

int AM_RequestLong4(ea_t request_endpoint, int reply_endpoint,
		    int dest_offset, handler_t handler, 
		    void *source_addr, int nbytes,
		    int arg0, int arg1, int arg2, int arg3)
{
  int                segsize;
  caddr_t            segstartaddr;
  UDPAM_Buf          *buf;
  UDPAM_Long4_Pkt    *packet;
  struct sockaddr_in dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if (AM_GetSeg(request_endpoint, (void *)&segstartaddr, &segsize) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  while (request_endpoint->translation_table[reply_endpoint].wsize == 0)
    AM_Poll(request_endpoint->bundle);
  LockEP(request_endpoint);
  while ((buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  }
  UnlockEP(request_endpoint);
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = 
    request_endpoint->translation_table[reply_endpoint].name.port;
  dest_sockaddr.sin_addr.s_addr = 
    request_endpoint->translation_table[reply_endpoint].name.ip_addr;
  buf->type = UDPAM_REQUEST_LONG_4;
  packet = &(buf->packet.udpam_long4_pkt);
  packet->type = UDPAM_REQUEST_LONG_4;
  packet->tag = request_endpoint->translation_table[reply_endpoint].tag;
  packet->buf_id = buf->buf_id;   
  packet->seq_num = buf->seq_num;  
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_LONG - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(request_endpoint->bundle);
  }
  request_endpoint->translation_table[reply_endpoint].wsize--;
  LockEP(request_endpoint);
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);
}

int AM_RequestLong8(ea_t request_endpoint, int reply_endpoint,
		    int dest_offset, handler_t handler, 
		    void *source_addr, int nbytes,
		    int arg0, int arg1, int arg2, int arg3,
		    int arg4, int arg5, int arg6, int arg7)
{
  int                 segsize;
  caddr_t             segstartaddr;
  UDPAM_Buf           *buf;
  UDPAM_Long8_Pkt     *packet;
  struct sockaddr_in  dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if (AM_GetSeg(request_endpoint, &segstartaddr, &segsize) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  while (request_endpoint->translation_table[reply_endpoint].wsize == 0)
    AM_Poll(request_endpoint->bundle);  
  LockEP(request_endpoint);
  while ((buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  }
  UnlockEP(request_endpoint);
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = 
    request_endpoint->translation_table[reply_endpoint].name.port;
  dest_sockaddr.sin_addr.s_addr = 
    request_endpoint->translation_table[reply_endpoint].name.ip_addr;
  buf->type = UDPAM_REQUEST_LONG_8;
  packet = &(buf->packet.udpam_long8_pkt);
  packet->type = UDPAM_REQUEST_LONG_8;
  packet->tag = request_endpoint->translation_table[reply_endpoint].tag;
  packet->buf_id = buf->buf_id;    
  packet->seq_num = buf->seq_num;  
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->arg4 = arg4;
  packet->arg5 = arg5;
  packet->arg6 = arg6;
  packet->arg7 = arg7;
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);     
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_LONG - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(request_endpoint->bundle);
  }
  request_endpoint->translation_table[reply_endpoint].wsize--;
  LockEP(request_endpoint);
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);
}

int AM_RequestLongAsync4(ea_t request_endpoint, int reply_endpoint,
			 int dest_offset, handler_t handler, 
			 void *source_addr, int nbytes,
			 int arg0, int arg1, int arg2, int arg3)
{
  return(AM_RequestLong4(request_endpoint, reply_endpoint, dest_offset, 
			 handler, source_addr, nbytes, arg0, arg1, arg2, 
			 arg3));
}

int AM_RequestLongAsync8(ea_t request_endpoint, int reply_endpoint,
			 int dest_offset, handler_t handler, 
			 void *source_addr, int nbytes,
			 int arg0, int arg1, int arg2, int arg3,
			 int arg4, int arg5, int arg6, int arg7)
{
  return(AM_RequestLong8(request_endpoint, reply_endpoint, dest_offset, 
			 handler, source_addr, nbytes, arg0, arg1, arg2, 
			 arg3, arg4, arg5, arg6, arg7));
}

int AM_Reply4(void *token, handler_t handler, int arg0, 
	      int arg1, int arg2, int arg3)
{
  UDPAM_Buf          *buf;
  UDPAM_Short4_Pkt   *packet; 
  ea_t               source_ep;
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 

  source_ep = ((Token *)(token))->dest_ep;
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;
  LockEP(source_ep);
  while ((buf = (UDPAM_Buf *)Dequeue(source_ep))== (UDPAM_Buf *)NULL) { 
    UnlockEP(source_ep);
    AM_Poll(source_ep->bundle);  
    LockEP(source_ep);
  }
  UnlockEP(source_ep);
  buf->type = UDPAM_REPLY_4;  
  packet = &(buf->packet.udpam_short4_pkt);
  packet->type = UDPAM_REPLY_4;
  packet->tag = ((Token *)(token))->tag;
  packet->buf_id = ((Token *)(token))->buf_id;   /* Buffer ID of Request */
  packet->seq_num = ((Token *)(token))->seq_num; /* Seq Num of Request Buf */
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(source_ep->bundle);
  }
  LockEP(source_ep);
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_Reply8(void *token, handler_t handler, int arg0, int arg1, int arg2,
	      int arg3, int arg4, int arg5, int arg6, int arg7)
{
  UDPAM_Buf          *buf;
  UDPAM_Short8_Pkt   *packet; 
  ea_t               source_ep;
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   

  source_ep = ((Token *)(token))->dest_ep;
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;
  LockEP(source_ep);
  while ((buf = (UDPAM_Buf *)Dequeue(source_ep))== (UDPAM_Buf *)NULL) {
    UnlockEP(source_ep);
    AM_Poll(source_ep->bundle);  
    LockEP(source_ep);
  }
  UnlockEP(source_ep);
  buf->type = UDPAM_REPLY_8;  
  packet = &(buf->packet.udpam_short8_pkt);
  packet->type = UDPAM_REPLY_8;
  packet->tag = ((Token *)(token))->tag;
  packet->buf_id = ((Token *)(token))->buf_id;   /* Buffer ID of Request */
  packet->seq_num = ((Token *)(token))->seq_num; /* Seq Num of Req. Buf */
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->arg4 = arg4;
  packet->arg5 = arg5;
  packet->arg6 = arg6;
  packet->arg7 = arg7;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(source_ep->bundle);
  }
  LockEP(source_ep);
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_ReplyMedium4(void *token, handler_t handler,
		    int nbytes, void *source_addr,
		    int arg0, int arg1, int arg2, int arg3)
{
  UDPAM_Buf          *buf;
  UDPAM_Medium4_Pkt  *packet;
  ea_t               source_ep;
  struct sockaddr_in dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   

  source_ep = ((Token *)(token))->dest_ep;
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;
  LockEP(source_ep);
  while ((buf = (UDPAM_Buf *)Dequeue(source_ep))== (UDPAM_Buf *)NULL) { 
    UnlockEP(source_ep);
    AM_Poll(source_ep->bundle);  
    LockEP(source_ep);
  }
  UnlockEP(source_ep);
  buf->type = UDPAM_REPLY_MEDIUM_4;  
  packet = &(buf->packet.udpam_medium4_pkt);
  packet->type = UDPAM_REPLY_MEDIUM_4;
  packet->tag = ((Token *)(token))->tag;
  packet->buf_id = ((Token *)(token))->buf_id;   /* Buffer ID of Request */
  packet->seq_num = ((Token *)(token))->seq_num; /* Seq Num of Req. Buf */
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  memcpy(packet->data, source_addr, nbytes);     /* EEEK! */ 
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(source_ep->bundle);
  }
  LockEP(source_ep);
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_ReplyMedium8(void *token, handler_t handler,
		    int nbytes, void *source_addr,
		    int arg0, int arg1, int arg2, int arg3,
		    int arg4, int arg5, int arg6, int arg7)
{
  UDPAM_Buf          *buf;
  UDPAM_Medium8_Pkt  *packet;
  ea_t               source_ep;
  struct sockaddr_in dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   

  source_ep = ((Token *)(token))->dest_ep;
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;
  LockEP(source_ep);
  while ((buf = (UDPAM_Buf *)Dequeue(source_ep))== (UDPAM_Buf *)NULL) {
    UnlockEP(source_ep);
    AM_Poll(source_ep->bundle);  
    LockEP(source_ep);
  }
  UnlockEP(source_ep);
  buf->type = UDPAM_REPLY_MEDIUM_8;  
  packet = &(buf->packet.udpam_medium8_pkt);
  packet->type = UDPAM_REPLY_MEDIUM_8;
  packet->tag = ((Token *)(token))->tag;
  packet->buf_id = ((Token *)(token))->buf_id;   /* Buffer ID of Request */
  packet->seq_num = ((Token *)(token))->seq_num; /* Seq Num of Req. Buf */
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->arg4 = arg4;
  packet->arg5 = arg5;
  packet->arg6 = arg6;
  packet->arg7 = arg7;
  memcpy(packet->data, source_addr, nbytes);     /* EEEK! */ 
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(source_ep->bundle);
  }
  LockEP(source_ep);
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_ReplyLong4(void *token, int dest_offset,
		  handler_t handler, void *source_addr, int nbytes, 
		  int arg0, int arg1, int arg2, int arg3)
{
  int                 segsize;
  ea_t                source_ep;
  caddr_t             segstartaddr;
  UDPAM_Buf           *buf;
  UDPAM_Long4_Pkt     *packet;
  struct sockaddr_in  dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   
  source_ep = ((Token *)(token))->dest_ep;
  if (AM_GetSeg(source_ep, &segstartaddr, &segsize) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;
  LockEP(source_ep);
  while ((buf = (UDPAM_Buf *)Dequeue(source_ep)) == (UDPAM_Buf *)NULL) {
    UnlockEP(source_ep);
    AM_Poll(source_ep->bundle);    
    LockEP(source_ep);
  }
  UnlockEP(source_ep);
  buf->type = UDPAM_REPLY_LONG_4;
  packet = &(buf->packet.udpam_long4_pkt);
  packet->type = UDPAM_REPLY_LONG_4;
  packet->tag = ((Token *)(token))->tag;
  packet->buf_id = ((Token *)(token))->buf_id;   /* Buffer ID of Request */
  packet->seq_num = ((Token *)(token))->seq_num; /* Seq Num of Req. Buf */ 
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, UDPAM_MAX_LONG);
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(source_ep->bundle);
  }
  LockEP(source_ep);
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep); 
  return(AM_OK);
}

int AM_ReplyLong8(void *token, int dest_offset,
		  handler_t handler, void *source_addr, int nbytes, 
		  int arg0, int arg1, int arg2, int arg3,
		  int arg4, int arg5, int arg6, int arg7)
{
  int                   segsize;
  ea_t                  source_ep;
  caddr_t               segstartaddr;
  UDPAM_Buf             *buf;
  UDPAM_Long8_Pkt       *packet;
  struct sockaddr_in    dest_sockaddr;
  
  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   
  source_ep = ((Token *)(token))->dest_ep;
  if (AM_GetSeg(source_ep, &segstartaddr, &segsize) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;
  LockEP(source_ep);
  while ((buf = (UDPAM_Buf *)Dequeue(source_ep)) == (UDPAM_Buf *)NULL) {
    UnlockEP(source_ep);
    AM_Poll(source_ep->bundle);    
    LockEP(source_ep);
  }
  UnlockEP(source_ep);
  buf->type = UDPAM_REPLY_LONG_8;
  packet = &(buf->packet.udpam_long8_pkt);
  packet->type = UDPAM_REPLY_LONG_8;
  packet->tag = ((Token *)(token))->tag;
  packet->buf_id = ((Token *)(token))->buf_id;   /* Buffer ID of Request */
  packet->seq_num = ((Token *)(token))->seq_num; /* Seq Num of Req. Buf */ 
  packet->handler = handler;
  packet->arg0 = arg0;
  packet->arg1 = arg1;
  packet->arg2 = arg2;
  packet->arg3 = arg3;
  packet->arg4 = arg4;
  packet->arg5 = arg5;
  packet->arg6 = arg6;
  packet->arg7 = arg7;
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, (void *)source_addr, UDPAM_MAX_LONG);
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
    AM_Poll(source_ep->bundle);
  }
  LockEP(source_ep);
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);
}

int AM_MaxShort()
{
  return(UDPAM_MAX_SHORT);
}

int AM_MaxMedium()
{
  return(UDPAM_MAX_MEDIUM);
}

int AM_MaxLong()
{
  return(UDPAM_MAX_LONG);
}

int AM_Poll(eb_t bundle)
{
  int                 i, n, bytesRecv, type, sourceEndpoint, count;
  int                 size = sizeof(struct sockaddr_in);
  op_t                errorCode;
  struct timeval      timeout;
  struct timeout_elem *dupElem, *replyElem;
  struct ep_elem      *listElem;
  struct sockaddr_in  sender;
  ea_t                endpoint;
  fd_set              tempFdset; 
  Token               token; 
  ArgBlock            argblock;
  UDPAM_Buf           *buf;
  char                packet[MAX_PACKET_SIZE];
/*  static ep_elem      *lastElem;   Could do this since we are locked */ 

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   
  timeout.tv_sec = timeout.tv_usec = 0;
  LockBundle(bundle);
  memcpy(&tempFdset, &(bundle->fdset), sizeof(tempFdset));
  if ((n = select(bundle->maxfd + 1, &tempFdset, (fd_set *)NULL, 
		  (fd_set *)NULL, &timeout)) < 0) {
    perror("select error\n");
    exit(-1);
  }
  else if (n == 0) {
    UnlockBundle(bundle);
    return(AM_OK);
  }
  /*
   * The following code is unfair since it favors endpoints at the head of
   * the list.  This could be fixed by maintaining a static variable that
   * tracks the last endpoint serviced and using this variable as a starting
   * point on the next poll.  What if the bundle changes though?  The code
   * below pulls in a message into a generic buffer which is as big as
   * the largest message we might see.  For the short and medium cases, this
   * shouldn't hurt us unnecessarily since recvfrom() will return after
   * pulling in a UDP datagram, *not* the amount of bytes we specify.  After
   * grabbing the data, cast the packet to short and switch(ACK!).  Stay
   * here while count < maximum number of messages and the number of sockets
   * ready for reading (when the poll occurred) is greater than zero.
   */
  bundle->mask = AM_NOEVENTS;  /* ATOMICALLY CLEAR EVENT MASK */
  count = 0;
  while ((count < MAX_MESSAGES_PER_POLL) && (n > 0)) {
    listElem = bundle->head;
    for (i = 0; i < bundle->num_eps; i++) {
      endpoint = listElem->endpoint;
      LockEP(endpoint);    /* Perhaps this go after FD_ISSET, race perhaps? */
      if (FD_ISSET(endpoint->socket, &tempFdset)) {
	if ((bytesRecv = recvfrom(endpoint->socket, (char *)&packet, 
				  sizeof(packet), 0,
				  (struct sockaddr *)&sender, &size)) < 0) {
	  if (errno == EWOULDBLOCK) {     /* Nothing left in sockbuf */
	    FD_CLR(endpoint->socket, &tempFdset); 
	    n--;
	    UnlockEP(endpoint);
	    goto end;
	  }
	  else {
	    perror("recvfrom error\n");
	    exit(-1);
	  }
	} 
	else {
	  count++;  /* Pulled in a message from endpoint's recv buf */
	}

	/*
         * Cast packet to short4 and switch on type.  This is disgusting, but
	 * it saves me a system call to peek at the message.
	 */
	switch((type = ((UDPAM_Short4_Pkt *)&packet)->type)) {
	case UDPAM_REQUEST_4: {
	  UDPAM_Short4_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Short4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_4, 
					     packetPtr->tag, 
					     packetPtr->handler, 0, 
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, NULL);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    UnlockEP(endpoint);
	  }
	  else {
	    UnlockEP(endpoint);
	    ((Handler4)(endpoint->handler_table[packetPtr->handler]))(
 		       (void *)&token, packetPtr->arg0, packetPtr->arg1, 
		       packetPtr->arg2, packetPtr->arg3);
	    /* No Reply Generated.... Need to Lock...ack... Gen Empty Reply()..
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
		 if (AM_Reply4(&token, ) != AM_OK) {
		 }
   	    }
	    */
	  }
	  break;
	}
        case UDPAM_REQUEST_8: {
	  UDPAM_Short8_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Short8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_8, 
					     packetPtr->tag, 
					     packetPtr->handler, 0,
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6, 
			  packetPtr->arg7, NULL);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    UnlockEP(endpoint);
	  }
	  else {
	    UnlockEP(endpoint);
	    ((Handler8)(endpoint->handler_table[packetPtr->handler]))(
 		       (void *)&token, packetPtr->arg0, packetPtr->arg1, 
		       packetPtr->arg2, packetPtr->arg3, packetPtr->arg4,
		       packetPtr->arg5, packetPtr->arg6, packetPtr->arg7);
	  }
	  break;
	}
	case UDPAM_REQUEST_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Medium4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_MEDIUM_4, 
					     packetPtr->tag, 
					     packetPtr->handler, 0, 
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_MEDIUM, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 0, 
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, 0, 0, 0, 0, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    UnlockEP(endpoint);
	  }
	  else {
	    UnlockEP(endpoint);
	    ((HandlerMedium4)(endpoint->handler_table[packetPtr->handler]))(
 		             (void *)&token, &(packetPtr->data), packetPtr->nbytes, 
			     packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			     packetPtr->arg3);
	  }
	  break;
	}
	case UDPAM_REQUEST_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Medium8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_MEDIUM_8,
					     packetPtr->tag, 
					     packetPtr->handler, 0, 
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_MEDIUM, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 0, 
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			  packetPtr->arg6, packetPtr->arg7, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    UnlockEP(endpoint);
	  }
	  else {
	    UnlockEP(endpoint);
	    ((HandlerMedium8)(endpoint->handler_table[packetPtr->handler]))(
 		             (void *)&token, &(packetPtr->data), packetPtr->nbytes, 
			     packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			     packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			     packetPtr->arg6, packetPtr->arg7);
	  }
	  break;
	}
	case UDPAM_REQUEST_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);

	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_LONG_4,
					     packetPtr->tag, 
					     packetPtr->handler, 
					     packetPtr->dest_offset,
					     packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_LONG, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 
			  packetPtr->dest_offset, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    UnlockEP(endpoint);
	  }
	  else {
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes);
	    UnlockEP(endpoint);
	    ((HandlerLong4)(endpoint->handler_table[packetPtr->handler]))(
			   (void *)&token, packetPtr->dest_offset, packetPtr->nbytes,
			   packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			   packetPtr->arg3);
	  }
	  break;
	}
	case UDPAM_REQUEST_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->dest_offset, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_LONG_8,
					     packetPtr->tag, 
					     packetPtr->handler,
					     packetPtr->dest_offset,
					     packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_LONG, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 
			  packetPtr->dest_offset, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6, 
			  packetPtr->arg7, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    UnlockEP(endpoint);
	  }
	  else {
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes);
	    UnlockEP(endpoint);
	    ((HandlerLong8)(endpoint->handler_table[packetPtr->handler]))(
			   (void *)&token, packetPtr->dest_offset, packetPtr->nbytes,
			   packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			   packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			   packetPtr->arg6, packetPtr->arg7);
	  }
	  break;
	}
	case UDPAM_REPLY_4: {
	  UDPAM_Short4_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Short4_Pkt *)&packet;  
	  /* Need this so we can modify the window size */
	  sourceEndpoint = GlobalToIndex(endpoint, &sender);  
	  BuildToken(&token, &sender, packetPtr->buf_id, 
		     packetPtr->seq_num, packetPtr->tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0))
	                   != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, NULL);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    UnlockEP(endpoint);
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    UnlockEP(endpoint);
	    ((Handler4)(endpoint->handler_table[packetPtr->handler]))(
                       (void *)&token, packetPtr->arg0, packetPtr->arg1, 
		       packetPtr->arg2, packetPtr->arg3);
	    /* Hmm... can I get interrupted in a sys call?..Noo...GLUnix */
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  }
	  break;
	}
	case UDPAM_REPLY_8: {
	  UDPAM_Short8_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Short8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6, 
			  packetPtr->arg7, NULL);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    UnlockEP(endpoint);
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    UnlockEP(endpoint);
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    ((Handler8)(endpoint->handler_table[packetPtr->handler]))(
                       (void *)&token, packetPtr->arg0, packetPtr->arg1, 
		       packetPtr->arg2, packetPtr->arg3, packetPtr->arg4, 
		       packetPtr->arg5, packetPtr->arg6, packetPtr->arg7);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  }
	  break;
	}
	case UDPAM_REPLY_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Medium4_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_MEDIUM, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 0, 
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, 0, 0, 0, 0, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    UnlockEP(endpoint);
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    UnlockEP(endpoint);
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    ((HandlerMedium4)(endpoint->handler_table[packetPtr->handler]))(
                             (void *)&token, &(packetPtr->data), packetPtr->nbytes,
			     packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			     packetPtr->arg3);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  }
	  break;
	}
	case UDPAM_REPLY_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Medium8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_MEDIUM, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 0, 
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			  packetPtr->arg6, packetPtr->arg7, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    UnlockEP(endpoint);
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    UnlockEP(endpoint);
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    ((HandlerMedium8)(endpoint->handler_table[packetPtr->handler]))(
                             (void *)&token, &(packetPtr->data), packetPtr->nbytes,
			     packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			     packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			     packetPtr->arg6, packetPtr->arg7);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  }
	  break;
	}
	case UDPAM_REPLY_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long4_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, packetPtr->dest_offset, 
			   packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_LONG, 0, 0, 
			  &token, packetPtr->handler, 0, packetPtr->nbytes, 
			  packetPtr->dest_offset, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    UnlockEP(endpoint);
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    UnlockEP(endpoint);
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes); 
	    ((HandlerLong4)(endpoint->handler_table[packetPtr->handler]))(
			   (void *)&token, packetPtr->dest_offset, packetPtr->nbytes,
			    packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			    packetPtr->arg3);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  }
	  break;
	}
	case UDPAM_REPLY_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->tag,endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, packetPtr->dest_offset,
			   packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_LONG, 0, 0, &token, 
			  packetPtr->handler, 0, packetPtr->nbytes, 
			  packetPtr->dest_offset, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg4, 
			  packetPtr->arg7, packetPtr->data);
	    ((ErrorHandler)(endpoint->handler_table[0]))(AM_REQUEST, errorCode,
							 (void *)&argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    UnlockEP(endpoint);
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    UnlockEP(endpoint);
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes); 
	    ((HandlerLong4)(endpoint->handler_table[packetPtr->handler]))(
			   (void *)&token, packetPtr->dest_offset, packetPtr->nbytes,
			    packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			    packetPtr->arg3);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  }
	  break;
	}
        case UDPAM_REPLYACK: {
	  UDPAM_ReplyACK_Pkt *packetPtr;

	  packetPtr = (UDPAM_ReplyACK_Pkt *)&packet;
	  if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			sender, replyElem) == 1) {
	    /* 
	     * Take Reply off TxTimeout & Free Reply Storage
	     */
	    buf = replyElem->message;  
	    RemoveTimeout(endpoint, replyElem->message); 
	    Enqueue(endpoint, buf);
	    UnlockEP(endpoint);  
	  }
	  else {
	    UnlockEP(endpoint);  /* Same thing */
	  }
	  break;
	}
        default:
	    fprintf(stderr, "Bad Packet Received\n");  /* Drop it */
	    break;
	  } /* switch(... */
      }
end:
      listElem = listElem->next;  /* Next Endpoint Elem */
    }
  }
  UnlockBundle(bundle);
  return(AM_OK);
}
