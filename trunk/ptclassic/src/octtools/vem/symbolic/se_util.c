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
/*
 * Symbolic Editing Utilities
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains useful utilities used by the symbolic
 * editing commands.
 *
 * Should add highlighting to dialogs.
 */

#include "general.h"
#include "symbolic.h"
#include "se.h"
#include "da.h"
#include "oh.h"
#include "region.h"
#include "vemDM.h"
#include "vemUtil.h"
#include "windows.h"

static octObject *seResolveTerms
  ARGS((octObject *fct, int num_terms, octObject *aterms, char *help));
static int seg_suitable
  ARGS((tapLayerListElement *elem, octObject *path, symConFunc *confun));
static int isect
  ARGS((struct octPoint *p11, struct octPoint *p12,
	struct octPoint *p21, struct octPoint *p22,
	struct octPoint *result));
static void do_parallel
  ARGS((struct octPoint *mod1, struct octPoint *mod2,
	struct octPoint *p1, struct octPoint *p2,
	struct octPoint *ip));
static void mod_points
  ARGS((struct octPoint *p1, struct octPoint *p2,
	struct octPoint *new_p1));



int seFindActual(fct, win, pnt, term)
octObject *fct;			/* What facet to search */
Window win;			/* What window          */
struct octPoint *pnt;		/* Location in oct      */
octObject *term;		/* Terminal (returned)  */
/*
 * This routine searches in `fct' for an actual terminal located
 * at `pnt' in `win'.  If gravity is active, it will expand the search
 * region by the gravity amount.  If more than one terminal is
 * found, the user will be queried to choose one.  If one is
 * found, it will be returned in `term' along with its implementation
 * bounding box.  If no terminal is found, it will return zero.
 */
{
    struct octBox extent;
    octObject *aterms, *one_term;
    int num_terms;
    static char *help_buf =
"The new formal terminal you requested\n\
has several possible implementations.\n\
Please choose the actual terminal you\n\
wish to use to implement the terminal.";

    if (!win || (wnGravSize(win, pnt, &extent) != VEM_OK)) {
	extent.lowerLeft = extent.upperRight = *pnt;
    }

    term->objectId = oct_null_id;
    num_terms = seActualSearch(fct, &extent, (int (*)()) 0, (char *) 0, &aterms);
    if (num_terms > NONE_FOUND) {
	/* We found some */
	if (num_terms > 1) {
	    if ( (one_term =
		  seResolveTerms(fct, num_terms, aterms, help_buf)) ) {
		*term = *one_term;
	    }
	} else {
	    *term = aterms[0];
	}
    } else return 0;
    VEMFREE(aterms);
    if (octIdIsNull(term->objectId)) return 0;
    else return 1;
}



static octObject *seResolveTerms(fct, num_terms, aterms, help)
octObject *fct;			/* Facet                */
int num_terms;			/* Number of terminals  */
octObject *aterms;		/* Terminals themselves */
char *help;			/* Help message if any  */
/*
 * Posts a dialog asking the user to choose exactly one
 * of the terminals listed in `terms'.
 */
{
    octObject inst;
    octObject *result = (octObject *) 0;
    dmWhichItem *items;
    int i;
    int ri = 0;			/* Selected term (default is going to be the first) */
    char buf[VEMMAXSTR];

    items = VEMARRAYALLOC(dmWhichItem, num_terms);
    for (i = 0;  i < num_terms;  i++) {
	inst.objectId = aterms[i].contents.term.instanceId;
	VEM_OCTCKRVAL(octGetById(&inst), (octObject *) 0);
	(void) sprintf(buf, "`%s' of `%s:%s'",
		       aterms[i].contents.term.name,
		       inst.contents.instance.master,
		       inst.contents.instance.view);
	items[i].itemName = VEMSTRCOPY(buf);
    }
    if (dmWhichOne("Which Terminal?", num_terms, items,
		   &ri, (int (*)()) 0, help) == VEM_OK)
      {
	  result = aterms+ri;
      }
    for (i = 0;  i < num_terms;  i++) VEMFREE(items[i].itemName);
    VEMFREE(items);
    return result;
}


int seConnectP(term, data)
octObject *term;		/* Terminal to examine */
char *data;			/* User data (ti *)    */
/*
 * Returns a non-zero result if `term' has an implementation
 * on the layer given in `data' and the implementation
 * is at least as large on a side as given in `data'.
 * It is also valid if it is a connector terminal that
 * can be upgraded appropriately.
 */
{
    seLwInfo *info = (seLwInfo *) data;
    octObject new_conn;

    if (symSegConnP(term, info->elem)) {
	return 1; 
    } else if (symContactP(term)) {
	/* 
	 * May still be a possibility if its a connector term and it
	 * may be upgraded safely.
	 */
	return symUpgrade(term, info->elem, info->confun, &new_conn);
    } else {
	return 0;
    }
}

int seSegActual(fct, win, pnt, elem, confun, term)
octObject *fct;			/* What facet to search */
Window win;			/* What window          */
struct octPoint *pnt;		/* Location in oct      */
tapLayerListElement *elem;	/* Segment info         */
symConFunc *confun;		/* Connector function   */
octObject *term;		/* Terminal (returned)  */
/*
 * This routine looks for an actual terminal at `pnt' in `win'
 * that has an implementation that allows connection to
 * a segment whose description is `elem'.  It will use gravity 
 * to increase the search if gravity is on.  If more than one 
 * terminal meets the conditions, it will ask the user to choose 
 * one using a dialog box.  The terminal and its implementation 
 * bounding box will be returned in `term' and `bb'.  The routine
 * returns zero if no appropriate terminals were found.
 * The routine returns a positive number if sucessful,
 * zero if it couldn't find any terminals, and INAPPROPRIATE
 * if it couldn't find any appropriate terminals.
 */
{
    struct octBox extent;
    octObject *aterms, *reg;
    int num;
    seLwInfo info;
    static char *help_buf =
"The new segment you requested can be connected\n\
to several possible terminals.  Please choose the\n\
terminal you would like to use as the connection\n\
endpoint for the segment.";

    if (!win || (wnGravSize(win, pnt, &extent) != VEM_OK)) {
	extent.lowerLeft = extent.upperRight = *pnt;
    }

    info.elem = elem;  info.confun = confun;
    num = seActualSearch(fct, &extent, seConnectP, (char *) &info, &aterms);
    term->objectId = oct_null_id;
    if (num > 0) {
	if (num > 1) {
	    if ( (reg = seResolveTerms(fct, num, aterms, help_buf)) ) {
		*term = *reg;
	    } else {
		num = 0;
	    }
	} else {
	    *term = aterms[0];
	}
    }
    return num;
}



int seActualSearch(fct, extent, pred_func, data, aterms)
octObject *fct;			/* What facet to search */
struct octBox *extent;		/* Area to search       */
int (*pred_func)();		/* Predicate function   */
char *data;			/* Data to pred_func    */
octObject **aterms;		/* Terminals (returned) */
/*
 * This routine searches in `fct' for an actual terminal located
 * at `spot'.  If gravity is active, it will expand the search
 * region by the gravity amount.  The routine will pass each
 * terminal found to `pred_func':
 *   int pred_func(term, data)
 *   octObject *term;
 *   char *data;
 * If the routine returns a non-zero value, it will be included
 * in the list of terminals returned.  After looking for 
 * the terminals, it will return the number of terminals found
 * or these two error conditions:
 *   NONE_FOUND:	No terminals found
 *   INAPPROPRIATE:	No terminals meeting `pred_func' were found
 *
 * The routine returns a locally allocated array of terminals that
 * should be freed after use.
 */
{
    int alloc, idx, rejected;
    symAreaGen gen;

    alloc = 5;
    rejected = idx = 0;
    *aterms = VEMARRAYALLOC(octObject, alloc);
    symAreaInit(fct, OCT_TERM_MASK, extent, &gen);
    while (symAreaNext(gen, *aterms+idx)) {
	if (!pred_func || ((*pred_func)(*aterms+idx, data))) {
	    idx++;
	    if (idx >= alloc) {
		alloc *= 2;
		*aterms = VEMREALLOC(octObject, *aterms, alloc);
	    }
	} else {
	    rejected++;
	}
    }
    symAreaEnd(gen);
    if (idx > 0) {
	return idx;
    } else if (rejected) {
	return INAPPROPRIATE;
    } else {
	return NONE_FOUND;
    }
}



#define DEF_NUM	5

static void free_items(ptr)
char *ptr;
{
    dmWhichItem *item = (dmWhichItem *) ptr;

    VEMFREE(item->itemName);
    VEMFREE(item->userData);
}

static char *build_name(seg)
octObject *seg;			/* Segment */
/*
 * Comes up with a useful name for a segment.  Could probably be
 * even more useful.
 */
{
    char buf[1024];
    octObject lyr, net;

    lyr.objectId = oct_null_id;
    (void) octGenFirstContainer(seg, OCT_LAYER_MASK, &lyr);
    net.objectId = oct_null_id;
    (void) octGenFirstContainer(seg, OCT_NET_MASK, &net);
    (void) strcpy(buf, "Segment");
    if (octIdIsNull(lyr.objectId)) {
	(void) strcat(buf, " (no layer)");
    } else {
	(void) strcat(buf, " on ");
	(void) strcat(buf, lyr.contents.layer.name);
    }
    if (octIdIsNull(net.objectId)) {
	(void) strcat(buf, " (no net)");
    } else {
	if (net.contents.net.name && strlen(net.contents.net.name)) {
	    (void) strcat(buf, " net ");
	    (void) strcat(buf, net.contents.net.name);
	} else {
	    (void) strcat(buf, " auto net");
	}
    }
    return VEMSTRCOPY(buf);
}

int seg_reallyMeetsPoint( path, extent )
    octObject *path;
    octBox *extent;
/*
 * This routine determines if the path does in fact meet the 
 * point contained inside the 'extent' box.
 * this is a problem mostly for non-mahattan paths, because
 * the simple minded bounding box test is not adequate.
 * If the path is manhattan, it is assumed that the answer is positive.
 */
{
    int32 num;
    struct octPoint *points;
    octCoord dx, dy;

    ohGetPoints( path, &num, &points );
    dx = points[1].x - points[0].x;
    dy = points[1].y - points[0].y;
    
    if ( dx == 0 || dy == 0 ) return 1; /* Path is manhattan, the box is assumed to meet it. */

    {
	struct octPoint c;	/* Center of the box. */
	double m,q;		/* Coefficients to represent line of path y=mx+q */
	double d;		/* Distance of center from straight line. */
	int thresh;		/* Threshold */

	ohBoxCenter( extent, &c );
	
	m = (double)dy/(double)dx; 		/* Angular coeff of the path */
	q = points[0].y - m * points[0].x;      /*  */

	d = (m*c.x - c.y + q ) / sqrt( m*m+1 );
	if ( d < 0.0 ) d = -d;
	thresh = path->contents.path.width + ohBoxWidth( extent ) / 2 ;
	/* printf( "Distance = %lf Threshold = %d\n", d , thresh ); */
	return ( d <= thresh ) ;
    }
    /* NOTREACHED */
}



int seFindSeg(fct, win, pnt, elem, confun, seg, sp)
octObject *fct;			/* What facet to search in */
Window win;			/* Window (for gravity)    */
struct octPoint *pnt;		/* Point to search under   */
tapLayerListElement *elem;	/* Segment description     */
symConFunc *confun;		/* Connector function      */
octObject *seg;			/* Chosen segment (if any) */
struct octPoint sp[2];		/* Segment points          */
/*
 * This routine looks for suitable segments under `pnt' that
 * can connect to a segment described by `elem'.  A suitable
 * segment is one where a connector could be placed to connect
 * the segment with the described segment.  The search area
 * will be expanded by the gravity amount.  If there is more
 * than one suitable segment, a dialog will appear asking the
 * user to choose one.  If exactly one is chosen, the routine
 * will return a non-zero status and fill in the chosen segment
 * in `seg' and place the segment points in `sp'.
 */
{
    struct octBox extent;
    regObjGen gen;
    octObject path, *cp;
    dary items;
    int result = 0;
    int32 np;
    dmWhichItem one_item;
    static char *seg_help_buf =
"There are several possible segments that can be\n\
broken to connect with the new segment.  Please\n\
choose the one you would like to connect to.";

    /* Expand by gravity amount */
    if (wnGravSize(win, pnt, &extent) != VEM_OK) {
	extent.lowerLeft = extent.upperRight = *pnt;
    }

    items = daAlloc(dmWhichItem, DEF_NUM);
    if (regObjStart(fct, &extent, OCT_PATH_MASK, &gen, 0) == REG_OK) {
	while (regObjNext(gen, &path) == REG_OK) {
	    if (! seg_reallyMeetsPoint( &path, &extent ) ) continue;
	    if (seg_suitable(elem, &path, confun)) {
		/* Add to list of segments */
		one_item.itemName = build_name(&path);
		cp = VEMALLOC(octObject);
		*cp = path;
		one_item.userData = (Pointer) cp;
		one_item.flag = 0;
		*daSetLast(dmWhichItem, items) = one_item;
	    }
	}
	seg->objectId = oct_null_id;
	if (daLen(items) > 0) {
	    if (daLen(items) > 1) {
		/* Ask user to resolve */
		if (dmWhichOne("Which Segment", daLen(items),
			       daData(dmWhichItem, items), &result,
			       (int (*)()) 0, seg_help_buf) == VEM_OK) {
		    /* Choice made */
		    *seg = *((octObject *) daGet(dmWhichItem, items, result)->userData);
		}
	    } else {
		/* Only one choice */
		*seg = *((octObject *) daGet(dmWhichItem, items, 0)->userData);
	    }
	}
	/* Free array */
	daFree(items, free_items);
	if (octIdIsNull(seg->objectId)) {
	    return 0;
	} else {
	    np = 2;
	    (void) octGetPoints(seg, &np, sp);
	    return 1;
	}
    } else {
	/* No segments found */
	return 0;
    }
}


static int seg_suitable(elem, path, confun)
tapLayerListElement *elem;	/* New segment        */
octObject *path;		/* Old segment        */
symConFunc *confun;		/* Connector function */
/*
 * Returns a non-zero value if the new segment described by
 * `elem' can be connected to the existing segment `path'.
 * Right now, this means a connector must exist for 
 * connecting the segments.  Might later add that they
 * must be orthogonal.
 */
{
    tapLayerListElement full_desc[2];
    octObject new_conn;
    char message[SYM_MAX_MSG];

    symSegDesc(path, (struct octPoint *) 0, &(full_desc[0]));
    full_desc[1] = *elem;
    return (*confun->func)(2, full_desc, &new_conn, message, confun->data);
}



void seBreakSeg(fct, seg, sp, elem, p1, p2, confun, term)
octObject *fct;			/* Facet for break    */
octObject *seg;			/* Segment to break   */
struct octPoint sp[2];		/* Segment points     */
tapLayerListElement *elem;	/* New segment        */
struct octPoint *p1, *p2;	/* New segment pnts   */
symConFunc *confun;		/* Connector function */
octObject *term;		/* Final terminal     */
/*
 * This routine breaks the segment `seg' to allow the
 * segment described by `elem' to connect with it.  It
 * determines the intersection point and places a suitable
 * connector at that point then invokes symReconnect()
 * to actually cause the breakage.  The end points will
 * also be modified to gravitate to the intersection
 * point.  Finally, the new connector terminal will
 * be returned in `term' to be used with symSegments().
 */
{
    struct octPoint i_pnt;
    tapLayerListElement full_desc[3];
    octObject conn;

    if (!isect(p1, p2, sp, sp+1, &i_pnt)) {
	do_parallel(p1, p2, sp, sp+1, &i_pnt);
    }
    mod_points(p1, p2, &i_pnt);

    /* Now the connector */
    symSegDesc(seg, &i_pnt, &(full_desc[0]));
    full_desc[1] = full_desc[0];
    full_desc[2] = *elem;
    symConnector(fct, i_pnt.x, i_pnt.y, 3, full_desc, confun, &conn);

    /* This will break the overlapping segment */
    symReconnect(&conn, confun, (symDelItemFunc *) 0);

    /* Return the terminal */
    (void) octGenFirstContent(&conn, OCT_TERM_MASK, term);
}



#define RD(val) (((val) < 0.0) ? ((int) (val)-0.5) : ((int) (val)+0.5))
#define D(val)	((double) (val))
#define B(sl, x, y)	(D(y) - ((sl) * D(x)))
#define EQD(x, y)    (VEMABS((x)-(y)) <= ((VEMMAX(VEMABS(x),VEMABS(y)))*1e-05))

static int isect(p11, p12, p21, p22, result)
struct octPoint *p11, *p12;	/* First line  */
struct octPoint *p21, *p22;	/* Second line */
struct octPoint *result;	/* Intersection (if any) */
/*
 * Computes the intersection between the extended line between p11 and p12
 * and the segment p21 to p22.  If the lines do not intersect,  the routine
 * will return zero.  This does everything in floating point.
 */
{
    double d1x, d1y, d2x, d2y;
    double m1, m2, xval;


    d1x = p12->x - p11->x;
    d1y = p12->y - p11->y;

    d2x = p22->x - p21->x; 
    d2y = p22->y - p21->y;


    if (d1x == 0.0) {
	/* First line is vertical */
	if (d2x == 0.0) return 0;
	else {
	    result->x = p11->x;
	    result->y = RD((d2y * D(result->x - p21->x))/d2x + D(p21->y));
	    return ((d2y > 0) ?
		    (result->y >= p21->y) && (result->y <= p22->y) :
		    (result->y >= p22->y) && (result->y <= p21->y));
	}
    } else if (d2x == 0) {
	/* Second line is vertical */
	result->x = p21->x;
	result->y = RD((d1y * D(result->x - p11->x))/d1x + D(p11->y));
	return ((d2y > 0) ?
		(result->y >= p21->y) && (result->y <= p22->y) :
		(result->y >= p22->y) && (result->y <= p21->y));
    } else {
	/* Neither line is vertical */
	m1 = d1y/d1x;  m2 = d2y/d2x;
	if (EQD(m1, m2)) return 0; /* Lines are parallel. */
	xval = (B(m2, p21->x, p21->y) - B(m1, p11->x, p11->y))/(m1-m2);	/* Fixed by AC on Sept 17 1991 */
	result->y = RD(m1 * xval + (D(p11->y) - m1 * D(p11->x)));
	result->x = RD(xval);
	return ((d2y > 0) ?
		(result->y >= p21->y) && (result->y <= p22->y) :
		(result->y >= p22->y) && (result->y <= p21->y));
    }
}



static void do_parallel(mod1, mod2, p1, p2, ip)
struct octPoint *mod1, *mod2;	/* Modifiable segment */
struct octPoint *p1, *p2;	/* Existing segment   */
struct octPoint *ip;		/* Intersection       */
/*
 * The two segments are parallel.  This routine determines
 * the closest intersection between them and returns it
 * in `ip'.
 */
{
    octCoord diff_x, diff_y;

    diff_x = mod2->x - mod1->x;
    diff_y = mod2->y - mod1->y;
    if (diff_x == 0) {
	/* Vertical */
	ip->x = p1->x;
	ip->y = mod1->y;
    } else if (diff_y == 0) {
	/* Horizontal */
	ip->x = mod1->x;
	ip->y = p1->y;
    } else {
	struct octPoint other;

	/* Non-manhattan - call isect on perpendicular line */
	other.x = diff_y + mod1->x;
	other.y = -diff_x + mod1->y;
	isect(mod1, &other, p1, p2, ip);
    }
}



static void mod_points(p1, p2, new_p1)
struct octPoint *p1, *p2;	/* Modified segment */
struct octPoint *new_p1;	/* New first point  */
/*
 * Changes `p1' to `new_p1' and if the original segment
 * was manhattan, modifies `p2' along with it.
 */
{
    if ((p1->x - p2->x) == 0) {
	/* Vertical */
	p2->x = new_p1->x;
    } else if ((p1->y - p2->y) == 0) {
	/* Horizontal */
	p2->y = new_p1->y;
    }
    *p1 = *new_p1;
}


void seDeleteOne(obj)
octObject *obj;
/*
 * Deletes `obj'.  This is a convenience routine that bundles
 * `obj' into a temporary bag, deletes it using symDeleteObjects()
 * and then removes the bag.  Does not handle deleting formal
 * terminals so that display updates occur appropriately.
 */
{
    octObject fct, bag;

    octGetFacet(obj, &fct);
    bag.type = OCT_BAG;
    bag.contents.bag.name = ";;; Deleted Objects ;;;";
    if (vuTemporary(&fct, &bag) == OCT_OK) {
	VEM_OCTCKRET(octAttach(&bag, obj));
	symDeleteObjects(&bag, (symDelItemFunc *) 0);
	VEM_OCTCKRET(octDelete(&bag));
    }
}
