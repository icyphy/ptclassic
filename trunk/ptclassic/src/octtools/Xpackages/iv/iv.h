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
#ifndef IV_H
#define IV_H

#include <X11/Xlib.h>

#define IV_PKG_NAME "iv"

/* Return codes */
#define IV_OK  			1
#define IV_EXTRANEOUS_EVENT	2
#define IV_ERROR		-1

/* Types of the variables */
#define IV_DOUBLE		0
#define IV_INT			1
#define IV_BOOLEAN		2
#define IV_STRING               3
#define IV_TOGGLE               4
#define IV_WHICH                5

/* Posting definitions */
#define IV_NO_OPT   0x000
#define IV_INTERACT 0x001
#define IV_MOUSE    0x002

typedef int (*iv_PFI)();

typedef struct iv_which_text {
    char *value;		/* Optional Default value        */
    int  select;                /* Non-zero, if item selected    */
} ivWhichItem;


typedef struct s_Buf {
    char* buf;
    int count;
    int size;
} ivBuf;

extern char* ivBufGetString();
extern int   ivBufGetLength();

typedef struct IVvar {
    int		type;	       /* double, integer, boolean, string, or func */
    union any_number {
	double        *real;
	int           *integer;
	int           *boolean;
	char          **string;
        ivWhichItem   *itemArray;
    } varP;
    union special_types {
	char        *precision; /* control string for IV_DOUBLE types       */
	int         selectCount;
    } st;
    char   	*doc;		/* documentation string                     */
    iv_PFI	func;		/* pointer to a function to be called       */
    Window	TextWin;
    Window	ValueWin;
    Window      ToggleWin;
    Window	PlusButton;
    Window	MinusButton;
    long id;          /* Id associated to each variable by their addresses. */
    int         enable;         /*  If 0, all disable input to the variable */
} IVvarType;



typedef struct _IV_Window {
    Display *display;
    Window w;
    int rows;			/* current number of rows in the window */
    int numberOfVars;
    int oldNumOfVars;		/* The number of vars when the window was last mapped */
    int valueWidth;		/* Width of window showing value */
    int rowWidth;
    int	docWidth;		/* Width of window for documentation text */
    int docLen;			/*  */

    IVvarType *array;		/*  */
    int bgPixel;	        /* Background color                 */
    int editBgPixel;		/* Background color for value field */
    int buttonPixel;	        /* color of the buttons             */
    int editFontPixel;		/* color of the text                */
    int textFontPixel;		/* color of the text                */
    int titleFontPixel;		/* color of the title               */
    int cursorPixel;		/* color of the mouse cursor        */
    int bdrPixel;		/* Border color                     */
    char *titleString;

    GC  docGC;
    GC  normValueGC;
    GC  editValueGC;
    GC  titleGC;
    XFontStruct*  valueFont;
    /*  Variables used to edit the variable */
    int editIndex;		/* Index to win currently being edited. */
    int eraseFlag;
    Window theWin;
    int firstIter;		/*  */
    ivBuf editBuffer;
    ivBuf oldBuffer;
    ivBuf buffer;
} IVWindow;


typedef struct _IVAssocEntry {
    int IVWinType;    /* What type of window     */
    int index;        /* index to array of lines */
    IVWindow *IVwin;		/* Backpointer to the IVwindow */
} IVAssocEntry;
     

#define IV_NULL_FUNCTION ((iv_PFI) 0)

extern IVWindow *iv_InitWindow();	  /* Init iv                           */
extern void iv_SetEraseFlag();    /* Sets the erase flag               */
extern int iv_SetEnableFlag();    /* Sets the enable flag              */
extern int iv_WhichSelect();      /* Returns index of the selected     */
extern int iv_AddVar();	  /* Add a new interactive variable    */
extern int iv_PromptVar();       /* Prompt a variable for change.     */
extern int iv_UpdateVar();       /* Print the current value of a var  */
extern int iv_HandleEvent();	  /* Filter for IV events              */
extern int iv_ProcessAllEvents(); /* Handle ALL X events               */
extern int iv_MapWindow();	  /* Draw the final IV window          */
extern void iv_FreeWindow();         /* Destroys IV window and frees all  */
				  /* resources associated with IV.     */
#endif /* IV_H */
