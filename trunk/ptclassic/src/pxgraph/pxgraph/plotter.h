/*******************************************************************
SCCS version identification
$Id$

Copyright (c) 1989-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
    /* 
     * HP plotter definition - these are dependent on the
     * SPECIFIC MODEL of HP plotter used, and should always
     * be modified when going to a new plotter.
     * 
     * all dimensions are in plotter units.
     *
     * MINX and MINY are the smallest x and y values that
     * are inside the soft clip limits of the plotter
     * MAXX and MAXY are the largest x and y values that
     * are inside the soft clip limits of the plotter MINUS
     * MINX and MINY, so they give the dimension of the
     * soft clip area.
     *
     * PLOTTERTYPE is a character string which identifies the
     * plotter that should be used.  control information will
     * be read for ~cad/lib/technology/$TECHNOLOGY/$PLOTTERTYPE.map
     * and output will go to /usr/ucb/lpr -Pplt$PLOTTERTYPE
     *
     */
#define PLOTTERTYPE "7550"
#define P1X 80
#define P1Y 320
#define P2X 10080
#define P2Y 7520
#define MAXX 10000
#define MAXY 7200

#define PLOTTERNAME "paper"

#define PENGRID 1
#define PENAXIS 2
#define TEXTCOLOR 1
#define PEN1 3
#define PEN2 4
#define PEN3 5 
#define PEN4 6
#define PEN5 7
#define PEN6 8
#define PEN7 2
#define PEN8 1

#define LINE1 2
#define LINE2 4
#define LINE3 5 
#define LINE4 6
#define LINE5 2
#define LINE6 4
#define LINE7 5
#define LINE8 6

#define MARK1 "L"
#define MARK2 "K"
#define MARK3 "M"
#define MARK4 "O"
#define MARK5 "G"
#define MARK6 "F"
#define MARK7 "E"
#define MARK8 "A"

