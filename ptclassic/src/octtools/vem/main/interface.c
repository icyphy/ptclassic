#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/*
 * Create command for symbolic
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988, 1990
 *
 * This file dispatches to the appropriate create commands based on
 * the argument types found on the arg list.
 *
 * Work to be done:
 *  - becomes part of the vem specific symbolic commands
 *  - set up warning and merge functions
 */

#include "general.h"
#include "message.h"
#include "vemUtil.h"
#include "vulcan.h"
#include "oh.h"			/* For ohFormatName */


static void handler( type, msg )
    vulcanMessageType type;
    char* msg;
{
    char* head = "MSG";
    switch ( type ) {
    case VULCAN_INFORMATION:
	head = "INFO";
	break;
    case VULCAN_WARNING:
	head = "WARN";
	break;
    case VULCAN_SEVERE:
	head = "ERR";
	break;
    case VULCAN_PARTIAL:
	head = "PARTIAL";
	break;
    case VULCAN_DEBUGGING:
	head = "DEBUG";
	break;
    case VULCAN_FATAL:
	head = "FATAL";
	break;
    }
    vemMsg( MSG_C, "%s: %s", head, msg );
}

vemStatus interfaceCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
{
    octObject contents, interface;

    contents.objectId = spot->theFct;
    octGetById( &contents );
    interface = contents;
    interface.contents.facet.facet = "interface";
    interface.contents.facet.mode = "a";
    
    octOpenFacet( &interface );
    vemMsg( MSG_C, "Vulcan %s\n", ohFormatName( &contents ) );

    vulcanMessageHandler( handler );

    vulcan( &contents, &interface );

    return VEM_OK;
}


