/******************************************************************
Version identification:
@(#)transport.c	1.15 7/30/96

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h> 	/* define sigemptyset and sigaction */
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <udpam.h>
#include <am.h>
#include <transport.h>
#include <misc.h>

static Bool initialized = FALSE;  /* AM layer initialized? */

/*
 * static void ReplyACK(int sender, struct sockaddr_in receiver, 
 *                      int buf_id, int seq_num)
 *
 * DESCRIPTION:  Send replying endpoint (receiver) an unreliable ACK (hint) to 
 * free up reply storage.
 */
/*inline*/ static 
void ReplyACK(int sender, struct sockaddr_in receiver, int buf_id, int seq_num)
{
  UDPAM_ReplyACK_Pkt packet;
  
  packet.type    = UDPAM_REPLYACK;
  packet.buf_id  = buf_id;         /* Buf ID of Request Buf */
  packet.seq_num = seq_num;        /* Seq Num of Request Buf */

  while (sendto(sender, (const char *)&packet, sizeof(packet), 0,  
		(struct sockaddr *)&receiver, sizeof(receiver)) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
}

/*
 * Try and bounce message back to sender from recv_endpoint.  If no buffer 
 * space is available, drop the message and let the sender time out.  The 
 * seq_num/buf_id fields represent the sequence number and buffer id of the 
 * message that we're sending back.  mesg_type, error_type, and argblock are 
 * parameters that are later passed to the error handler.  Note: bounced 
 * messages are treated essentially like requests.
 */
static
int BounceMessage(ea_t recv_endpoint, struct sockaddr_in sender, 
		  tag_t tag, int mesg_type, op_t error_type, 
		  ArgBlock *argblock)
{
  UDPAM_Buf         *buf;
  UDPAM_Error_Pkt   *packet; 

  buf = (UDPAM_Buf *)Dequeue(recv_endpoint);
  if (buf == NULL) {
    DPRINTF(("BounceMessage: No Buffer Space Left...Dropping\n"));
    return(-1);    /* return -1, Drop message */
  }
  buf->type = UDPAM_ERROR;
  packet = &(buf->packet.udpam_error_pkt);
  packet->type = UDPAM_ERROR;

  /*
   * Set error message's buf_id/seq_num.  These are *not* the same as the
   * the buf_id/seq_num of the message that caused the error to occur.  These
   * live in the argblock.
   */
  packet->buf_id = buf->buf_id;    
  packet->seq_num = buf->seq_num;

  packet->tag = tag;                     /* Bad sender's tag */
  packet->src_tag = recv_endpoint->tag;  /* EP responding to error tag */
  packet->mesg_type = mesg_type;
  packet->error_type = error_type;
  memcpy(&(packet->argblock), argblock, sizeof(packet->argblock));
  while (sendto(recv_endpoint->socket, (const char *)packet, 
		sizeof(UDPAM_Error_Pkt), 0, (struct sockaddr *)&sender, 
		sizeof(sender)) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }  
  }
  AddTimeout(recv_endpoint, buf, packet->buf_id, packet->seq_num, &sender);
  DPRINTF(("BounceMessage: Bounced Message of Type 0x%x w/ ErrorCode 0x%x\n", 
	   mesg_type, error_type));
  return(0);
}

/*
 * Send an empty reply from endpoint to err_requester with request buffer 
 * identifier buf_id and request sequence number seq_num.
 * 
 * Used in two cases:
 * 1) Request handler does not generate a reply.
 * 2) Error message received.  Emply reply needed.
 */
static
int EmptyReply(ea_t endpoint, struct sockaddr_in *err_requester, tag_t tag, 
	       int buf_id, int seq_num)
{
  UDPAM_Short4_Pkt  *packet;  /* Send a Short4 Reply to "ack" error "Req" */
  UDPAM_Buf         *buf;

  buf = (UDPAM_Buf *)Dequeue(endpoint);
  if (buf == NULL) {
    DPRINTF(("EmptyReply: No Buffers Left.\n"));
    return(-1);               /* return -1, drop message */ 
  }
  buf->type = UDPAM_REPLY_4;  
  packet = &(buf->packet.udpam_short4_pkt);
  packet->type = UDPAM_REPLY_4;  
  packet->buf_id = buf_id;         /* Buffer id of Error Request */
  packet->seq_num = seq_num;       /* Seq Number of Error Request */
  packet->tag = tag;
  packet->handler = UDPAM_EMPTY_HANDLER;  /* Specify the empty handler */
  while (sendto(endpoint->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)err_requester,
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(endpoint, buf, packet->buf_id, packet->seq_num, err_requester);
  return(AM_OK);          
}

/*
 * inline static void AM_GetRequestPacket(ea_t request_endpoint, int reply_endpoint,
 *                                        UDPAM_Buf **buf)
 * 
 * DESCRIPTION:  Dequeues a UDPAM buffer from request_endpoint's send queue and 
 * adjusts the number of credits for destination endpoint reply_endoint.
 */
/*inline*/ static 
void AM_GetRequestPacket(ea_t request_endpoint, int reply_endpoint, UDPAM_Buf **buf)
{
  while (request_endpoint->translation_table[reply_endpoint].wsize == 0) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);
    LockEP(request_endpoint);
  }
  request_endpoint->translation_table[reply_endpoint].wsize--;  
  while ((*buf = (UDPAM_Buf *)Dequeue(request_endpoint)) == (UDPAM_Buf *)NULL) {
    UnlockEP(request_endpoint);
    AM_Poll(request_endpoint->bundle);    
    LockEP(request_endpoint);
  } 
}

/*
 * If a reply on TxTimeout is found that has a matching 
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
 * If a duplicate request was seen, resend the appropriate reply.
 */
static
void ResendReply(ea_t reply_endpoint, struct timeout_elem *dup_elem)
{
  DPRINTF(("ResendReply: Message = {Type=0x%x, ReqBufID=%d ReqSeqNum=%d}\n",
	   dup_elem->message->type, dup_elem->message->buf_id, 
	   dup_elem->message->seq_num));
  /*
   * Note: EAGAIN errors simply mean that the kernel is still busy pushing
   * things out at a lower level.  This should eventually succeed.  
   */
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
    }
    break;
  case UDPAM_REPLY_MEDIUM_4:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_medium4_pkt),
		  sizeof(dup_elem->message->packet.udpam_medium4_pkt) -
		  (UDPAM_MAX_MEDIUM - 
		   dup_elem->message->packet.udpam_medium4_pkt.nbytes), 0,
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
    }
    break;
  case UDPAM_REPLY_MEDIUM_8:
    while (sendto(reply_endpoint->socket, 
	       (const char *)&(dup_elem->message->packet.udpam_medium8_pkt),
	       sizeof(dup_elem->message->packet.udpam_medium8_pkt) -		
	       (UDPAM_MAX_MEDIUM - 
	       dup_elem->message->packet.udpam_medium8_pkt.nbytes), 0,
	       (struct sockaddr *)&(dup_elem->destination),
	       sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
    }
    break;
  case UDPAM_REPLY_LONG_4:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_long4_pkt),
		  sizeof(dup_elem->message->packet.udpam_long4_pkt) - 
		  (UDPAM_MAX_LONG - 
		  dup_elem->message->packet.udpam_long4_pkt.nbytes), 0,
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0) {
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
    }
    break;
  case UDPAM_REPLY_LONG_8:
    while (sendto(reply_endpoint->socket, 
		  (const char *)&(dup_elem->message->packet.udpam_long8_pkt),
		  sizeof(dup_elem->message->packet.udpam_long8_pkt) -
		  (UDPAM_MAX_LONG - 
		  dup_elem->message->packet.udpam_long8_pkt.nbytes), 0,
		  (struct sockaddr *)&(dup_elem->destination),
		  sizeof(dup_elem->destination)) < 0){
      if (errno != EAGAIN) {
	perror("sendto error\n");
	exit(-1);
      }
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
  if (((pktTag != replyEndpoint->tag) && (pktTag != AM_ALL)) ||
      (replyEndpoint->tag == AM_NONE))
    return(EBADTAG);
  else if (((pktHandler < 0) || (pktHandler >= replyEndpoint->num_handlers) ||
	   (replyEndpoint->handler_table[pktHandler] == abort)) &&
	   ((type != UDPAM_GET_XFER_4) && (type != UDPAM_GET_XFER_8)))
    return(EBADHANDLER);
  else if (replyEndpoint->txfree.num_elements == 0) 
    return(ECONGESTION);
  else if (((type == UDPAM_REQUEST_LONG_4) || (type == UDPAM_REQUEST_LONG_8) ||
	   (type == UDPAM_GET_XFER_4) || (type == UDPAM_GET_XFER_8)) && 
	   ((destOffset < 0) || (destOffset > replyEndpoint->length)))
    return(EBADSEGOFF);
  else if (((type == UDPAM_REQUEST_LONG_4) || (type == UDPAM_REQUEST_LONG_8) ||
	   (type == UDPAM_GET_XFER_4) || (type == UDPAM_GET_XFER_8)) && 
	   (nbytes > (replyEndpoint->length - destOffset)))
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
  if (((pktTag != requestEndpoint->tag) && (pktTag != AM_ALL)) ||
      (requestEndpoint->tag == AM_NONE))
    return(EBADTAG);
  else 
  if (((pktHandler < 0) && (pktHandler != UDPAM_EMPTY_HANDLER)) || 
	   (pktHandler >= requestEndpoint->num_handlers) ||
	   ((requestEndpoint->handler_table[pktHandler] == abort) &&
	   (pktHandler != UDPAM_EMPTY_HANDLER))) 
    return(EBADHANDLER);
  else if (((type == UDPAM_REPLY_LONG_4) || (type == UDPAM_REPLY_LONG_8)) && 
	   ((destOffset < 0) || (destOffset > requestEndpoint->length)))
    return(EBADSEGOFF);
  else if (((type == UDPAM_REPLY_LONG_4) || (type == UDPAM_REPLY_LONG_8)) && 
	   (nbytes > (requestEndpoint->length - destOffset)))
    return(EBADLENGTH);
  else 
    return(NO_ERRORS);
}

/*
 * Cheap hack to avoid race condition.  Scenario -- Req arrives, space is
 * available for a reply, slow handler, other requests from other threads
 * on the same endpoint, no buffers, reply, Ack!
 */
static 
void EnterRaceHack(ea_t ea)
{
  ea->txfree.num_elements--;  /* Temporarily adjust this.  Reserve buf */
  ASSERT(ea->txfree.num_elements >= 0);  /* Still should be >= 0 */
}

static 
void ExitRaceHack(ea_t ea)
{
  ea->txfree.num_elements++;  /* Fix it up before replying */
}

/*
 * *Internal* functions that reply to gets.  
 */
static 
int AM_GetXferReply4(void *token, int source_offset, handler_t handler, 
		     int dest_offset, int nbytes, int arg0, int arg1,
		     int arg2, int arg3)
{
  UDPAM_Buf               *buf;
  UDPAM_GetXferReply4_Pkt *packet; 
  ea_t                    source_ep;
  struct sockaddr_in      dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   

  source_ep = ((Token *)(token))->dest_ep;
/*
  if ((dest_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);
*/
  dest_sockaddr.sin_family      = AF_INET;
  dest_sockaddr.sin_port        = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  buf = (UDPAM_Buf *)Dequeue(source_ep);  
  ASSERT(buf != NULL);

  buf->type = UDPAM_GET_XFER_REPLY_4;
  packet = &(buf->packet.udpam_getxferreply4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_GET_XFER_REPLY_4, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3);
  packet->source_offset = source_offset;
  packet->nbytes = nbytes;
  /* Copy data from replying endpoints VM segment at offset source_offset */
  memcpy(packet->data, (caddr_t)(source_ep->start_addr) + source_offset, 
	 nbytes);
  packet->dest_offset = dest_offset;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  return(AM_OK);
}

static 
int AM_GetXferReply8(void *token, int source_offset, handler_t handler, 
		     int dest_offset, int nbytes, int arg0, int arg1, 
		     int arg2, int arg3, int arg4, int arg5, int arg6, 
		     int arg7)
{
  UDPAM_Buf               *buf;
  UDPAM_GetXferReply8_Pkt *packet; 
  ea_t                    source_ep;
  struct sockaddr_in      dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   

  source_ep = ((Token *)(token))->dest_ep;
/*
  if ((dest_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);
*/
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  buf = (UDPAM_Buf *)Dequeue(source_ep);  
  ASSERT(buf != NULL);

  buf->type = UDPAM_GET_XFER_REPLY_8;
  packet = &(buf->packet.udpam_getxferreply8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_GET_XFER_REPLY_8, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3, 
		    arg4, arg5, arg6, arg7);
  packet->source_offset = source_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, (caddr_t)(source_ep->start_addr) + source_offset, nbytes);
  packet->dest_offset = dest_offset;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  return(AM_OK);  
}

/*
 * void ThreadExitHandler(int sig)
 *
 * DESCRIPTION:  SIGTERM signal handler routine to make threads go 
 * away correctly.
 */
void ThreadExitHandler(int sig)
{
  thr_exit(0);
}

/*
 * Active Message API 
 */
int AM_Init()
{ 
  struct sigaction action;

  initialized = TRUE;
  /* Make threads go away correctly */
  action.sa_handler = ThreadExitHandler;
  (void)sigemptyset(&action.sa_mask);
  (void)sigaction(SIGTERM, &action, NULL); 
  return(AM_OK);
}
 
int AM_Terminate()
{
  initialized = FALSE;
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
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);
  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_REQUEST_4; 
  packet = &(buf->packet.udpam_short4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_REQUEST_4, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3);
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet), 0, (struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
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

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);
  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_REQUEST_8;     
  packet = &(buf->packet.udpam_short8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_REQUEST_8, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
  while (sendto(request_endpoint->socket, (const char *)packet, 
		sizeof(*packet), 0, (struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);          
}

int AM_RequestI4(ea_t request_endpoint, int reply_endpoint,
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

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);
  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_REQUEST_MEDIUM_4;     
  packet = &(buf->packet.udpam_medium4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_REQUEST_MEDIUM_4, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3);
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);
  packet->source_addr = source_addr;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_MEDIUM - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);    
  return(AM_OK);            
}

int AM_RequestI8(ea_t request_endpoint, int reply_endpoint,
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

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);
  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);  
  buf->type = UDPAM_REQUEST_MEDIUM_8;     
  packet = &(buf->packet.udpam_medium8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_REQUEST_MEDIUM_8, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);     
  packet->source_addr = source_addr;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_MEDIUM - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num,
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);            
}

int AM_RequestXfer4(ea_t request_endpoint, int reply_endpoint,
		    int dest_offset, handler_t handler, 
		    void *source_addr, int nbytes,
		    int arg0, int arg1, int arg2, int arg3)
{
  UDPAM_Buf          *buf;
  UDPAM_Long4_Pkt    *packet;
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);

  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_REQUEST_LONG_4;
  packet = &(buf->packet.udpam_long4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_REQUEST_LONG_4, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3);
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);
  packet->source_addr = source_addr;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_LONG - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);
}

int AM_RequestXfer8(ea_t request_endpoint, int reply_endpoint,
		    int dest_offset, handler_t handler, 
		    void *source_addr, int nbytes,
		    int arg0, int arg1, int arg2, int arg3,
		    int arg4, int arg5, int arg6, int arg7)
{
  UDPAM_Buf           *buf;
  UDPAM_Long8_Pkt     *packet;
  struct sockaddr_in  dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);

  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_REQUEST_LONG_8;
  packet = &(buf->packet.udpam_long8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_REQUEST_LONG_8, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, nbytes);     
  packet->source_addr = source_addr;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet) - (UDPAM_MAX_LONG - nbytes), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);
}

int AM_RequestXferAsync4(ea_t request_endpoint, int reply_endpoint,
			 int dest_offset, handler_t handler, 
			 void *source_addr, int nbytes,
			 int arg0, int arg1, int arg2, int arg3)
{
  return(AM_RequestXfer4(request_endpoint, reply_endpoint, dest_offset, 
			 handler, source_addr, nbytes, arg0, arg1, arg2, 
			 arg3));
}

int AM_RequestXferAsync8(ea_t request_endpoint, int reply_endpoint,
			 int dest_offset, handler_t handler, 
			 void *source_addr, int nbytes,
			 int arg0, int arg1, int arg2, int arg3,
			 int arg4, int arg5, int arg6, int arg7)
{
  return(AM_RequestXfer8(request_endpoint, reply_endpoint, dest_offset, 
			 handler, source_addr, nbytes, arg0, arg1, arg2, 
			 arg3, arg4, arg5, arg6, arg7));
}

int AM_GetXfer4(ea_t request_endpoint, int reply_endpoint, int source_offset, 
		handler_t handler, int dest_offset, int nbytes, int arg0, 
		int arg1, int arg2, int arg3)
{
  UDPAM_Buf          *buf;
  UDPAM_GetXfer4_Pkt *packet; 
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);
  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_GET_XFER_4;
  packet = &(buf->packet.udpam_getxfer4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_GET_XFER_4, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3);
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  packet->source_offset = source_offset;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet), 0, (struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);            
}

int AM_GetXfer8(ea_t request_endpoint, int reply_endpoint, int source_offset, 
		handler_t handler, int dest_offset, int nbytes, int arg0, 
		int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, 
		int arg7)
{
  UDPAM_Buf          *buf;
  UDPAM_GetXfer8_Pkt *packet; 
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT); 
  if (AM_GetTranslationInuse(request_endpoint, reply_endpoint) != AM_OK) 
    return(AM_ERR_BAD_ARG);

  ASSERT(request_endpoint->translation_table[reply_endpoint].wsize >= 0);
  LockEP(request_endpoint);
  AM_GetRequestPacket(request_endpoint, reply_endpoint, &buf);
  DEST_SOCKADDR_SET(dest_sockaddr, request_endpoint, reply_endpoint);
  buf->type = UDPAM_GET_XFER_8;
  packet = &(buf->packet.udpam_getxfer8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_GET_XFER_8, buf->seq_num, buf->buf_id, 
		    request_endpoint->tag, 
		    request_endpoint->translation_table[reply_endpoint].tag,
		    handler, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  packet->source_offset = source_offset;
  while (sendto(request_endpoint->socket, (const char *)packet,
		sizeof(*packet), 0, (struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(request_endpoint, buf, packet->buf_id, packet->seq_num, 
	     &dest_sockaddr);   
  UnlockEP(request_endpoint);
  return(AM_OK);            
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
  dest_sockaddr.sin_family      = AF_INET;
  dest_sockaddr.sin_port        = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  /*
   * AM_Poll always checks if there is buffer space (i.e. txfree.num_elements > 0)
   * before accepting a request.  Thus, the following Dequeue operation should
   * never fail.  AM says reply operations should be able to pull in replies
   * for other endpoints in the bundle.  This never occurs.
   */
  LockEP(source_ep);
  ExitRaceHack(source_ep);
  buf = (UDPAM_Buf *)Dequeue(source_ep);  
  ASSERT(buf != NULL);
  buf->type = UDPAM_REPLY_4;  
  packet = &(buf->packet.udpam_short4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_REPLY_4, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3);
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
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
  ExitRaceHack(source_ep);
  buf = (UDPAM_Buf *)Dequeue(source_ep);  /* Should always succeed */
  ASSERT(buf != NULL);

  buf->type = UDPAM_REPLY_8;  
  packet = &(buf->packet.udpam_short8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_REPLY_8, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, 
		    arg3, arg4, arg5, arg6, arg7);
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_ReplyI4(void *token, handler_t handler,
	       void *source_addr, int nbytes, 
	       int arg0, int arg1, int arg2, int arg3)
{
  UDPAM_Buf          *buf;
  UDPAM_Medium4_Pkt  *packet;
  ea_t               source_ep;
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   

  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxMedium()))
    return(AM_ERR_BAD_ARG);

  source_ep = ((Token *)(token))->dest_ep;
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  LockEP(source_ep);
  ExitRaceHack(source_ep);
  buf = (UDPAM_Buf *)Dequeue(source_ep);  /* Should always succeed */
  ASSERT(buf != NULL);

  buf->type = UDPAM_REPLY_MEDIUM_4;  
  packet = &(buf->packet.udpam_medium4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_REPLY_MEDIUM_4, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3);
  memcpy(packet->data, source_addr, nbytes);     
  packet->source_addr = source_addr;
  packet->nbytes = nbytes;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_ReplyI8(void *token, handler_t handler,
	       void *source_addr, int nbytes, 
	       int arg0, int arg1, int arg2, int arg3,
	       int arg4, int arg5, int arg6, int arg7)
{
  UDPAM_Buf          *buf;
  UDPAM_Medium8_Pkt  *packet;
  ea_t               source_ep;
  struct sockaddr_in dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxMedium()))
    return(AM_ERR_BAD_ARG);

  source_ep = ((Token *)(token))->dest_ep;
  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  LockEP(source_ep);
  ExitRaceHack(source_ep);
  buf = (UDPAM_Buf *)Dequeue(source_ep);  /* Should always succeed */
  ASSERT(buf != NULL);

  buf->type = UDPAM_REPLY_MEDIUM_8;  
  packet = &(buf->packet.udpam_medium8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_REPLY_MEDIUM_8, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3, 
		    arg4, arg5, arg6, arg7);
  memcpy(packet->data, source_addr, nbytes);     
  packet->source_addr = source_addr;
  packet->nbytes = nbytes;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);          
}

int AM_ReplyXfer4(void *token, int dest_offset,
		  handler_t handler, void *source_addr, int nbytes, 
		  int arg0, int arg1, int arg2, int arg3)
{
  ea_t                source_ep;
  UDPAM_Buf           *buf;
  UDPAM_Long4_Pkt     *packet;
  struct sockaddr_in  dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   
  source_ep = ((Token *)(token))->dest_ep;
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  LockEP(source_ep);
  ExitRaceHack(source_ep);
  buf = (UDPAM_Buf *)Dequeue(source_ep);  
  ASSERT(buf != NULL);

  buf->type = UDPAM_REPLY_LONG_4;
  packet = &(buf->packet.udpam_long4_pkt);
  UDPAM_PACKET4_SET(packet, UDPAM_REPLY_LONG_4, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3);
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, source_addr, UDPAM_MAX_LONG);
  packet->source_addr = source_addr;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
  AddTimeout(source_ep, buf, packet->buf_id, packet->seq_num, &dest_sockaddr);
  UnlockEP(source_ep);
  return(AM_OK);
}

int AM_ReplyXfer8(void *token, int dest_offset,
		  handler_t handler, void *source_addr, int nbytes, 
		  int arg0, int arg1, int arg2, int arg3,
		  int arg4, int arg5, int arg6, int arg7)
{
  ea_t                  source_ep;
  UDPAM_Buf             *buf;
  UDPAM_Long8_Pkt       *packet;
  struct sockaddr_in    dest_sockaddr;

  if (initialized == FALSE)
    return(AM_ERR_NOT_INIT);   
  source_ep = ((Token *)(token))->dest_ep;
  if ((source_addr == (char *)NULL) || (nbytes > AM_MaxLong())) 
    return(AM_ERR_BAD_ARG);

  dest_sockaddr.sin_family = AF_INET;
  dest_sockaddr.sin_port = ((Token *)(token))->source_port;
  dest_sockaddr.sin_addr.s_addr = ((Token *)(token))->source_ip_addr;

  LockEP(source_ep);
  ExitRaceHack(source_ep);
  buf = (UDPAM_Buf *)Dequeue(source_ep);  /* Should always succeed */
  ASSERT(buf != NULL);

  buf->type = UDPAM_REPLY_LONG_8;
  packet = &(buf->packet.udpam_long8_pkt);
  UDPAM_PACKET8_SET(packet, UDPAM_REPLY_LONG_8, ((Token *)(token))->seq_num, 
		    ((Token *)(token))->buf_id, source_ep->tag, 
		    ((Token *)(token))->tag, handler, arg0, arg1, arg2, arg3, 
		    arg4, arg5, arg6, arg7);
  packet->dest_offset = dest_offset;
  packet->nbytes = nbytes;
  memcpy(packet->data, (void *)source_addr, UDPAM_MAX_LONG);
  packet->source_addr = source_addr;
  while (sendto(source_ep->socket, (const char *)packet, 
		sizeof(*packet), 0, 
		(struct sockaddr *)&dest_sockaddr, 
		SOCKADDR_IN_SIZE) < 0) {
    if (errno != EAGAIN) {
      perror("sendto error\n");
      exit(-1);
    }
  }
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

/*
 * Geez...
 */
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
   * Max number of messages per poll scales with the number of endpoints.
   * This ensures that if messages are present in an endpoint's receive 
   * pool, at least one of those messages will be pulled out.
   */
  count = 0;
   while ((count < bundle->num_eps) && (n > 0)) { 
    listElem = bundle->head;
    for (i = 0; i < bundle->num_eps; i++) {
      endpoint = listElem->endpoint;
      LockEP(endpoint);  
      if (FD_ISSET(endpoint->socket, &tempFdset)) {
	if ((bytesRecv = recvfrom(endpoint->socket, (char *)&packet, 
				  sizeof(packet), 0,
				  (struct sockaddr *)&sender, &size)) < 0) {
	  if (errno == EWOULDBLOCK) {     /* Nothing left in sockbuf */
	    FD_CLR(endpoint->socket, &tempFdset); 
	    n--;
	    /* UnlockEP(endpoint); */
	    goto end;
	  }
	  else {
	    perror("recvfrom error\n");
	    exit(-1);
	  }
	} 
	else {
	  count++;                     /* Pulled in another message */
	  /* Move this AM_Wait() */
	  /* bundle->mask = AM_NOEVENTS; */ /* Atomically Clear Event Mask */
	}

	/*
         * Cast packet to short4 and switch on type.  This is disgusting, but
	 * it saves me a system call to peek at the message.
	 */
	type = ((UDPAM_Short4_Pkt *)&packet)->type;

	/*
	 * If a request comes in and there are no buffers left to 
	 * generate a reply, drop it and let the sender time out. 
	 * Unreliable NACK perhaps?
	 */
	if ((type & REQUEST) && (endpoint->txfree.num_elements == 0)) {
	  DPRINTF(("No space for reply.  Dropping request\n"));
	  /* UnlockEP(endpoint); */
	  goto end;
	}

	switch(type) {
	case UDPAM_REQUEST_4: {
	  UDPAM_Short4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Short4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_4, 
					     packetPtr->tag, 
					     packetPtr->handler, 0, 
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_4, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num); 
	    /*
	     * If the request is resent on the sender's side, bounce it back
	     * again.  This code shouldn't handle duplicates since it can't
	     * distinguish between the first bad request and another request
	     * that looks like it sent as a result of handler0 executing
	     * on the requesting endpoint's side.
	     */
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REQUEST_4, 
			  errorCode, &argblock);
	    UnlockEP(endpoint);
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint); */
 	  }
	  else {
	    /* 
	     * Execute handler with endpoint unlocked.  Do we have a race
	     * here?  
	     */
	    EnterRaceHack(endpoint);
	    UnlockEP(endpoint);
	    ((Handler4)(endpoint->handler_table[packetPtr->handler]))(
 		       (void *)&token, packetPtr->arg0, packetPtr->arg1, 
		       packetPtr->arg2, packetPtr->arg3);
	    LockEP(endpoint);
	    /*
	     * If handler didn't generate a reply, generate one now.
	     */
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      ExitRaceHack(endpoint);
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
        case UDPAM_REQUEST_8: {
	  UDPAM_Short8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Short8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_8, 
					     packetPtr->tag, 
					     packetPtr->handler, 0,
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_8, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6, 
			  packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REQUEST_8, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint); */
	  }
	  else {
	    EnterRaceHack(endpoint);
	    UnlockEP(endpoint);
	    ((Handler8)(endpoint->handler_table[packetPtr->handler]))(
 		       (void *)&token, packetPtr->arg0, packetPtr->arg1, 
		       packetPtr->arg2, packetPtr->arg3, packetPtr->arg4,
		       packetPtr->arg5, packetPtr->arg6, packetPtr->arg7);
	    LockEP(endpoint)
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      ExitRaceHack(endpoint);
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /*UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REQUEST_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Medium4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_MEDIUM_4, 
					     packetPtr->tag, 
					     packetPtr->handler, 0, 
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_MEDIUM_4, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr,
			  0, packetPtr->nbytes, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, 
			  AM_REQUEST_MEDIUM_4, errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint); */
	  }
	  else {
	    EnterRaceHack(endpoint);
	    UnlockEP(endpoint);
	    ((HandlerMedium4)(endpoint->handler_table[packetPtr->handler]))(
 		             (void *)&token, &(packetPtr->data), packetPtr->nbytes, 
			     packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			     packetPtr->arg3);
	    LockEP(endpoint);
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      ExitRaceHack(endpoint);
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REQUEST_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Medium8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_MEDIUM_8,
					     packetPtr->tag, 
					     packetPtr->handler, 0, 
					     0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_MEDIUM_8, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr, 
			  0, packetPtr->nbytes, 0, 0, packetPtr->arg0,
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6,
			  packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, 
			  AM_REQUEST_MEDIUM_8, errorCode, &argblock);
	    /*UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /*UnlockEP(endpoint); */
	  }
	  else {
	    EnterRaceHack(endpoint);
	    UnlockEP(endpoint);
	    ((HandlerMedium8)(endpoint->handler_table[packetPtr->handler]))(
 		             (void *)&token, &(packetPtr->data), packetPtr->nbytes, 
			     packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			     packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			     packetPtr->arg6, packetPtr->arg7);
	    LockEP(endpoint);
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      ExitRaceHack(endpoint);
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REQUEST_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);

	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_LONG_4,
					     packetPtr->tag, 
					     packetPtr->handler, 
					     packetPtr->dest_offset,
					     packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_LONG_4, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr,
			  0, packetPtr->nbytes, packetPtr->dest_offset, 0,
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, 0, 0, 0, 0, packetPtr->buf_id, 
			  packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, 
			  AM_REQUEST_LONG_4, errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint); */
	  }
	  else {
	    memcpy((caddr_t)(endpoint->start_addr) +  packetPtr->dest_offset, 
		   packetPtr->data, packetPtr->nbytes);
	    EnterRaceHack(endpoint);
	    UnlockEP(endpoint);
	    ((HandlerLong4)(endpoint->handler_table[packetPtr->handler]))(
			   (void *)&token, 
			   &((char *)(endpoint->start_addr))[packetPtr->dest_offset], 
			   packetPtr->nbytes, packetPtr->arg0, 
                           packetPtr->arg1, packetPtr->arg2, 
			   packetPtr->arg3);
	    LockEP(endpoint);
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      ExitRaceHack(endpoint);
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REQUEST_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_REQUEST_LONG_8,
					     packetPtr->tag, 
					     packetPtr->handler,
					     packetPtr->dest_offset,
					     packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REQUEST_LONG_8, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr, 
			  0, packetPtr->nbytes, packetPtr->dest_offset, 0,
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			  packetPtr->arg6, packetPtr->arg7, packetPtr->buf_id, 
			  packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, 
			  AM_REQUEST_LONG_8, errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint);*/
	  }
	  else {
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes);
	    EnterRaceHack(endpoint);
	    UnlockEP(endpoint);
	    ((HandlerLong8)(endpoint->handler_table[packetPtr->handler]))(
			   (void *)&token, 
			   &((char *)(endpoint->start_addr))[packetPtr->dest_offset],
			   packetPtr->nbytes,
			   packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			   packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			   packetPtr->arg6, packetPtr->arg7);
	    LockEP(endpoint);
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      ExitRaceHack(endpoint);
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REPLY_4: {
	  UDPAM_Short4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Short4_Pkt *)&packet;  
	  /* 
	   * Gotta be in ep's translation table. This is needed to adjust the
	   * window size.
	   */
	  sourceEndpoint = GlobalToIndex(endpoint, &sender);  
	  BuildToken(&token, &sender, packetPtr->buf_id, 
		     packetPtr->seq_num, packetPtr->src_tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0))
	                   != NO_ERRORS) {
	    DPRINTF(("REPLY_4: errorCode=0x%x\n", errorCode));
	    BuildArgBlock(&argblock, AM_REPLY_4, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REPLY_4, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  else { 
	    /* Increment Window Size */
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    /*
	     * If this was a AM layer generated reply, do nothing.  Otherwise,
	     * execute the specified reply handler.
	     */ 
	    if (packetPtr->handler != UDPAM_EMPTY_HANDLER) {
	      ((Handler4)(endpoint->handler_table[packetPtr->handler]))(
			(void *)&token, packetPtr->arg0, packetPtr->arg1, 
			packetPtr->arg2, packetPtr->arg3);
	    }
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REPLY_8: {
	  UDPAM_Short8_Pkt *packetPtr;
	  
	  packetPtr = (UDPAM_Short8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_8, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 0, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6, 
			  packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REPLY_8, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  else { 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 

	    if (packetPtr->handler != UDPAM_EMPTY_HANDLER) {
	      ((Handler8)(endpoint->handler_table[packetPtr->handler]))(
		(void *)&token, packetPtr->arg0, packetPtr->arg1, 
		packetPtr->arg2, packetPtr->arg3, packetPtr->arg4, 
		packetPtr->arg5, packetPtr->arg6, packetPtr->arg7);
	    }
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  break;
	}
	case UDPAM_REPLY_MEDIUM_4: {
	  UDPAM_Medium4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Medium4_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_MEDIUM_4, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr,
			  0, packetPtr->nbytes, 0, 0, packetPtr->arg0,
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  0, 0, 0, 0, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REPLY_MEDIUM_4, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint);*/
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  else { 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    
	    if (packetPtr->handler != UDPAM_EMPTY_HANDLER) {	    
	      ((HandlerMedium4)(endpoint->handler_table[packetPtr->handler]))(
			      (void *)&token, &(packetPtr->data), 
			      packetPtr->nbytes, packetPtr->arg0, 
			      packetPtr->arg1, packetPtr->arg2, 
			      packetPtr->arg3);
	    }
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}
	case UDPAM_REPLY_MEDIUM_8: {
	  UDPAM_Medium8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Medium8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, 0, 0)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_MEDIUM_8, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr,
			  0, packetPtr->nbytes, 0, 0, packetPtr->arg0, 
			  packetPtr->arg1, packetPtr->arg2, packetPtr->arg3, 
			  packetPtr->arg4, packetPtr->arg5, packetPtr->arg6, 
			  packetPtr->arg7, packetPtr->buf_id, packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REPLY_MEDIUM_8, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint); */
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  else { 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 

	    if (packetPtr->handler != UDPAM_EMPTY_HANDLER) {	    
	      ((HandlerMedium8)(endpoint->handler_table[packetPtr->handler]))(
			      (void *)&token, &(packetPtr->data), packetPtr->nbytes,
			      packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			      packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			      packetPtr->arg6, packetPtr->arg7);
	    }
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  break;
	}
	case UDPAM_REPLY_LONG_4: {
	  UDPAM_Long4_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long4_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, packetPtr->dest_offset, 
			   packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_LONG_4, 0, 0, 
			  &token, packetPtr->handler, packetPtr->source_addr, 
			  0, packetPtr->nbytes, packetPtr->dest_offset, 0, 
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2,
			  packetPtr->arg3, 0, 0, 0, 0, packetPtr->buf_id, 
			  packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REPLY_LONG_4, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint);*/
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  else { 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes); 

	    if (packetPtr->handler != UDPAM_EMPTY_HANDLER) {	    
	      ((HandlerLong4)(endpoint->handler_table[packetPtr->handler]))(
		    (void *)&token, 
		    &((char *)(endpoint->start_addr))[packetPtr->dest_offset], 
		    packetPtr->nbytes,
		    packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
		    packetPtr->arg3);
	    }
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  break;
	}
	case UDPAM_REPLY_LONG_8: {
	  UDPAM_Long8_Pkt *packetPtr;

	  packetPtr = (UDPAM_Long8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag,endpoint);
	  if ((errorCode = ReplyErrorCheck(endpoint, type, packetPtr->tag, 
			   packetPtr->handler, packetPtr->dest_offset,
			   packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_REPLY_LONG_8, 0, 0, &token, 
			  packetPtr->handler, packetPtr->source_addr,
			  0, packetPtr->nbytes,  packetPtr->dest_offset, 0,
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2,
			  packetPtr->arg3, packetPtr->arg4, packetPtr->arg5,
			  packetPtr->arg4, packetPtr->arg7, packetPtr->buf_id, 
			  packetPtr->seq_num);
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_REPLY_LONG_8, 
			  errorCode, &argblock);
	    /* UnlockEP(endpoint);*/
	    break;	    
	  }
	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  else { 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    memcpy((caddr_t)(endpoint->start_addr) + packetPtr->dest_offset, 
		   &(packetPtr->data), packetPtr->nbytes); 

	    if (packetPtr->handler != UDPAM_EMPTY_HANDLER) {	    
	      ((HandlerLong8)(endpoint->handler_table[packetPtr->handler]))(
		    (void *)&token, 
		    &((char *)(endpoint->start_addr))[packetPtr->dest_offset], 
		    packetPtr->nbytes,
		    packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
		    packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
		    packetPtr->arg6, packetPtr->arg7);
	    }
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  break;
	}
	case UDPAM_GET_XFER_4: {
	  UDPAM_GetXfer4_Pkt *packetPtr;

	  packetPtr = (UDPAM_GetXfer4_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);	  
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_GET_XFER_4, 
					     packetPtr->tag, 
					     packetPtr->handler, 
					     packetPtr->source_offset,
					     packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_GET_XFER_4, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 
			  packetPtr->nbytes, packetPtr->dest_offset,
			  packetPtr->source_offset,
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, 0, 0, 0, 0, packetPtr->buf_id, 
			  packetPtr->seq_num); 
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_GET_XFER_4,
			  errorCode, &argblock);
	    /* UnlockEP(endpoint);  */
	    break;	    
	  }	  
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint);*/
	  }
	  else {  /* Have lock */
	    if (AM_GetXferReply4(&token, packetPtr->source_offset,
				 packetPtr->handler, packetPtr->dest_offset,
				 packetPtr->nbytes, packetPtr->arg0,
				 packetPtr->arg1, packetPtr->arg2, 
				 packetPtr->arg3) != AM_OK) {
	      fprintf(stderr, "AM_GetXferReply4 error\n");
	      abort();
	    }
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) {
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    }
	    /* UnlockEP(endpoint);*/
	  }
	  break;
	}	  

	case UDPAM_GET_XFER_8: {
	  UDPAM_GetXfer8_Pkt *packetPtr;

	  packetPtr = (UDPAM_GetXfer8_Pkt *)&packet;
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);	  
	  if ((errorCode = RequestErrorCheck(endpoint, UDPAM_GET_XFER_8, 
					     packetPtr->tag, 
					     packetPtr->handler, 
					     packetPtr->source_offset,
					     packetPtr->nbytes)) != NO_ERRORS) {
	    BuildArgBlock(&argblock, AM_GET_XFER_8, 0, 0, &token, 
			  packetPtr->handler, 0, 0, 
			  packetPtr->nbytes, packetPtr->dest_offset, 
			  packetPtr->source_offset,
			  packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			  packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
			  packetPtr->arg6, packetPtr->arg7, packetPtr->buf_id, 
			  packetPtr->seq_num); 
	    BounceMessage(endpoint, sender, packetPtr->src_tag, AM_GET_XFER_8,
			  errorCode, &argblock);
	    /* UnlockEP(endpoint);*/
	    break;	    
	  }	  
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint); */
	  }
	  else {
	    if (AM_GetXferReply8(&token, packetPtr->source_offset,
				 packetPtr->handler, packetPtr->dest_offset,
				 packetPtr->nbytes, packetPtr->arg0,
				 packetPtr->arg1, packetPtr->arg2, 
				 packetPtr->arg3, packetPtr->arg4,
				 packetPtr->arg5, packetPtr->arg6,
				 packetPtr->arg7) != AM_OK) {
	      fprintf(stderr, "AM_GetXferReply8 error\n");
	      abort();
	    }
	    if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  sender, replyElem) == 0) 
	      EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
			 packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}

        case UDPAM_GET_XFER_REPLY_4: {
	  UDPAM_GetXferReply4_Pkt *packetPtr;

	  packetPtr = (UDPAM_GetXferReply4_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);

	  /* These are internally generated, Errors?  Nah...they should be kewl */

	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  else { 
	    /* Cast for sol2.5.cfront, or else error:
	    cannot do pointer arithmetic on operand of unknown size */
	    /* memcpy(endpoint->start_addr + packetPtr->dest_offset, 
		   packetPtr->data, packetPtr->nbytes); */
	    memcpy((void *)((int)endpoint->start_addr + 
			    packetPtr->dest_offset), 
		   packetPtr->data, packetPtr->nbytes); 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    /* Cast for sol2.5.cfront, or else error:
	    cannot do pointer arithmetic on operand of unknown size */
	    ((HandlerGet4)(endpoint->handler_table[packetPtr->handler]))(
                           /* (void *)&token, endpoint->start_addr + 
			    packetPtr->dest_offset, packetPtr->nbytes, */
                            (void *)&token, 
			    (void *) ((int)endpoint->start_addr + 
			    packetPtr->dest_offset), packetPtr->nbytes,
                            packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			    packetPtr->arg3);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  break;
	}	  

	case UDPAM_GET_XFER_REPLY_8: {
	  UDPAM_GetXferReply8_Pkt *packetPtr;

	  packetPtr = (UDPAM_GetXferReply8_Pkt *)&packet;
	  sourceEndpoint = GlobalToIndex(endpoint, &sender); 
	  BuildToken(&token, &sender, packetPtr->buf_id, packetPtr->seq_num, 
		     packetPtr->src_tag, endpoint);

	  /* These are internally generated, Errors?  Nah...they should be kewl */

	  if (SeenReply(endpoint, packetPtr->buf_id, packetPtr->seq_num)) {
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint); */
	  }
	  else { 
	    /* Cast for sol2.5.cfront, or else error:
	    cannot do pointer arithmetic on operand of unknown size */
	    /* memcpy(endpoint->start_addr + packetPtr->dest_offset, 
		   packetPtr->data, packetPtr->nbytes); */
	    memcpy((void *)((int)endpoint->start_addr + 
			    packetPtr->dest_offset), 
		   packetPtr->data, packetPtr->nbytes); 
	    endpoint->translation_table[sourceEndpoint].wsize++;
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->buf_id])); 
	    ((HandlerGet8)(endpoint->handler_table[packetPtr->handler]))(
                           (void *)&token, packetPtr->data, packetPtr->nbytes,
                            packetPtr->arg0, packetPtr->arg1, packetPtr->arg2, 
			    packetPtr->arg3, packetPtr->arg4, packetPtr->arg5, 
                            packetPtr->arg6, packetPtr->arg7);
	    ReplyACK(endpoint->socket, sender, packetPtr->buf_id, 
		     packetPtr->seq_num);
	    /* UnlockEP(endpoint);*/
	  }
	  break;
	}

        case UDPAM_REPLYACK: {
	  UDPAM_ReplyACK_Pkt *packetPtr;

	  packetPtr = (UDPAM_ReplyACK_Pkt *)&packet;
	  if (FindReply(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			sender, replyElem) == 1) {
	    /* 
	     * Take reply off TxTimeout and free reply storage.
	     */
	    buf = replyElem->message;  
	    RemoveTimeout(endpoint, replyElem->message); 
	    Enqueue(endpoint, buf);
	  }
	  /* UnlockEP(endpoint);   */
	  break;
	}
	case UDPAM_ERROR: {
	  UDPAM_Error_Pkt *packetPtr;
	  
	  DPRINTF(("UDPAM_ERROR packet received\n"));
	  packetPtr = (UDPAM_Error_Pkt *)&packet;	  
	  /*
	   * Since error messages are simply requests (that cause hander0 to
	   * execute), we still have to make sure the request handler executes
	   * only once.  If we've already seen the error message (i.e. an empty 
	   * reply has been sent and lives on txtimeout), resend the empty reply,
	   * but do not execute the handler.
	   */
	  if (SeenRequest(endpoint, packetPtr->buf_id, packetPtr->seq_num, 
			  &sender, &dupElem)) {
	    ResendReply(endpoint, dupElem);
	    /* UnlockEP(endpoint);*/
	    break;
	  }
	  /* 
	   * Pull message(req or reply) that caused the error to occur off TxTimeout.
	   * Send endpoint that sent the message (i.e. orignal destination) an
	   * empty reply indicating that the error message has been received
	   * and handler0 has fired.  The buffer used to send the message
	   * that caused the error has its sequence number increased as part
	   * of RemoveTimeout().
	   */
	  if (packetPtr->mesg_type & REQUEST) {
	    RemoveTimeout(endpoint, &(endpoint->txpool[packetPtr->argblock.buf_id]));
	    Enqueue(endpoint, &(endpoint->txpool[packetPtr->argblock.buf_id]));
	  }
	  else { 
	    FindReply(endpoint, packetPtr->argblock.buf_id, 
		      packetPtr->argblock.seq_num, sender, replyElem);
	    RemoveTimeout(endpoint, replyElem->message);
	    Enqueue(endpoint, replyElem->message);
	  }

	  /* 
	   * buf_id/seq_num of *ERROR* message, not the message that caused the error.
	   * I hope there's space for this.  Otherwise, let the error "request"
	   * time out.
	   */
	  EmptyReply(endpoint, &sender, packetPtr->src_tag, packetPtr->buf_id, 
		     packetPtr->seq_num);
	  /* UnlockEP(endpoint); */    /* Hmm...*/
	  ((ErrorHandler)(endpoint->handler_table[0]))(packetPtr->error_type, 
  		          packetPtr->mesg_type, (void *)&(packetPtr->argblock));
	  break;
	}
        default:
	    DPRINTF(("Bad packet received...throwing it on the floor\n"));
	    break;
        
	}  /* switch(...) */
      }
end:
      UnlockEP(endpoint);
      listElem = listElem->next;  /* Next endpoint elem */
    } /* for (i = 0; i < bundle->num_eps; i++) */
  }
  UnlockBundle(bundle);
  return(AM_OK);
}
