#if !defined(_TOPCB_H_)
#define _TOPCB_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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
*/

typedef TOPStatus (TOPCallbackFunc)( TOPPtr cbClosure, TOPPtr userData);
typedef struct _TOPCallback	TOPCallback,  *TOPCallbackHandle;
typedef struct _TOPCallbacks	TOPCallbacks, *TOPCallbacksHandle;

#define TOP_CB_OK	0
#define TOP_CB_REMOVE	1

/*
 *	topCB.c 
 */
extern TOPCallbacks*	topCallbacksCreate( char *listName);
extern void		topCallbacksDestroy( TOPCallbacks *pCBS);
extern TOPCallback*	topCallbacksFind( TOPCallbacks *pCBS, 
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPCallback*	topCallbacksAdd( TOPCallbacks *pCBS, 
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topCallbacksRemove( TOPCallbacks *pCBS,
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topCallbacksCall( TOPCallbacks *pCBS, TOPPtr userData);
/*
 *	topIdleCB.c
 */
extern void		topIdleCallbacksInit();
extern void		topIdleCallbacksUninit();
extern TOPCallback*	topIdleCallbacksAdd( TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topIdleCallbacksRemove( 
			  TOPCallbackFunc *cbF, TOPPtr cbC);
extern TOPStatus	topIdleCallbacksCall( TOPPtr userData);
#endif /* _TOPCB_H_ */
