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

/* stolen from sys/types.h on 4.3 BSD */

#include "port.h"

#define	RPC_NBBY	8		/* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * RPC_FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef	RPC_FD_SETSIZE
#define	RPC_FD_SETSIZE	256
#endif /* RPC_FD_SETSIZE */

typedef long	rpc_fd_mask;
#define RPC_NFDBITS	(sizeof(rpc_fd_mask) * RPC_NBBY)	/* bits per mask */
#ifndef rpc_howmany
#define	rpc_howmany(x, y)	(((x)+((y)-1))/(y))
#endif /* rpc_howmany */

typedef	struct rpc_fd_set {
	rpc_fd_mask	fds_bits[rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS)];
} rpc_fd_set;

#define	RPC_FD_SET(n, p)	((p)->fds_bits[(n)/RPC_NFDBITS] |= (1 << ((n) % RPC_NFDBITS)))
#define	RPC_FD_CLR(n, p)	((p)->fds_bits[(n)/RPC_NFDBITS] &= ~(1 << ((n) % RPC_NFDBITS)))
#define	RPC_FD_ISSET(n, p)	((p)->fds_bits[(n)/RPC_NFDBITS] & (1 << ((n) % RPC_NFDBITS)))

#ifdef SYSV
#define RPC_FD_ZERO(p)	memset((char *)(p), 0, sizeof(*(p)))
#else
#define RPC_FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif

extern void RPC_FD_AND();
extern void RPC_FD_OR();
extern void RPC_FD_OR_3();
extern void RPC_FD_NOT();
extern int RPC_FD_ALL_ZERO();
extern int RPC_FD_FIRST_SET();
extern int RPC_FD_FIRST_SET_AND();
extern int RPC_FD_ANY_COMMON_BITS();
