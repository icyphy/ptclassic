/* Active Message Endpoint and Bundle Management Functions*/
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <udpam.h>
#include <am.h>
#include <misc.h>  /* Need this? */
#ifdef SOLARIS 
#include <thread.h>
#include <sys/filio.h>  
#endif /* SOLARIS */

static

void BindAndSetSocket(int sd)
{
  int                arg = 1, h_errno;
  int                opt = SB_MAX;  
  int                opt_len = sizeof(opt);
  struct sockaddr_in addr;
  struct hostent     *host;
  char               **p, hostname[256]; /* MAX_HOSTNAME_LEN? */
  char               buffer[256];        /* Large enough to hold result */
  int                bufferlen = 256;
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(0);    /* System specified UDP port */
  if ((host = (struct hostent *)malloc(sizeof(struct hostent))) == NULL) {
    perror("malloc error\n");
    exit(-1);
  }
  if (gethostname(hostname, 256*sizeof(char)) < 0) {
    perror("gethostbyname error\n");
    exit(-1);
  }
  if ((host = gethostbyname_r(hostname, host, buffer, bufferlen, 
			      &h_errno)) == NULL) {
    perror("gethostbyname_r error\n");
    exit(-1);
  }
  p = host->h_addr_list;
  if (*p != 0) 
    (void)memcpy(&addr.sin_addr.s_addr, *p, sizeof(addr.sin_addr.s_addr));  
  if (bind(sd, (struct sockaddr *)(&addr), sizeof(addr)) < 0) {
    perror("bind error\n");
    exit(-1);
  }
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (void *)(&opt), opt_len) < 0) {
    perror("setsockopt error\n");
    exit(-1);    
  }
  if (setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (void *)&opt, opt_len) < 0) {
    perror("setsockopt error\n");
    exit(-1);    
  }
  if (setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (void *)&opt, opt_len) < 0) {
    perror("setsockopt error\n");
    exit(-1);    
  }
  if (ioctl(sd, FIONBIO, &arg) < 0) {
    perror("ioctl error\n");
    exit(-1);
  }
}

/*
 * Endpoint and Bundle API Functions
 */
int AM_AllocateBundle(int type, eb_t *endb)
{
  if ((type != AM_SEQ) && (type != AM_PAR)) 
    return(AM_ERR_BAD_ARG);

  if (((*endb) = (eb_t)calloc(1, sizeof(struct _eb_t))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
#ifdef SOLARIS
  if (mutex_init(&(*endb)->lock, USYNC_THREAD, NULL) != 0) {
    perror("mutex_init error\n");
    exit(-1);
  }
  if (sema_init(&(*endb)->synch_var, 0, USYNC_THREAD, NULL) != 0) {
    perror("sema_wait error\n");
    exit(-1);
  }
#endif /* SOLARIS */
  (*endb)->type = type;
  (*endb)->mask = AM_NOEVENTS;
  FD_ZERO(&(*endb)->fdset); 
  if (thr_create(NULL, 0, TimeoutThread, (void *)(*endb), THR_BOUND,
		 &(*endb)->bundle_thread) < 0) {
    perror("thr_create error\n");
    exit(-1);
  }
  return(AM_OK);
}

int AM_AllocateEndpoint(eb_t bundle, ea_t *endp)
{
  struct ep_elem *new_elem;
  int            i, size = sizeof(struct sockaddr_in);
  
  if (((*endp) = (ea_t)calloc(1, sizeof(struct _ea_t))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  /* Initialize TxPool */
  if (((*endp)->txpool = calloc(UDPAM_DEFAULT_TXBUFS, sizeof(UDPAM_Buf)))
      == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  (*endp)->num_txbufs = UDPAM_DEFAULT_TXBUFS; 
  /* Initialize TxFree FIFO */
  if (((*endp)->txfree.pointers = (UDPAM_Buf **)calloc(UDPAM_DEFAULT_TXBUFS, 
			           sizeof(UDPAM_Buf *))) ==  NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  (*endp)->txfree.head = 0; 
  (*endp)->txfree.tail = UDPAM_DEFAULT_TXBUFS - 1;
  (*endp)->txfree.num_elements = UDPAM_DEFAULT_TXBUFS;
  /* Initialize TxTimeout */
  if (((*endp)->txtimeout.unackmessages = (struct timeout_elem *)
       calloc(UDPAM_DEFAULT_TXBUFS, sizeof(struct timeout_elem))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  for (i = 0; i < UDPAM_DEFAULT_TXBUFS; i++) {
    (*endp)->txpool[i].buf_id = i;                       /* TxPool */
    (*endp)->txfree.pointers[i] = &((*endp)->txpool[i]); /* TxFree */
    (*endp)->txtimeout.unackmessages[i].message = (UDPAM_Buf *)NULL; 
    (*endp)->txtimeout.unackmessages[i].prev = (struct timeout_elem *)NULL;
    (*endp)->txtimeout.unackmessages[i].next = (struct timeout_elem *)NULL;
  } 
  (*endp)->txtimeout.head = (struct timeout_elem *)NULL;
  (*endp)->txtimeout.tail = (struct timeout_elem *)NULL;
  (*endp)->txtimeout.num_elements = 0;
  /* Initialize Translation/Handler tables, etc. */
  if (((*endp)->translation_table = (struct translation *)
       calloc(AM_MIN_NUMBER_OF_TRANSLATIONS, sizeof(struct translation))) 
      == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  for (i = 0; i < AM_MIN_NUMBER_OF_TRANSLATIONS; i++) {
    (*endp)->translation_table[i].tag = AM_NONE;
  }
  (*endp)->num_translations = AM_MIN_NUMBER_OF_TRANSLATIONS;
  
  if (((*endp)->handler_table = (func *)calloc(AM_MIN_NUMBER_OF_HANDLERS, 
					       sizeof(func))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  for (i = 0; i < AM_MIN_NUMBER_OF_HANDLERS; i++) 
    (*endp)->handler_table[i] = abort;
  (*endp)->num_handlers = AM_MIN_NUMBER_OF_HANDLERS;    
  (*endp)->start_addr = (void *)0;
  (*endp)->length = 0;
  (*endp)->tag = AM_NONE;
#ifdef SOLARIS
 if (mutex_init(&((*endp)->lock), USYNC_THREAD, NULL) != 0) {
   perror("mutex_init error\n");
   exit(-1);
 }
#endif /* SOLARIS */
  if (((*endp)->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket error\n");
    exit(-1);
  }
  if (getsockname((*endp)->socket, (struct sockaddr *)&((*endp)->sockaddr),
		  &size) < 0) {
    perror("getsockname error\n");
    exit(-1);
  }
  BindAndSetSocket((*endp)->socket);
  if ((new_elem = (struct ep_elem *)malloc(sizeof(struct ep_elem))) == NULL) {
    perror("malloc error\n");
    exit(-1);
  }
  LockBundle(bundle);
  /* Add endpoint to bundle's list of endpoints */
  if (bundle->num_eps == 0) { 
    new_elem->prev = (struct ep_elem *)NULL;
    new_elem->endpoint = (*endp);
    new_elem->next = (struct ep_elem *)NULL;
  }
  else {
    bundle->head->prev = new_elem;
    new_elem->prev = (struct ep_elem *)NULL;
    new_elem->endpoint = (*endp);
    new_elem->next = bundle->head;
  }
  bundle->head = new_elem;
  bundle->num_eps++;
  FD_SET((*endp)->socket, &(bundle->fdset));
  if ((*endp)->socket > bundle->maxfd)
    bundle->maxfd = (*endp)->socket;
  UnlockBundle(bundle);
  (*endp)->bundle = bundle;
  return(AM_OK);
}

int AM_Map(ea_t ea, int index, en_t remote_endpoint, tag_t tag)
{
  if ((ea == NULL) || (index < 0) || (index >= ea->num_translations)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  if (!ea->translation_table[index].inuse) {
    ea->translation_table[index].name.port = remote_endpoint.port;
    ea->translation_table[index].name.ip_addr = remote_endpoint.ip_addr;
    ea->translation_table[index].tag = tag;      
    ea->translation_table[index].inuse = 1;
    ea->translation_table[index].wsize = UDPAM_DEFAULT_WSIZE;  
    UnlockEP(ea);
    return(AM_OK);
  }
  else {
    UnlockEP(ea);
    return(AM_ERR_IN_USE);
  }
}

int AM_MapAny(ea_t ea, int *index, en_t remote_endpoint, tag_t tag)
{
  int i = 0;

  if ((ea == NULL) || (index == NULL)) 
     return(AM_ERR_BAD_ARG);
   LockEP(ea);
  while (i < ea->num_translations) {
    if (!ea->translation_table[i].inuse)  /* Linear Search, Ack! */
      break;
    else
      i++;
  }
  if (i != ea->num_translations) {
    *index = i;           
    ea->translation_table[i].name.port = remote_endpoint.port;
    ea->translation_table[i].name.ip_addr = remote_endpoint.ip_addr;    
    ea->translation_table[i].tag = tag;
    ea->translation_table[i].inuse = 1;
    ea->translation_table[i].wsize = UDPAM_DEFAULT_WSIZE;
    UnlockEP(ea);
    return(AM_OK);
  }
  else {
    UnlockEP(ea);
    return(AM_ERR_RESOURCE);
  }
}

int AM_Unmap(ea_t ea, int index)
{
  if ((ea == NULL) || (index < 0) || (index >= ea->num_translations)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  ea->translation_table[index].inuse = 0; 
  UnlockEP(ea); 
  return(AM_OK);
}

int AM_FreeEndpoint(ea_t ea)
{
  int            i = 0;
  struct ep_elem *temp;
  fd_set         fdset;
  
  if (ea == NULL) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  LockBundle(ea->bundle);
  FD_CLR(ea->socket, &(ea->bundle->fdset));
  close(ea->socket);
  if (ea->socket == ea->bundle->maxfd) {
    memcpy(&fdset, &(ea->bundle->fdset), sizeof(ea->bundle->fdset));
    i = ea->bundle->maxfd - 1;
    while (i > 0) {
      if (FD_ISSET(i, &fdset)) {
	ea->bundle->maxfd = i;
	break;
      }
      else
	i--;
    }
  }
  temp = ea->bundle->head;
  while (temp != (struct ep_elem *)NULL) {
    if (temp->endpoint == ea) 
      break;
    else 
      temp = temp->next;
  }
  if (ea->bundle->num_eps == 1)                   
    ea->bundle->head = (struct ep_elem *)NULL;
  else if (temp->endpoint == ea->bundle->head->endpoint) { 
    temp->next->prev = (struct ep_elem *)NULL;   
    ea->bundle->head = temp->next;    
  }
  else if (temp->next == (struct ep_elem *)NULL)  
    temp->prev->next = (struct ep_elem *)NULL;
  else {                                          
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;
  }
  ea->bundle->num_eps--; 
  UnlockBundle(ea->bundle);
  free(ea->txpool);
  free(ea->txfree.pointers);
  free(ea->txtimeout.unackmessages);
  free(ea->translation_table);
  free(ea->handler_table);
  free(ea);    
  return(AM_OK);
}

int AM_FreeBundle(eb_t bundle)
{
  int            error_code;
  struct ep_elem *elem;
  
  if (bundle == NULL) 
    return(AM_ERR_BAD_ARG);
  LockBundle(bundle); 
  if (thr_kill(bundle->bundle_thread, SIGKILL) != 0) {
    perror("thr_kill error\n");
    exit(-1);
  }
  if (bundle->num_eps > 0) {
    elem = bundle->head;
    while (elem != (struct ep_elem *)NULL) {
      if ((error_code = AM_FreeEndpoint(elem->endpoint)) != AM_OK)
	return(error_code);
      elem = elem->next;
    }
  }
  free(bundle);
  return(AM_OK);
}

int AM_MoveEndpoint(ea_t ea, eb_t from_bundle, eb_t to_bundle)
{
  int            i = 0;
  struct ep_elem *temp;

  if ((ea == NULL) || (from_bundle == NULL) || (to_bundle == NULL) ||
       (ea->bundle != from_bundle)) 
    return(AM_ERR_BAD_ARG);

  LockEP(ea); 
  LockBundle(from_bundle);
  temp = from_bundle->head;
  while (i < from_bundle->num_eps) {
    if (temp->endpoint == ea) 
      break;
    else {
      temp = temp->next;
      i++;
    }
  }
  if (i == from_bundle->num_eps)  
    return(AM_ERR_BAD_ARG);   /* ea not in from_bundle */
 
  if (from_bundle->num_eps == 1)                   
    from_bundle->head = (struct ep_elem *)NULL;
  else if (temp->endpoint == from_bundle->head->endpoint) {
    temp->next->prev = (struct ep_elem *)NULL;   
    from_bundle->head = temp->next;    
  }
  else if (temp->next == (struct ep_elem *)NULL)  
    temp->prev->next = (struct ep_elem *)NULL;
  else {                                          
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;
  }  
  from_bundle->num_eps--; 
  LockBundle(to_bundle);
  if (to_bundle->num_eps == 0) {
    temp->prev = temp->next = (struct ep_elem *)NULL;
    to_bundle->head = temp;
  }
  else {
    to_bundle->head->prev = temp;
    temp->next = to_bundle->head;
    to_bundle->head = temp;
  }
  UnlockBundle(from_bundle); 
  to_bundle->num_eps++;
  ea->bundle = to_bundle;
  UnlockBundle(to_bundle); 
  UnlockEP(ea); 
  return(AM_OK);
}

int AM_SetExpectedResources(ea_t ea, int n_endpoints, 
			    int n_outstanding_messages)
{
  if ((ea == NULL) || (n_endpoints < 0) || (n_outstanding_messages < 0)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  /* Your code here... */
  UnlockEP(ea);
  return(AM_OK);
}

int AM_SetTag(ea_t ea, tag_t tag)
{
  if (ea == NULL) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea); 
  ea->tag = tag;
  UnlockEP(ea);
  return(AM_OK);
}

int AM_GetTag(ea_t ea, tag_t *tag)
{
  if ((ea == NULL) || (tag == NULL)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea); 
  *tag = ea->tag;
  UnlockEP(ea);
  return(AM_OK);
}

int AM_GetTranslationName(ea_t ea, int i, en_t *gan)
{
  if ((ea == NULL) || (i < 0) || (i >= ea->num_translations) || 
      (gan == NULL)) {
    return(AM_ERR_BAD_ARG);
  }
  LockEP(ea); 
  if (ea->translation_table[i].inuse) {
    gan->port = ea->translation_table[i].name.port;
    gan->ip_addr = ea->translation_table[i].name.ip_addr;
    UnlockEP(ea);
    return(AM_OK);
  }
  else {
    UnlockEP(ea);
    return(AM_ERR_BAD_ARG);
  }
}

int AM_GetTranslationTag(ea_t ea, int i, tag_t *tag)
{
  if ((ea == NULL) || (i < 0) || (i >= ea->num_translations) || 
      (tag == NULL)) 
    return(AM_ERR_BAD_ARG);    

  LockEP(ea); 
  if (ea->translation_table[i].inuse) {
    *tag = ea->translation_table[i].tag;
    UnlockEP(ea);
    return(AM_OK);
  }
  else {
    UnlockEP(ea);
    return(AM_ERR_BAD_ARG);
  }
}

int AM_GetTranslationInuse(ea_t ea, int i)
{
  if ((ea == NULL) || (i < 0) || (i >= ea->num_translations)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea); 
  if (ea->translation_table[i].inuse) {
    UnlockEP(ea); 
    return(AM_OK);
  }
  else {
    UnlockEP(ea); 
    return(AM_ERR_BAD_ARG);  /* ? */
  }
}

int AM_MaxNumTranslations(int *ntrans)
{
  if (ntrans == NULL) 
    return(AM_ERR_BAD_ARG);
  else {
    *ntrans = UDPAM_MAX_NUMBER_OF_TRANSLATIONS;
    return(AM_OK);
  }
}

int AM_GetNumTranslations(ea_t ea, int *ntrans)
{
  if ((ea == NULL) || (ntrans == NULL)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  *ntrans = ea->num_translations;
  UnlockEP(ea);
  return(AM_OK);
}

/*
 * Ack!
 */
int AM_SetNumTranslations(ea_t ea, int n_translations)
{
  int i, max, error_code;

  if (ea == NULL) 
    return(AM_ERR_BAD_ARG);
  if ((error_code = AM_MaxNumTranslations(&max)) != AM_OK) 
    return(error_code);
  if ((n_translations > max) || 
      (n_translations < AM_MIN_NUMBER_OF_TRANSLATIONS)) 
    return(AM_ERR_BAD_ARG);

  LockEP(ea);
  if (n_translations > ea->num_translations) {
    if ((ea->translation_table = 
           (struct translation *)realloc(ea->translation_table,
 	   n_translations*sizeof(struct translation))) == NULL) {
      perror("realloc error\n");
      exit(-1);
    }
    for (i = ea->num_translations; i < n_translations; i++) {
      ea->translation_table[i].name.port = 0;
      ea->translation_table[i].name.ip_addr = 0;
      ea->translation_table[i].tag = AM_NONE;
      ea->translation_table[i].inuse = 0;
      ea->translation_table[i].wsize = 0;
    }
  }
  else {
    /* No Need To Zero These */
    for (i = n_translations; i < ea->num_translations; i++) {
      	ea->translation_table[i].name.port = 0;
      	ea->translation_table[i].name.ip_addr = 0;
	ea->translation_table[i].tag = AM_NONE;
      	ea->translation_table[i].inuse = 0;
	ea->translation_table[i].wsize = 0;
      }
    if ((ea->translation_table = 
	 (struct translation *)realloc(ea->translation_table, 
         n_translations*sizeof(struct translation))) == NULL) {
      perror("realloc error\n");  
      exit(-1);
    }
  }
  ea->num_translations = n_translations;       
  UnlockEP(ea);
  return(AM_OK);
}

int AM_GetSourceEndpoint(void *token, en_t *gan)
{
  if ((token == NULL) || (gan == NULL)) 
    return(AM_ERR_BAD_ARG);
  gan->port = ((Token *)(token))->source_port;
  gan->ip_addr = ((Token *)(token))->source_ip_addr;
  return(AM_OK);
}

int AM_GetDestEndpoint(void *token, ea_t *endp)
{
  if ((token == NULL) || (endp == NULL)) 
    return(AM_ERR_BAD_ARG);
  *endp = ((Token *)(token))->dest_ep;
  return(AM_OK);
}

int AM_SetHandler(ea_t ea, handler_t handler, void (*function)())
{
  if ((ea == NULL) || (handler < 0) || (handler > ea->num_handlers)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea); 
  if (ea->handler_table[handler] == abort) 
    ea->handler_table[handler] = function;
  else {
    UnlockEP(ea);
    return(AM_ERR_IN_USE);
  }
  UnlockEP(ea);
  return(AM_OK);
}

/* 
 * Note: abort() == unused entry 
 */
int AM_SetHandlerAny(ea_t ea, handler_t *handler, void (*function)())
{
  int i = 0;
  
  if ((ea == NULL) || (handler == NULL)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  while (i < ea->num_handlers) {
    if (ea->handler_table[i] == abort)
      break;
    else
      i++;
  }    
  if (i != ea->num_handlers) {
    *handler = i;
    ea->handler_table[i] = function;
    UnlockEP(ea);
    return(AM_OK);
  }
  else {
    UnlockEP(ea);
    return(AM_ERR_RESOURCE);
  }
}

int AM_GetNumHandlers(ea_t ea, int *n_handlers)
{
  if ((ea == NULL) || (n_handlers == NULL)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  *n_handlers = ea->num_handlers;
  UnlockEP(ea);
  return(AM_OK);
}

int AM_SetNumHandlers(ea_t ea, int n_handlers)
{
  int i;

  if ((ea == NULL) || (n_handlers < 0) || (n_handlers > AM_MaxNumHandlers()))
    return(AM_ERR_BAD_ARG);

  LockEP(ea);
  if (n_handlers > ea->num_handlers) {
    if ((ea->handler_table = (func *)realloc(ea->handler_table,
         sizeof(n_handlers*sizeof(func)))) == NULL) {
      perror("malloc error\n");
      exit(-1);
    }
    for (i = ea->num_handlers; i < n_handlers; i++) 
      ea->handler_table[i] = abort;
  }
  else { 
    for (i = n_handlers; i < ea->num_handlers; i++) 
      ea->handler_table[i] = abort ;
    if ((ea->handler_table = (func *)realloc(ea->handler_table,
             n_handlers*sizeof(func))) == NULL) {
      perror("realloc error\n");
      exit(-1);
    }
  }
  ea->num_handlers = n_handlers;
  UnlockEP(ea);
  return(AM_OK);
}

int AM_MaxNumHandlers()
{
  return(UDPAM_MAX_NUMBER_OF_HANDLERS);
}

int AM_SetSeg(ea_t ea, void *base_address, int nbytes)
{
  if ((ea == NULL) || (nbytes < 0)) 
    return(AM_ERR_BAD_ARG);
  LockEP(ea);
  ea->start_addr = base_address;
  ea->length = nbytes;
  UnlockEP(ea); 
  return(AM_OK);    
}

int AM_GetSeg(ea_t ea, void *base_address, int *nbytes) 
{
  if ((ea == NULL) || (nbytes == NULL))
    return(AM_ERR_BAD_ARG);
  *((caddr_t *)base_address) = ea->start_addr;
  *nbytes = ea->length;
  return(AM_OK);
}

int AM_MaxSegLength(int *nbytes)
{
  if (nbytes == NULL) 
    return(AM_ERR_BAD_ARG);
  *nbytes = UDPAM_MAX_SEG_LENGTH;
  return(AM_OK);  
}

int AM_GetEventMask(eb_t eb)
{
  if (eb == NULL) 
    return(AM_ERR_BAD_ARG);
  return(eb->mask);
}

/* 
 * No support for AM_MSGRECEIVED
 */
int AM_SetEventMask(eb_t eb, int mask)
{
  if ((eb == NULL) || ((mask != AM_NOEVENTS) && (mask != AM_EMPTYTONOT))) 
    return(AM_ERR_BAD_ARG);
  LockBundle(eb);
  eb->mask = mask; 
  UnlockBundle(eb);
  return(AM_OK);
}

/* 
 * If the bundle's event mask is set to AM_EMPTYTONOT, wait until a
 * message arrives in one of the bundle's endpoints' receive pools.
 * Otherwise, return.  We do not deal with AM_MSGRECEIVED because
 * a) it's not implementable in UDP, and b) does anyone really 
 * case?  It seems like AM_EMPTYTONOT can give applications the
 * same kind of functionality.
 */
int AM_WaitSema(eb_t eb)
{
  fd_set temp_fdset;

  if (eb->mask & AM_EMPTYTONOT) {
    memcpy(&temp_fdset, &(eb->fdset), sizeof(temp_fdset));
    if (select(eb->maxfd + 1, &temp_fdset, (fd_set *)NULL, (fd_set *)NULL, 
	       NULL) < 0) {
      perror("select error\n");
      exit(-1);
    }
  }
  return(AM_OK);
}

int AM_PostSvar(eb_t eb)
{
  return(AM_OK);  
}

static
void BindAndSetKnownSocket(int sd, int port)
{
  int                arg = 1, h_errno;
  int                opt = SB_MAX;
  int                opt_len = sizeof(opt);
  struct sockaddr_in addr;
  struct hostent     *host;
  char               **p, hostname[256]; /* MAX_HOSTNAME_LEN? */
  char               buffer[256];        /* Large enough to hold result */
  int                bufferlen = 256;
 
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);    /* System specified UDP port */
  if ((host = (struct hostent *)malloc(sizeof(struct hostent))) == NULL) {
    perror("malloc error\n");
    exit(-1);
  }
  if (gethostname(hostname, 256*sizeof(char)) < 0) {
    perror("gethostbyname error\n");
    exit(-1);
  }
  if ((host = gethostbyname_r(hostname, host, buffer, bufferlen,
                              &h_errno)) == NULL) {
    perror("gethostbyname_r error\n");
    exit(-1);
  }
  p = host->h_addr_list;
  if (*p != 0)
    (void)memcpy(&addr.sin_addr.s_addr, *p, sizeof(addr.sin_addr.s_addr));
  if (bind(sd, (struct sockaddr *)(&addr), sizeof(addr)) < 0) {
    perror("bind error\n");
    exit(-1);
  }
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (void *)(&opt), opt_len) < 0) {
    perror("setsockopt error\n");
    exit(-1);
  }
  if (setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (void *)&opt, opt_len) < 0) {
    perror("setsockopt error\n");
    exit(-1);
  }
  if (setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (void *)&opt, opt_len) < 0) {
    perror("setsockopt error\n");
    exit(-1);
  }
  if (ioctl(sd, FIONBIO, &arg) < 0) {
    perror("ioctl error\n");
    exit(-1);
  }
}

int AM_AllocateKnownEndpoint(eb_t bundle, ea_t *endp, int port)
{
  struct ep_elem *new_elem;
  int            i, size = sizeof(struct sockaddr_in);
 
  if (((*endp) = (ea_t)calloc(1, sizeof(struct _ea_t))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  /* Initialize TxPool */
  if (((*endp)->txpool = calloc(UDPAM_DEFAULT_TXBUFS, sizeof(UDPAM_Buf)))
      == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  (*endp)->num_txbufs = UDPAM_DEFAULT_TXBUFS;
  /* Initialize TxFree FIFO */
  if (((*endp)->txfree.pointers = (UDPAM_Buf **)calloc(UDPAM_DEFAULT_TXBUFS,
                                   sizeof(UDPAM_Buf *))) ==  NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  (*endp)->txfree.head = 0;
  (*endp)->txfree.tail = UDPAM_DEFAULT_TXBUFS - 1;
  (*endp)->txfree.num_elements = UDPAM_DEFAULT_TXBUFS;
  /* Initialize TxTimeout */
  if (((*endp)->txtimeout.unackmessages = (struct timeout_elem *)
       calloc(UDPAM_DEFAULT_TXBUFS, sizeof(struct timeout_elem))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  for (i = 0; i < UDPAM_DEFAULT_TXBUFS; i++) {
    (*endp)->txpool[i].buf_id = i;                       /* TxPool */
    (*endp)->txfree.pointers[i] = &((*endp)->txpool[i]); /* TxFree */
    (*endp)->txtimeout.unackmessages[i].message = (UDPAM_Buf *)NULL;
    (*endp)->txtimeout.unackmessages[i].prev = (struct timeout_elem *)NULL;
    (*endp)->txtimeout.unackmessages[i].next = (struct timeout_elem *)NULL;
  }
  (*endp)->txtimeout.head = (struct timeout_elem *)NULL;
  (*endp)->txtimeout.tail = (struct timeout_elem *)NULL;
  (*endp)->txtimeout.num_elements = 0;
  /* Initialize Translation/Handler tables, etc. */
  if (((*endp)->translation_table = (struct translation *)
       calloc(AM_MIN_NUMBER_OF_TRANSLATIONS, sizeof(struct translation)))
      == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  for (i = 0; i < AM_MIN_NUMBER_OF_TRANSLATIONS; i++) {
    (*endp)->translation_table[i].tag = AM_NONE;
  }
  (*endp)->num_translations = AM_MIN_NUMBER_OF_TRANSLATIONS;
 
  if (((*endp)->handler_table = (func *)calloc(AM_MIN_NUMBER_OF_HANDLERS,
                                               sizeof(func))) == NULL) {
    perror("calloc error\n");
    exit(-1);
  }
  for (i = 0; i < AM_MIN_NUMBER_OF_HANDLERS; i++)
    (*endp)->handler_table[i] = abort;
  (*endp)->num_handlers = AM_MIN_NUMBER_OF_HANDLERS;
  (*endp)->start_addr = (void *)0;
  (*endp)->length = 0;
  (*endp)->tag = AM_NONE;
#ifdef SOLARIS
 if (mutex_init(&((*endp)->lock), USYNC_THREAD, NULL) != 0) {
   perror("mutex_init error\n");
   exit(-1);
 }
#endif /* SOLARIS */
  if (((*endp)->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket error\n");
    exit(-1);
  }
  if (getsockname((*endp)->socket, (struct sockaddr *)&((*endp)->sockaddr),
                  &size) < 0) {
    perror("getsockname error\n");
    exit(-1);
  }
  BindAndSetKnownSocket((*endp)->socket, port);
  if ((new_elem = (struct ep_elem *)malloc(sizeof(struct ep_elem))) == NULL) {
    perror("malloc error\n");
    exit(-1);
  }
  LockBundle(bundle);
  /* Add endpoint to bundle's list of endpoints */
  if (bundle->num_eps == 0) {
    new_elem->prev = (struct ep_elem *)NULL;
    new_elem->endpoint = (*endp);
    new_elem->next = (struct ep_elem *)NULL;
  }
  else {
    bundle->head->prev = new_elem;
    new_elem->prev = (struct ep_elem *)NULL;
    new_elem->endpoint = (*endp);
    new_elem->next = bundle->head;
  }
  bundle->head = new_elem;
  bundle->num_eps++;
  FD_SET((*endp)->socket, &(bundle->fdset));
  if ((*endp)->socket > bundle->maxfd)
    bundle->maxfd = (*endp)->socket;
  UnlockBundle(bundle);
  (*endp)->bundle = bundle;
  return(AM_OK);
}

