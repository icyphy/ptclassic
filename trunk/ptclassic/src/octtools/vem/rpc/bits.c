#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"
#include "bits.h"

void
RPC_FD_AND(a, b, c)
rpc_fd_set *a, *b, *c;
{
    int i;

    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        c->fds_bits[i] = a->fds_bits[i] & b->fds_bits[i];
    }
}


void
RPC_FD_OR(a, b, c)
rpc_fd_set *a, *b, *c;
{
    int i;

    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        c->fds_bits[i] = a->fds_bits[i] | b->fds_bits[i];
    }
}


void
RPC_FD_OR_3(a, b, c, d)
rpc_fd_set *a, *b, *c, *d;
{
    int i;

    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        d->fds_bits[i] = a->fds_bits[i] | b->fds_bits[i] | c->fds_bits[i];
    }
}


void
RPC_FD_NOT(a, c)
rpc_fd_set *a, *c;
{
    int i;

    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        c->fds_bits[i] = ~a->fds_bits[i];
    }
}


int
RPC_FD_FIRST_SET(a)
rpc_fd_set *a;
{
    int i;
    int set;

    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        set = ffs(a->fds_bits[i]);
        if (set > 0) {
            return(set + i * sizeof(rpc_fd_mask));
        }
    }

    return(0);
}


int
RPC_FD_ALL_ZERO(a)
rpc_fd_set *a;
{
    int i;
    
    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        if (a->fds_bits[i] != 0) {
            return(0);
        }
    }

    return(1);
}


/* another in a series of seriously horrible kludges */
int
RPC_FD_ANY_COMMON_BITS(a, b)
rpc_fd_set *a, *b;
{
    int i;

    
    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
	if (a->fds_bits[i] & b->fds_bits[i]) {
	    return(1);
	}
    }
    return(0);
}


/* yes, another one */
int
RPC_FD_FIRST_SET_AND(a, b)
rpc_fd_set *a, *b;
{
    int set, i;
    
      
    for (i = 0; i < rpc_howmany(RPC_FD_SETSIZE, RPC_NFDBITS); i++) {
        set = ffs(a->fds_bits[i] & b->fds_bits[i]);
        if (set > 0) {
            return(set + i * sizeof(rpc_fd_mask));
        }
    }

    return(0);
}
