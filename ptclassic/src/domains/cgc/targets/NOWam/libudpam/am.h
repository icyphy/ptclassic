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

 Programmer: The NOW project, Berkeley

*******************************************************************/

#ifndef _AM_H
#define _AM_H   

#define AM_ALL  0xffffffff
#define AM_NONE 0x00000000

#define AM_PAR   1
#define AM_SEQ   0

#define AM_OK           0      /* Function completed successfully */
#define AM_ERR_NOT_INIT 1      /* Active message layer not initialized */
#define AM_ERR_NOT_TERM 2      /* Active message layer not initialized */
#define AM_ERR_BAD_ARG  3      /* Invalid function parameter passed */
#define AM_ERR_RESOURCE 4      /* Problem with requested resource */
#define AM_ERR_NOT_SENT 5      /* Asynchronous message not sent */
#define AM_ERR_IN_USE   6      /* Resource currently in use */

/* Error codes for error handler */
typedef int op_t;

#define EBADARGS     0x1   
#define EBADENTRY    0x2   
#define EBADTAG      0x4
#define EBADHANDLER  0x8
#define EBADSEGOFF   0x10
#define EBADLENGTH   0x20
#define EBADENDPOINT 0x40   /* Not Applicable to UDPAM */
#define ECONGESTION  0x80
#define EUNREACHABLE 0x100  

#define AM_REQUEST        1
#define AM_REQUEST_MEDIUM 2 
#define AM_REQUEST_LONG   3
#define AM_REPLY          4
#define AM_REPLY_MEDIUM   5
#define AM_REPLY_LONG     6

/* Events */ 
#define AM_NOEVENTS     0x0 
#define AM_EMPTYTONOT   0x1
#define AM_MSGRECEIVED  0x2 

/* Types of Handlers */
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

/*
 * Active Messages API 
 */
extern int AM_Init();
extern int AM_Terminate();
 
/* Request Functions */ 
extern int AM_Request4(ea_t request_endpoint, int reply_endpoint, 
		       handler_t handler, 
		       int arg0, int arg1, int arg2, int arg3); 

extern int AM_Request8(ea_t request_endpoint, int reply_endpoint, 
		       handler_t handler, 
		       int arg0, int arg1, int arg2, int arg3,
		       int arg4, int arg5, int arg6, int arg7);

extern int AM_RequestMedium4(ea_t request_endpoint, int reply_endpoint,
			     handler_t handler, void *source_addr, int nbytes,
			     int arg0, int arg1, int arg2, int arg3);

extern int AM_RequestMedium8(ea_t request_endpoint, int reply_endpoint,
			     handler_t handler, void *source_addr, int nbytes,
			     int arg0, int arg1, int arg2, int arg3,
			     int arg4, int arg5, int arg6, int arg7);
extern int AM_RequestMediumAsync4(ea_t request_endpoint, int reply_endpoint,
			  handler_t handler, void *source_addr, int nbytes,
			  int arg0, int arg1, int arg2, int arg3);

extern int AM_RequestMediumAsync8(ea_t request_endpoint, int reply_endpoint,
			  handler_t handler, void *source_addr, int nbytes,
			  int arg0, int arg1, int arg2, int arg3,
 			  int arg4, int arg5, int arg6, int arg7);

extern int AM_RequestLong4(ea_t request_endpoint, int reply_endpoint,
			   int dest_offset, handler_t handler, 
			   void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3); 

extern int AM_RequestLong8(ea_t request_endpoint, int reply_endpoint,
			   int dest_offset, handler_t handler, 
			   void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3,
			   int arg4, int arg5, int arg6, int arg7);

extern int AM_RequestLongAsync4(ea_t request_endpoint, int reply_endpoint,
			   int dest_offset, handler_t handler, 
			   void *source_addr, int nbytes,
			   int arg0, int arg1, int arg2, int arg3); 

extern int AM_RequestLongAsync8(ea_t request_endpoint, int reply_endpoint,
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

extern int AM_ReplyMedium4(void *token, handler_t handler,
			   int nbytes, void *source_addr,
			   int arg0, int arg1, int arg2, int arg3);

extern int AM_ReplyMedium8(void *token, handler_t handler,
			   int nbytes, void *source_addr, 
			   int arg0, int arg1, int arg2, int arg3,	       
			   int arg4, int arg5, int arg6, int arg7);

extern int AM_ReplyLong4(void *token, int dest_offset,
			 handler_t handler, void *source_addr, int nbytes, 
			 int arg0, int arg1, int arg2, int arg3);

extern int AM_ReplyLong8(void *token, int dest_offset,
			 handler_t handler, void *source_addr, int nbytes, 
			 int arg0, int arg1, int arg2, int arg3,
			 int arg4, int arg5, int arg6, int arg7);

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
extern int AM_WaitSema(eb_t eb);
extern int AM_PostSvar(eb_t eb);

extern int AM_AllocateKnownEndpoint(eb_t bundle, ea_t *endp, int port);

#endif /* _AM_H */
