/* $Id$ */
#define STRDUP(xx)		(strcpy((char *)malloc((strlen(xx)+1)), (xx)))
#define LOG10(xx)		(xx == 0.0 ? 0.0 : log10(xx) + 1e-15)
#ifndef MAX
#define MAX(xx,yy)		(((xx) > (yy)) ? (xx) : (yy))
#define MIN(xx,yy)		(((xx) < (yy)) ? (xx) : (yy))
#endif
#define WITHIN(min,max,num)	(((num) > (min)) && ((num) < (max)))
#define MAPX(xx)		(canvasInfoPtr->llx+ \
				 (canvasInfoPtr->urx-canvasInfoPtr->llx)* \
				 ((xx)-pwInfoPtr->xMin)/(pwInfoPtr->xMax- \
						       pwInfoPtr->xMin))
#define MAPY(xx)		(canvasInfoPtr->lly- \
				 (canvasInfoPtr->lly-canvasInfoPtr->ury)* \
				 ((xx)-pwInfoPtr->yMin)/(pwInfoPtr->yMax- \
						       pwInfoPtr->yMin))

#define REVMAPX(rr)		(pwInfoPtr->xMin+ \
				 ((rr)-canvasInfoPtr->llx)* \
				 (pwInfoPtr->xMax-pwInfoPtr->xMin)/ \
				 (canvasInfoPtr->urx-canvasInfoPtr->llx))
#define REVMAPY(rr)		(pwInfoPtr->yMin- \
				 ((rr)-canvasInfoPtr->lly)* \
				 (pwInfoPtr->yMax-pwInfoPtr->yMin)/ \
				 (canvasInfoPtr->lly-canvasInfoPtr->ury))

#define COMMAND_SIZE 250
#define INCR_ALLOC_PTS 128
#define LOG_SCALE		0
#define LINEAR_SCALE		1
#define POLAR_AXES		2
#define SCATTER			0
#define LINE			1
#define LINE_SCATTER		2
#define NUM_MAJOR_TICKS		10
#define XTICKLENGTH		5
#define YTICKLENGTH		5
#define DONT_MOVE		0
#define MOVE			1
#define RADIUS			5.0
#define COLOR_LOWER		0x7000
#define COLOR_UPPER		0xff00
#define COLOR_STEP		0x4000
#define COLOR_RED		0
#define COLOR_GREEN		1
#define COLOR_BLUE		2
#define TICKMARKS		0
#define GRIDLINE		1

static char scmd[COMMAND_SIZE];
static char tmpStr[COMMAND_SIZE];
static int tmpInt;
static double tmpDouble;

#define LABEL_CHAR_PAD		2.5
#define LABEL_NUM_CHAR		7
#define PAD_TOP			15
#define PAD_BOTTOM		15
#define PAD_XLABEL_BORDER	15
#define PAD_XTITLE_XLABEL	15
#define PAD_TITLE		15
#define PADY			(PAD_TOP+PAD_BOTTOM+PAD_XLABEL_BORDER+ \
				PAD_XTITLE_XLABEL+PAD_TITLE)

#define READWH(fontPtr,s)	XTextExtents(fontPtr, s, strlen(s),\
				&dummy, &dummy, &dummy, &bbox)
#define WIDTH			(bbox.rbearing - bbox.lbearing)
#define HEIGHT			(bbox.ascent + bbox.descent)

#define PAD_RIGHT		(15+5*canvasInfoPtr->charWidth)
#define PAD_LEFT		15
#define PAD_YLABEL_BORDER	15
#define PAD_YTITLE_YLABEL	15
#define PADX			(PAD_RIGHT+PAD_LEFT+PAD_YLABEL_BORDER+\
				PAD_YTITLE_YLABEL)

#ifdef DEC
#define REG_FONT			"fixed"
#define TITLE_FONT			"fixed"
#else
#define TITLE_FONT			"-adobe-helvetica-*-r-*-*-20-130-*-*-*-*-*-*"
#define REG_FONT			"-Adobe-Helvetica-Bold-R-Normal--*-120-*"
#endif

typedef struct graph_canvasInfo {
    int canvasWidth; int canvasHeight;
    double xTickLabelYCoord; double yTickLabelXCoord;
    double llx; double lly; double urx; double ury;
    double xStart; double xStep; double xEnd;
    double yStart; double yStep; double yEnd;
    double titleXCoord; double titleYCoord;
    double xTitleXCoord; double xTitleYCoord;
    double yTitleXCoord; double yTitleYCoord;
/*    double yUnitsXCoord; double yUnitsYCoord;
    double xUnitsXCoord; double xUnitsXCoord;		*/
    int charWidth; int charHeight;
    int titleHeight; int yTitleWidth; int xTitleHeight;
    int maxWidth; int maxHeight;
} graph_CanvasInfo;

typedef struct graph_dataset graph_DataSet;
struct graph_dataset {
    char *setName;
    int type;
    char *color;
    char *ovalColor;
    int dataMemSize;
    int numPoints;
    int numPointsAlloc;
    int moveOvalFlag; int recreate;
    int offScale;
    double *xvec; double *yvec; int *id;
    double prevX; double prevY;
    double discardedX; double discardedY;
    graph_DataSet *prevSetPtr; graph_DataSet *nextSetPtr;
};
   
typedef struct graph_pwInfo graph_PWInfo;
struct graph_pwInfo {
    char *toplevel; char *identifier; char *win;
    char *loc; int width; int height;
    int autoscale; int xAxisScale; int yAxisScale;
    char *xAxisTitle; char *yAxisTitle;
    double xMin; double xMax;	/* range of values along the x-axis */
    double yMin; double yMax;
    double curXMin; double curXMax; double curYMin; double curYMax;
    graph_CanvasInfo *canvasInfoPtr;
    int numSets; graph_DataSet *firstSetPtr; graph_DataSet *lastSetPtr;
    int red; int green; int blue; int color;
    graph_PWInfo *prevPwPtr;
    graph_PWInfo *nextPwPtr;
    int zoomNum;
    int marks;
    int showAxes;
};
