/*
 * am.h:  Header for Active Message 2.0 API
 *
 * "Copyright (c) 1995 by Brent N. Chun and The Regents of the University 
 * of California.  All rights reserved."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:              Brent N. Chun
 * Version:             $Revision$
 * Creation Date:       Sat Nov  4 17:25:01 PST 1995
 * Filename:            am.h
 */
#ifndef _AM_H
#define _AM_H   

#define AM_ALL  0xffffffff     /* Deliver all messages to endpoint */  
#define AM_NONE 0x00000000     /* Deliver no messages to endpoint */

#define AM_PAR   1             /* Concurrent bundle/endpoint access */
#define AM_SEQ   0             /* Sequential bundle/endpoint access */

/*
 * Return values to Active Message and Endpoint/Bundle API functions
 */
#define AM_OK           0      /* Function completed successfully */
#define AM_ERR_NOT_INIT 1      /* Active message layer not initialized */
#define AM_ERR_NOT_TERM 2      /* Active message layer not initialized */
#define AM_ERR_BAD_ARG  3      /* Invalid function parameter passed */
#define AM_ERR_RESOURCE 4      /* Problem with requested resource */
#define AM_ERR_NOT_SENT 5      /* Asynchronous message not sent */
#define AM_ERR_IN_USE   6      /* Resource currently in use */

/*
 * Error codes for error handler  
 */
typedef int op_t;

#define EBADARGS     0x1   
#define EBADENTRY    0x2   
#define EBADTAG      0x4
#define EBADHANDLER  0x8
#define EBADSEGOFF   0x10
#define EBADLENGTH   0x20
#define EBADENDPOINT 0x40   
#define ECONGESTION  0x80
#define EUNREACHABLE 0x100  

/*
 * Message types 
 */
#define REQUEST  0x10000000
#define REPLY    0x20000000

#define AM_REQUEST_4        (0x00000001 | REQUEST)
#define AM_REPLY_4          (0x00000001 | REPLY)
#define AM_REQUEST_8        (0x00000002 | REQUEST)
#define AM_REPLY_8          (0x00000002 | REPLY)
#define AM_REQUEST_MEDIUM_4 (0x00000004 | REQUEST)
#define AM_REPLY_MEDIUM_4   (0x00000004 | REPLY)
#define AM_REQUEST_MEDIUM_8 (0x00000008 | REQUEST) 
#define AM_REPLY_MEDIUM_8   (0x00000008 | REPLY)
#define AM_REQUEST_LONG_4   (0x00000010 | REQUEST)   
#define AM_REPLY_LONG_4     (0x00000010 | REPLY)
#define AM_REQUEST_LONG_8   (0x00000020 | REQUEST)
#define AM_REPLY_LONG_8     (0x00000020 | REPLY)

/*
 * Gets are treated like request/reply_xfer operations with data being 
 * transferred from a replying endpoint's virtual memory segment to a 
 * region of the requesting process's address space.
 */
#define AM_GET_XFER_4       (0x00000040 | REQUEST)
#define AM_GET_XFER_REPLY_4 (0x00000040 | REPLY)
#define AM_GET_XFER_8       (0x00000080 | REQUEST)  
#define AM_GET_XFER_REPLY_8 (0x00000080 | REQUEST)

/* 
 * Events 
 */ 
#define AM_NOEVENTS     0x0 
#define AM_NOTEMPTY     0x1

/* Old stuff */
#define AM_EMPTYTONOT   0x1
#define AM_MSGRECEIVED  0x2 

/* 
 * Types of handlers 
 */
typedef void (*ErrorHandler)(int status, op_t opcode, void *argblock); 
typedef void (*Handler4)(void *token, int arg0, int arg1, int arg2,
			 int arg3);  
typedef void (*Handler8)(void *token, int arg0, int arg1, int arg2, int arg3,
			 int arg4, int arg5, int arg6, int arg7); 
typedef void (*HandlerMedium4)(void *token, void *buf, int nbytes, 
			       int arg0, int arg1, int arg2, int arg3);  
typedef void (*HandlerMedium8)(void *token, void *buf, int nbytes, 
			       int arg0, int arg1, int arg2, int arg3,
			       int arg4, int arg5, int arg6, int arg7); 
typedef void (*HandlerLong4)(void *token, int dest_offset, int nbytes, 
			     int arg0, int arg1, int arg2, int arg3);  
typedef void (*HandlerLong8)(void *token, int dest_offset, int nbytes, 
			     int arg0, int arg1, int arg2, int arg3, 
			     int arg4, int arg5, int arg6, int arg7); 
typedef void (*HandlerGet4)(void *token, void *buf, int nbytes, int arg0,
			    int arg1, int arg2, int arg3);
typedef void (*HandlerGet8)(void *token, void *buf, int nbytes, int arg0,
			    int arg1, int arg2, int arg3, int arg4, int arg5,
			    int arg6, int arg7);

/*
 * Active Message API 
 */
extern int AM_Init();
extern int AM_Terminate();
 
/* 
 * Request functions 
 */ 
extern int AM_Request4(ea_t request_endpoint, int reply_endpoint, 
		       handler_t handler, 
		       int arg0, int arg1, int arg2, int arg3); 

extern int AM_Request8(ea_t request_endpoint, int reply_endpoint, 
		       handler_t handler, 
		       int arg0, int arg1, int arg2, int arg3,
		       int arg4, int arg5, int arg6, int arg7);

extern int AM_RequestI4(ea_t request_endpoint, int reply_endpoint,
			handler_t handler, void *source_addr, int nbytes,
			int arg0, int arg1, int arg2, int arg3);

extern int AM_RequestI8(ea_t request_endpoint, int reply_endpoint,
			handler_t handler, void *source_addr, int nbytes,
			int arg0, int arg1, int arg2, int arg3,
			int arg4, int arg5, int arg6, int arg7);

extern int AM_RequestXfer4(ea_t request_endpoint, int reply_endpoint,
			   int dest_offset, handler_t handler, 
			   void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3); 

extern int AM_RequestXfer8(ea_t request_endpoint, int reply_endpoint,
			   int dest_offset, handler_t handler, 
			   void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3,
			   int arg4, int arg5, int arg6, int arg7);

extern int AM_RequestXferAsync4(ea_t request_endpoint, int reply_endpoint,
				int dest_offset, handler_t handler, 
				void *source_addr, int nbytes,
				int arg0, int arg1, int arg2, int arg3); 
extern int AM_RequestXferAsync8(ea_t request_endpoint, int reply_endpoint,
				int dest_offset, handler_t handler, 
				void *source_addr, int nbytes,
				int arg0, int arg1, int arg2, int arg3,
				int arg4, int arg5, int arg6, int arg7);

/* 
 * Reply functions 
 */
extern int AM_Reply4(void *token, handler_t handler,
		     int arg0, int arg1, int arg2, int arg3);

extern int AM_Reply8(void *token, handler_t handler,
		     int arg0, int arg1, int arg2, int arg3,	       
		     int arg4, int arg5, int arg6, int arg7);

extern int AM_ReplyI4(void *token, handler_t handler, 
		      void *source_addr, int nbytes, int arg0, 
		      int arg1, int arg2, int arg3);

extern int AM_ReplyI8(void *token, handler_t handler,
		      void *source_addr, int nbytes, 
		      int arg0, int arg1, int arg2, int arg3,	       
		      int arg4, int arg5, int arg6, int arg7);

extern int AM_ReplyXfer4(void *token, int dest_offset,
			 handler_t handler, void *source_addr, int nbytes, 
			 int arg0, int arg1, int arg2, int arg3);

extern int AM_ReplyXfer8(void *token, int dest_offset,
			 handler_t handler, void *source_addr, int nbytes, 
			 int arg0, int arg1, int arg2, int arg3,
			 int arg4, int arg5, int arg6, int arg7);

extern int AM_GetXfer4(ea_t request_endpoint, int reply_endpoint, 
		       int source_offset, handler_t handler, int dest_offset,
		       int nbytes, int arg0, int arg1, int arg2, int arg3);

extern int AM_GetXfer8(ea_t request_endpoint, int reply_endpoint, 
		       int source_offset, handler_t handler, int dest_offset,
		       int nbytes, int arg0, int arg1, int arg2, int arg3,
		       int arg4, int arg5, int arg6, int arg7);

extern int AM_GetXferAsynch4(ea_t request_endpoint, int reply_endpoint, 
			     int source_offset, handler_t handler, 
			     int dest_offset, int nbytes, int arg0, int arg1, 
			     int arg2, int arg3);
extern int AM_GetXferAsynch8(ea_t request_endpoint, int reply_endpoint, 
			     int source_offset, handler_t handler, 
			     int dest_offset, int nbytes, int arg0, int arg1, 
			     int arg2, int arg3, int arg4, int arg5, int arg6, 
			     int arg7);

extern int AM_Poll(eb_t bundle);
extern int AM_MaxShort();
extern int AM_MaxMedium();
extern int AM_MaxLong();

/*
 * Endpoint and Bundle API  
 */
extern int AM_AllocateBundle(int type, eb_t *endb);
extern int AM_AllocateEndpoint(eb_t bundle, ea_t *endp);
extern int AM_Map(ea_t ea, int index, en_t remote_endpoint, tag_t tag); 
extern int AM_MapAny(ea_t ea, int *index, en_t remote_endpoint, tag_t tag); 
extern int AM_Unmap(ea_t ea, int index);
extern int AM_FreeEndpoint(ea_t ea);
extern int AM_FreeBundle(eb_t bundle);
extern int AM_MoveEndpoint(ea_t ea, eb_t from_bundle, eb_t to_bundle);
extern int AM_SetExpectedResources(ea_t ea, int n_endpoints, 
				   int n_outstanding_messages);
extern int AM_SetTag(ea_t ea, tag_t tag);
extern int AM_GetTag(ea_t ea, tag_t *tag);
extern int AM_GetTranslationName(ea_t ea, int i, en_t *gan);
extern int AM_GetTranslationTag(ea_t ea, int i, tag_t *tag);
extern int AM_GetTranslationInuse(ea_t ea, int i);
extern int AM_MaxNumTranslations(int *ntrans);
extern int AM_GetNumTranslations(ea_t ea, int *ntrans);
extern int AM_SetNumTranslations(ea_t ea,int n_translations);
extern int AM_GetSourceEndpoint(void *token, en_t *gan);
extern int AM_GetDestEndpoint(void *token, ea_t *endp);
extern int AM_GetMsgTag(void *token, tag_t *tagp);
extern int AM_SetHandler(ea_t ea, handler_t handler, void (*function)());
extern int AM_SetHandlerAny(ea_t ea, handler_t *handler, void (*function)());
extern int AM_GetNumHandlers(ea_t ea, int *n_handlers);
extern int AM_SetNumHandlers(ea_t ea,int n_handlers);
extern int AM_MaxNumHandlers();
extern int AM_SetSeg(ea_t ea, void *addr, int nbytes);
extern int AM_GetSeg(ea_t ea, void *addr, int *nbytes);
extern int AM_MaxSegLength(int *nbytes);
extern int AM_GetEventMask(eb_t eb);
extern int AM_SetEventMask(eb_t eb, int mask);
extern int AM_Wait(eb_t eb);

/*
 * Old names for those using older versions of the spec.  This will go away
 * later..
 */
#define AM_RequestMedium4(request_endpoint,reply_endpoint,handler,source_addr,nbytes,arg0,arg1,arg2,arg3) \
  AM_RequestI4(request_endpoint,reply_endpoint,handler,source_addr,nbytes,arg0,arg1,arg2,arg3)

#define AM_RequestMedium8(request_endpoint,reply_endpoint,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
  AM_RequestI8(request_endpoint,reply_endpoint,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7)

#define AM_RequestLong4(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3) \
  AM_RequestXfer4(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3)

#define AM_RequestLong8(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
  AM_RequestXfer8(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7)

#define AM_RequestLongAsync4(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3) \
  AM_RequestXferAsync4(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3) 

#define AM_RequestLongAsync8(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
  AM_RequestXferAsync8(request_endpoint,reply_endpoint,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) 

#define AM_ReplyMedium4(token,handler,nbytes,source_addr,arg0,arg1,arg2,arg3) \
  AM_ReplyI4(token,handler,nbytes,source_addr,arg0,arg1,arg2,arg3)
#define AM_ReplyMedium8(token,handler,nbytes,source_addr,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
  AM_ReplyI8(token,handler,nbytes,source_addr,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7)

#define AM_ReplyLong4(token,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3) \
  AM_ReplyXfer4(token,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3)
#define AM_ReplyLong8(token,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) \
  AM_ReplyXfer8(token,dest_offset,handler,source_addr,nbytes,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7)

#define AM_WaitSema(eb)     AM_Wait(eb)  

extern int AM_AllocateKnownEndpoint(eb_t bundle, ea_t *endp, int port);

#endif /* _AM_H */
