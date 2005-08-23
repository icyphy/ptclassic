/* Version Identification:
 * @(#)mkarray.h	1.1	12/15/93
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
 * mkarray.h
 * Copyright (c) 1986, Charles J Kring. All rights reserved.
 *
 * This is the necessary header file to use libmkarray.a.
 */

/* the name of this package						*/
#define MK_PKG_NAME mkPkgName
extern char mkPkgName[];

/* oct objects which can legally be attached to the array               */
#define LEGAL_ELEM_MASK OCT_INSTANCE_MASK|OCT_TERM_MASK

/* the number of unique areas returned by the area mapping procedures	*/
#define MK_AREA_ARRAY_SIZE 15

/* the bag containing everything mkarray did				*/
#define ROOTBAG "MKARRAYSTRUCTURE"	
/* the bag containing objects which cause errors in mkarray		*/
#define ERRORBAG "ERRORBAGS"
/* the bag containing the nets and segments created by mkarray		*/
#define DIDITBAG "MKARRAYDIDIT"

/* the names of some needed Oct Symbolic Policy bags 			*/
#define CONNECTORSBAG "CONNECTORS"
#define INSTANCESBAG "INSTANCES"
#define JOINEDTERMSBAG "JOINEDTERMS"
#define JOINEDBAG "JOINED"

/* oct objects which make the rule facet				*/

/* the top level rule ( RULEBAG ) which is named ( RULENAMEPROP )	*/
#define RULEBAG "RULE"
#define RULENAMEPROP "RULENAME"

/* the RULEBAG contains CONNECTBAGS which specify a wiring rule		*/
/* the wiring rule is a connection between the first and second 	*/
/* terminal or TERMGROUP in the CONNECT bag.				*/
#define CONNECTBAG "CONNECT"
#define TERMGROUPBAG "TERMGROUP"

/* the ordered bags of horizontal and vertical routing layers		*/
/* the preferred routing order is the order of attachment to the bag	*/
#define HORIZONTAL_LAYERS "HORIZONTAL_LAYERS"
#define VERTICAL_LAYERS "VERTICAL_LAYERS"

/* options to the CONNECT BAGS and layers in the *LAYERS bags		*/
/* the segment width = max( min layer width, value of MINWIDTHPROP )	*/ 
#define MK_WIDTH_PROP "WIDTH"
#define MK_MIN_WIDTH 0
/* the segment width = min layer width )				*/
#define MK_MAX_WIDTH -1
/* the segment width = min( max layer width, size of smallest terminal )*/ 

/* possible errors in ERRORBAG						*/
#define BAD_TERM "BAD_TERM"
#define BAD_INST "BAD_INSTANCE"
#define BAD_SEG "BAD_SEGMENT"
#define BAD_PROP "BAD_PROP"
#define BENT_SEG "NON_MANHATTAH_SEGMENT"

/* mkutils return codes							*/
#define MK_ALREADY_OPEN 2
#define MK_OK 1
#define MK_ERROR -1
#define MK_OCT_ERROR -2
#define MK_TAP_ERROR -3
#define MK_BAD_CONTAINER -4
#define MK_CORRUPT_ID -5

/* a MK_NULL_COORD -> that coodinate goes to the edge of the array	*/
#define MK_NULL_COORD -1
/* MK_UMMAPPED_COORD -> an unmapped subarray coordinate			*/
#define MK_UNMAPPED_COORD -2

/* possible segment orientations( specified in mkNewSegment )		*/
#define MK_ORIENTATION_PROP "orientation"
#define MK_DC_SEG 0		/* mkarray determines the orientation 	*/
#define MK_HOR_SEG 1		/* horizontal segment			*/
#define MK_VER_SEG 2		/* vertical segment			*/
#define MK_NONMAN_SEG 3		/* non manhattan segment		*/

typedef octId mkId;

extern octBox *mkArea();
extern octBox *mkCol();
extern octBox *mkCols();
extern octBox *mkPoint();
extern octBox *mkRow();
extern octBox *mkRows();
extern octBox *mkUnionBB();
extern octBox *mkIntersection();
extern octBox *mkArrayBB();
extern octBox *mkObjectBB();
extern octBox *mkTranslate();
extern void mkAttach();
extern void mkAddArrayArea();
extern void mkFreeArray();
extern void mkGetArrayByObjectId();
extern void mkLinkSubarray();
extern void mkMergeRules();
extern void mkSetArrayOrigin();
extern void mkSetArrayPitch();
extern void mkSetDebug();
extern void mkSetExpansion();
extern void mkSetPrintGraph();
extern void mkWireIntraInstance();
extern void mkOpenArray();
extern void mkOpenSubarray();
extern void mkPlaceInstances();
extern void mkPlaceNets();
extern void mkPlaceArray();
extern void mkReadRules();
extern void mkSolveArray();
extern void mkPrintSubarrayLinking();
extern void mkPrintTranslation();
extern void mkWireArea();
extern void mkNewSegment();
extern void mkGetGridCoords();
extern void mkAddConnectToRule();
extern void mkCreateRule();
