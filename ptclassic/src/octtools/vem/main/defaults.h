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
 * Program wide defaults and parameters
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains declarations for use of the defaults package
 * implemented in defaults.c.  It is a hash table based system
 * which can return information of certain defined types.  Note:
 * all intermodule global variables should be defined here (not just 
 * used arbitrarily!).  Include "general.h" before this file.
 *
 * Every value exists in two forms at all times.  The first form is
 * the normal text representation of the value as it might be read
 * from the startup file or as input by the user.  The second form
 * is its "useful" form which is used by the various routines which
 * use default values.  For example,  a number of the default values
 * are colors.  The text value for the color is the text representation
 * for the color ("red" for example).  The useful value is a pointer
 * to a Color structure which contains the pixel value and red, green,
 * and blue values.
 *
 * Transformation from one form to the other is accomplished by
 * build in transformation functions provided for each supported
 * type.  See the dfInit routine for details.
 *
 * It is important to note that ALL globally available data should
 * be represented in this module so that the interface to this
 * data is uniform.
 *
 * Below is a list of the currently defined default values,  the
 * type of the useful form of the default,  and a short description
 * of the default.  The default module supports a number of
 * useful types.  To get a useful default value,  the caller
 * uses the access routine for that type.  These are listed
 * below the table in the module interface section.
 *
 * Name			Type		Comment
 *
 * vem.lambda		(int)		Units to snap points to
 * vem.text.font.norm	(XFontStruct *)	Normal font (used in console)
 * vem.text.font.bold	(XFontStruct *)	Boldface font (used in console)
 * vem.text.font.italic	(XFontStruct *)	Italic font (used in console)
 * vem.text.color.norm	(XColor *)	Color of normal font
 * vem.text.color.bold	(XColor *)	Color of bold font
 * vem.text.color.italic (XColor *)	Color of italic font
 * vem.console.bngd	(XColor *)	Console background color
 * vem.console.fgnd	(XColor *)	Console foreground color
 * vem.console.cursor	(XColor *)	Console cursor color
 * vem.background.color	(XColor *)	Background color table entry
 * vem.window.options	(int)		Default window options
 * vem.cursor.color	(XColor *)	Color of graphics cursor
 * vem.label.font	(XFontStruct *)	Font for labels
 * vem.border.width	(int)		Default width of VEM window borders
 * vem.border.color	(XColor *)	Default border color
 * vem.grid.pixels	(int)		Minimum grid difference (in pixels)
 * vem.grid.base	(int)		Logarithmic base of grid lines
 * vem.grid.minbase	(int)		Minimum base of grid lines
 * vem.grid.zero.color	(XColor *)	Zero grid line color
 * vem.grid.zero.style	(flPattern)	Zero grid line style
 * vem.grid.zero.size	(int)		Zero grid line width (pixels)
 * vem.grid.major.units	(int)		Frequency of major grid lines (OCT units)
 * vem.grid.major.color	(XColor *)	Color of major grid lines
 * vem.grid.major.style	(flPattern)	Line style of major grid lines
 * vem.grid.major.size	(int)		Major grid line width (pixels)
 * vem.grid.minor.units	(int)		Minor grid line frequency
 * vem.grid.minor.color	(XColor *)	Minor grid line color
 * vem.grid.minor.style	(flPattern)	Minor grid line style
 * vem.grid.minor.size	(int)		Minor grid line width (pixels)
 * vem.title.color	(XColor *)	Color of window titles
 * vem.title.backgr	(XColor *)	Color of window title background
 * vem.title.font	(XFontStruct *)	Title font (suggested fixed width)
 * vem.boundbox.thres	(float)		Threshold for drawing name in box
 * vem.boundbox.font	(XFontStruct *)	Bounding box name font
 * vem.boundbox.min	(int)		Abstraction level of bounding box
 * vem.formal.color	(XColor *)	Color of formal terminals
 * vem.formal.style	(flPattern)	Outline style of formal terminals
 * vem.actual.color	(XColor *)	Color of actual terminals
 * vem.actual.style	(flPattern)	Outline style of actual terminals
 * vem.inst.color	(XColor *)	Instance bounding box color
 * vem.inst.style	(flPattern)	Instance bounding box style
 * vem.interface	(String)	Standard interface name for cells
 * vem.version		(String)	Standard version string for cells
 * vem.bindings		(String)	Command bindings file
 * vem.select.color	(XColor *)	Selection color
 * vem.select.style	(int)		Selection line style
 * vem.logfile		(String)	File to put log
 * vem.inst.priority	(int)		Priority level of instance drawing
 * vem.argument.color	(XColor *)	Color of argument lines and points
 * vem.argument.pixels	(int)		Mouse travel before point becomes box
 * vem.solid.threshold	(int)		Size (in Pixels) before solid fill
 * vem.menuCells	(String)	Directory containing menu cells
 * vem.abstraction	(int)		Size (in Pixels) before contact abs.
 * vem.hilite.color	(XColor *)	Color of highlight set
 * vem.hilite.style	(int)		Highlight line style
 * vem.grid.minor.dots	(int)		If on,  minor grid is drawn as dots
 * vem.grid.major.dots	(int)		If on,  major grid is drawn as dots
 */


#ifndef DEFAULTS
#define DEFAULTS

#include "general.h"
#include "attributes.h"
#include "list.h"

extern char *def_pkg_name;

extern void dfInit ARGS((Display *disp, STR progname));
  /* Defines and initializes all default values */

extern vemStatus dfStore ARGS((STR name, STR value));
  /* Store a parameter value in the default list */

extern STR dfGetText ARGS((STR name));
  /* Gets the string version for editing purposes */

extern void dfGetInt ARGS((STR name, int *value));
  /* Gets useful values for defaults of type (int)        */
extern void dfGetString ARGS((STR name, STR *value));
  /* Gets useful values for defaults of type (String)     */
extern void dfGetFloat ARGS((STR name, float *value));
  /* Gets useful values for defaults of type (float)      */
extern void dfGetDouble ARGS((STR name;	double *value));
  /* Gets useful values for defaults of type (double)      */
extern void dfGetCol ARGS((STR name, XColor **value));
  /* Gets the color structure for a color (XColor *)      */
extern void dfGetPixel ARGS((STR name, XColor **value));
  /* Gets color and fills in pixel value (if needed)      */
extern void dfGetFont ARGS((STR name, XFontStruct **value));
  /* Gets useful values for defaults of type (XFontStruct *) */
extern void dfGetFill ARGS((STR name, flPattern *value));
  /* Gets a fill pattern,  returns type (flPattern)       */
extern void dfGetAttr ARGS((STR name, atrHandle *value));
  /* Gets an attribute,  returns type (atrHandle)         */
extern void dfGetFontList ARGS((STR name, lsList *value));
  /* Gets a font list, returns type (lsList)              */

extern void dfDump ARGS((int option));
  /* Dumps all defaults to console or log file */

#endif
