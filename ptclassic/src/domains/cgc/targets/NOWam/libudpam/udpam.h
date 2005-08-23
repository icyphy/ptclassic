/* Header file for Active Messages over UDP */
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

#ifndef _UDPAM_H
#define _UDPAM_H "$Id$";

#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef SOLARIS 
#include <thread.h>
#endif /* SOLARIS */

/*
 * AM Specified values 
 */
#define AM_MIN_NUMBER_OF_HANDLERS     256
#define AM_MIN_NUMBER_OF_TRANSLATIONS 256

/* 
 * Define some implementation specific values.  UDPAM_MAX_{SHORT,
 * MEDIUM, and LONG} are the minimum values required by AM 2.0.
 */
#define AM_MAX_SIZE                       8192       
#define UDPAM_DEFAULT_TXBUFS              64
#define UDPAM_DEFAULT_BUFS                8
#define UDPAM_MAX_NUMBER_OF_TRANSLATIONS  512
#define UDPAM_MAX_NUMBER_OF_HANDLERS      512
#define UDPAM_MAX_SEG_LENGTH              (1 << 28)  /* 256 MB */
#define UDPAM_MAX_SHORT                   8
#define UDPAM_MAX_MEDIUM                  512
#define UDPAM_MAX_LONG                    8192

#define UDPAM_DEFAULT_WSIZE    4  /* Default # of Outstanding Requests */
#define SOCKADDR_IN_SIZE       sizeof(struct sockaddr_in)

typedef int  handler_t;         /* Handlers are just indices to tables */
typedef long int tag_t;         /* Endpoint tags */  
typedef void (*func)();         /* Pointer to real handler function */

/*
 * Each packet contains at least a type, seq_num (seq_num of request), 
 * buf_id (buf_id of req), tag, handler, and arguments 
 */
typedef struct {
  int       type, seq_num, buf_id;                 
  tag_t     tag;                    
  handler_t handler;                
  int       arg0, arg1, arg2, arg3; 
} UDPAM_Short4_Pkt;

typedef struct {
  int       type, seq_num, buf_id;
  tag_t     tag;                   
  handler_t handler;                
  int       arg0, arg1, arg2, arg3;
  int       arg4, arg5, arg6, arg7;
} UDPAM_Short8_Pkt;

typedef struct {
  int       type, seq_num, buf_id;
  tag_t     tag;                   
  handler_t handler;                
  int       arg0, arg1, arg2, arg3;
  int       nbytes;                 
  char      data[UDPAM_MAX_MEDIUM];
} UDPAM_Medium4_Pkt;

typedef struct {
  int       type, seq_num, buf_id;
  tag_t     tag;                   
  handler_t handler;                
  int       arg0, arg1, arg2, arg3;
  int       arg4, arg5, arg6, arg7;
  int       nbytes;                 
  char      data[UDPAM_MAX_MEDIUM];
} UDPAM_Medium8_Pkt;

typedef struct {
  int       type, seq_num, buf_id;
  tag_t     tag;                   
  handler_t handler;
  int       arg0, arg1, arg2, arg3;
  int       dest_offset;
  int       nbytes;
  char      data[UDPAM_MAX_LONG];  
} UDPAM_Long4_Pkt;

typedef struct {
  int       type, seq_num, buf_id;
  tag_t     tag;                   
  handler_t handler;                
  int       arg0, arg1, arg2, arg3;
  int       arg4, arg5, arg6, arg7;
  int       dest_offset;
  int       nbytes;                 
  char      data[UDPAM_MAX_LONG];
} UDPAM_Long8_Pkt;

typedef struct {
  int  type;
  int  buf_id;
  int  seq_num; 
} UDPAM_ReplyACK_Pkt;

/*
 * Generic UDPAM Buffers for All Tranport Operations
 */
typedef struct {
  int   type;     /* Replicate These Three Elements In Pool*/
  int   buf_id;   /* Buffer ID (fixed) */
  int   seq_num;  /* Buffer Sequence Number */
  union {
    UDPAM_Short4_Pkt    udpam_short4_pkt;
    UDPAM_Short8_Pkt    udpam_short8_pkt;
    UDPAM_Medium4_Pkt   udpam_medium4_pkt; 
    UDPAM_Medium8_Pkt   udpam_medium8_pkt; 
    UDPAM_Long4_Pkt     udpam_long4_pkt;
    UDPAM_Long8_Pkt     udpam_long8_pkt;
    UDPAM_ReplyACK_Pkt  udpam_replyack_pkt;
  } packet;
} UDPAM_Buf;

#define MAX_PACKET_SIZE  sizeof(UDPAM_Buf)

/*
 * Source EP is the endpoint the sending endpoint.  dest_ep is the 
 * receiving endpoint.  
 */
typedef struct {
  u_short      source_port;     /* Source EP UDP Port */
  u_long       source_ip_addr;  /* Source EP IP Address */
  int          buf_id;          /* Buffer ID for Request Message */
  int          seq_num;         /* Sequence Num for Request Message */
  tag_t        tag;
  struct _ea_t *dest_ep;        /* Destination EP */
} Token;

/*
 * Argblock for requests and replies.  Everything you ever wanted
 * to know about requests & replies.
 */
typedef struct {
  struct _ea_t *request_endpoint;
  int          reply_endpoint;
  Token        token;                   
  handler_t    handler;
  void         *source_addr;
  int          nbytes;
  int          dest_offset;
  int          arg0, arg1, arg2, arg3;
  int          arg4, arg5, arg6, arg7;
  char         *data; 
} ArgBlock;

/*
 * Three types of messages for UDPAM protocol 
 */
#define REQUEST  0x100
#define REPLY    0x200 
#define REPLYACK 0x400

/*
 * UDPAM message types (4, odd, 8, even) 
 */
#define UDPAM_REQUEST_4        (0x001 | REQUEST)
#define UDPAM_REPLY_4          (0x001 | REPLY)
#define UDPAM_REQUEST_8        (0x002 | REQUEST)

#define UDPAM_REPLY_8          (0x002 | REPLY)
#define UDPAM_REQUEST_MEDIUM_4 (0x003 | REQUEST)
#define UDPAM_REPLY_MEDIUM_4   (0x003 | REPLY)
#define UDPAM_REQUEST_MEDIUM_8 (0x004 | REQUEST) 
#define UDPAM_REPLY_MEDIUM_8   (0x004 | REPLY)
#define UDPAM_REQUEST_LONG_4   (0x005 | REQUEST)   
#define UDPAM_REPLY_LONG_4     (0x005 | REPLY)
#define UDPAM_REQUEST_LONG_8   (0x006 | REQUEST)
#define UDPAM_REPLY_LONG_8     (0x006 | REPLY)
#define UDPAM_REPLYACK         REPLYACK

/*
 * Global Endpoint Names
 */
typedef struct _en_t {
  u_short  port;           /* Request/Reply UDP port */
  u_long   ip_addr;        /* IP address */
} en_t;

/*
 * UDPAM Buffer FIFO for TxFree
 */
typedef struct {
  UDPAM_Buf **pointers;    /* Array of pointers to pool buffers */
  int       head;          /* Head of FIFO (index into array of pointers) */
  int       tail;          /* Tail of FIFO (index into array of pointers) */
  int       num_elements;  /* Number of Elements */
} UDPAM_BufFifo;

/*
 * Doubly linked list to manage timeout lists containing unacknowledged
 * messages.  destination field indicates where a message on the list.
 * timestamp indicates when the message was sent.  req_buf_id and req_seq_num
 * are used to identify a particular request-reply operation.
 */
typedef struct timeout_list {
  struct timeout_elem {
    UDPAM_Buf           *message;    /* Pointer to un-ACKed message buffer */
    int                 req_buf_id;  /* Buffer ID of Request */
    int                 req_seq_num; /* Seq Num of Request Buffer*/
    struct timeval      timestamp;   /* Timestamp for message */
    struct sockaddr_in  destination; /* Message Destination */
    int                 num_tries;   /* Number of Times This Mesg Sent */ 
    struct timeout_elem *prev;       /* Previous element on timeout list */  
    struct timeout_elem *next;       /* Next element on timeout list */
  } *unackmessages;                  /* List of un-ACKed messages */ 
  struct timeout_elem *head;         /* Head of timeout list */
  struct timeout_elem *tail;         /* Tail of timeout list */
  int                 num_elements;  /* Number of unACKed messages on list */
} UDPAM_TimeoutList;

/*
 * Endpoint Objects (receive pool is implicit)
 */
typedef struct _ea_t {
  UDPAM_Buf         *txpool;       /* Send Pool (TxPool) */
  UDPAM_BufFifo     txfree;        /* FIFO of Free Bufs in Send Pool */
  UDPAM_TimeoutList txtimeout;     /* Timeout List for Bufs in Send Pool */
  int               num_txbufs;    /* Number of Send Pool Buffers */
  struct translation {
    en_t  name;                    /* Global endpoint name */
    tag_t tag;                     /* Tag */
    short inuse;                   /* Translation In Use */
    int   wsize;                   /* Number of Outstanding Reqs Allowed */
  } *translation_table;            /* Endpoint Translation Table */
  int       num_translations;      /* Number of translation table entries */
  func      *handler_table;        /* Endpoint Handler Table */
  int       num_handlers;          /* Number of handler table entries */
  void      *start_addr;           /* Start address of VM segment */
  int       length;                /* Length of VM segment */ 
  tag_t     tag;                   /* Endpoint tag */
#ifdef SOLARIS 
  mutex_t   lock;                  /* Mutex lock for locking EP */
#endif /* SOLARIS */
  int       socket;                /* UDP Socket for Request/Replies */
  struct sockaddr_in sockaddr;     /* Socket Address for EP(i.e. global name) */ 
  struct _eb_t *bundle;            /* Bundle that contains this EP */
} *ea_t;

/*
 * Endpoint Bundle Objects
 */
typedef struct _eb_t {
  struct ep_elem {
    ea_t            endpoint;   /* Address of endpoint object */
    struct ep_elem  *prev;      /* Pointer to previous element on the list */
    struct ep_elem  *next;      /* Pointer to next element on the list */    
  } *head;                      /* Head of bundle's list of endpoints */
  int            num_eps;       /* Number of endpoints in this bundle */
#ifdef SOLARIS
  mutex_t        lock;          /* Mutex lock for locking EP */
  sema_t         synch_var;     /* Synchronization variable */
#endif /* SOLARIS */
  int            mask;          /* Event mask */
  int            type;          /* Shared or local bundle */
  fd_set         fdset;         /* Set of socket descriptors to select() on */
  int            maxfd;         /* Largest file descriptor for select() */
  thread_t       bundle_thread; /* Per-Bundle Thread */
} *eb_t;

#endif /* _UDPAM_H */

