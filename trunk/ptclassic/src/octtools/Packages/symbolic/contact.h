/* Version Identification:
 * $Id$
 */
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
 * Contact Maintenence
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * Exported definitions from contact.c.
 */

#ifndef _CONTACT_H_
#define _CONTACT_H_

#include "ansi.h"
#include "oct.h"
#include "tap.h"

extern int symContactP
  ARGS((octObject *obj));
  /* Returns a non-zero status if `obj' is a connector or connector terminal */

#define SYM_MAX_MSG	2048

typedef struct sym_con_func_defn {
    int (*func)			/* Function to call            */
      ARGS((int nLayers, tapLayerListElement *layerList,
	    octObject *instance, char why[SYM_MAX_MSG],
	    char *data));
    char *data;			/* User data passed to function */
} symConFunc;

extern symConFunc *symDefConFunc;
   /* Exported default connector function */

extern void symAddToConnectorsBag
  ARGS((octObject *connector));
  /* Adds an instance to the connectors bag for the facet */

extern void symConnector
  ARGS((octObject *parent, octCoord x, octCoord y,
	int ne, tapLayerListElement elems[], symConFunc *confun,
	octObject *connector));
  /* Makes a new connector */

extern int symUpgrade
  ARGS((octObject *term, tapLayerListElement *elem, symConFunc *confun,
	octObject *new_conn));
  /* Examines connector terminal for possible upgrade */

#endif /* _CONTACT_H_ */
