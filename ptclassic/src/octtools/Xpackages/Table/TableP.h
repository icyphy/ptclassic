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
 * Table - Forms-based composite widget/geometry manager for the X Toolkit
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains the Table private declarations.
 */

#ifndef _TableP_h
#define _TableP_h

#include "Table.h"

/*
 * Local definitions
 */

typedef void (*XtTblRProc)();
  /*
   * Widget table;
   * Widget subwidget;
   * Position r, c;
   * Dimension hspan, vspan;
   * XtTblMask options;
   */

typedef Boolean (*XtTblLProc)();
   /*
    * Widget w;
    * Position *r, *c;
    * Dimension *hspan, *vspan;
    * XtTblMask *options;
    */

typedef struct _TableLocTbl *TableLocTblPtr;
   /*
    * Opaque reference to actual widget location table
    * defined in Table.c
    */

typedef struct _TableDefLoc *TableDefLocPtr;
   /*
    * Opaque reference to default widget location table defined
    * in Table.c.
    */    

typedef struct _TableVector *TableVecPtr;
   /*
    * Opaque reference to vectors used for giving size of
    * each row and column.
    */

typedef enum _TableVecState { INVALID, MINIMUM } TableVecState;

/*
 * Information kept in class record
 */

typedef struct {
    XtTblRProc position_child;	/* Register location of some child widget  */
    XtTblLProc find_child;	/* Return information about a child widget */
} TableClassPart;

/*
 * Class hierarchy
 */

typedef struct _TableClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    TableClassPart	table_class;
} TableClassRec;

extern TableClassRec tableClassRec;

/*
 * Information in instance record
 */

typedef struct _TablePart {
    Dimension		int_width;   /* Inner horizontal padding          */
    Dimension		int_height;  /* Inner vertical padding            */
    Dimension		row_spacing; /* Space between rows                */
    Dimension		col_spacing; /* Space between columns             */
    Cursor		cursor;	     /* Cursor to use in widget           */
    XtTblMask		def_options; /* Default layout options            */
    TableDefLocPtr	init_layout; /* Initial layout spec from resource */
    TableDefLocPtr	layout_db;   /* Merged table                      */
    TableLocTblPtr	real_layout; /* Actual current layout information */
    TableVecState	vec_state;   /* Current state of vectors          */
    Cardinal		num_rows;    /* Number of rows                    */
    TableVecPtr		rows;	     /* Heights of each row               */
    Cardinal		num_cols;    /* Number of columns                 */
    TableVecPtr		cols;	     /* Widths of each column             */
    Cardinal		vec_height;  /* Sum of current rows               */
    Cardinal		vec_width;   /* Sum of current columns            */
} TablePart;

/*
 * Instance hierarchy
 */

typedef struct _TableRec {
    CorePart		core;
    CompositePart	composite;
    TablePart		table;
} TableRec;

#endif /* _TableP_h */
