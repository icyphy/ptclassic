/*******************************************************************
SCCS version identification
$Id$

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
/*
 * Hardcopy Devices
 *
 * This file contains the basic output device table.  The hardcopy
 * dialog is automatically constructed from this table.
 */


#include "xgout.h"
#include "hard_devices.h"
#include "params.h"

extern int hpglInit();
extern int mifInit();
extern int psInit();
extern int idrawInit();

#if defined(hpux) || defined(__hpux) 
#define USE_LP
#endif

#ifdef USE_LP
/* sigh.  Not all machines have lpr, so we use lp in a pinch */
struct hard_dev hard_devices[] = {
    { "HPGL", hpglInit, "lp -d%s", "xgraph.hpgl", "paper",
	27.5, "1", 14.0, "1", 12.0, NONE },
    { "MIF", mifInit,
	"(cat > /tmp/ptxgraphpr$; export FMPRINTER; FMPRINTER=%s; fmprint /tmp/ptxgraphpr$; /bin/rm /tmp/ptxgraphpr$)", "~/xgraph.mif", "lw",
	16.0, "Helvetica", 12.0,
	"Helvetica", 10.0, NONE },
    { "Postscript", psInit, "lp -d%s", "xgraph.ps", "lw",
	19.0, "Times-Bold", 18.0, "Times-Roman", 12.0, NO },
    { "Idraw", idrawInit,
	"cat > /usr/tmp/idraw.tmp.ps; %s /usr/tmp/idraw.tmp.ps&",
	"~/.clipboard", "/usr/local/idraw", 19.0, "Times-Bold", 18.0,
	"Times-Roman", 12.0, NONE }
};
#else /* USE_LP */
struct hard_dev hard_devices[] = {
    { "HPGL", hpglInit, "lpr -P%s", "xgraph.hpgl", "paper",
	27.5, "1", 14.0, "1", 12.0, NONE },
    { "MIF", mifInit,
	"(cat > /tmp/ptxgraphpr$; export FMPRINTER; FMPRINTER=%s; fmprint /tmp/ptxgraphpr$; /bin/rm /tmp/ptxgraphpr$)", "~/xgraph.mif", "lw",
	16.0, "Helvetica", 12.0,
	"Helvetica", 10.0, NONE },
    { "Postscript", psInit, "lpr -P%s", "xgraph.ps", "lw",
	19.0, "Times-Bold", 18.0, "Times-Roman", 12.0, NO },
    { "Idraw", idrawInit,
	"cat > /usr/tmp/idraw.tmp.ps; %s /usr/tmp/idraw.tmp.ps&",
	"~/.clipboard", "/usr/local/idraw", 19.0, "Times-Bold", 18.0,
	"Times-Roman", 12.0, NONE }
};
#endif /* USE_LP */
int hard_count = sizeof(hard_devices)/sizeof(struct hard_dev);

#define CHANGE_D(name, field) \
if (param_get(name, &val)) { \
    if (val.type == DBL) { \
       hard_devices[idx].field = val.dblv.value; \
    } \
}

#define CHANGE_S(name, field) \
if (param_get(name, &val)) { \
    if (val.type == STR) { \
       (void) strcpy(hard_devices[idx].field, val.strv.value); \
    } \
}


void hard_init()
/*
 * Changes values in hard_devices structures in accordance with
 * parameters set using the parameters module.
 */
{
    char newname[1024];
    int idx;
    params val;

    for (idx = 0;  idx < hard_count;  idx++) {
	(void) sprintf(newname, "%s.Dimension", hard_devices[idx].dev_name);
	CHANGE_D(newname, dev_max_dim);
	(void) sprintf(newname, "%s.OutputTitleFont", hard_devices[idx].dev_name);
	CHANGE_S(newname, dev_title_font);
	(void) sprintf(newname, "%s.OutputTitleSize", hard_devices[idx].dev_name);
	CHANGE_D(newname, dev_title_size);
	(void) sprintf(newname, "%s.OutputAxisFont", hard_devices[idx].dev_name);
	CHANGE_S(newname, dev_axis_font);
	(void) sprintf(newname, "%s.OutputAxisSize", hard_devices[idx].dev_name);
	CHANGE_D(newname, dev_axis_size);
    }
}
