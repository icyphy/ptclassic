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
 * Formal Terminal Creation
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * The functions here implement the user interface for creating new
 * symbolic formal terminals.
 */

#include "general.h"
#include "symbolic.h"
#include "commands.h"
#include "xvals.h"
#include "vemDM.h"
#include "st.h"
#include "region.h"
#include "windows.h"
#include "vemUtil.h"
#include "message.h"
#include "defaults.h"
#include "se.h"

#define IMPL_BAG_NAME	";;; Terminal Implementations ;;;"

static int check_term_impl
  ARGS((octObject *impl, octObject *final));

static int check_previous
  ARGS((octObject *fct, char *name));

static int check_promotion
  ARGS((char *name, octObject *impl));

static int check_nets
  ARGS((octObject *impl));

static void check_type_direction
  ARGS((char *name, octObject *impl, int *set_p, char **type, char **dir));

#ifdef AVOID_DIALOGS
static void ask_term_dir
  ARGS((char *tn, int *set_p, char **type, char **dir));
#endif

static void sub_typedir
  ARGS((octObject *impl, int *set_p, char **type, char **dir));



static void seTermFunc(fterm, obj, detach_p, data)
octObject *fterm;		/* Formal terminal  */
octObject *obj;			/* Object itself    */
int detach_p;			/* Whether a detach */
char *data;			/* User data (facet) */
/*
 * This routine is called when implementation geometry is attached to
 * or detached from a formal terminal.  It turns out that auto-redisplay
 * does not catch the case of formal terminal creation (although
 * it might in the future).  For now,  we do it by force.
 */
{
    struct octBox bb;
    octObject *fct = (octObject *) data;

    if (obj->type == OCT_TERM) {
	if (regFindImpl(obj, &bb) == REG_OK) {
	    wnQRedisplay(fct->objectId, &bb);
	} else if (octBB(obj, &bb) == OCT_OK) {
	    wnQRedisplay(fct->objectId, &bb);
	}
    }

}




#define TYPE_SET	0x01
#define DIR_SET		0x02

vemStatus seCreateFormalTerminal(facet, implementation, name, tt, dir)
octObject *facet;		/* Target facet		           */
octObject *implementation;	/* Optional bag of implementations */
char *name;			/* Terminal name                   */
char *tt;			/* Terminal type (if any)          */
char *dir;			/* Terminal direction (if any)     */
/*
 * This command actually creates a new symbolic formal terminal.  It
 * checks to make sure all of the implementations are correct and
 * issues warnings if there are already terminals named `name'
 * or the implementations are attached to other terminals.  If
 * `tt' or `dir' are non-zero, they will be added as annotation
 * to the terminal.  Otherwise, it tries to determine the termtype
 * and direction from the underlying implementation.  If that is
 * inconclusive, it asks the user using a dialog box.
 */
{
    octObject newTerm, finalBag;
    int type_direction_p;
    symTermFunc tf;

    tf.func = seTermFunc;  tf.data = (char *) facet;

    /* Parse specification for terminal name */
    type_direction_p = 0;
    if (tt) type_direction_p |= TYPE_SET;
    if (dir) type_direction_p |= DIR_SET;

    finalBag.type = OCT_BAG;
    finalBag.contents.bag.name = IMPL_BAG_NAME;
    if (vuCreateTemporary(facet, &finalBag) == OCT_OK) {
	if (check_term_impl(implementation, &finalBag) == 0) {
	    vemMsg(MSG_A, "No valid implementation objects for terminal\n");
	    return VEM_CORRUPT;
	}
    } else {
	vemMsg(MSG_A, "Can't create temporary bag\n");
	return VEM_CORRUPT;
    }

    /* Now check to make sure there isn't already a formal named this */
    if (!check_previous(facet, name)) {
	VEM_OCTCKRVAL(octDelete(&finalBag), VEM_CORRUPT);
	return VEM_CORRUPT;
    }

    /* Now see if the implementations are already promoted */
    if (!check_promotion(name, &finalBag)) {
	VEM_OCTCKRVAL(octDelete(&finalBag), VEM_CORRUPT);
	return VEM_CORRUPT;
    }

    /* All of the implementations must be on the same net */
    if (!check_nets(&finalBag)) {
	VEM_OCTCKRVAL(octDelete(&finalBag), VEM_CORRUPT);
	return VEM_CORRUPT;
    }

    /* Now handle termtype and direction properties */
    check_type_direction(name, &finalBag, &type_direction_p, &tt, &dir);
    
    /* Prepare to make new terminal */
    newTerm.type = OCT_TERM;
    newTerm.contents.term.name = name;
    newTerm.contents.term.instanceId = oct_null_id;
    newTerm.contents.term.width = 1;

    symNewFormal(facet, &newTerm, &finalBag,
		 ((type_direction_p & TYPE_SET) ? tt : (char *) 0),
		 ((type_direction_p & DIR_SET) ? dir : (char *) 0),
		 &tf);

    bufMarkActive( facet->objectId );
    VEM_OCTCKRVAL(octDelete(&finalBag), VEM_CORRUPT);

    return VEM_OK;
}

#define BAD_TERM	0x01
#define BAD_INST	0x02
#define BAD_OBJ		0x04

static int check_term_impl(impl, final)
octObject *impl;		/* Implementation bag */
octObject *final;		/* Final bag          */
/*
 * This routine looks through `impl' attaching valid formal terminal
 * implementation objects to `final'.  It returns the number of
 * valid implementations found.  It produces diagnostics if unsupported
 * objects are found.
 */
{
    octGenerator gen;
    octObject obj, aterm;
    int total, ignored;

    total = 0;
    if (octInitGenContents(impl,
			   OCT_TERM_MASK|OCT_INSTANCE_MASK|OCT_PATH_MASK,
			   &gen) == OCT_OK) {
	ignored = 0;
	while (octGenerate(&gen, &obj) == OCT_OK) {
	    switch (obj.type) {
	    case OCT_TERM:
		if (obj.contents.term.instanceId != 0) {
		    /* Actual terminal */
		    VEM_OCTCKRVAL(octAttach(final, &obj),
				  VEM_CORRUPT);
		    total++;
		} else if (!(ignored & BAD_TERM)) {
		    /* Formal terminal */
		    vemMsg(MSG_C, "WARNING: formal terminals cannot be implementations of other formal terminals.\n");
		    ignored |= BAD_TERM;
		}
		break;
	    case OCT_INSTANCE:
		/* Only connectors allowed */
		if (symContactP(&obj)) {
		    /* Get actual terminal */
		    VEM_OCTCKRVAL(octGenFirstContent(&obj,
						     OCT_TERM_MASK,
						     &aterm),
				  VEM_CORRUPT);
		    VEM_OCTCKRVAL(octAttach(final, &aterm),
				  VEM_CORRUPT);
		    total++;
		} else if (!(ignored & BAD_INST)) {
		    /* Real instance */
		    vemMsg(MSG_C, "WARNING: instances cannot implement formal terminals.\n");
		    ignored |= BAD_INST;
		}
		break;
	    case OCT_PATH:
		VEM_OCTCKRVAL(octAttach(final, &obj),
			      VEM_CORRUPT);
		total++;
		break;
	    default:
		if (!(ignored & BAD_OBJ)) {
		    vemMsg(MSG_C, "WARNING: unsupported object types ignored.\n");
		} else {
		    ignored |= BAD_OBJ;
		}
		break;
	    }
	}
    } else {
	vemMsg(MSG_A, "ERROR: Can't generate terminal implementations\n");
	total = 0;
    }
    return total;
}



static int check_previous(fct, name)
octObject *fct;			/* Facet to check */
char *name;			/* Terminal name  */
/* 
 * Checks to see if there is already a formal terminal on `fct'
 * named `name'.  If there is and expert mode is off, it will
 * query the user to see if he wants to continue.  If it
 * is in expert mode, a warning will be produced.
 */
{
    octObject ft;
    int expert = vemExpert( 0 );

    ft.type = OCT_TERM;
    ft.contents.term.name = name;
    if (octGetByName(fct, &ft) == OCT_OK) {
	if (expert) {
	    vemMsg(MSG_C, "WARNING: replacing existing terminal `%s'\n", name);
	    return 1;		/* Always do it if expert. */
	} 
	(void) sprintf(errMsgArea,
"There is already a formal terminal\n\
named `%s' in this cell.\n\
Do you really want to replace it?", name);
	if (dmConfirm("Duplicate Terminal", errMsgArea, "Yes", "No") == VEM_OK) {
	    return 1;
	} else {
	    return 0;
	}
    } else {
	return 1;
    }
}



static enum st_retval build_list(key, value, arg)
char *key, *value, *arg;
/*
 * Builds up a list of terminal names.
 */
{
    if (arg[0]) {
	(void) strcat(arg, ", ");
    }
    (void) strcat(arg, key);
    return ST_CONTINUE;
}

static int check_promotion(name, impl)
char *name;			/* Terminal name          */
octObject *impl;		/* Bag of implementations */
/*
 * This routine checks to see if any of the implementations given
 * in `impl' are already implementations of some other formal terminal.
 * If they are and expert mode is set, a warning will be produced.
 * If expert mode is off, the user will be queried.
 */
{
    octGenerator gen, up;
    octObject obj, term;
    st_table *term_names;
    char area[VEMMAXSTR*5];
    int expert = vemExpert( 0 );
    int result = 1;

    VEM_OCTCKRVAL(octInitGenContents(impl, OCT_ALL_MASK, &gen), 0);
    term_names = st_init_table(strcmp, st_strhash);
    while (octGenerate(&gen, &obj) == OCT_OK) {
	VEM_OCTCKRVAL(octInitGenContainers(&obj, OCT_TERM_MASK, &up), 0);
	while (octGenerate(&up, &term) == OCT_OK) {
	    (void) st_insert(term_names, term.contents.term.name, (char *) 0);
	}
    }
    if (st_count(term_names) > 0) {
	area[0] = '\0';
	(void) st_foreach(term_names, build_list, area);
	if (expert) {
	    vemMsg(MSG_C, "WARNING: some implementations of the formal terminal\n");
	    vemMsg(MSG_C, "         %s are already implementations of other\n",
		   name);
	    vemMsg(MSG_C, "         terminals.  These terminals will be deleted:\n");
	    vemMsg(MSG_C, "         %s", area);
	} else {
	    (void) sprintf(errMsgArea, 
"Some implementations of the formal terminal\n\
%s are already implementations of the following\n\
terminals:\n\
%s\n\
If you proceed, these terminals will be deleted.\n\
Do you want to proceed?", name, area);
	    if (dmConfirm("Duplicate Promotion", errMsgArea, "Yes", "No") != VEM_OK) {
		result = 0;
	    }
	}
    }
    st_free_table(term_names);
    return result;
}


static int check_nets(impl)
octObject *impl;		/* Implementations      */
/*
 * This routine checks to make sure all of the implementations of `impl'
 * are on the same net.  If not, it will produce a diagnostic and return
 * zero.
 * However, if there is only one object in the implementation, the presence
 * of the net is irrelevant.
 */
{
    octGenerator gen;
    octObject obj;
    octId cur_net;
    octObject net;
    int count = 0;		/* To count objects in implementation. */

    cur_net = oct_null_id;
    
    VEM_OCTCKRVAL(octInitGenContents(impl, OCT_ALL_MASK, &gen), 0);
    while (octGenerate(&gen, &obj) == OCT_OK) count++;
    if (count <= 1 ) {
	return 1;	/* Only one object, just go ahead. */
    }
    VEM_OCTCKRVAL(octInitGenContents(impl, OCT_ALL_MASK, &gen), 0);
    while (octGenerate(&gen, &obj) == OCT_OK) {
	if (symLocNet(&obj, &net)) {
	    if (!octIdIsNull(cur_net) &&
		!octIdsEqual(cur_net, net.objectId)) {
		/* Not the same net */
		VEM_OCTCKRVAL(octFreeGenerator(&gen), 0);
		vemMsg(MSG_C, "ERROR: All terminal implementations must be on the same net.\n");
		return 0;
	    }
	    cur_net = net.objectId;
	} else {
	    VEM_OCTCKRVAL(octFreeGenerator(&gen), 0);
	    vemMsg(MSG_C, "ERROR: No net associated with implementation.\n");
	    return 0;
	}
    }
    return 1;
}



static char *term_types[] = { "UNKNOWN", "SIGNAL", "SUPPLY", "GROUND" };
#define NTYPES	sizeof(term_types)/sizeof(char *)
static char *term_dirs[] = { "UNKNOWN", "INPUT", "OUTPUT", "INOUT" };
#define NDIRS 	sizeof(term_dirs)/sizeof(char *)

static void check_type_direction(name, impl, set_p, type, dir)
char *name;			/* Terminal name for diagnostics     */
octObject *impl;		/* Terminal implementations          */
int *set_p;			/* May be TYPE_SET, DIR_SET, or both */
char **type;			/* Terminal type                     */
char **dir;			/* Terminal direction 		     */
/*
 * Checks the type and direction of the terminal implementations
 * and sets them appropriately if they are unset.
 * If either attribute is set, it checks to make sure it is the
 * right value.  If either attribute is unset and expert mode
 * is on, it will query the user.
 */
{
    int i, ask;
    (void)vemExpert( 0 );
    sub_typedir(impl, set_p, type, dir);

    ask = 0;
    if (*set_p & TYPE_SET) {
	for (i = 0;  i < NTYPES;  i++) {
	    if (STRCOMP(*type, term_types[i]) == 0) break;
	}
	if (i >= NTYPES) {
	    vemMsg(MSG_C, "WARNING: %s: Unknown terminal type\n", type);
	}
    } else {
	ask = 1;
    }

    if (*set_p & DIR_SET) {
	for (i = 0;  i < NDIRS;  i++) {
	    if (STRCOMP(*dir, term_dirs[i]) == 0) break;
	}
	if (i >= NDIRS) {
	    vemMsg(MSG_C, "WARNING: %s: Unknown terminal direction\n", dir);
	}
    } else {
	ask = 1;
    }

#ifdef AVOID_DIALOGS
    if (!expert & ask) {
	ask_term_dir(name, set_p, type, dir);
    }
#endif
}


static void sub_typedir(impl, set_p, type, dir)
octObject *impl;		/* Implementation bag */
int *set_p;			/* What ones are set  */
char **type;			/* Terminal type      */
char **dir;			/* Terminal direction */
/*
 * Determines the type and direction for the terminal implementations
 * in `impl' and compares them to the ones provided.  Produces warnings
 * if they differ.  Sets them if they are not already set.
 */
{
    char *sttype = (char *) 0, *sdir = (char *) 0;
    octGenerator gen;
    octObject term, prop;

    VEM_OCTCKRET(octInitGenContents(impl, OCT_TERM_MASK, &gen));
    prop.type = OCT_PROP;
    while (octGenerate(&gen, &term) == OCT_OK) {
	prop.contents.prop.name = SYM_TERM_TYPE;
	if (symGetTermProp(&term, &prop)) {
	    if (!sttype) {
		sttype = prop.contents.prop.value.string;
	    } else {
		/* Conflicting types */
		vemMsg(MSG_C, "WARNING: The underlying terminal implementations\n");
		vemMsg(MSG_C, "         have differing terminal types.\n");
	    }
	}
	prop.contents.prop.name = SYM_TERM_DIR;
	if (symGetTermProp(&term, &prop)) {
	    if (!sdir) {
		sdir = prop.contents.prop.value.string;
	    } else {
		/* Conflicting directions */
		vemMsg(MSG_C, "WARNING: The underlying terminal implementations\n");
		vemMsg(MSG_C, "         have differing terminal directions.\n");
	    }
	}
	    
    }
    if (sttype) {
	if (*set_p & TYPE_SET) {
	    if (STRCOMP(sttype, *type) != 0) {
		vemMsg(MSG_C, "WARNING: Specified type doesn't match\n");
		vemMsg(MSG_C, "         type of implementation.\n");
	    }
	} else {
	    *set_p |= TYPE_SET;
	    *type = sttype;
	}
    }
    if (sdir) {
	if (*set_p & DIR_SET) {
	    if (STRCOMP(sdir, *dir) != 0) {
		vemMsg(MSG_C, "WARNING: Specified direction doesn't match\n");
		vemMsg(MSG_C, "         type of implementation.\n");
	    }
	} else {
	    *set_p |= DIR_SET;
	    *dir = sdir;
	}
    }
}


#ifdef AVOID_DIALOGS
static void ask_term_dir(tn, set_p, type, dir)
char *tn;			/* Terminal name                     */
int *set_p;			/* May be TYPE_SET, DIR_SET, or both */
char **type;			/* Terminal type */
char **dir;			/* Terminal direction */
/*
 * Asks the user for terminal direction and type.
 */
{
    char *temp;
    char *ty[NTYPES];
    char *dirs[NDIRS];
    int i;
    dmEnumItem items[2];
    char t_buf[1024];
    static char *dir_type_help =
"Symbolic formal terminals optionally may have annotation specifying\n\
the type and direction of the terminal.  You can set these properties\n\
by pressing the mouse in the value fields presented in the dialog.\n\
All of the possibilities will be shown.  If you leave the value as\n\
UNKNOWN, no annotation will be added to the terminal.  Press the\n\
OK button when you are finished.  Press the Dismiss button if you\n\
don't want to set the terminal type or direction.";

    /* We reload each time to insure the proper default */
    for (i = 0;  i < NTYPES;  i++) ty[i] = term_types[i];
    for (i = 0;  i < NDIRS;   i++) dirs[i] = term_dirs[i];

    items[0].itemPrompt = "Type";
    items[0].ni = NTYPES;
    items[0].enum_vals = ty;
    items[0].userData = (Pointer) 0;
    items[0].selected = 0;
    if (*set_p & TYPE_SET) {
	for (i = 0;  i < NTYPES;  i++) {
	    if (strcmp(ty[i], *type) == 0) {
		items[0].selected = i;
		break;
	    }
	}
    }
    items[1].itemPrompt = "Direction";
    items[1].ni = NDIRS;
    items[1].enum_vals = dirs;
    items[1].userData = (Pointer) 0;
    items[1].selected = 0;
    if (*set_p & DIR_SET) {
	for (i = 0;  i < NTYPES;  i++) {
	    if (strcmp(dirs[i], *dir) == 0) {
		items[1].selected = i;
		break;
	    }
	}
    }
    (void) sprintf(t_buf, "Terminal Type and Direction for %s", tn);
    if (dmMultiEnum(t_buf, 2, items, dir_type_help) == VEM_OK) {
	temp = items[0].enum_vals[items[0].selected];
	if (strcmp(temp, "UNKNOWN") != 0) {
	    *set_p |= TYPE_SET;
	    *type = temp;
	} else {
	    *set_p &= ~TYPE_SET;
	}
	temp = items[1].enum_vals[items[1].selected];
	if (strcmp(temp, "UNKNOWN") != 0) {
	    *set_p |= DIR_SET;
	    *dir = temp;
	} else {
	    *set_p &= ~TYPE_SET;
	}
    }
}
#endif


/*ARGSUSED*/
vemStatus sePromote(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* Argument list            */
/*
 * This command promotes a set of actual terminals supplied on
 * the argument list to formal terminals.  It also allows a
 * format string to be specified to automatically name the
 * new terminals.  The string should be standard printf style
 * with exactly one %ld to substitute a terminal count.
 */
{
    vemOneArg *arg1, *arg2;
    octObject itemBag, aterm, fct, implBag;
    octGenerator gen;
    /* RLS added new variables for automatic naming */
    char *formatstring = (char*)NULL, buffer[1024], *term_name;
    int format = 0, count = 0;
    lsGen lgen;

    seInit();

    /* Check arguments */
    if ((lsLength(cmdList) < 1) || (lsLength(cmdList) > 2)) {
	vemMessage("formal: object(s) [formatstring] : promote\n",
			    MSG_NOLOG|MSG_DISP);
	return VEM_ARGRESP;
    }

    lgen = lsStart(cmdList);
    lsNext(lgen, (Pointer *) &arg1, LS_NH);
    if (arg1->argType != VEM_OBJ_ARG) {
	vemMessage("formal: object(s) [formatstring] : promote\n",
			    MSG_NOLOG|MSG_DISP);
	lsEnd(lgen);
	return VEM_ARGRESP;
    }

    if (lsLength(cmdList) == 2) {
	format = 1;
	lsNext(lgen, (Pointer *) &arg2, LS_NH);
	if (arg2->argType != VEM_TEXT_ARG) {
	    vemMessage("formal: object(s) [formatstring] : promote\n",
				MSG_NOLOG|MSG_DISP);
	    lsEnd(lgen);
	    return VEM_ARGRESP;
	}
	formatstring = arg2->argData.vemTextArg;
    }
    lsEnd(lgen);

    if (bufWritable(arg1->argData.vemObjArg.theFct) != VEM_OK) {
	vemMsg(MSG_C, "Buffer is read only.\n");
	return VEM_ARGRESP;
    }
    fct.objectId = arg1->argData.vemObjArg.theFct;
    if (octGetById(&fct) != OCT_OK) {
	vemMsg(MSG_A, "Can't get facet associated with buffer:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    itemBag.objectId = arg1->argData.vemObjArg.theBag;
    if (octGetById(&itemBag) != OCT_OK) {
	vemMsg(MSG_A, "Cannot get terminals for promotion:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    if (octInitGenContents(&itemBag, OCT_TERM_MASK, &gen) != OCT_OK) {
	vemMsg(MSG_A, "Can't initialize generation of objects:\n  %s\n",
	       octErrorString());
	return VEM_CORRUPT;
    }
    while (octGenerate(&gen, &aterm) == OCT_OK) {
	if (format) {
	    (void) sprintf(buffer, formatstring, count++);
	    term_name = buffer;
	} else {
	    term_name = aterm.contents.term.name;
	}
	implBag.type = OCT_BAG;
	implBag.contents.bag.name = IMPL_BAG_NAME;
	VEM_OCTCKRVAL(vuCreateTemporary(&fct, &implBag), VEM_CORRUPT);
	VEM_OCTCKRVAL(octAttach(&implBag, &aterm), VEM_CORRUPT);
	seCreateFormalTerminal(&fct, &implBag, term_name,
			       (char *) 0, (char *) 0);
	VEM_OCTCKRVAL(octDelete(&implBag), VEM_CORRUPT);
    }
    return VEM_OK;
}
