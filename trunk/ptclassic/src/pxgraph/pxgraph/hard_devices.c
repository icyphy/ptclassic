/*
 * Hardcopy Devices
 *
 * This file contains the basic output device table.  The hardcopy
 * dialog is automatically constructed from this table.
 */

#include "copyright.h"
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
