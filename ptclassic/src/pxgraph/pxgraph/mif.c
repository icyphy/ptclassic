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
 * MIF Output
 *
 * Edward A. Lee
 */

/*
 * Basic scaling parameters
 */
#define MICRONS_PER_INCH        2.54E+04
#define POINTS_PER_INCH         72.0
#define INCHES_PER_POINT        1.0/72.0
#define VDPI                    1200
#define VDPID                   1200.0
#define TICK_LENGTH             120

#define MAPX(x) (x)/VDPID
#define MAPY(y) (y)/VDPID

#include <stdio.h>
#include "xgout.h"
/* FIXME -- remove plotter.h dependence */
#include "plotter.h"
#include <math.h>
#define MAX(a,b) ( ((a)>(b)) ? (a) : (b) )
#define MIN(a,b) ( ((a)<(b)) ? (a) : (b) )

static void mifText();
static void mifSeg();
static void mifDot();
static void mifEnd();

static int rd(dbl)
double dbl;
/* Short and sweet rounding function */
{
    if (dbl < 0.0) {
        return ((int) (dbl - 0.5));
    } else {
        return ((int) (dbl + 0.5));
    }
}

typedef struct {
    int axis_size;
    char* axis_family;
    int title_size;
    char* title_family;
    FILE *plotterFile;
} mydata;

/*
 * The local "device units" are "dots", or 1/VDPI inches (1/1200),
 * an attempt to capture the precision of the best current
 * printers.  The input data is in microns, so to convert
 * it, multiply by VDPI/MICRONS_PER_INCH.
 */
int
mifInit(stream,width,height,title_family, title_size,
        axis_family, axis_size, flags, outInfo,errmsg)
        FILE *stream;   /* output stream */
    int width;          /* desired width of space in microns */
    int height;         /* desired height in microns */
    char *title_family; /* name of font for titles */
    double title_size;  /* size of font for titles */
    char *axis_family;  /* name of font for axes */
    double axis_size;   /* size of font for axes */
    int flags;       	/* predicate values (ignored) */
    xgOut *outInfo;     /* my structure */
    char errmsg[ERRBUFSIZE];    /* a place to complain to */
{
    mydata *myInfo;
    double MY_scale, font_size;

    myInfo = (mydata*)malloc(sizeof(mydata));
    if(myInfo == NULL) return(0);

    outInfo->max_segs = 1000;

    MY_scale = VDPI / MICRONS_PER_INCH;
    outInfo->dev_flags = 0;
    outInfo->area_w = width*MY_scale;
    outInfo->area_h = height*MY_scale;
    outInfo->axis_pad = 0.1 * VDPI;
    outInfo->bdr_pad = 0.1 * VDPI;

    font_size = axis_size * INCHES_PER_POINT * VDPI;
    outInfo->axis_width = rd( 0.55*font_size );
    outInfo->axis_height = rd( font_size );
    myInfo->axis_size = rd( axis_size );
    myInfo->axis_family = axis_family;

    font_size = title_size * INCHES_PER_POINT * VDPI;
    outInfo->title_width = rd( 0.55*font_size );
    outInfo->title_height = rd( font_size );
    myInfo->title_size = rd( title_size );
    myInfo->title_family = title_family;

    outInfo->xg_text = mifText;
    outInfo->xg_seg = mifSeg;
    outInfo->xg_dot = mifDot;
    outInfo->xg_end = mifEnd;

    outInfo->tick_len = 0.07*VDPI;

    outInfo->user_state = (char *)myInfo;
    myInfo->plotterFile = stream;

    fprintf(myInfo->plotterFile,"<MIFFile> # Generated by pxgraph\n");
    fprintf(myInfo->plotterFile,"<Pen 0>\n");
    fprintf(myInfo->plotterFile,"<PenWidth .5>\n");
    fprintf(myInfo->plotterFile,"<Fill 15>\n");
    return(1);
}

static void 
mifText(userState,x,y,text,just,style)
    mydata *userState; 	/* my state information  */
    int x,y;    	/* coords of text origin */
    char *text; 	/* what to put there */
    int just;   	/* how to justify */
    /* where the origin is relative to where the text should go
     * as a function of the various values of just 

    T_UPPERLEFT     T_TOP       T_UPPERRIGHT
    T_LEFT          T_CENTER    T_RIGHT
    T_LOWERLEFT     T_BOTTOM    T_LOWERRIGHT

    */
    int style;  	/* T_AXIS = axis font, T_TITLE = title font */
{
    int height, yadjust;

    /* Since MIF uses the text baseline for Y alignment, we may have */
    /* to adjust.  height = the height of the text in dots. */

    fprintf(userState->plotterFile,"<TextLine\n <GroupID 1>\n");
    switch(style) {
        case T_AXIS:
            fprintf(userState->plotterFile,
		" <Font <FFamily `%s'> <FSize %d> <FBold No>>\n",
                userState->axis_family,
                userState->axis_size);
	    height = rd( userState->axis_size * VDPI * INCHES_PER_POINT );
            break;
        case T_TITLE:
            fprintf(userState->plotterFile,
		" <Font <FFamily `%s'> <FSize %d> <FBold Yes>>\n",
                userState->title_family,
                userState->title_size);
	    height = rd( userState->title_size * VDPI * INCHES_PER_POINT );
            break;
        default:
            printf("bad text style %d in mifText\n",style);
            exit(1);
            break;
    }

    switch(just) {
        case T_UPPERLEFT:
            fprintf(userState->plotterFile," <TLAlignment Left>\n");
	    yadjust = height;
            break;
        case T_TOP:
            fprintf(userState->plotterFile," <TLAlignment Center>\n");
	    yadjust = height;
            break;
        case T_UPPERRIGHT:
            fprintf(userState->plotterFile," <TLAlignment Right>\n");
	    yadjust = height;
            break;
        case T_LEFT:
            fprintf(userState->plotterFile," <TLAlignment Left>\n");
	    yadjust = height/2;
            break;
        case T_CENTER:
            fprintf(userState->plotterFile," <TLAlignment Center>\n");
	    yadjust = height/2;
            break;
        case T_RIGHT:
            fprintf(userState->plotterFile," <TLAlignment Right>\n");
	    yadjust = height/2;
            break;
        case T_LOWERLEFT:
            fprintf(userState->plotterFile," <TLAlignment Left>\n");
	    yadjust = 0;
            break;
        case T_BOTTOM:
            fprintf(userState->plotterFile," <TLAlignment Center>\n");
	    yadjust = 0;
            break;
        case T_LOWERRIGHT:
            fprintf(userState->plotterFile," <TLAlignment Right>\n");
	    yadjust = 0;
            break;
        default:
            printf("bad justification type %d in mifText\n",just);
            exit(1);
            break;
    }
    fprintf(userState->plotterFile," <TLOrigin %f %f>\n",
    		MAPX(x),MAPY(y+yadjust));

    fprintf(userState->plotterFile," <String `%s'>>\n", text);
}



#ifdef NEVER
static int penselect[8] = { PEN1, PEN2, PEN3, PEN4, PEN5, PEN6, PEN7, PEN8};
#endif

static char* dashselect[] = {
    "<DashedStyle Solid>",
    "<DashedStyle Dashed><NumSegments 2><DashSegment 2pt><DashSegment 2pt>",
    "<DashedStyle Dashed><NumSegments 2><DashSegment 4pt><DashSegment 4pt>",
    "<DashedStyle Dashed><NumSegments 4><DashSegment 3pt><DashSegment 2pt><DashSegment 1pt><DashSegment 2pt>",
    "<DashedStyle Dashed><NumSegments 6><DashSegment 6pt><DashSegment 2pt><DashSegment 1pt><DashSegment 2pt><DashSegment 1pt><DashSegment 2pt>",
    "<DashedStyle Dashed><NumSegments 2><DashSegment 6pt><DashSegment 6pt>",
    "<DashedStyle Dashed><NumSegments 6><DashSegment 6pt><DashSegment 2pt><DashSegment 1pt><DashSegment 2pt><DashSegment 3pt><DashSegment 2pt>",
    "<DashedStyle Dashed><NumSegments 4><DashSegment 8pt><DashSegment 2pt><DashSegment 4pt><DashSegment 2pt>",
};

static void 
mifSeg(userState,ns,segs,width,style,lappr,color)
    mydata *userState;    /* my state information (not used) */
    int ns;         /* number of segments */
    XSegment *segs; /* X array of segments */
    int width;      /* width of lines in pixels */
    int style;      /* L_VAR = dotted, L_AXIS = grid, L_ZERO = axis*/
    int lappr;      /* line style */
    int color;      /* line color */
{
    int i;

    /* Group all objects so they can be moved together */
    fprintf(userState->plotterFile,"<PolyLine\n <GroupID 1>\n");
    fprintf(userState->plotterFile,"<HeadCap Butt>\n");
    if (style == L_ZERO) {
        fprintf(userState->plotterFile," <PenWidth .5>\n");
        fprintf(userState->plotterFile,
		" <DashedPattern <DashedStyle Solid>>\n");
    } else if (style == L_AXIS) {
        fprintf(userState->plotterFile," <PenWidth .5>\n");
        fprintf(userState->plotterFile,
		" <DashedPattern <DashedStyle Solid>>\n");
    } else if (style == L_VAR) {
	/* color is ignored */

	/* line type */
        if( (lappr < 0) || (lappr >7) ) {
            printf("out of range line style %d in mifLine\n",lappr);
            exit(1);
        }
        fprintf(userState->plotterFile," <DashedPattern %s>\n",
		dashselect[lappr]);

	/* pen width */
	if(width<=1)
            fprintf(userState->plotterFile," <PenWidth .5>\n");
	else
            fprintf(userState->plotterFile," <PenWidth %dpt>\n",width/12);
    } else {
        printf("unknown style %d in mifLine\n",style);
        exit(1);
    }
    for(i=0;i<ns;i++) {
	if(!i)
            fprintf(userState->plotterFile," <Point %f %f>\n",
                    MAPX(segs[i].x1),
                    MAPY(segs[i].y1));
        else {
	    if((segs[i].x1!=segs[i-1].x2) ||
	    (segs[i].y1!=segs[i-1].y2) ) {
                /* MOVE -- create a new segment */

	        /* First, terminate the existing one */
                fprintf(userState->plotterFile,">\n");

	        /* Use a recursive call to create a new one */
                mifSeg(userState,ns-i,&(segs[i]),width,style,lappr,color);
	        return;
            }
        }
        /* DRAW */
        fprintf(userState->plotterFile," <Point %f %f>\n",
                    MAPX(segs[i].x2),
                    MAPY(segs[i].y2));
    }
    fprintf(userState->plotterFile,">\n");
}

/* bullet, plus, times, diamond, heart, pound, lozenge, club */
static char *markselect[8] =
	{ "\xb7", "+", "\xb4", "\xa8", "\xa9", "#", "\xe0", "\xa7"};

static void 
mifDot(userState,x,y,style,type,color)
    mydata *userState;    /* my state information (not used) */
    int x,y;    /* coord of dot */
    int style;  /* type of dot */
    int type;   /* dot style variation */
    int color;  /* color of dot */
{
    fprintf(userState->plotterFile,"<TextLine\n  <TLAlignment Center>\n");
    fprintf(userState->plotterFile,"  <TLOrigin %f %f>\n", MAPX(x), MAPY(y));
    /* Use symbol font, centered vertically */

    if(style == P_PIXEL) {
	fprintf(userState->plotterFile,
		"  <Font <FFamily Symbol> <FSize 10> <FDY 3.5pt>>\n");
	fprintf(userState->plotterFile,"  <String `\xd7'>\n"); /* dot */
    } else if (style == P_DOT) {
	fprintf(userState->plotterFile,
		"  <Font <FFamily Symbol> <FSize 10> <FDY 3pt>>\n");
	fprintf(userState->plotterFile,"  <String `\xb7'>\n"); /* bullet */
    } else if (style == P_MARK) {
        if( (type<0) || (type>7) ) {
            printf("unknown marker type %d in mifDot\n",type);
            exit(1);
        }
	fprintf(userState->plotterFile,
		"  <Font <FFamily Symbol> <FSize 8> <FDY 3pt>>\n");
	fprintf(userState->plotterFile,"  <String `%s'>\n", markselect[type]);
    } else {
        printf("unknown marker style %d in mifDot\n",style);
        exit(1);
    }
    fprintf(userState->plotterFile,">\n");
}

static void 
mifEnd(userState)
    mydata *userState;    /* my state information (not used) */

{
    fprintf(userState->plotterFile,"<Group <ID 1>>\n");
    fflush(userState->plotterFile);
    return;
}
