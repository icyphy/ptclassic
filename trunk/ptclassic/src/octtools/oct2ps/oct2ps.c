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
 * WARNING!!!!!    THIS PROGRAM HAS EVOLVED FROM A VERY SIMPLE PROGRAM
 * TO ONE WITH FAR TOO MANY OPTIONS.  IT HAS MANY WARTS AND I CAN'T
 * EVEN REMEMBER WHY SOME OF THE CODE EXISTS (SEE THE #ifdef HUH).
 *
 * oct2ps
 *
 * Generate a PostScript representation of an OCT facet
 *
 * PostScript is a trademark of Adobe.
 */

#include "copyright.h"

/*
 * the output of this program is a PostScript representation
 * of an OCT facet.  the representation is broken up in
 * the following way:
 *
 *   %!                                     [included for all but -L]
 *   oct2ps functions                       [generated from prologue file, -P]
 *   geometric data and font information    [generated from the OCT facet]
 *   showpage                               [included for all but -L]
 *
 * if oct2ps is invoked with the -L flag, the output is meant to
 * be included in a LaTeX document, so '%!' and 'showpage' are removed
 *
 */

#include "port.h"
#include "oct.h"
#include "tr.h"
#include "st.h"
#include "tap.h"
#include "oh.h"
#include "options.h"
#include "utility.h"
#include "da.h"
#include "lel.h"
#include "vov.h"

extern char *ctime();
extern long time();

extern int octIdCmp(), octIdHash();

static void processCell();
static void processTerminals();
static void setTechnology();
static void drawPath();
static char *tail();
static int layerGenCompare();

/* transform stack */
tr_stack *stack;

/* font table */
st_table *TechnologyTable;

/* layer table */
st_table *LayerTable;

struct layerdescription {
    int name;
    int outlinep;
    int fillp;
    int colorp;
    int priority;
    double red;
    double green;
    double blue;
};


struct dLayer {
    octObject layer;
    struct layerdescription* description;
};

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif
#define STREQ(a,b)	(strcmp((a), (b)) == 0)

#define BOXNORM(box) \
    { \
 	octCoord temp;                                                   \
        temp = MAX((box).lowerLeft.x, (box).upperRight.x);              \
        (box).lowerLeft.x = MIN((box).lowerLeft.x, (box).upperRight.x); \
        (box).upperRight.x = temp;					\
        temp = MAX((box).lowerLeft.y, (box).upperRight.y);              \
        (box).lowerLeft.y = MIN((box).lowerLeft.y, (box).upperRight.y); \
        (box).upperRight.y = temp;                                      \
    }	 

#define MAXLAYERS 256		/* Maximum number of layers */

#define AUTOMATIC 0
#define PORTRAIT 1
#define LANDSCAPE 2

/* 72 points per inch in PostScript */
#define POINTSPERINCH 72.0

/* default font for labels, terminal names and instance names */
#define FONT "Times-Bold"
/* default font size */
#define FONTSIZE 7

/* width and height of the layer font, must be a multiple of 8 */
#define LAYERFONTWIDTH 32

/*
 * special codes for empty and filled layers
 *
 * EMPTY_LAYER is for layers with bit patterns of all '0', no geometry
 * will be output for them.
 *
 * FILLED_LAYER is for layers with bit patterns of all '1', the tiling
 * routines in PROLOGUE special case this and output filled boxes.
 *
 * if the value of FILLED_LAYER is changed it also must be changed
 * in PROLOGUE.
 *
 * the numbers 0 and 256 must be outside the expected range of numbers
 * that will be generated for a layer
 */
#define EMPTY_LAYER 0
#define FILLED_LAYER 256
	      
/* page dimensions and non-printable region */
#define DIM1 8.5
#define DIM2 11.0
#define NONPRINT 0.6

#define FORMAL 0
#define ACTUAL 1

char facetType[32];
char prologueFile[1024];
char *techDirRoot;
char fontName[32];
int plotFormals = 0;
int plotActuals = 0;
int plotConnectors = 0;
int plotCellLabel = 0;
int plotLabels = 0;
int plotOutlines = 0;
int plotColor = 0;
int alwaysDisplayLabel = 1;
int Abstraction = 0;
double xoffset = 0.0;
double yoffset = 0.0;

double scale;


optionStruct optionList[] = {
    { "p",	0,		"plot in portrait mode, scaling to fit on the " },
    { OPT_CONT,	0,		"page or in the box defined by -b" },
    { "l",	0,		"plot in landscape mode, scaling to fit on the " },
    { OPT_CONT,	0,		"page or in the box defined by -b" },
    { "a",	0,		"choose -l or -p, which ever gives the biggest plot (default)" },
    { "x",	0,		"plot the Oct labels" },
    { "X",	0,		"used with `-x', only draw labels that fit in the bounding box of the label" },
    { "t",	0,		"plot the formal terminals (bb and name)" },
    { "C",	0,		"generate code for color postscript printers" },
    { "n",	0,		"plot the actual terminals (bb and name)" },
    { "c",	0,		"label the connector instance (when using `-f bb')" },
    { "N",	0,		"put the name and date in the lower left of the plot" },
    { "B",	0,		"plot the outlines of the geometries" },
    { "b",	"WxH",		"width and height to fit the plot in " },
    { OPT_CONT,	0,		"(in inches, defaults to the full page)" },
    { "O",	"XxY",		"additional offset (in inches)" },
    { "s",	"scale",	"multiply the scale facet determined by -a, -l, or -p " },
    { OPT_CONT,	0,		"by `scale' (defaults to 1.0)" },
    { "A",	"abs",		"do not plot anything that is smalled then " },
    { OPT_CONT,	0,		"`abs' points (there are 72 points per inch)" },
    { "f",	"facet",	"type of facet to use for instances " },
    { OPT_CONT,	0,		"(contents, interface, or bb; defaults to contents)" },
    { "o",      "outfile",      "output file name (default to stdout)" },
    { "P",	"prologue",	"replace the standard PostScript prologue file with `prologue'" },
    { "T",	"techdir",	"replace the standard technology directory root with `techdir'" },
    { "F",	"font/size",	"change the font and size (defaults to Times-Bold/7)" },
    { "S",	"layers",	"suppress the comma separated list of layers" },
    { "L",	0,		"output the PostScript in a form that can be " },
    { OPT_CONT,	0,		"use by the `special' command of `latex' and `dvi2ps'" },
    { OPT_RARG,	"cell:view[:facet]",	"facet defaults to `contents'" },
    { OPT_DESC, 0,		"The output of the program is PostScript that can be sent to a PostScript printer" },
    { 0,	0,		0 }
};


int main(argc,argv)
int argc;
char **argv;
{
    octObject facet;
    octStatus status;
    struct octBox bb, box;
    double scalep, scalel, scalexp, scaleyp, scalexl, scaleyl;
    double userscale = 1.0;
    int mode = AUTOMATIC;
    int option;
    extern int optind;
    extern char *optarg;
    int i, c;
    char *temp, *ptr;
    int fontSize;
    FILE *fp;
    double width, height;
    int boxscale = 0;
    double dim1 = DIM1;
    double dim2 = DIM2;
    int latex = 0;
    long clock;
    double nonprint = NONPRINT;
    int bbdx, bbdy;
    char *outFileName = 0;

    extern char *version;
    
    (void) strcpy(facetType, "contents");
    (void) sprintf(prologueFile, "%s/lib/oct2ps.pro", "$OCTTOOLS");
    techDirRoot = NIL(char);

    (void) strcpy(fontName, FONT);
    fontSize = FONTSIZE;

    LayerTable = st_init_table(strcmp, st_strhash);

    while ((option = optGetOption(argc, argv)) != EOF) {
	switch (option) {
	    case 'a': mode = AUTOMATIC; break;
	    case 'p': mode = PORTRAIT; break; 
            case 'l': mode = LANDSCAPE; break; 
	    case 's':
                userscale = atof(optarg);
		if (userscale <= 0.0) {
		    (void) fprintf(stderr, "zero or negative scales not allowed\n");
		    VOVend(-1);
		}
                break;

	    case 'A':
                Abstraction = atof(optarg);
		if (Abstraction < 0.0) {
		    (void) fprintf(stderr, "negative abstractions not allowed\n");
		    VOVend(-1);
		}
                break;

    	    case 'f': (void) strcpy(facetType, optarg); break; 
	    case 'b':
		(void) sscanf(optarg, "%lfx%lf", &width, &height);
		if ((height < 0.1) || (width < 0.1)) {
		    (void) fprintf(stderr, "too small of a box\n");
		    VOVend(-1);
		}
                boxscale = 1;
                break;

	    case 'O': (void) sscanf(optarg, "%lfx%lf", &xoffset, &yoffset); break; 
	    case 'o': outFileName = util_strsav( optarg ); break;
    	    case 'P': (void) strcpy(prologueFile, optarg); break; 
    	    case 'S':
		ptr = optarg;
		while ((ptr = strtok(ptr, ",")) != NIL(char)) {
		    (void) st_insert(LayerTable, ptr, (char *)0);
		    ptr = NIL(char);
                }
		break;

    	    case 'T':
		techDirRoot = (char *) malloc((unsigned) strlen(optarg) + 1);
		(void) strcpy(techDirRoot, optarg);
		break;

    	    case 'F':
		(void) sscanf(optarg, "%[^/]/%d", fontName, &fontSize);
		if (fontSize < 0.1) {
		    (void) fprintf(stderr, "font size too small\n");
		    VOVend(-1);
		}
		break;

    	    case 'n': plotActuals = 1; break; 
    	    case 't': plotFormals = 1; break; 
    	    case 'C': plotColor = 1; break; 
    	    case 'x': plotLabels = 1; break; 
    	    case 'X': alwaysDisplayLabel = 0; break; 
    	    case 'c': plotConnectors = 1; break; 
    	    case 'N': plotCellLabel = 1; break; 
	    case 'B': plotOutlines = 1; break; 
    	    case 'L': latex = 1; break;
	    default: optUsage();
	}
    }

    if (argc == optind+1) {
	ohUnpackDefaults(&facet, "r", "::contents");
	if (ohUnpackFacetName(&facet, argv[optind]) != OCT_OK) {
	    optUsage();
	}
    } else {
	optUsage();
    }
    
    VOVbegin( argc, argv );
    VOVredirect( 0 );		/* Do not redirect stdout */

    if ( outFileName ) {
	VOVoutput( VOV_UNIX_FILE, outFileName );
	if ( freopen( util_file_expand(outFileName), "w", stdout ) == 0 ) {
	    perror( "opening out file" );
	}
    }

    VOVinputFacet( &facet );

    if ((status = octOpenFacet(&facet)) < OCT_OK) {
	octError("can not open the facet");
	VOVend(-1);
    }

    if (status == OCT_INCONSISTENT) {
	ohPrintInconsistent(&facet, stderr);
	VOVend(-1);
    }

    /* set up tap */
    if (techDirRoot != NIL(char)) {
	(void) tapRootDirectory(techDirRoot);
    }
    
    if (plotColor) {
        (void) tapSetDisplayType("PostScript-Color", TAP_COLOR);
    } else {
        (void) tapSetDisplayType("PostScript-BW", TAP_BW);
    }

    /* copy the prologue file to standard output */

    temp = util_file_expand(prologueFile);
    if ((fp = VOVfopen(temp, "r")) == NULL) {
	(void) fprintf(stderr, "can not open the prologue file, %s (%s)\n",
			       prologueFile, temp);
	VOVend(-1);
    }

    if (latex == 0) {
	/* meant to go directly to the printer */
	(void) printf("%%!\n");
	(void) time(&clock);
	(void) printf("%% %s:%s:%s - output from oct2ps on %s",
		      facet.contents.facet.cell,
		      facet.contents.facet.view,
		      facet.contents.facet.facet,
		      ctime(&clock));
    } else {
	/* override a few modes for included PostScript */
	mode = PORTRAIT;
	userscale = 1.0;
	(void) time(&clock);
	(void) printf("%% %s:%s:%s - output for a latex/dvi2ps special - from oct2ps on %s",
		      facet.contents.facet.cell,
		      facet.contents.facet.view,
		      facet.contents.facet.facet,
		      ctime(&clock));
    }

    (void) printf("%% OCT2PS (compiled %s)\n", version);

    /* output the command line */
    (void) printf("%%\n%%");
    for (i = 0; i < --argc; i++) {
	(void) printf(" %s", argv[i]);
    }
    (void) printf(" %s:%s:%s", facet.contents.facet.cell,
		  facet.contents.facet.view,
		  facet.contents.facet.facet);
    (void) printf("\n%%\n");

    /* copy the prologue to standard output */
    while ((c = fgetc(fp)) != EOF) {
	putc(c, stdout);
    }

    (void) fclose(fp);


    /* determine the scaling and translation */
    
    OH_ASSERT_DESCR(octBB(&facet, &bb), "bounding box of the facet");

    bbdx = bb.upperRight.x - bb.lowerLeft.x;
    bbdy = bb.upperRight.y - bb.lowerLeft.y;

    if ((bbdx == 0) || (bbdy == 0)) {
	(void) fprintf(stderr, "OCT2PS: Fatal Error: the width or height of the facet bounding box is zero\n");
	VOVend(-1);
    }

    if (boxscale == 1) {
	dim1 = width;
	dim2 = height;
	nonprint = 0.0;
    }

    if (latex == 1) {
	nonprint = 0.0;
    }

    box.lowerLeft.x = box.lowerLeft.y = 0;
    box.upperRight.x = box.upperRight.y = 0;

    scalexp = ((dim1 - nonprint) * POINTSPERINCH) / bbdx;
    scaleyp = ((dim2 - nonprint) * POINTSPERINCH) / bbdy;
    scalep = MIN(scalexp, scaleyp);
    
    scalexl = ((dim2 - nonprint) * POINTSPERINCH) / bbdx;
    scaleyl = ((dim1 - nonprint) * POINTSPERINCH) / bbdy;
    scalel = MIN(scalexl, scaleyl);

    stack = tr_create();

    if (mode == AUTOMATIC) {
	if (scalel > scalep) {
	    mode = LANDSCAPE;
	} else {
	    mode = PORTRAIT;
	}
    }
    
    (void) printf("/fctm matrix currentmatrix def\n");

    (void) printf("%d %d translate\n",
		  (int) (POINTSPERINCH * xoffset),
		  (int) (POINTSPERINCH * yoffset));

    box.lowerLeft.x += (int) (xoffset * POINTSPERINCH);
    box.lowerLeft.y += (int) (yoffset * POINTSPERINCH);
		  
    if (mode == PORTRAIT) {
	scale = scalep * userscale;
	/* center in X and Y */
	if (latex == 0) {
	    (void) printf("%d %d translate\n",
		   (int) (((POINTSPERINCH * DIM1) - scale * bbdx) / 2.0),
		   (int) (((POINTSPERINCH * DIM2) - scale * bbdy) / 2.0));

	    box.lowerLeft.x += 
		   (int) (((POINTSPERINCH * DIM1) - scale * bbdx) / 2.0);
	    box.lowerLeft.y += 
		   (int) (((POINTSPERINCH * DIM2) - scale * bbdy) / 2.0);

	} else {
	    /* used to be center of page for x and box for y */
	    /* changed to lower left for both -- EAL 1/5/94  */
	    (void) printf("0 0 translate\n");
	}
	
    } else {
	scale = scalel * userscale;
	/* center in X and Y */
	(void) printf("90 rotate\n");
	(void) printf("%d %d translate\n",
	       (int) (((POINTSPERINCH * DIM2) - scale * bbdx) / 2.0),
	       (int) (((POINTSPERINCH * DIM1) - scale * bbdy) / 2.0));

	box.lowerLeft.x += 
	       (int) (((POINTSPERINCH * DIM2) - scale * bbdx) / 2.0);
	box.lowerLeft.y += 
	       (int) (((POINTSPERINCH * DIM1) - scale * bbdy) / 2.0);

	(void) printf("0 %d translate\n", (int) (0 - (POINTSPERINCH * DIM1)));

	box.lowerLeft.y += (int) (0 - (POINTSPERINCH * DIM1));

    }

    box.upperRight.x = (int) (bbdx * scale) + box.lowerLeft.x;
    box.upperRight.y = (int) (bbdy * scale) + box.lowerLeft.y;

    (void) printf("%%%%BoundingBox: %d %d %d %d\n",
		   (int)box.lowerLeft.x,
		   (int)box.lowerLeft.y,
		   (int)box.upperRight.x,
		   (int)box.upperRight.y);
    
    (void) printf("/sc %f def\n", scale);
    (void) printf("/fsize %f def\n", (double) fontSize);
    (void) printf("/fscale fsize sc div def\n");
    /*
     * psize is the size of a bitmap in PostScript units, independent of scaling
     *
     * this particular number seems to work fine
     */
    (void) printf("/psize %d def\n", LAYERFONTWIDTH / 4);
    (void) printf("/scsize psize sc div def\n");
    (void) printf("/lfont { /%s findfont fscale scalefont setfont } def\n",
		       fontName);
    (void) printf("/bmw scsize def\n");
    (void) printf("/bmh scsize def\n");
    (void) printf("sc sc scale\n");
    (void) printf("%d %d translate\n", (int)(-bb.lowerLeft.x),
		  (int)(-bb.lowerLeft.y));

    TechnologyTable = st_init_table(octIdCmp, octIdHash);

    /* generate the PostScript for the facet */
    processCell((octObject *) 0, &facet, 0);

    if (plotCellLabel == 1) {
	(void) printf("lfont\n");
	(void) printf("%d %d moveto (%s) show\n",
	       (int)bb.lowerLeft.x,
	       (int)bb.lowerLeft.y,
	       facet.contents.facet.cell);
    }

    if (latex == 0) {
	/* don't output the page for an included PostScript image */
	(void) printf("showpage\n");
    }

    if ( outFileName ) {
	fflush( stdout );
	fclose( stdout );
    }
    VOVend(0);
    return(0);
}


/* labels */

typedef struct lbl_desc {
    dary line;			/* Line itself (type is char) */
} lblDesc;

#define LABEL_LINES	10
#define LABEL_LENGTH	40

/* Maintains space for decomposing labels into lines */
static dary lblInfo = (dary) 0;	/* Type is (lblDesc) */

static void
prep_line(count)
int count;
/*
 * Sets up the `count' line in the global dary `lblInfo'.
 */
{
    if (daLen(lblInfo) <= count) {
	lblDesc init;
	
	init.line = daAlloc(char, LABEL_LENGTH);
	*daSet(lblDesc, lblInfo, count) = init;
    } else {
	daReset(daGet(lblDesc, lblInfo, count)->line);
    }
}

static int
decompLbl(lbl_str)
char* lbl_str;			/* Label string */
/*
 * This routine discovers line breaks in `lbl_str'.  The decomposed
 * label is written into the dynamic array of lines `lblInfo'.  This
 * array also contains line length information.  The routine returns
 * the number of lines found in the label (which may be zero).
 */
{
    char *idx;
    int count = 0;
    dary local_line;

    /* if (!lblInfo) */ lblInfo = daAlloc(lblDesc, LABEL_LINES);
    count = 0;
    prep_line(count);
    if (lbl_str) {
	for (idx = lbl_str;  *idx;  idx++) {
	    if (*idx == '\n') {
		/* Line break */
		local_line = daGet(lblDesc, lblInfo, count)->line;
		prep_line(++count);
	    } else {
		local_line = daGet(lblDesc, lblInfo, count)->line;
		if ((*idx == '(') || (*idx == ')')) {
		    *daSetLast(char, local_line) = '\\';
		}
		*daSetLast(char, local_line) = *idx;
	    }
	}
	/* Final null termination */
	local_line = daGet(lblDesc, lblInfo, count)->line;
    } else {
	return 0;
    }
    return count+1;
}


#ifdef NEVER
/*Formerly called in dspLabel()*/
static char *
parens(string)
char *string;
{
    static char buffer[1024];
    char *ptr;
    int i = 0;

    for (ptr = string; *ptr != 0; ptr++) {
	if (*ptr == '(') {
	    buffer[i++] = '\\';
	}
	if (*ptr == ')') {
	    buffer[i++] = '\\';
	}
	buffer[i++] = *ptr;
    }
    buffer[i] = '\0';
    return buffer;
}
#endif

/* Round a double argument and return an int.  Not all platforms
 * (hppa) have an irint() and/or rint() call.
 */
static int ptirint(x)
double x;
{
  return ( (x-(int)(x) < 0.5) ? (int)x : (int)(x+1) ); 
}

static void
dspLabel(oneLabel)
struct octLabel *oneLabel;
/*
 * Draws a label.  First an appropriate font is chosen based on
 * the text height.  Then the text is broken into lines (with
 * pixel widths).  Then the starting position is computed and
 * each line is rendered according to the specified justifications.
 * The color of the label is set by the current attribute.  If
 * the text cannot be rendered,  the bounding box outline is
 * drawn instead.   If `opt' contains DSP_BOX,  the reference
 * box is always drawn.
 */
{
    int idx, lines;
    int maxheight;			/* size of text */
    int regwidth, regheight;		/* region size  */

    /* Now break lines in text */
    lines = decompLbl(oneLabel->label);

    /* Check to see if we can reject the label */
    maxheight = lines * oneLabel->textHeight;
    regheight = ABS(oneLabel->region.lowerLeft.y - oneLabel->region.upperRight.y);
    if (maxheight > regheight) {
	return;
    }

    printf("%%\n");
    printf("gsave\n");
    /* the font is scaled down slightly here */
    printf("/Helvetica findfont %d scalefont setfont\n", 
	ptirint(0.8*oneLabel->textHeight));

    printf("/maxwidth 0 def\n");
    for (idx = 0;  idx < daLen(lblInfo);  idx++) {
	printf("/width (%s) stringwidth pop def\n", 
	       daData(char, daGet(lblDesc, lblInfo, idx)->line));
	printf("width maxwidth gt {/maxwidth width def} if\n");
    }
    regwidth = ABS(oneLabel->region.upperRight.x - oneLabel->region.lowerLeft.x);

    if (!alwaysDisplayLabel) {
	printf("maxwidth %d le {\n", regwidth);
    }

    /* Compute left margin */
    /* Shift label slightly to clear lines if left justified -- EAL 1/5/94 */
    switch (oneLabel->horizJust) {
    case OCT_JUST_LEFT:
	printf("/margin %d def\n", (int)oneLabel->region.lowerLeft.x + 2);
	break;
    case OCT_JUST_CENTER:
	printf("/temp %d maxwidth sub 2 div def\n", regwidth);
	printf("/margin %d temp add def\n", (int)oneLabel->region.lowerLeft.x);
	break;
    case OCT_JUST_RIGHT:
	printf("/temp %d maxwidth sub def\n", regwidth);
	printf("/margin %d temp add def\n", (int)oneLabel->region.lowerLeft.x);
	break;
    default:
	printf("label display: bad horizJust\n");
	VOVend(-1);
    }

    /* Compute starting vertical position */
    /* Shift label slightly to clear lines if bottom justified -- EAL 1/5/94 */
    switch (oneLabel->vertJust) {
    case OCT_JUST_BOTTOM:
	printf("/y %d def\n",
	   (int)(2 + oneLabel->region.lowerLeft.y + maxheight - oneLabel->textHeight));
	break;
    case OCT_JUST_CENTER:
	printf("/y %d def\n",
	       (int)(oneLabel->region.upperRight.y - (regheight - maxheight) / 2 - oneLabel->textHeight));
	break;
    case OCT_JUST_TOP:
	printf("/y %d %d sub def\n", 
	       (int)oneLabel->region.upperRight.y,
	       (int)oneLabel->textHeight);
	break;
    default:
	printf("label display: bad vertJust\n");
	VOVend(-1);
    }

    /* Begin drawing sequence */
    for (idx = 0;  idx < lines;  idx++) {
	switch (oneLabel->lineJust) {
	case OCT_JUST_LEFT:
	    printf("/x margin def\n");
	    break;
	case OCT_JUST_CENTER:
	    printf("/temp (%s) stringwidth pop def\n", 
		   daData(char, daGet(lblDesc, lblInfo, idx)->line));
	    printf("/x margin maxwidth temp sub 2 div add def\n");
	    break;
	case OCT_JUST_RIGHT:
	    printf("/temp (%s) stringwidth pop def\n", 
		   daData(char, daGet(lblDesc, lblInfo, idx)->line));
	    printf("/x margin maxwidth temp sub add def\n");
	    break;
	default:
	    printf("label display: bad lineJust\n");
	    VOVend(-1);
	}

	printf("x y moveto\n");
	/* removed call to "parens" for daData below - EAL */
	printf("(%s) show\n", daData(char, daGet(lblDesc, lblInfo, idx)->line));
	printf("/y y %d sub def\n", (int)oneLabel->textHeight);
    }
    if (!alwaysDisplayLabel) {
	printf("} if\n");
    }
    printf("grestore\n");
    printf("%%\n");

    return;
}


static void
processCell(parent, facet, level)
octObject *parent, *facet;
int level;
{
    octObject poly, box, path, inst, master, label, circle;
    struct dLayer layers[FILLED_LAYER];
    octObject connectors;
    struct octPoint points[1024];
    struct octBox bb;
    struct octTransform transform;
    octGenerator boxgen, polygen, pathgen, labelgen, layergen;
    octGenerator instgen, circlegen;
    int i,j;
    int32 numpoints;
    st_table *layerTable;
    char *bbname;
    int numLayers;
    

    level++;

    /* gsave for possible font change */
    (void) printf("gsave\n");
    (void) setTechnology(facet, &layerTable);

    /* get the oct transform to give to the octTransform functions */
    tr_get_oct_transform(stack, &transform);

    /*
     * 1. generate the instances and recursively call this routine
     *    a. generate the actual terminals and produce a label
     *        and bounding box (only at the top level, level == 1)
     * 2. generate over the layers and produce PostScript for each type of
     *    geometric object
     * 3. generate the formal terminals and produce a label
     *    and bounding box (only at the top level, level == 1)
     */


    /*
     * generate over all instances
     *
     * if the facetType is 'bb', produce an outline of the boundingo
     * box of the instance with the name of the master centered in it.
     * if level == 1, draw the bounding boxes of the actual terminal
     * implementations and put the terminal name in the upper right
     * of the terminal bounding box.
     *
     * otherwise, recursively call 'processCell'
     */
    
    OH_ASSERT_DESCR(octInitGenContents(facet, OCT_INSTANCE_MASK, &instgen),
	      "can not start the instance generator");

    if STREQ(facetType, "bb") {
	(void) printf("gsave\n");
	(void) printf("lfont\n");
	
	if (plotConnectors == 0) {
	    connectors.type = OCT_BAG;
	    connectors.contents.bag.name = "CONNECTORS";
	    if (octGetByName(facet, &connectors) < OCT_OK) {
		(void) fprintf(stderr, "%% can not get the CONNECTORS bag; plotting connector names\n");
		plotConnectors = 1;
	    }
	}
    }

    while (octGenerate(&instgen, &inst) == OCT_OK) {

	OH_ASSERT_DESCR(octBB(&inst, &bb), "can not get the inst bb");
	tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
	tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
	BOXNORM(bb);
	
	if (MAX((bb.upperRight.x - bb.lowerLeft.x),
		(bb.upperRight.y - bb.lowerLeft.y)) * scale > Abstraction) {
		
	    /* just display the bounding box for an instance */
	    if STREQ(facetType, "bb") {

		if ((plotConnectors == 1)
		    || (octIsAttached(&connectors, &inst) != OCT_OK)) {
		    /* build the name to center in the bounding box */
		    bbname = strrchr(inst.contents.instance.master, '/');
		    if (bbname == 0) {
			bbname = inst.contents.instance.master;
		    } else {
			bbname++;		/* get rid of '/' */
		    }
		    
		    (void) printf("%d %d %d %d (%s) ot\n",
				  (int)bb.lowerLeft.x, (int)bb.lowerLeft.y,
				  (int)bb.upperRight.x, (int)bb.upperRight.y,
				  bbname);
		} else {
		    (void) printf("%d %d %d %d con\n",
				  (int)bb.lowerLeft.x, (int)bb.lowerLeft.y,
				  (int)bb.upperRight.x, (int)bb.upperRight.y);
		}		
	    } else {
	    
		master.type = OCT_FACET;
		master.contents.facet.facet = facetType;
		master.contents.facet.mode = "r";
		OH_ASSERT_DESCR(octOpenMaster(&inst, &master), "can not open the master");

		tr_push(stack);
		tr_add_transform(stack, &inst.contents.instance.transform, 0);
		processCell(&inst, &master, level);
		tr_pop(stack);
	    }
	}

	if ((level == 1) && (plotActuals == 1)) {
	    master.type = OCT_FACET;
	    master.contents.facet.facet = "interface";
	    master.contents.facet.mode = "r";
	    OH_ASSERT_DESCR(octOpenMaster(&inst, &master), "can not open the master");
	    tr_push(stack);
	    tr_add_transform(stack, &inst.contents.instance.transform, 0);
	    processTerminals(&master, ACTUAL, STREQ(facetType, "bb") ? 1 : 0);
	    tr_pop(stack);
	}
    }

    if STREQ(facetType, "bb") {
	(void) printf("grestore\n");
    }

    
    /*
     * generate over the layers
     */

    OH_ASSERT_DESCR(octInitGenContents(facet, OCT_LAYER_MASK, &layergen),"layer gen");

    /*
       First make a sorted array of the layers based on there user
       priority.  We must generate the layers from lowest priority to
       highest so the postscript will adhere tothe user priorities.
    */	
    numLayers = 0;
    while (octGenerate(&layergen,&layers[numLayers].layer) == OCT_OK) {
        if (st_is_member(LayerTable, layers[numLayers].layer.contents.layer.name)) {
            continue;
        }

	/*
	 * each layer is a character in a font, pick the character
	 * value that coresponds to the layer
	 */

	if (st_lookup(layerTable, layers[numLayers].layer.contents.layer.name,
		      (char **) &layers[numLayers].description) == 0) {
	    (void) fprintf(stderr, "%% unknown layer, %s, skipping\n", 
			   layers[numLayers].layer.contents.layer.name);
	    continue;
	}
	numLayers++;
    }
    qsort(layers,numLayers,sizeof(struct dLayer),layerGenCompare);

    /* Iterate over all the layers */
    for (j = 0 ; j < numLayers ; j++) {
	octObject *layer;
	struct layerdescription *layerDescription;
	layer = &layers[j].layer;
	layerDescription = layers[j].description;

	/*
	 * if the layer pattern is all zeros or ones, special case it
	 * (in the code that draws geometry - in the prologue)
	 */

	/*
	 * set the layer
	 *
	 * all geometries will be draw in this layer
	 */
	(void) printf("/bm %d def\n", layerDescription->name);
        if (plotColor && layerDescription->colorp) {
            (void) printf("%f %f %f setrgbcolor\n", 
				layerDescription->red,
				layerDescription->green,
				layerDescription->blue);
        }

	/*
	 * generate all polygons
	 *
	 * the PostScript generated is a clip path that represents
	 * the outline of the polygon and then a command to tile (stipple)
	 * fill the bounding box of the polygon (which will be clipped
	 * to the outline of the polygon)
	 *
	 * the polygon is surrounded by "gsave grestore" to undo the
	 * effects of the clip path
	 *
	 * the command is to tile fill is 'tp' which 
	 * tile fills a box using an existing clipping region
	 */
	
	OH_ASSERT_DESCR(octInitGenContents(layer, OCT_POLYGON_MASK, &polygen), "polygon gen");

	while (octGenerate(&polygen, &poly) == OCT_OK) {

	    OH_ASSERT_DESCR(octBB(&poly, &bb), "can not get the polygon bb");
	    tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
	    tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
	    BOXNORM(bb);

	    if (MAX((bb.upperRight.x - bb.lowerLeft.x),
		    (bb.upperRight.y - bb.lowerLeft.y)) * scale > Abstraction) {
		
		(void) printf("gsave\n");
    
		numpoints = sizeof(points)/sizeof(struct octPoint);
		OH_ASSERT_DESCR(octGetPoints(&poly, &numpoints, points),
			  "can not get the polygon points");
		octTransformPoints(numpoints, points, &transform);
	    
		(void) printf("newpath\n");
		(void) printf("%d %d moveto\n",
			      (int)points[0].x, (int)points[0].y);
		for (i = 1; i < numpoints; i++) {
		    (void) printf("%d %d lineto\n",
				  (int)points[i].x, (int)points[i].y);
		}
		(void) printf("closepath clip\n");
		
		(void) printf("%d %d %d %d tp\n",
			      (int)bb.lowerLeft.x, (int)bb.lowerLeft.y,
			      (int)bb.upperRight.x, (int)bb.upperRight.y);
	    
		(void) printf("grestore\n");

		/* see if the outline should be generated */
		if ((layerDescription->outlinep == 1) || (plotOutlines == 1)) {
		    (void) printf("newpath\n");
		    (void) printf("%d %d moveto\n", 
				  (int)points[0].x, (int)points[0].y);
		    for (i = 1; i < numpoints; i++) {
			(void) printf("%d %d lineto\n", 
				      (int)points[i].x, (int)points[i].y);
		    }
		    (void) printf("closepath stroke\n");
		}
	    }
	}

	
	/*
	 * generate all circles
	 *
	 * if the inner radius of the circle is 0,
	 * the PostScript generated is a clip path that represents
	 * the outline of the circle and then a command to tile (stipple)
	 * fill the bounding box of the circle (which will be clipped
	 * to the outline of the circle)
	 *
	 * if the inner radius of the circle is equal to the outer radius,
	 * the PostScript generated is an arc with no fill.
	 *
	 * otherwise, use outer radius.....
	 */
  
	OH_ASSERT_DESCR(octInitGenContents(layer, OCT_CIRCLE_MASK, &circlegen),
		  "can not start the circle generator");
	
	while (octGenerate(&circlegen, &circle) == OCT_OK) {

	    OH_ASSERT_DESCR(octBB(&circle, &bb), "can not get the circle bb");
	    tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
	    tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
	    BOXNORM(bb);

	    if (MAX((bb.upperRight.x - bb.lowerLeft.x),
		    (bb.upperRight.y - bb.lowerLeft.y)) * scale > Abstraction) {
		
		octTransformGeo(&circle, &transform);
	    
		(void) printf("gsave\n");
		(void) printf("newpath\n");
	    
		if (circle.contents.circle.outerRadius == circle.contents.circle.innerRadius) {
		    (void) printf("%d %d %d %f %f arc\n",
				  (int)circle.contents.circle.center.x,
				  (int)circle.contents.circle.center.y,
				  (int)circle.contents.circle.outerRadius,
				  (double) circle.contents.circle.startingAngle / 10.0,
				  (double) circle.contents.circle.endingAngle / 10.0);

		    (void) printf("stroke\n");
		} else if (circle.contents.circle.innerRadius == 0) {
		    (void) printf("%d %d %d %f %f arc\n",
				  (int)circle.contents.circle.center.x,
				  (int)circle.contents.circle.center.y,
				  (int)circle.contents.circle.outerRadius,
				  (double) circle.contents.circle.startingAngle / 10.0,
				  (double) circle.contents.circle.endingAngle / 10.0);

		    (void) printf("clip\n");
		    OH_ASSERT_DESCR(octBB(&circle, &bb), "can not get the circle bb");
		    tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
		    tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
		    BOXNORM(bb);
		    (void) printf("%d %d %d %d tp\n",
				  (int)bb.lowerLeft.x, (int)bb.lowerLeft.y,
				  (int)bb.upperRight.x, (int)bb.upperRight.y);
		} else {
		    (void) printf("%d %d %d %f %f arc\n",
				  (int)circle.contents.circle.center.x,
				  (int)circle.contents.circle.center.y,
				  (int)circle.contents.circle.outerRadius,
				  (double) circle.contents.circle.startingAngle / 10.0,
				  (double) circle.contents.circle.endingAngle / 10.0);
		    (void) printf("stroke\n");
		}
		(void) printf("grestore\n");
	    }
	}

	/*
	 * generate all boxes
	 *
	 * the PostScript generated is a command to tile (stipple)
	 * fill a box clipped to the box
	 *
	 * the command is 'tb' which creates a clip path the size
	 * of the box and then tile fills it
	 */
	
	OH_ASSERT_DESCR(octInitGenContents(layer, OCT_BOX_MASK, &boxgen),
		  "can not start the box generator");

	while (octGenerate(&boxgen, &box) == OCT_OK) {

	    OH_ASSERT_DESCR(octBB(&box, &bb), "can not get the box bb");
	    tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
	    tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
	    BOXNORM(bb);
		
	    if (MAX((bb.upperRight.x - bb.lowerLeft.x),
		    (bb.upperRight.y - bb.lowerLeft.y)) * scale > Abstraction) {
		
		octTransformGeo(&box, &transform);
		
		BOXNORM(box.contents.box);
	    
		(void) printf("%d %d %d %d tb\n",
			      (int)box.contents.box.lowerLeft.x,
			      (int)box.contents.box.lowerLeft.y,
			      (int)box.contents.box.upperRight.x,
			      (int)box.contents.box.upperRight.y);
		
		if ((layerDescription->outlinep == 1) || (plotOutlines == 1)) {
		    (void) printf("%d %d %d %d bx\n",
				  (int)box.contents.box.lowerLeft.x,
				  (int)box.contents.box.lowerLeft.y,
				  (int)box.contents.box.upperRight.x,
				  (int)box.contents.box.upperRight.y);
		}
	    }
	}

	
	/* 
	 * generate over all labels
	 *
	 * produces PostScript commands to set the font and scale it
	 * an to call a command which outputs a text string at a point
	 *
	 * the command is 'lb' which outputs a text string at a point
	 * in the current font
	 */
	
	if (plotLabels == 1) {

	    OH_ASSERT_DESCR(octInitGenContents(layer, OCT_LABEL_MASK, &labelgen),
		"can not start the label generator");

	    while (octGenerate(&labelgen, &label) == OCT_OK) {
		octTransformGeo(&label, &transform);
		lelEvalLabel(parent, &label, "interface");
		dspLabel(&label.contents.label);
	    }
	}


	/* 
	 * generate over all paths
	 *
	 * see 'drawPath' for more information
	 */
	
	OH_ASSERT_DESCR(octInitGenContents(layer, OCT_PATH_MASK, &pathgen),
	          "can not start the path generator");

	while (octGenerate(&pathgen, &path) == OCT_OK) {

	    OH_ASSERT_DESCR(octBB(&path, &bb), "can not get the path bb");
	    tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
	    tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
	    BOXNORM(bb);
	    
	    if (MAX((bb.upperRight.x - bb.lowerLeft.x),
		    (bb.upperRight.y - bb.lowerLeft.y)) * scale > Abstraction) {
		
		numpoints = sizeof(points)/sizeof(struct octPoint);
		OH_ASSERT_DESCR(octGetPoints(&path, &numpoints, points),
			  "can not get the path points");
		octTransformPoints(numpoints, points, &transform);
		octTransformGeo(&path, &transform);
		drawPath((int)path.contents.path.width, (int)numpoints, points,
		     (plotOutlines | layerDescription->outlinep), layerDescription->fillp);
	    }
	}
    }

    /* 
     * generate over all formal terminals
     *
     * if level == 1 the formal terminals are the formal terminals
     * of the top level cell
     */

    if ((plotFormals == 1) && (level == 1)) {
	processTerminals(facet, FORMAL, 0);
    }

    /* grestore to old font */
    (void) printf("grestore\n");

    return;
}


/*
 * generate the terminals attached to 'container', find the
 * implementations, produce bounding box outlines and labels
 * (lower left corner labels for formal terminals, upper right
 * corner labels for actual terminals).
 *
 * the PostScript produced is either a call to 'ft' or 'at'
 * which produce terminal bounding boxes with labels in the
 * proper locations.
 */

static void
processTerminals(container, type, bbp)
octObject *container;
int type;			/* ACTUAL or FORMAL */
int bbp;			/* 0 - normal, 1 - font already in effect */
{
    octGenerator termgen, geogen;
    octObject term, geo, master, inst;
    struct octBox bb;
    int fontset = 0;
    
    OH_ASSERT_DESCR(octInitGenContents(container, OCT_TERM_MASK, &termgen),
	      "can not start the terminal generator");

    while (octGenerate(&termgen, &term) == OCT_OK) {
	    
	/*
	 * find the implementation
	 */

	tr_push(stack);
	
	OH_ASSERT_DESCR(octInitGenContents(&term, OCT_TERM_MASK|OCT_BOX_MASK, &geogen),
		  "can not start the terminal implementation generator");
	    
	while (octGenerate(&geogen, &geo) == OCT_OK) {
		
	    if (geo.type == OCT_TERM) {
		/*
		 * whoops, found a promoted terminal
		 * - get the instance
		 * - open the master
		 * - get the corresponding formal terminal on the master
		 */
		OH_ASSERT_DESCR(octGenFirstContainer(&geo, OCT_INSTANCE_MASK, &inst),
			  "can not get the terminal instance");

		tr_add_transform(stack, &inst.contents.instance.transform, 0);
		
		/* now open the master */
		master.type = OCT_FACET;
		master.contents.facet.facet = "interface";
		master.contents.facet.mode = "r";
		master.contents.facet.version = OCT_CURRENT_VERSION;
		OH_ASSERT_DESCR(octOpenMaster(&inst, &master), "can not open the master");

		OH_ASSERT_DESCR(octGetByName(&master, &geo), "can not get the formal terminal");

		/* XXX should be multiple geos!!! */
		OH_ASSERT_DESCR(octGenFirstContent(&geo, OCT_GEO_MASK, &geo),
			  "can not get the terminal implementation");
	    }

	    OH_ASSERT_DESCR(octBB(&geo, &bb), "can not get the terminal implementation bb\n");
		
	    tr_transform(stack, &bb.lowerLeft.x, &bb.lowerLeft.y);
	    tr_transform(stack, &bb.upperRight.x, &bb.upperRight.y);
	    BOXNORM(bb);
		
	    if ((fontset == 0) && (bbp == 0)) {
		(void) printf("gsave\n");
		(void) printf("lfont\n");
		fontset = 1;
	    }
		
	    (void) printf("%d %d %d %d (%s) %s\n",
		   (int)bb.lowerLeft.x,
		   (int)bb.lowerLeft.y,
		   (int)bb.upperRight.x,
		   (int)bb.upperRight.y,
		   term.contents.term.name,
		   (type == FORMAL) ? "ft" : "at");
	}

	tr_pop(stack);
	
    }

    if ((fontset == 1) && (bbp == 0)) {
	(void) printf("grestore\n");
	fontset = 0;
    }

    return;
}


/*
 * get the stipple pattern from the technology facet and
 * generate a PostScript font where each letter in the font
 * represents the fill pattern for a layer
 *
 * producing geometry with a stipple fill pattern is simply:
 *
 * - create a clip path that represents the geometry
 * - 'show' strings of the proper letter covering the entire
 *   clip path (with a little slop)
 *
 *   AAAAAAAAAA
 *   A--------A
 *   A|AAAAAA|A   ->   AAAAAA
 *   A|AAAAAA|A   ->   AAAAAA
 *   A--------A
 *   AAAAAAAAAA
 *
 * 'A' is a character that represents the fill pattern for a layer
 *  '-' and '|' are the clip path
 *
 * the characters are always placed such that tile (stipple) filling
 * works (two geometries of the same layer will overlap with no
 * interference pattern).
 *
 */

#ifdef DEBUG
enum st_retval
printit(key, value, arg)
char *key;
char *value;
char *arg;
{
    (void) fprintf(stderr, "%%    key is %s\n", key);
    return ST_CONTINUE;
}
#endif

/*
 * convert a string of '0' and '1' to a number
 */

static int
bitStringToNumber(str)
char *str;
{
    int i, value = 0;

    for (i = 0; i < 8; i++) {
	if (str[i] == '1') {
	    value += 1 << (7 - i);
	}
    }
    return value;
}

static void
setTechnology(facet, layerTable)
octObject *facet;
st_table **layerTable;
{
    char layerName[MAXLAYERS][128];
    char row[8][10];
    int i, j, layers, bitmapName;
    struct layerdescription *layerDescription;
    int pri, ncolors, fill, border, width, height;
    octObject layer, techfacet;
    octGenerator layergen;
    char *bitPtr;
    char technology[1024];
    char buffer[32];
    octId *id;

    
    techfacet.type = OCT_FACET;
    techfacet.objectId = tapGetFacetIdFromObj(facet);
    if (octIdIsNull(techfacet.objectId)) {
	(void) fprintf(stderr, "%% no technology facet for %s:%s:%s\n",
		       facet->contents.facet.cell,
		       facet->contents.facet.view,
		       facet->contents.facet.facet);
	(void) fprintf(stderr, "%% probably missing the TECHNOLOGY or VIEWTYPE property\n");
	VOVend(-1);
    }
    OH_ASSERT_DESCR(octGetById(&techfacet), "can not get the tech facet\n");
    (void) strcpy(buffer, octFormatId(techfacet.objectId));

    (void) strcpy(technology, tail(tapGetDirectory(facet)));

    if (st_is_member(TechnologyTable, (char *) &techfacet.objectId) == 1) {
	(void) printf("/sctm matrix currentmatrix def\n");
	(void) printf("fctm setmatrix\n");
	(void) printf("sf-%s\n", buffer);
	(void) printf("sctm setmatrix\n");
	(void) st_lookup(TechnologyTable, (char *) &techfacet.objectId, (char **) layerTable);
	
	return;
    }

    /* font does not exist, build it */
    
    layers = 0;
    *layerTable = st_init_table(strcmp, st_strhash);

    (void) printf("/sctm matrix currentmatrix def\n");
    (void) printf("fctm setmatrix\n");
    
    /*
     * process each layer description
     */

    OH_ASSERT_DESCR(octInitGenContents(&techfacet, OCT_LAYER_MASK, &layergen), "can not init the layer gen");
    while (octGenerate(&layergen, &layer) == OCT_OK) {
        if( layers >= MAXLAYERS) {
         fprintf(stderr,"Warning: oct2ps: Increase MAXLAYERS and recompile\n");
	 return;
        }
	(void) strcpy(layerName[layers], layer.contents.layer.name);
	
	tapGetDisplayInfo(&layer, &pri, &ncolors, &fill, &border);

	if (fill == TAP_STIPPLED) {
	    tapGetDisplayPattern(&layer, TAP_FILL_PATTERN, &width, &height, &bitPtr);

	    if ((width != 8) && (height != 8)) {
		(void) fprintf(stderr,"%% Width and Height of a Fill Pattern must be 8\n");
		VOVend(-1);
	    }
	    for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
		    row[i][j] = bitPtr[(i * 8) + j];
		}
	    }
	}
#ifdef HUH
	(void) printf("%%\n");
	(void) printf("%% layer name %s\n", layerName[layers]);
	layerDescription->fillp = 1;
	if (fill == TAP_EMPTY) {
	    layerDescription->fillp = 0;
	    (void) printf("%% empty layer\n");
	} else if (fill == TAP_SOLID) {
	    (void) printf("%% solid layer\n");
	} else {
	    for (i = 0; i < 8; i++) {
		(void) printf("%% %s\n", row[i]);
	    }
	}
/*	(void) printf("%% %s\n", outline);*/
#endif
	
	layerDescription = (struct layerdescription *) malloc((unsigned) sizeof(struct layerdescription));
	
	if (border == TAP_SOLID) {
	    layerDescription->outlinep = 1;
	} else {
	    layerDescription->outlinep = 0;
	}

        /*
         * deal with color
         */
        if (plotColor) {
	    unsigned short red, green, blue;

	    layerDescription->colorp = 1;
	    tapGetDisplayColor(&layer, 0, &red, &green, &blue);
	    layerDescription->red
		 	= ((double) red) / ((double) TAP_MAX_INTENSITY);
	    layerDescription->green
		 	= ((double) green) / ((double) TAP_MAX_INTENSITY);
	    layerDescription->blue
		 	= ((double) blue) / ((double) TAP_MAX_INTENSITY);
	    layerDescription->priority = pri;
        } else {
	    layerDescription->colorp = 0;
        }

	/*
	 * put the character definition for the layer
	 *
	 * each character is represented by a 32x32 bit matrix
	 *
	 * only a 32x8 bit matrix is built here and the font build
	 * will create the other rows by replicating the 32x8 matrix
	 *
	 * 32x32 was chosen in the belief (possibly mistaken) that the
	 * bigger the bit matrix, the less time to generate a plot on a
	 * PostScript machine (fewer number of characters to output)
	 */

	if (fill == TAP_EMPTY) {
	    layerDescription->name = EMPTY_LAYER;
	    (void) st_insert(*layerTable, util_strsav(layerName[layers]), (char *) layerDescription);
	    layers++;
	    continue;
	}

	if (fill == TAP_SOLID) {
	    layerDescription->name = FILLED_LAYER;
	    (void) st_insert(*layerTable, util_strsav(layerName[layers]), (char *) layerDescription);
	    layers++;
	    continue;
	}
	
	/* stippled layer */

	layerDescription->name = EMPTY_LAYER + 1;
	(void) st_insert(*layerTable, util_strsav(layerName[layers]), (char *) layerDescription);
	
	(void) printf("/%s-%s [ ", layerName[layers], buffer);
	for (i = 0; i < 8; i++) {
	    for (j = 0; j < LAYERFONTWIDTH / 8; j++) {
		(void) printf("%d ", bitStringToNumber(row[i]));
	    }
	}
	(void) printf("] putbm def\n");

	(void) printf("/%s-%s-BM { %s-%s } def\n",
		      layerName[layers], buffer, layerName[layers], buffer);
	layers++;
    }

    if (layers > 256) {
	(void) fprintf(stderr, "%% tried to define more than 256 layers\n");
	VOVend(-1);
    }

    /*
     * output the PostScript to create a font containing the above characters
     */

    /*
     * not sure how to properly size dictionaries....
     */
    (void) printf("/Dict-%s %d dict def\n", buffer,
		  MAX(2 * layers, 16));
    (void) printf("/$workingdict %d dict def\n", MAX(2 * (layers + 2), 18));
    (void) printf("Dict-%s begin\n", buffer);
    (void) printf("/FontType 3 def\n");
    (void) printf("/FontMatrix [1.0 %d div 0 0 1.0 %d div 0 0] def\n",
			LAYERFONTWIDTH, LAYERFONTWIDTH);
    (void) printf("/FontBBox [0 0 %d %d] def\n", LAYERFONTWIDTH, LAYERFONTWIDTH);

    (void) printf("/Encoding %d array def\n", layers+1);

    /* start at 1, 0 is the code for an EMPTY layer */
    bitmapName = 1;

    for (i = 0; i < layers; i++) {
	(void) st_lookup(*layerTable, layerName[i], (char **) &layerDescription);
	if ((layerDescription->name != EMPTY_LAYER)
	    && (layerDescription->name != FILLED_LAYER)) {
	    (void) printf("Encoding %d /%s put\n",
			  bitmapName, layerName[i]);
	    layerDescription->name = bitmapName;
	    (void) st_insert(*layerTable, util_strsav(layerName[i]), (char *) layerDescription);
	}	    
	bitmapName++;
    }

    (void) printf("/CharProcs %d dict dup begin\n", MAX(2 * layers, 16));
    for (i = 0; i < layers; i++) {
	(void) st_lookup(*layerTable, layerName[i], (char **) &layerDescription);
	if ((layerDescription->name != EMPTY_LAYER)
	    && (layerDescription->name != FILLED_LAYER)) {
	    (void) printf("/%s { %d %d true [1 0 0 1 0 0] { %s-%s-BM } imagemask } def\n",
		   layerName[i], LAYERFONTWIDTH, LAYERFONTWIDTH, layerName[i], buffer);
	}
    }

    (void) printf("end def\n");
    (void) printf("/BuildChar\n");
    (void) printf("{$workingdict begin\n");
    (void) printf("/charcode exch def\n");
    (void) printf("/fontdict exch def\n");
    (void) printf("fontdict /CharProcs get\n");
    (void) printf("fontdict /Encoding get\n");
    (void) printf("charcode get get\n");
    (void) printf("gsave\n");
    (void) printf("0 setgray newpath\n");
    (void) printf("%d 0\n", LAYERFONTWIDTH);
    (void) printf("0 0\n");
    (void) printf("%d %d\n", LAYERFONTWIDTH, LAYERFONTWIDTH);
    (void) printf("setcachedevice\n");
    (void) printf("exec\n");
    (void) printf("grestore\n");
    (void) printf("end\n");
    (void) printf("} def\n");
    (void) printf("end\n");
    
    /* set the font table and the font */
    id = ALLOC(octId, 1);
    *id = techfacet.objectId;
    (void) st_insert(TechnologyTable, (char *) id, (char *) *layerTable);
    (void) printf("/font-%s Dict-%s definefont pop\n", buffer, buffer);
    (void) printf("/sf-%s { /font-%s findfont scsize scalefont setfont } def\n",
		  buffer, buffer);
    (void) printf("sf-%s\n", buffer);
    (void) printf("sctm setmatrix\n");
    
    return;
}

/*
 * draw a path
 *
 * the following two routines are stolen and modified from VEM - 3/18/87
 *
 * modifications:
 *
 * - output PostScript, don't call X functions
 * - mirror the coordinate space in Y
 *   (0,0 in X is upperLeft, lowerLeft in PostScript)
 * - sqrt(X^2 + Y^2) replaced with hypot(X,Y)
 *
 * the PostScript produced for a path is:
 *
 * if a manhattan path, use 'tb' (tiled box)
 *    making sure to put in the proper end extensions
 * if a zero width path, produce a standard zero width PostScript path
 * if non-manhattan,
 *    use 'tp' with a clip path and draw circles at the end of each segment
 *
 */

static void
drawNonManhattanBox(x1, y1, x2, y2, size, outlinep)
octCoord x1, y1;
octCoord x2, y2;
int size;
int outlinep;
{
    struct octPoint points[4];
    double distRatio, hypot();
    struct octPoint temporary;
    int i;
    int llx, lly, urx, ury;

    /* Compute point 'size' units along line between (x1,y1) and (x2,y2) */
    distRatio = ((double) size) / hypot((double) (x2-x1), (double) (y2-y1));
    temporary.x = (int) (distRatio * ((double) (x2-x1)));
    temporary.y = (int) (distRatio * ((double) (y2-y1)));

    /* Generate points by inverting temporary and adding to x1, y1 */

    points[0].x = x1 - temporary.y;
    points[0].y = y1 + temporary.x;
    points[1].x = x1 + temporary.y;
    points[1].y = y1 - temporary.x;

    /* Generate new temporary relative to x2, y2 */

    temporary.x = (int) (distRatio * ((double) (x1-x2)));
    temporary.y = (int) (distRatio * ((double) (y1-y2)));

    /* Generate final points relative to x2 and y2. */

    points[2].x = x2 - temporary.y;
    points[2].y = y2 + temporary.x;
    points[3].x = x2 + temporary.y;
    points[3].y = y2 - temporary.x;

    /* Make sure there is some area to fill */

    if (points[0].x == points[1].x) points[1].x += 1;
    if (points[2].x == points[3].x) points[2].x += 1;

    /* Draw the box */

    if (size > 0) {
	(void) printf("gsave\n");
	(void) printf("newpath\n");
    
	(void) printf("%d %d moveto\n", (int)points[0].x, (int)points[0].y);
	for (i = 1; i < 4; i++) {
	    (void) printf("%d %d lineto\n", (int)points[i].x, (int)points[i].y);
	}

	(void) printf("closepath clip\n");

	if (outlinep == 1) {
	    (void) printf("newpath\n");
	    (void) printf("%d %d moveto\n",
			  (int)points[0].x, (int)points[0].y);
	    for (i = 1; i < 4; i++) {
		(void) printf("%d %d lineto\n",
			      (int)points[i].x, (int)points[i].y);
	    }
	    (void) printf("closepath stroke\n");
	}
	
	llx = MIN(points[0].x, points[1].x);
	llx = MIN(llx, points[2].x);
	llx = MIN(llx, points[3].x);
	
	lly = MIN(points[0].y, points[1].y);
	lly = MIN(lly, points[2].y);
	lly = MIN(lly, points[3].y);
	    
	urx = MAX(points[0].x, points[1].x);
	urx = MAX(urx, points[2].x);
	urx = MAX(urx, points[3].x);
	
	ury = MAX(points[0].y, points[1].y);
	ury = MAX(ury, points[2].y);
	ury = MAX(ury, points[3].y);
	
	(void) printf("%d %d %d %d tp\n",
	       llx, lly, urx, ury);
	(void) printf("grestore\n");
	
    } else {
	(void) printf("newpath %d %d moveto %d %d lineto stroke\n",
	       (int)x1, (int)y1, (int)x2, (int)y2);
    }
    
    return;
}


#define PREVNONMAN	0x01	/* Previous segment is non-manhattan */
#define NOWNOTMAN	0x02	/* Current segment is non-manhattan  */

/*ARGSUSED*/
static void
drawPath(width, numpoints, points, outlinep, fillp)
int width;
int numpoints;
struct octPoint *points;
int outlinep, fillp;
{
    int index, widthOverTwo = width >> 1;
    short prevManFlag = 1, nowManFlag = 1;
    struct octBox box;

    if (numpoints < 2) {
	return;
    }
    
    /* Main Loop */
    for (index = 0;  index < numpoints-1;  index++) {
	prevManFlag = nowManFlag;  nowManFlag = 1;
	if (points[index].x == points[index+1].x) {
	    /* Vertical box */
	    if (widthOverTwo > 0) {
		box.upperRight.x = points[index].x + widthOverTwo;
		box.lowerLeft.x = points[index].x - widthOverTwo;
		if (points[index].y < points[index+1].y) {
		    /* Extend down if previous is manhattan and not the start */
		    if (prevManFlag && index) {
			box.lowerLeft.y = points[index].y - widthOverTwo;
		    } else {
			box.lowerLeft.y = points[index].y;
		    }
		    box.upperRight.y = points[index+1].y;
		} else {
		    /* Extend up if previous is manhattan and not start */
		    if (prevManFlag && index) {
			box.upperRight.y = points[index].y + widthOverTwo;
		    } else {
			box.upperRight.y = points[index].y;
		    }
		    box.lowerLeft.y = points[index+1].y;
		}

		(void) printf("%d %d %d %d tb\n",
			      (int)box.lowerLeft.x,
			      (int)box.lowerLeft.y,
			      (int)box.upperRight.x,
			      (int)box.upperRight.y);
		
		if (outlinep == 1) {
		    (void) printf("%d %d %d %d bx\n",
				  (int)box.lowerLeft.x,
				  (int)box.lowerLeft.y,
				  (int)box.upperRight.x,
				  (int)box.upperRight.y);
		}
		
	    } else {
#ifdef notdef		
		if (fillp) {
#endif
		    (void) printf("%d %d %d %d ln\n",
				  (int)points[index].x, (int)points[index].y,
				  (int)points[index+1].x,
				  (int)points[index+1].y);
#ifdef notdef		    
		}
#endif
	    }
	    
	} else if (points[index].y == points[index+1].y) {
	    /* Horizontal box */
	    if (widthOverTwo > 0) {
		box.upperRight.y = points[index].y + widthOverTwo;
		box.lowerLeft.y = points[index].y - widthOverTwo;
		if (points[index].x < points[index+1].x) {
		    /* Extend to the left previous is manhattan and not start */
		    if (prevManFlag && index) {
			box.lowerLeft.x  = points[index].x - widthOverTwo;
		    } else {
			box.lowerLeft.x = points[index].x;
		    }
		    box.upperRight.x = points[index+1].x;
		    
		} else {
		    /* Extend to right if previous is manhattan and not start */
		    if (prevManFlag && index) {
			box.upperRight.x = points[index].x + widthOverTwo;
		    } else {
			box.upperRight.x = points[index].x;
		    }
		    box.lowerLeft.x = points[index+1].x;
		}

		(void) printf("%d %d %d %d tb\n",
			      (int)box.lowerLeft.x,
			      (int)box.lowerLeft.y,
			      (int)box.upperRight.x,
			      (int)box.upperRight.y);

		if (outlinep == 1) {
		    (void) printf("%d %d %d %d bx\n",
				  (int)box.lowerLeft.x,
				  (int)box.lowerLeft.y,
				  (int)box.upperRight.x,
				  (int)box.upperRight.y);
		}
		
	    } else {
#ifdef notdef		
		if (fillp) {
#endif
		    (void) printf("%d %d %d %d ln\n",
				  (int)points[index].x, (int)points[index].y,
				  (int)points[index+1].x, (int)points[index+1].y);
#ifdef notdef		    
		}
#endif
	    }
	} else {
	    /* Non-manhattan */
	    if (widthOverTwo > 0) {
		drawNonManhattanBox(points[index].x, points[index].y,
				    points[index+1].x, points[index+1].y,
				    widthOverTwo, outlinep);
	    } else {
		/* Too small to fill,  draw line */
#ifdef notdef		
		if (fillp) {
#endif
		    (void) printf("%d %d %d %d ln\n",
				  (int)points[index].x, (int)points[index].y,
				  (int)points[index+1].x, (int)points[index+1].y);
#ifdef notdef		    
		}
#endif
	    }
	    nowManFlag = 0;
	}
	
	if ((!prevManFlag) || (!nowManFlag) || ((index == 0) && (!nowManFlag))) {
	    /* Non-manhattan corner type (circle is drawn) */
	    if (widthOverTwo > 0) {
			
		(void) printf("gsave\n");
		(void) printf("newpath\n");
	    
		(void) printf("%d %d %d 0 360 arc\n",
			      (int)points[index].x,
			      (int)points[index].y,
		       widthOverTwo);
		
		(void) printf("closepath clip\n");
		
		(void) printf("%d %d %d %d tp\n",
			      (int)(points[index].x - widthOverTwo),
			      (int)(points[index].y - widthOverTwo),
			      (int)(points[index].x + widthOverTwo),
			      (int)(points[index].y + widthOverTwo));
		(void) printf("grestore\n");
	    }
	}
    }
    
    if (!nowManFlag) {
	/* Draw final corner type */
	if (widthOverTwo > 0) {
	    
	    (void) printf("gsave\n");
	    (void) printf("newpath\n");
	    
	    (void) printf("%d %d %d 0 360 arc\n",
			  (int)points[numpoints-1].x,
			  (int)points[numpoints-1].y,
			  widthOverTwo);
		
	    (void) printf("closepath clip\n");
	    
	    (void) printf("%d %d %d %d tp\n",
			  (int)(points[numpoints-1].x - widthOverTwo),
			  (int)(points[numpoints-1].y - widthOverTwo),
			  (int)(points[numpoints-1].x + widthOverTwo),
			  (int)(points[numpoints-1].y + widthOverTwo));
	    (void) printf("grestore\n");
	}
    }
    return;
}


static char *
tail(pathname)
char *pathname;
{
    char *p;
    
    p = strrchr(pathname, '/');

    if (p == NIL(char)) {
	return pathname;
    } else {
	return ++p;
    }
}

static int 
layerGenCompare(i,j)
struct dLayer *i, *j;
{
    return(i->description->priority - j->description->priority);
}
