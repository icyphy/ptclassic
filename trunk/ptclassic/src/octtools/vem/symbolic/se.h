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
 * Symbolic Editing Definitions
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains definitions for the commands exported
 * in the symbolic editing style.
 */

#ifndef _SE_H_
#define _SE_H_

#include "ansi.h"
#include "general.h"
#include "oct.h"
#include "tap.h"
#include "commands.h"
#include "symbolic.h"
#include "list.h"
#include "opts.h"		/* Pick up vemExpert() decl. */

extern void seInit();
  /* Symbolic policy library initialization */

extern vemStatus seCreateCmd
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seCreateFormalTerminal
  ARGS((octObject *fct, octObject *implementation,
	char *name, char *tt, char *dir));

extern vemStatus seCreateInst
  ARGS((vemPoint *spot, lsList cmdList, char *inst_fct));

extern vemStatus seCreatePath
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus sePromote
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seChangeSegs
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seReplace
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seDelete
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seSelectNet
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seMove
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seCopy
  ARGS((vemPoint *spot, lsList cmdList));

extern vemStatus seReconnect
  ARGS((vemPoint *spot, lsList cmdList));


/*
 * Some extra exports for schematic
 */


extern vemStatus seBasePath
  ARGS((octObject *fct, lsList cmdList, octObject *lyr, symConFunc *confun));

extern vemStatus seParseFormalTerminal
  ARGS((vemPoint *spot, octObject *facet, octObject *implementation,
	char *name));

extern vemStatus seBaseMove
  ARGS((vemPoint *spot, lsList cmdList, int cf));

extern vemStatus seBaseCopy
  ARGS((vemPoint *spot, lsList cmdList, int cf, symConFunc *confun));

extern vemStatus seBaseReconnect
  ARGS((vemPoint *spot, lsList cmdList, symConFunc *confun));

/*
 * Utilities
 */

extern int seFindActual
  ARGS((octObject *fct, Window win, struct octPoint *pnt, octObject *term));

#define NONE_FOUND	0
#define INAPPROPRIATE	-1

extern int seSegActual
  ARGS((octObject *fct, Window win, struct octPoint *pnt,
	tapLayerListElement *elem, symConFunc *confun,
	octObject *term));

extern int seFindSeg
  ARGS((octObject *fct, Window win, struct octPoint *pnt,
	tapLayerListElement *elem, symConFunc *confun,
	octObject *seg, struct octPoint sp[2]));

extern void seBreakSeg
  ARGS((octObject *fct, octObject *seg, struct octPoint sp[2],
	tapLayerListElement *elem,
	struct octPoint *p1, struct octPoint *p2,
	symConFunc *confun, octObject *term));

extern int seActualSearch
  ARGS((octObject *fct, struct octBox *extent,
	int (*pred_func) ARGS((octObject *term, char *data)),
	char *data, octObject **aterms));

typedef struct se_lw_info_defn {
    tapLayerListElement *elem;
    symConFunc *confun;
} seLwInfo;

extern int seConnectP
  ARGS((octObject *term, char *data));
  /* Data is actually (seLwInfo *) */

extern void seDeleteOne
  ARGS((octObject *obj));
  /* Convenience routine for deleting one object */

extern void seDelFunc
  ARGS((octObject *item, char *data));


#endif /* _SE_H_ */

