#ifndef _pthreadsCompat_h
#define _pthreadsCompat_h 1
/**************************************************************************
Version identification:
$Id$
Author: Christopher Hylands

Copyright (c) 1996-%Q% The Regents of the University of California.
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


/* This #ifdef block is repeated in src/octtools/Xpackages/rpc/rpc.h
 * because we can't include rpc.h here, as lots of stars include files
 * from pigilib
 */
#if defined(PTHPUX10) && !defined(PTNO_THREADS)
/* Under HPUX10 with DCE,
 * /opt/dce/newconfig/RelNotes/HPDCE1.3.1RelNotes.txt says:
 *
 *         Source code that is built into applications that use the CDS,
 *	   RPC, or security APIs must include <pthread.h> .  This is
 *	   necessary because the DCE RPC runtime library creates a small
 *	   number of private threads, on both the client and server
 *	   sides of an application.
 *  
 * If you don't have DCE, add -DPTNO_THREADS to ARCHFLAGS in config-hppa.mk
 */
#include <pthread.h>
#endif

#endif /* _pthreadsCompat_h */
