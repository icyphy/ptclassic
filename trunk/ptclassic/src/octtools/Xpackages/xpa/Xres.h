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
/*
 * Xresource Prototype Header
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This header file is freely distributable subject to the copyright
 * notice included below.
 */

#ifndef _XRES_PROTO_H_
#define _XRES_PROTO_H_

/*
 * Original <X11/Xresource.h> included verbatim from XV11R3 distribution
 * with the function declarations removed (they are replaced by prototypes
 * below).  Old function declarations are enclosed in #ifdef NO_PROTOTYPES.
 */

/* $XConsortium: Xresource.h,v 1.13 88/09/06 16:06:14 jim Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef _Xresource_h
#define _Xresource_h

/****************************************************************
 ****************************************************************
 ***                                                          ***
 ***                                                          ***
 ***          X Resource Manager Intrinsics                   ***
 ***                                                          ***
 ***                                                          ***
 ****************************************************************
 ****************************************************************/



/****************************************************************
 *
 * Miscellaneous definitions
 *
 ****************************************************************/

#ifdef CRAY
#ifndef __TYPES__
#define __TYPES__
#include <sys/types.h>			/* forgot to protect it... */
#endif /* __TYPES__ */
#else
#include <sys/types.h>
#endif /* CRAY */

#ifndef NULL
#define NULL 0
#endif

/****************************************************************
 *
 * ||| Memory Management (move out of here!)
 *
 ****************************************************************/

extern char *Xpermalloc();
    /* unsigned int size;   */

/****************************************************************
 *
 * Quark Management
 *
 ****************************************************************/

typedef int     XrmQuark, *XrmQuarkList;
#define NULLQUARK ((XrmQuark) 0)

typedef char *XrmString;
#define NULLSTRING ((XrmString) 0)
#ifdef NO_PROTOTYPES
/* find quark for string, create new quark if none already exists */
extern XrmQuark XrmStringToQuark(); /* name */
    /* XrmString name; */

/* find string for quark */
extern XrmString XrmQuarkToString(); /* quark */
    /* XrmQuark name; */

extern XrmQuark XrmUniqueQuark();
#endif /* NO_PROTOTYPES */
#define XrmStringsEqual(a1, a2) (strcmp(a1, a2) == 0)


/****************************************************************
 *
 * Conversion of Strings to Lists
 *
 ****************************************************************/
#ifdef NO_PROTOTYPES
extern void XrmStringToQuarkList();
    /* char		*name;  */
    /* XrmQuarkList     quarks; */  /* RETURN */

extern void XrmStringToBindingQuarkList();
    /* char		*name;      */
    /* XrmBindingList   bindings;   */  /* RETURN */
    /* XrmQuarkList     quarks;     */  /* RETURN */

#endif /* NO_PROTOTYPES */
/****************************************************************
 *
 * Name and Class lists.
 *
 ****************************************************************/

typedef XrmQuark     XrmName;
typedef XrmQuarkList XrmNameList;
#define XrmNameToString(name)		XrmQuarkToString(name)
#define XrmStringToName(string)		XrmStringToQuark(string)
#define XrmStringToNameList(str, name)	XrmStringToQuarkList(str, name)

typedef XrmQuark     XrmClass;
typedef XrmQuarkList XrmClassList;
#define XrmClassToString(class)		XrmQuarkToString(class)
#define XrmStringToClass(class)		XrmStringToQuark(class)
#define XrmStringToClassList(str,class)	XrmStringToQuarkList(str, class)



/****************************************************************
 *
 * Resource Representation Types and Values
 *
 ****************************************************************/

typedef XrmQuark     XrmRepresentation;
#define XrmStringToRepresentation(string)   XrmStringToQuark(string)
#define	XrmRepresentationToString(type)   XrmQuarkToString(type)

typedef struct {
    unsigned int    size;
    caddr_t	    addr;
} XrmValue, *XrmValuePtr;


/****************************************************************
 *
 * Resource Manager Functions
 *
 ****************************************************************/

typedef enum {XrmBindTightly, XrmBindLoosely} XrmBinding, *XrmBindingList;

typedef struct _XrmHashBucketRec *XrmHashBucket;
typedef XrmHashBucket *XrmHashTable;
typedef XrmHashTable XrmSearchList[];
typedef struct _XrmHashBucketRec *XrmDatabase;

#ifdef NO_PROTOTYPES
extern void XrmInitialize();

extern void XrmQPutResource();
    /* XrmDatabase	    *pdb;	*/
    /* XrmBindingList       bindings;   */
    /* XrmQuarkList	    quarks;     */
    /* XrmRepresentation    type;       */
    /* XrmValue		    *value;	*/

extern void XrmPutResource();
    /* XrmDatabase	    *pdb;       */
    /* char		    *specifier; */
    /* char		    *type;      */
    /* XrmValue		    *value;     */

extern void XrmQPutStringResource();
    /* XrmDatabase	    *pdb;       */
    /* XrmBindingList       bindings;   */
    /* XrmQuarkList	    quarks;     */
    /* char		    *str;       */

extern void XrmPutStringResource();
    /* XrmDatabase	    *pdb;       */
    /* char		    *specifier; */
    /* char		    *str;       */

extern void XrmPutLineResource();
    /* XrmDatabase	    *pdb;       */
    /* char		    *line;	*/

extern  XrmQGetResource();
    /* XrmDatabase	    db;		*/
    /* XrmNameList	    names;      */
    /* XrmClassList	    classes;    */
    /* XrmRepresentation    *type;      */  /* RETURN */
    /* XrmValue		    *value;     */  /* RETURN */

extern Bool XrmGetResource();
    /* XrmDatabase	    db;		*/
    /* char		    *name_str;  */
    /* char		    *class_str; */
    /* char		    *type;      */  /* RETURN */
    /* XrmValue		    *value;     */  /* RETURN */

extern Bool XrmQGetSearchList();
    /* XrmDatabase	    db;		*/
    /* XrmNameList	    names;      */
    /* XrmClassList	    classes;    */
    /* XrmSearchList	    searchList; */  /* RETURN */
    /* int		    listLength; */

extern Bool XrmQGetSearchResource();
    /* SearchList	    searchList; */
    /* XrmName		    name;       */
    /* XrmClass		    class;      */
    /* XrmRepresentation    *type;      */  /* RETURN */
    /* XrmValue		    *value;     */  /* RETURN */
#endif /* NO_PROTOTYPES */
/****************************************************************
 *
 * Resource Database Management
 *
 ****************************************************************/
#ifdef NO_PROTOTYPES
extern XrmDatabase XrmGetFileDatabase();
    /* char	    *filename;  */

extern XrmDatabase XrmGetStringDatabase();
    /* char	    *data;      */  /*  null terminated string */

extern void XrmPutFileDatabase();
    /* XrmDatabase  db;		*/
    /* char	    *filename   */

extern void XrmMergeDatabases();
    /* XrmDatabase  new;	*/
    /* XrmDatabase  *into;      */  /* RETURN */

#endif /* NO_PROTOTYPES */


/****************************************************************
 *
 * Command line option mapping to resource entries
 *
 ****************************************************************/

typedef enum {
    XrmoptionNoArg,	/* Value is specified in OptionDescRec.value	    */
    XrmoptionIsArg,     /* Value is the option string itself		    */
    XrmoptionStickyArg, /* Value is characters immediately following option */
    XrmoptionSepArg,    /* Value is next argument in argv		    */
    XrmoptionResArg,	/* Resource and value in next argument in argv      */
    XrmoptionSkipArg,   /* Ignore this option and the next argument in argv */
    XrmoptionSkipLine   /* Ignore this option and the rest of argv	    */
} XrmOptionKind;

typedef struct {
    char	    *option;	    /* Option abbreviation in argv	    */
    char	    *specifier;     /* Resource specifier		    */
    XrmOptionKind   argKind;	    /* Which style of option it is	    */
    caddr_t	    value;	    /* Value to provide if XrmoptionNoArg   */
} XrmOptionDescRec, *XrmOptionDescList;
#ifdef NO_PROTOTYPES
extern void XrmParseCommand();
    /* XrmDatabase	    *pdb;	    */
    /* XrmOptionDescList    options;	    */
    /* int		    num_options;    */
    /* char		    *prefix;	    */
    /* int		    *argc;	    */
    /* char		    **argv;	    */
#endif /* NO_PROTOTYPES */


#endif /* _Xresource_h */
/* DON'T ADD STUFF AFTER THIS #endif */


/*
 * End inclusion of <X11/Xresource.h>
 */

#ifndef XARGS
#ifdef __STDC__
#define XARGS(args)	args
struct _XrmHashBucketRec {
    int dummy;
};
#else
#define XARGS(args)	()
#endif
#endif

extern void XrmInitialize();

extern void XrmStringToQuarkList
  XARGS((char *name, XrmQuarkList quarks));

extern void XrmStringToBindingQuarkList
  XARGS((char *name, XrmBindingList bindings, XrmQuarkList quarks));

extern Bool XrmQGetSearchList
  XARGS((XrmHashBucket db, XrmNameList names, XrmClassList classes,
	 XrmSearchList searchList, int listLength));

extern Bool XrmQGetSearchResource
  XARGS((XrmSearchList searchList, XrmName name, XrmClass class,
	 XrmRepresentation *pType, XrmValue *pVal));

extern void XrmQPutResource
  XARGS((XrmDatabase *pdb, XrmBindingList bindings,
	 XrmQuarkList quarks, XrmRepresentation type, XrmValuePtr value));

extern void XrmPutResource
  XARGS((XrmDatabase *pdb, char *specifier, char *type, XrmValuePtr value));

extern void XrmQPutStringResource
  XARGS((XrmDatabase *pdb, XrmBindingList bindings,
	 XrmQuarkList quarks, char *str));

extern void XrmPutStringResource
  XARGS((XrmDatabase *pdb, char *specifier, char *str));

extern void XrmPutLineResource
  XARGS((XrmDatabase *pdb, char *line));

extern XrmDatabase XrmGetStringDatabase
  XARGS((char *data));

extern XrmDatabase XrmGetFileDatabase
  XARGS((char *fileName));

extern void XrmPutFileDatabase
  XARGS((XrmDatabase db, char *fileName));

extern void XrmMergeDatabases
  XARGS((XrmDatabase new, XrmDatabase *into));

extern Bool XrmQGetResource
  XARGS((XrmHashBucket db, XrmNameList names, XrmClassList classes,
	 XrmRepresentation *pType, XrmValuePtr pValue));

extern Bool XrmGetResource
  XARGS((XrmHashBucket db, XrmString name_str, XrmString class_str,
	 XrmString *pType_str, XrmValuePtr pValue));

extern void XrmParseCommand
  XARGS((XrmDatabase *pdb, XrmOptionDescList options, int num_options,
	 char *prefix, int *argc, char **argv));

extern XrmQuark XrmStringToQuark
  XARGS((XrmString name));

extern XrmQuark XrmUniqueQuark();

extern XrmString XrmQuarkToString
  XARGS((XrmQuark quark));

#endif /* _XRES_PROTO_H_ */
