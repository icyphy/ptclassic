/* Version Identification:
 * $Id$
 */
/*
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

#ifndef RPCAPPLICATION_H
#define RPCAPPLICATION_H

/*
 * Remote Procedure Call Package for VEM (RPC)
 *
 * Copyright (c) 1986, 1987, 1988, Rick L Spickelmier.  All rights reserved.
 *
 * Use and copying of this software and preparation of derivative works
 * based upon this software are permitted.  However, any distribution of
 * this software or derivative works must include the above copyright
 * notice.
 *
 * This software is made available AS IS, and neither the Electronics
 * Research Laboratory or the University of California make any
 * warranty about the software, its performance or its conformity to
 * any specification.
 *
 * Suggestions, comments, or improvements are welcome and should be
 * addressed to:
 *
 *   Rick L Spickelmier
 *   Electronics Research Laboratory
 *   Cory Hall
 *   University of California
 *   Berkeley, CA   94720
 *
 *   rpc@eros.Berkeley.EDU  (ARPANET)
 *   ..!ucbvax!eros!rpc        (UUCP)
 *
 */

/*
 * internal header file for the RPC application library
 *
 * not to be used by mere mortals (i.e. users)
 *
 * Rick L Spickelmier, 11/11/86
 *
 */

#define RPCLIBRARY

#define RPCLOCATION "application"

#define RPC_STDERR mktemp("/tmp/stderr.XXXXXX")

#include "rpc.h"
#include "rpcInternal.h"
#include "st.h"

extern STREAM RPCSendStream;
extern STREAM RPCReceiveStream;
extern char *RPCHostName;
extern char *RPCProtocol;
extern st_table *RPCDemonTable;

/*
 * remote user selection
 *
 *   user calls RPCUserIO to set up this structure and if any of these
 *   masks are selected on, the Function is called
 */

struct RPCUser {
    long ReadMask;			/* user read mask */
    long WriteMask;			/* user write mask */
    long ExceptionMask;			/* user exception mask */
    void (*Function)();			/* user function to call */
};

extern struct RPCUser RPCUserSelection;

/* handle connection/network operations */
extern rpcStatus	RPCEnd();
extern rpcStatus	RPCConnectToServer();
extern rpcStatus	RPCApplicationProcessEvents();

#endif
