#if !defined(_AXISINT_H_)
#define _AXISINT_H_ 1
/* 
    RCS: $Header$
    XPAxisInt.h :: Internal header file for XPAxis

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/

typedef double XPAxisFlt;

#define XPAXIS_PAD_VERT	2
#define XPAXIS_SCROLLRES 1000

typedef TOPMask XPAxisFsMask;
#define XPAxisF_On		TOP_BIT(0)
#define XPAxisF_IdleCalc	TOP_BIT(1)
#define XPAxisF_LabelText	TOP_BIT(3)
#define XPAxisF_GeomXfNotify	TOP_BIT(4)
#define XPAxisF_CalcGeom	TOP_BIT(8)
#define XPAxisF_CalcLabels	TOP_BIT(9)
#define XPAxisF_CalcExpose	TOP_BIT(10)
#define XPAxisF_CalcGeomChk	TOP_BIT(11)
#define XPAxisF_CalcVars	TOP_BIT(12)
#define XPAxisF_CalcXf		TOP_BIT(13)
#define XPAxisF_CalcXfNotify	TOP_BIT(14)
#define XPAxisF_CalcNonTriv	TOP_BIT(15)
#define XPAxisF_Calc (						\
    XPAxisF_CalcLabels|XPAxisF_CalcVars|XPAxisF_CalcExpose	\
    |XPAxisF_CalcGeom|XPAxisF_CalcGeomChk			\
    |XPAxisF_CalcXf|XPAxisF_CalcXfNotify|XPAxisF_CalcNonTriv	\
)
				
enum _XPAxisState {
     XPAxis_State_Null,
     XPAxis_State_Norm,
     XPAxis_State_AdjLimLo,
     XPAxis_State_AdjLimHi,
     XPAxis_State_AdjLimBoth,
};
typedef enum _XPAxisState XPAxisState;

enum _XPAxisSide {
    XPAxisSide_Null,
    XPAxisSide_Left,
    XPAxisSide_Right,
    XPAxisSide_Top,
    XPAxisSide_Bottom,
};
typedef enum _XPAxisSide XPAxisSide;


typedef struct _XPAxisTick XPAxisTick;
struct _XPAxisTick {
    int			Flags;
    XPAxisFlt		wPos;
    XPAxisFlt		wVal;
    XPoint		tl1;	/* start point for tick line */
    XPoint		tl2;	/* end point for tick line */
    XPoint		ts;	/* draw point for tick string(label) */
    char		Lbl[20];
};
#define XPAxisTicF_Major	TOP_BIT(1)
#define XPAxisTicF_Label	TOP_BIT(2)

struct _XPAxisWdg {
    /* mode of operation state */
    int			SN;
    char*		Tag;
    Tcl_Interp*		ip;
    Tk_Window		tkwin;
    Display		*dpy;		/* dpy of tkwin */
    Tk_Window		dataWdgNew;	/* resource -datawindow */
    Tk_Window		dataWdg;	/* saved vers of newDataWdg */
    char*		dataWdgCmd;	/* resource -command */
    Tk_Window		scrollWdg;	/* resource -scrollwindow */
    char*		scrollCmd;	/* resource -scrollcommand */
    TOPLogical		scrollB;	/* resource -showscrollbar */
    char*		menuName;	/* resource -menu (cls Menubutton) */
    Tk_Uid		stateUid;	/* resource -state, (cls Menubutton) */

    XPAxisState		State;
    XPAxisFsMask	Flags;
    TOPLogical		autoSizeB;	/* resource -autosize */
    double		autoSizePad;	/* resource -autosizepad */
    TOPLogical		zoomedFullB;	/* internal state */

    /* overall layout */
    Tk_Uid		sideUid;	/* resource -side */
    XPAxisSide		side;		/* from sideUid */
    TOPLogical		vertB;		/* from sideUid */
    TOPLogical		flipB;		/* resource -flip */
    Tk_Uid		scaleUid;	/* resource -scale (lin,log,dslog) */
    XPXfaMode		scaleMode;	/* from scaleUid */
    XRectangle		axisGeom;
    XRectangle		dataGeom;

    /* the transform */
    double		wLo;		/* resource -lo */
    double		wMid;		/* internal state */
    double		wHi;		/* resource -hi */
    double		wClip;		/* resource -clip */
    int			vClip;		/* resource -clipWidth */
    double		wRangeLo;	/* set by cmd: range */
    double		wRangeHi;	/* set by cmd: range */
    double		padFactor;	/* resource -pad */
    XPXfa		TF;
    int			vLen;
    int			vDataWdgOfs;
    int			vDataWdgLen;
    double		wScrollLo;	/* autoset */
    double		wScrollHi;	/* autoset */
    XPXfa		STF;

    /* visuals */
    int			relief;		/* resource -relief, (cls Menubutton) */
    Cursor		cursor;		/* resource -cursor */
    XColor*		bgColor;	/* resource -background */
    Tk_3DBorder		bgBorder;	/* resource -background */
    char*		labelStr;	/* resource -label */
    int			labelLen;	/* strlen(labelStr) */
    XFontStruct*	labelFont;	/* resource -labelFont */
    XCharStruct		labelBbox;	/* XTextExtents of labelStr */
    GC			labelGC;	/* GC for labelStr */
    TOPLogical		showLabelB;	/* resource -showlabel */
    XColor*		clipColor;	/* reousrce -clipcolor */
    GC			clipGC;		/* GC for clip highlight */

    /* ticks */
    Tk_Uid		tickmodeUid;	/* resource -tickmode */
    TOPLogical		tickInsideB;	/* from tickmode */
    TOPLogical		tickOutsideB;	/* from tickmode */
    XColor*		tickColor;	/* resource -tickColor */
    XFontStruct*	tickFont;	/* resource -tickFont */
    int			vMinorTicLen;	/* resource -minorTicLen */
    int			vMajorTicLen;	/* resource -majorTicLen */
    XCharStruct		tickCI;		/* tickFont.max_bounds */
    short		tickCI_height;	/* ascent + descent */
    GC			tickGC;
    TOPVector		Tics;		/* of XPAxisTick[] */
    char*		tickFmt;	/* resource -tickFmt */
    char		realTickFmt[30];	/* from tickFmt */
    int			tickLabDigits;	/* #digits in realTickFmt */
    int			vTicLabWidth;	/* tickLabDigits in pixels */
    int			reqMinorDivs;
    double		reqMajorStep;
    int			NumTicLabSameDigits;

    /* sludge */
    int			MaxMajorTics;
};

extern void		_xpAxisExpose( XPAxisWdg *pAxis);
extern void 		_xpAxisCalcLabels( XPAxisWdg *pAxis);
extern void		_xpAxisIdleCalc( XPAxisWdg *pAxis, int calcMask);

extern char _XpAxisSubPackage[];
#endif /* _AXISINT_H_ */
