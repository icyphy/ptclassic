/* $Id$ 
* MODULE NAME: SCRMGR.C                                                  
*                                                                      
* MODULE DESCRIPTION                                                   
*   This module contains all the terminal I/O functions used by the DSP56000
* simulator.  It also contains the functions which intercept and handle
* the BREAK signal.  Conditional compilation directives choose MSDOS (MSDOS),
* UNIX (UNIX), or VAX VMS (VMS) versions of the I/O routines.
*
*   The I/O functions operate on a dsp_window structure that is contained
* within the dsp_var structure that is allocated for each dsp device.    
* The dsp_window structure is defined in dsp_sdef.h and described  
* below. 
*
*    dsp_window 	 
*          .next        integer index of most recent insertion address
*                       of dbuf display buffer
*          .head        integer index of first displayed textline in
*                       the buffer. 
*          .tail        integer index of last displayed textline in buffer.
*          .dbuf        array of 100 lines of 120 characters
*
* MAIN FUNCTIONS: Called from other modules                            
* dsp_wredo(devindex) Repaint the screen from a device's screen buffer
* dsplwscr(bufptr,flag)
* dsplredraw(count)
* dsplcursor(line, column)
* dsplputc(c)
* dsplputs(line, column,text, flag)
* dsplgkey()
* dsplgetch()
* dsplscrnest()
* dsplunnest()

* LOCAL FUNCTIONS:  Lower level functions only used by this module     
* scr_erase()  Erase terminal display.
* gti_scroll(direction) Scroll display region up or down a line
* gti_ps(string, flag)
* out_char(string)
* outofband(info) VAX VMS function used for trapping control codes
* unsolicited(pasteid,flag) VAX VMS function for capturing unsolicited input
* int intrpt1()   Sets dsp_const.CTRLBR when CTRL-C is pressed
*/

#include "config.h"
#include "brackets.h"
#include "dspsim.h"
#include "dspsdef.h"
#include "dspsdcl.h"
#if macintosh
#include	<Quickdraw.h>
#include	<Windows.h>
#include 	<Types.h>
#include    <Files.h>
#include 	<Memory.h>
#include	<Events.h>
#include	<Menus.h>
#include	<Desk.h>
#include	<Fonts.h>
#include	<Dialogs.h>
#include	<StdIO.h>
#include	<Controls.h>
#include 	<Packages.h>
extern struct qd qd;
static int ch,cw;
static FontInfo sf;	
static ControlHandle scrollbar,haltbutton,gobutton,stepbutton;
static char screenbuf[24][80][2];

#define	appleMenu	1
#define	fileMenu	256
#define	helpMenu	257
#define	cmdMenu		258
#define	prevMenu	259
#define	NMENUS	5
#define SBarWidth	16
#define ROOTDIRID 2L	/* Directory id of root directory of volume */
#define FIGNORENAME -1  /* FDirIndex if not using ioNamePtr */
#define FIRSTTIME -1 	/* first time dialog filter is called by sfgetfile */
static MenuHandle myMenus[NMENUS];
static struct Rect screenRect, dragRect, pRect;
static struct WindowRecord wRecord;
static struct GrafPort *myWindow, *whichWindow;
static struct EventRecord myEvent;
static char macstring[256]; /* filename from menu entry */
#endif
#if VMS
#include ssdef
#include descrip
#include stsdef
#include smgdef
#define SMG$_BATSTIPRO 0x128019
static unsigned int kbid;
static short int out_of_band;
static short int ctrlbrk;
static int throwaway,retch,ctrlchar; /* character from keyboard */
static unsigned int dispid;
static unsigned int paste_id;
static unsigned int scrolldir;
static unsigned int attrc=0;
static unsigned int attr=0;
static  struct dsc$descriptor_s os;
static  int keyunsolicited;
#endif
/*
*   The scroll region view pointers
*/
#if MSDOS
static union REGS inregs;
static union REGS outregs;
static struct SREGS segregs;
static int h_attribute=0x07; /* initialized to non-highlite display*/
#endif


/*
* intrpt1 --- interrupt handler
*
* This function sets the dsp_const.CTRLBR flag any time a BREAK signal is
* intercepted in the MSDOS and UNIX versions of the simulator.  The VMS
* version uses the out_of_band() function to catch the CTRL-C signal.
*/
#if !macintosh
static
int intrpt1()
{
   (void)signal(SIGINT,intrpt1);
   dsp_const.CTRLBR = 1;
}
#endif

#if VMS
/*
* outofband --- intercepts control characters in VMS
*
* This function intercepts all control characters entered at the VAX     
* terminal.  The obchars variable in  dsplwinit() selects the out_of band
* characters that are intercepted.  This function sets CTRLBR if the CTRLC
* keyboard sequence is entered.  It always sets the out_of_band flag
* and sets the ctrlchar value to the character intercepted.  The control
* characters are remapped to a simulator input code in dsplgkey().
*/

static void
outofband(info)
struct  {
   unsigned long int SMG$L_PASTEBOARD_ID; /* Pasteboard ID              */
   unsigned long int SMG$L_ARG;           /* User's AST argument        */
   union  {
      unsigned char SMG$B_CHARACTER;      /* The character typed        */
      unsigned long int SMG$L_CHARACTER;  /* The character and 3 spaces */
      } SMG$R_CHARACTER_OVERLAY;
   } *info;                               /*SMG$R_BAND_INFORMATION_TABLE*/
{
   unsigned char chr;
   /* this routine is used to service trapped control characters */
   out_of_band=1;
   smg$cancel_input(&kbid);
   chr= info->SMG$R_CHARACTER_OVERLAY.SMG$B_CHARACTER;
   if(chr == SMG$K_TRM_CTRLC) dsp_const.CTRLBR=1; /* external control break signal */
   if(chr > 32) chr=0;
   ctrlchar=chr;
}

/*
* unsolicited --- receives unsolicited characters in VMS
*
* This function receives asynchronous input of unsolicited characters     
* on the VAX under VMS.  This is used during device execution so that 
* the CTRL-C sequence can be detected and halt the device execution.
*/

static void
unsolicited(pasteid,flag)
int pasteid,flag;
{
   /* this routine is used to service unsolicited keyboard input */
   if (!out_of_band) smg$read_keystroke(&kbid,&throwaway,NULL,NULL,&dispid);
   keyunsolicited=1;
}
#endif


/*
* dsplwinit --- initialize display parameters and keyboard input
*
* This function initializes the simulator window environment.  It is     
* normally only called at the start-up of the simulator, or when returning
* from the simulator SYSTEM commands.
*/

dsplwinit()
{
   /* This function initializes the screen window structure and the */
   /* keyboard characteristics for the particular system. */

#if macintosh
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	setupmenu();
	InitDialogs(0L);
	InitCursor();

	screenRect = qd.screenBits.bounds;
	SetRect(&dragRect, 4, 24, screenRect.right-4, screenRect.bottom-4);

	TextFont(monaco);
	TextSize(12);
	TextFace(normal);
	TextMode(srcCopy);
	GetFontInfo(&sf);
    ch = sf.ascent+sf.descent+sf.leading;
    cw = sf.widMax;
	InitPort(qd.thePort);
	pRect.top = 40;
	pRect.left = 5;
	pRect.bottom = 20+40+9+(ch*24);
	pRect.right = 5+2+(cw*80)+SBarWidth; /* 2 allows border */
	myWindow = NewWindow(&wRecord, &pRect, "SIM56000",TRUE, noGrowDocProc,
		-1L, TRUE, 0L);
	pRect = myWindow->portRect;
	pRect.left=pRect.right-SBarWidth;
	scrollbar=NewControl(myWindow,&pRect,"",TRUE,79,0,79,
		scrollBarProc,0L);
		
	pRect.top=0;
	pRect.right=pRect.left-1;
	pRect.left=pRect.right-(6*cw);
	pRect.bottom=20;
	
	haltbutton=NewControl(myWindow,&pRect,"HALT",TRUE,0,0,0,
		pushButProc,0L);
	pRect.right=pRect.left-1;
	pRect.left=pRect.right-(4*cw);
	gobutton=NewControl(myWindow,&pRect,"GO",TRUE,0,0,0,
		pushButProc,0L);
	pRect.right=pRect.left-1;
	pRect.left=pRect.right-(6*cw);
	stepbutton=NewControl(myWindow,&pRect,"STEP",TRUE,0,0,0,
		pushButProc,0L);
	
	pRect.top = 20+0;
	pRect.left = 0;
	pRect.right = 2+(cw*80); /* 2 allows border */
	pRect.bottom = 20+ch*21;
	
	SetPort(myWindow);
	TextFont(monaco);
	TextSize(12);
	TextFace(normal);
	TextMode(srcCopy);
#endif
#if MSDOS
   dsp_var=dsp_const.dsp_list[dsp_const.viewdev];
   if(signal(SIGINT,intrpt1)==(int(*)())-1){
        perror("couldn't set SIGINT");
        abort();
        }
   scr_erase(); /* erase the display */
#endif
#if VMS
   /* change keyboard and screen characteristics for simulator operation */
   int prow,pcol,srows,scols;
   int stline,endline;
   void outofband(),unsolicited(),scrout();
   unsigned int obchars;
   dsp_var=dsp_const.dsp_list[dsp_const.viewdev];
   obchars=0xffffffff; /* control-c, control-u, esc characters outofbound */
   ctrlbrk=out_of_band=keyunsolicited=0;
   scrolldir=SMG$M_UP;
   smg$create_pasteboard(&paste_id,NULL,&srows,&scols,NULL);
   smg$create_virtual_display(&srows,&scols,&dispid,NULL,NULL,NULL);
   smg$create_virtual_keyboard(&kbid,NULL,NULL);
   smg$set_out_of_band_asts(&paste_id,&obchars,outofband,0);
   stline=1; endline=dsp_const.cmdline;
   smg$set_display_scroll_region(&dispid,&stline,&endline);
   prow=1;pcol=1;
   smg$paste_virtual_display(&dispid,&paste_id,&prow,&pcol);
   smg$enable_unsolicited_input(&paste_id,unsolicited,0);
   dsp_const.numlines=srows;
   os.dsc$b_class=DSC$K_CLASS_S; /* constants in string descriptor structure */
   os.dsc$b_dtype=DSC$K_DTYPE_T;
   os.dsc$w_length=1; /* output 1 character at a time */
   scr_erase(); /* erase the display */
#endif
#if UNIX
  dsp_var=dsp_const.dsp_list[dsp_const.viewdev];
  (void) initscr(); /* initialize screen structures */
  crmode(); /* don't echo characters to screen from getch */
  noecho();
  nonl(); /* turn off newline optimize checks */
  if(signal(SIGINT,SIG_IGN)!=SIG_IGN)
      (void) signal(SIGINT,intrpt1);
  scr_erase(); /* erase the display */
#endif
}

/*
* scr_erase --- erase the terminal display
*
* Not much to say about this.
*/

static
scr_erase()
{
   /* Clears the display */
#if VMS
   dsplscrnest();
   smg$erase_display(&dispid,NULL,NULL,NULL,NULL);
   dsplrefresh();
#endif
#if UNIX
   dsplscrnest();
   clear();
   touchwin(curscr);
   dsp_const.numlines=LINES;
   dsplrefresh();
#endif
#if MSDOS
   int line;
   dsplscrnest();
   for(line=0;line<dsp_const.numlines;line++)   dsplputs( line, 0, "\t",0);
   dsplrefresh();
#endif
#if macintosh
   	int line;
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);
   	for(line=0;line<dsp_const.numlines;line++)   dsplputs( line, 0, "\t",0);
	SetPort(saveport);
#endif
}

/*
* dsp_wredo --- Repaint the screen from a device's display buffer
*
* This routine repaints the screen using the information in a device's
* display buffer. This is done after scrolling up through the display 
* buffer or after loading a simulator state file. Just the bottom page
* of the display is repainted after the screen is erased.  The multiple
* dsp simulation examples also use this function when switching the
* displayed device on a single window display.
*/

dsp_wredo(devindex)
int devindex;
{
#if macintosh
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);
	dsp_var=dsp_const.dsp_list[devindex];
   /* Select last page of buffered output for re-display */
    dsp_var->dsp_win.tail = (dsp_var->dsp_win.next+79)%100;
    dsp_var->dsp_win.head = (dsp_var->dsp_win.tail+80)%100;
    dsplredraw(100);
	SetPort(saveport);
#else
	dsp_var=dsp_const.dsp_list[devindex];
   scr_erase();
   /* Select last page of buffered output for re-display */
   dsp_var->dsp_win.tail = (dsp_var->dsp_win.next+79)%100;
   dsp_var->dsp_win.head = (dsp_var->dsp_win.tail+80)%100;
   dsplredraw(100);
#endif
}


/*
* dsplwscr --- write simulator output to the virtual screen buffer
*
* This function writes output to the scrolling portion of the simulator
* screen.  It also takes care of logging the information to a command
* log file or a session log file if logging has been enabled.  The
* "flag" parameter, when set to 0, indicates that the line corresponds
* to a simulator command.  Only commands are stored to the command log
* file.  The simulator screen will only be updated if the device index
* is the same as the dsp_const.viewdev value, but the last 100 lines of
* simulator output for each device are always available in the device   
* display buffer whether or not they are actually written to the screen.
*/

dsplwscr(bufptr,flag)
char *bufptr;
int  flag;
{

   dsp_var->dsp_win.next=(dsp_var->dsp_win.next==99)?0 :(dsp_var->dsp_win.next)+1; /* circular buffer */
   (void) strcpy(dsp_var->dsp_win.dbuf[dsp_var->dsp_win.next],bufptr); /* insert new text at "next" */
   if (dsp_var->devindex==dsp_const.viewdev){
      dsplredraw(100); /* display the window buffer last page */
      }

   if (flag==0){ /* check for a command.  If so, log to log c file */
      if (dsp_var->sim_stat.fileptrs[0]){ /* log commands only */
         (void) fprintf(dsp_var->sim_stat.fileptrs[0],"%s\n",bufptr);
		 }
#if macintosh
		dsplstackcmd(bufptr);
#endif
      }
   if (dsp_var->sim_stat.fileptrs[1]) { /* normal non-command (session) output */
      (void) fprintf(dsp_var->sim_stat.fileptrs[1],"%s\n",bufptr); /* log to log s file */
      }
}


/*
* dsplredraw --- redraws the screen after display updates
*
* This function takes care of the necessary steps to update the terminal 
* screen from a devices display buffer.  It is may be used to scroll up 
* or down through the display as well.  The display output if buffered   
* on systems that support buffering until the complete redraw is finished.
*/

dsplredraw(count)
 int count;
 {
   /* Repaint the screen from the dsp_var->dsp_win.dbuf buffer */

#if macintosh   
 	int y;
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);
	
	y=GetCtlValue(scrollbar)+count;
	if (y>79)y=79;
	else if (y<0) y=0;
	SetCtlValue(scrollbar,y);
	
   if (count< -19) { /* scroll display down by count number of lines */
      while (count++<0) {
         dsp_var->dsp_win.head=(dsp_var->dsp_win.head==0)? 99 :dsp_var->dsp_win.head-1;
         if (dsp_var->dsp_win.head !=dsp_var->dsp_win.next){ /* circular buffer. Don't go past "next" */
            dsp_var->dsp_win.tail=(dsp_var->dsp_win.tail==0)? 99 :dsp_var->dsp_win.tail-1;
            }
         else {
            dsp_var->dsp_win.head=(dsp_var->dsp_win.head==99)? 0 : dsp_var->dsp_win.head+1; 
            break; /* finished if dsp_var->dsp_win.next has occurred (at top of buffer ) */
            }
         }
		 EraseRect(&pRect);
		InvalRect(&pRect);
		SetPort(saveport);
		return;
      }
   else if (count>19 && count<=80){ /* scroll display up count number of lines */
      while (count-- >0) { /* scroll up one line at a time */
         if (dsp_var->dsp_win.tail == dsp_var->dsp_win.next) break;
         dsp_var->dsp_win.head=(dsp_var->dsp_win.head==99)? 0 :dsp_var->dsp_win.head+1;
         dsp_var->dsp_win.tail=(dsp_var->dsp_win.tail==99)? 0 :dsp_var->dsp_win.tail+1;
         }
		 EraseRect(&pRect);
		 InvalRect(&pRect);
		 SetPort(saveport);
		 return;
      }
#endif
   dsplscrnest(); /* Buffer display */

   if (count<=0) { /* scroll display down by count number of lines */
      while (count++<0) {
         dsp_var->dsp_win.head=(dsp_var->dsp_win.head==0)? 99 :dsp_var->dsp_win.head-1;
         if (dsp_var->dsp_win.head !=dsp_var->dsp_win.next){ /* circular buffer. Don't go past "next" */
            dsp_var->dsp_win.tail=(dsp_var->dsp_win.tail==0)? 99 :dsp_var->dsp_win.tail-1;
            gti_scroll(-1);
            dsplputs(0,0,dsp_var->dsp_win.dbuf[dsp_var->dsp_win.head],1);
            }
         else {
            dsp_var->dsp_win.head=(dsp_var->dsp_win.head==99)? 0 : dsp_var->dsp_win.head+1; 
            break; /* finished if dsp_var->dsp_win.next has occurred (at top of buffer ) */
            }
         }
      }
   else { /* scroll display up count number of lines */
      if (count>80&&((dsp_var->dsp_win.next+100-dsp_var->dsp_win.tail)%100)>21){ /* long jump */
         dsp_var->dsp_win.tail = (dsp_var->dsp_win.next+79)%100; /* reset pointers to display last page */
         dsp_var->dsp_win.head = (dsp_var->dsp_win.tail+80)%100;
         }
      while (count-- >0) { /* scroll up one line at a time */
         if (dsp_var->dsp_win.tail == dsp_var->dsp_win.next) break;
         dsp_var->dsp_win.head=(dsp_var->dsp_win.head==99)? 0 :dsp_var->dsp_win.head+1;
         dsp_var->dsp_win.tail=(dsp_var->dsp_win.tail==99)? 0 :dsp_var->dsp_win.tail+1;
         gti_scroll(1);
         dsplputs(20,0,dsp_var->dsp_win.dbuf[dsp_var->dsp_win.tail],1);
         }
      }
   dsplunnest();
}


/*
* gti_scroll --- scrolls the screen display up or down one line
*
* This function scrolls the area of the screen above the command line    
* up or down one line.  The top or bottom line are left blank after this
* function call, but will normally be updated by the dsplredraw() function
* to display the proper line from the device display buffer.
*/

static
gti_scroll(direction)
int  direction; /* 1=scroll up, -1= scroll down */
{
   /* Scroll display region up or down one line */
#if macintosh
	Rect rect;
	RgnHandle rh;
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);
	SetRect(&rect, 0,20+0,cw*80,20+ch*21);
	ScrollRect(&rect,0,-(ch*direction),rh=NewRgn());
    DisposeRgn(rh);
	SetPort(saveport);
#endif
#if MSDOS
    if (direction > 0)
   	inregs.x.ax = (0x600 | direction);/* scroll active page up */
    else {
   	direction = -direction;
   	inregs.x.ax = (0x700 | direction);/* scroll active page down */
      }
    inregs.x.bx = 0x700;	/* normal attribute for blank lines */
    inregs.x.cx = 0;		/* row 0, column 0 start of window */
    inregs.x.dx = 0x144f;	/* row 20, colum 80 end of window */
    segread (&segregs);
    int86x (0x10, &inregs, &outregs, &segregs);
#endif
#if UNIX
   if (direction > 0){
       	move (0, 0);		/* change cursor to top line */
      	deleteln ();		/* delete the top line, other lines move up */
      	move (dsp_const.cmdline - 1, 0);	/* add new line above command line */
      	insertln ();		/* other lines below move down  */
       }
   else {
   	move (dsp_const.cmdline - 1, 0);	/* Delete bottom line of display */
   	deleteln ();		/* bottom lines move up */
   	move (0, 0);		/* add new line at top */
   	insertln ();		/* other lines move down */
      }

#endif
#if VMS
    scrolldir= (direction > 0)? SMG$M_UP :SMG$M_DOWN;
    smg$scroll_display_area(&dispid,NULL,NULL,NULL,NULL,&scrolldir,NULL);
#endif
}


/*
* dsp_cursor --- moves the cursor to the specified line and column 
*
* This function moves the cursor and updates the curline and curclm     
* variables in dsp_var.
*/

dsplcursor(line, column)
int  line, column;
{

/* Move cursor to the specified line and column */

#if MSDOS
   dsplscrnest();
   dsp_var->curline = line;
   dsp_var->curclm = column;   inregs.x.dx = (line << 8) | column;
   inregs.x.bx = 0;
   inregs.h.ah = 2;
   segread(&segregs);
   int86x(0x10,&inregs,&outregs,&segregs);
   dsplunnest();
#endif
#if UNIX
   dsplscrnest();
   dsp_var->curline = line;
   dsp_var->curclm = column;
   move(line,column);
   dsplunnest();
#endif
#if VMS 
   dsplscrnest();
   dsp_var->curline = line;
   dsp_var->curclm = column;
   line++; column++; /* VAX co-ordinates are a little wierd. */
   smg$set_cursor_abs(&dispid,&line,&column);
   dsplunnest();
#endif
#if macintosh
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);
   	dsp_var->curline = line;
   	dsp_var->curclm = column;
	MoveTo(column*cw+1,line*ch+12+20);
	SetPort(saveport);
#endif
}


/*
* dsplputc --- output a character to the screen 
*
* This function outputs a character to the screen at the current cursor  
* position and updates the column position.  It is not used much in the
* simulator, so it doesn't need to be very efficient.  This implementation
* just passes the value on to gti_ps().
*/

dsplputc(c)
char c;
{
   /* Output a character to screen at current cursor position */
   /* Update current column position */
    char  text[2];
    text[1]= NULL;
    text[0] = c;
    dsplscrnest();
    gti_ps(text,0);
    dsplunnest();
}

/*
* dsplceol --- clear to end of line from current cursor position
*
* This function clears to end of line and leaves the cursor back at    
* its original position.
*/

dsplceol()
{
   /* clear to end of line from current cursor position */

#if macintosh
	Point pt;
	Rect rect;
	PenState ps;

	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);
	GetPenState(&ps);
    pt.h=ps.pnLoc.h;
    pt.v=ps.pnLoc.v;
	SetRect(&rect, pt.h,pt.v-sf.ascent,(cw*80)+2,pt.v+sf.descent);
	EraseRect(&rect);
	SetPort(saveport);
#endif
#if MSDOS
    int   locate;
    int  starclm;
    /* read cursor position */
    inregs.x.bx = 0;
    inregs.h.ah = 0x3;
    segread(&segregs);
    int86x(0x10,&inregs,&outregs,&segregs);
    locate = outregs.x.dx;

    starclm = dsp_var->curclm;
    inregs.x.ax = 0x600;         /* clear to end of line on screen */
    inregs.x.bx = 0x700;
    inregs.x.cx = (dsp_var->curline << 8) | starclm;
    inregs.x.dx = (dsp_var->curline << 8) | FULLINE - 1;
    segread(&segregs);
    int86x(0x10,&inregs,&outregs,&segregs);

    /* move cursor back to previous location */
    inregs.h.ah = 0x2;
    inregs.x.dx = locate;

    inregs.x.bx = 0;
    segread(&segregs);
    int86x(0x10,&inregs,&outregs,&segregs);
#endif
#if UNIX
    standend();
    clrtoeol(); /* clear to end of line */
    move(dsp_var->curline,dsp_var->curclm); /* back to previous location */
#endif
#if VMS
    attr=0;
    smg$erase_line(&dispid,NULL,NULL);
#endif
}

/*
* gti_ps --- put a formatted string to the terminal on current line
*
* This function outputs the string to the terminal.  Some formatting     
* characters are intercepted.  Curly braces {} set character highliting on
* and off.  The tab character and the newline will cause a clear to the 
* end of the line.  Non-format characters are output to the terminal in
* strings for maximum speed.  The output is buffered until the entire string
* is completed.
*/

static
gti_ps(string, flag)
char  *string;
int         flag; /*    flag         0=Normal hilite  1=on/off 2=BOLD */
{

#if MSDOS
    if (flag==2) h_attribute=0x0f;
    while (*string) {
      switch (*string) {
         case '\n': 
         case '\t': dsplceol(); break;
         case '{': if (flag==1) h_attribute=0x0f; break; /* BOLD HIGHLITE ATTRIBUTE */
         case '}': if (flag==1) h_attribute=0x07; break; /* NORMAL HIGHLITE ATTRIBUTE */

         default:
            if (++dsp_var->curclm == FULLINE) dsp_var->curclm-=1;
            else out_char(string);
            break;
         }
      string++;
      } /* end of while */
    h_attribute=0x07; /* return to Normal Hilite */
#endif
#if UNIX
    if (flag==2) standout(); /* BOLD HIGHLITE ATTRIBUTE */
   
    while (*string) {
      switch (*string) {
         case '\n':
         case '\t': dsplceol(); break;
         case '{': if (flag==1) standout(); break; /* BOLD HIGHLITE ATTRIBUTE */
         case '}': if (flag==1) standend(); break; /* NORMAL HIGHLITE ATTRIBUTE */
         default:
            if (++dsp_var->curclm == FULLINE) dsp_var->curclm-=1;
            else out_char(string);
            break;
         }
      string++;
      } /* end of while */

	 standend();
#endif
#if VMS
    if (flag==2) attr=SMG$M_BOLD;
   
    while (*string) {
      switch (*string) {
         case '\t':
         case '\n':dsplceol(); break;
         case '{': if (flag==1) attr=SMG$M_BOLD; break; /* BOLD HIGHLITE ATTRIBUTE */
         case '}': if (flag==1) attr=0; break; /* NORMAL HIGHLITE ATTRIBUTE */
         default:
            if (++dsp_var->curclm == FULLINE) dsp_var->curclm-=1;
            else out_char(string);
            break;
         }
      string++;
      } /* end of while */
   attr=0;
#endif 
#if macintosh
	Point pt;
	Rect rect;
	PenState ps;
	Pattern *pat;    
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);

	if (flag==2){
		pat=&qd.black; /* hilite */
		TextMode(srcBic);
		}
	else{
		pat=&qd.white;
		TextMode(srcCopy);
		}

    while (*string) {
      switch (*string) {
         case '\n':
         case '\t': dsplceol(); break;
         case '{': if (flag==1){
		 				pat=&qd.black;
						TextMode(srcBic);
						}
					break; /* BOLD HIGHLITE ATTRIBUTE */
         case '}': if (flag==1){
		 				pat=&qd.white;
						TextMode(srcCopy);
						}
					break; /* NORMAL HIGHLITE ATTRIBUTE */
         default:
            if (dsp_var->curclm < FULLINE){
				GetPenState(&ps);
				pt.h=ps.pnLoc.h;
				pt.v=ps.pnLoc.v;
				SetRect(&rect, pt.h,pt.v-sf.ascent,pt.h+cw,pt.v+sf.descent);
				FillRect(&rect,pat);
		        DrawChar(*string);
				dsp_var->curclm++;
				}
            break;
         }
      string++;
      } /* end of while */
	TextMode(srcCopy);
	SetPort(saveport);
#endif  
}

/*
* out_char --- output a single character to screen at current position
*
* This function outputs a single character to the screen.  It doesn't
* take care of updating the curline and curclm variables.
*/

static
out_char(chp)
char   *chp;
{
   /* Output a character to the terminal at the current cursor position. */
#if MSDOS
   int   locate;
   /* read cursor position */
   inregs.x.bx = 0;
   inregs.h.ah = 0x3;
   segread(&segregs);
   int86x(0x10,&inregs,&outregs,&segregs);
   locate = outregs.x.dx;

   /* put character and attribute to screen */
   inregs.h.bh = 0;
   inregs.x.cx = 0x0001;
   inregs.h.al = (*chp & 0x7f);
   inregs.h.ah = 0x9;
   inregs.h.bl = h_attribute;
   segread(&segregs);
   int86x(0x10,&inregs,&outregs,&segregs);

   /* move cursor up */
   inregs.h.ah = 0x2;
   inregs.x.dx = locate;
   inregs.h.dl++;
   inregs.x.bx = 0;
   segread(&segregs);
   int86x(0x10,&inregs,&outregs,&segregs);
#endif
#if UNIX
    addch(*chp);
#endif
#if VMS
   os.dsc$a_pointer=chp;
   smg$put_chars(&dispid,&os,NULL,NULL,NULL,&attr,&attrc,NULL);
#endif
}

/*
* dsplputs --- put a text string to the terminal at line and column
*
* This function is normally called by the simulator when writing to the
* command line, error line and help line.  It doesn't invoke the session
* logging calls that are associated with calls to dsplwscr().  The exact
* line and column are specified.  It doesn't store output in the device
* display buffer either.
*/

dsplputs(line, column,text, flag)
char  *text;
int   line, column;
int   flag;
{
   /* Put text string to terminal at line and column specified */
   /* The main work-horse is gti_ps. */
   dsplscrnest();
   dsplcursor(line, column);
   gti_ps(text, flag);
   dsplunnest();
}
#if macintosh
static	void draw_cursor() /* draw cursor at current position */
{
	Point pt;
	Rect rect;
	PenState ps;
	GrafPtr saveport;
 	GetPort(&saveport);
	SetPort(myWindow);

	GetPenState(&ps);
    pt.h=ps.pnLoc.h;
    pt.v=ps.pnLoc.v;
	SetRect(&rect, pt.h,pt.v,pt.h+cw,pt.v+1);
	InvertRect(&rect);
	SetPort(saveport);
}
#endif

/*
* dsplgkey --- get keyboard input and map to a simulator code   
*
* This function takes care of mapping raw keyboard input to a code     
* acceptable by the simulator.  If you don't happen to like the control
* characters used by the simulator, this is the function you should
* modify.  Vax users may also need to modify the outofband() function
* which intercepts control characters and does some translation before
* they arrive here.
*/

dsplgkey()
{
   /* get key from keyboard and map to simulator key */
#if macintosh
   int key;
	long l;

static int keymap[]={
   0,0,0,CTRLC,0,MOVEEOL,0,0,DELB,TABR,
   CR,DELF,CURL,CR,LINED,INSERT,0,0,CURR,0,
   PAGEU,LINEU,PAGED,0,0,0,DELEOL,ESCAPE,CURL,CURR,
   LINEU,LINED,SPACE,33,34,35,36,37,38,39,
   40,41,42,43,44,45,46,47,48,49,
   50,51,52,53,54,55,56,57,58,59,
   60,61,62,63,64,97,98,99,100,101,
   102,103,104,105,106,107,108,109,110,111,
   112,113,114,115,116,117,118,119,120,121,
   122,91,92,93,94,95,96,97,98,99,
   100,101,102,103,104,105,106,107,108,109,
   110,111,112,113,114,115,116,117,118,119,
   120,121,122,123,124,125,126,DELB
   };
   	draw_cursor();
	while (!GetNextEvent(keyDownMask+autoKeyMask,&myEvent))
		if (key=otherevents()){
			draw_cursor();
			if (key==CTRLC) dsp_const.CTRLBR=1;
			return (key);
			}
			
	if (myWindow == FrontWindow()) {
		key=(myEvent.message)&charCodeMask;
		if (myEvent.modifiers & cmdKey && (l = MenuKey(key))){
			docommand(l);
			key=0;
			}
		}
	else key=0;
	
	draw_cursor(); /* clear cursor */
	if (key>127||key<0) key=0;
	else key=keymap[key];
	if (key==CTRLC) dsp_const.CTRLBR=1;
   return (key);
#endif
#if   MSDOS
   /* get key scan code into AH, ascii result into AL.
    * If AL==0, map extended code to ASCII control character.
    * Otherwise return the ascii code from AL.
   */
   int key;
static int keymap[]={
   CTRLC,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,LINEU,PAGEU,0,CURL,0,CURR,0,MOVEEOL,
   LINED,PAGED,INSERT,DELF,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,TABR,DELEOL,0,0,
   0,0,0,0,0,0,0,0,0,0,
   0,0,0
   };

   segread(&segregs);
   inregs.x.ax = 0x0000;   /* direct console i/o */
   int86x(0x16,&inregs,&outregs,&segregs);
   if (outregs.h.al!=0)key=outregs.x.ax & 0x007f; /* already ascii */
   else key=keymap[(outregs.x.ax/0x100) & 0xff];/* Map special to ascii */
   return (key);
#endif 

#if VMS
   int key;
char ckey;
static int keymap[]={
   0,0,0,CTRLC,0,MOVEEOL,0,0,DELB,TABR,
   CR,DELF,CURL,CR,LINED,INSERT,0,0,CURR,0,
   PAGEU,LINEU,PAGED,0,0,0,DELEOL,ESCAPE,0,0,
   0,0,SPACE,33,34,35,36,37,38,39,
   40,41,42,43,44,45,46,47,48,49,
   50,51,52,53,54,55,56,57,58,59,
   60,61,62,63,64,97,98,99,100,101,
   102,103,104,105,106,107,108,109,110,111,
   112,113,114,115,116,117,118,119,120,121,
   122,91,92,93,94,95,96,97,98,99,
   100,101,102,103,104,105,106,107,108,109,
   110,111,112,113,114,115,116,117,118,119,
   120,121,122,123,124,125,126,DELB
   };
   if (out_of_band) retch=ctrlchar;
   else if (!keyunsolicited) smg$read_keystroke(&kbid,&retch,NULL,NULL,&dispid);
   if (out_of_band) retch=ctrlchar;
   else if (keyunsolicited) retch=throwaway;
   out_of_band=keyunsolicited=0;
   key=retch&0xff;
   if (key>127) key=0;
   else key= keymap[key];
   return (key);
#endif

#if UNIX
   int key;
char ckey;
static int keymap[]={
   0,0,0,CTRLC,0,MOVEEOL,0,0,DELB,TABR,
   CR,DELF,CURL,CR,LINED,INSERT,0,0,CURR,0,
   PAGEU,LINEU,PAGED,0,0,0,DELEOL,ESCAPE,0,0,
   0,0,SPACE,33,34,35,36,37,38,39,
   40,41,42,43,44,45,46,47,48,49,
   50,51,52,53,54,55,56,57,58,59,
   60,61,62,63,64,97,98,99,100,101,
   102,103,104,105,106,107,108,109,110,111,
   112,113,114,115,116,117,118,119,120,121,
   122,91,92,93,94,95,96,97,98,99,
   100,101,102,103,104,105,106,107,108,109,
   110,111,112,113,114,115,116,117,118,119,
   120,121,122,123,124,125,126,DELB
   };
	ckey = getch();
	key=ckey&0xff;
	if (key>127) key=0;
	else key=keymap[key];
   return (key);
#endif

}      /* end of dsp_gkey */
#if macintosh
pascal void myscroll(theCtrl,thePart)
	ControlHandle theCtrl;
	short thePart;
{
	int line,column;
	line=dsp_var->curline;
	column=dsp_var->curclm;
	switch (thePart){
		case inUpButton:dsplredraw(-1);break;
		case inDownButton:dsplredraw(1);break;
		case inPageUp:dsplredraw(-20);;break;
		case inPageDown:dsplredraw(20);;break;
		}
	dsplcursor(line,column);
}
static
otherevents()
{
	int key=0;
	int y1,y2,code,line,column;
	ControlHandle selControl;
	int thepart;
	GrafPtr saveport;
	static int scount=0;

	if (!(scount++&0xff))SystemTask();
	if (GetNextEvent(everyEvent^(keyDownMask+autoKeyMask), &myEvent)){
		switch(myEvent.what) {
			case mouseDown:
				code = FindWindow(&myEvent.where, &whichWindow);
				switch (code) {
					case inMenuBar:
						key=docommand(MenuSelect(&myEvent.where));
						break;
					case inSysWindow:
						SystemClick(&myEvent, whichWindow);
						break;
					case inDrag:
						DragWindow(whichWindow, &myEvent.where, &dragRect);
						break;
					case inGoAway:
						exit(0);
						break;
					case inGrow:
						break;
					case inContent:
						if (whichWindow != FrontWindow()) SelectWindow(whichWindow);
						else{
							GetPort(&saveport);
							SetPort(myWindow);
							GlobalToLocal(&myEvent.where);
							thepart=FindControl(&myEvent.where,myWindow,&selControl);
							if (selControl==scrollbar){
								if (thepart==inThumb){
									line=dsp_var->curline;
									column=dsp_var->curclm;
									y1=GetCtlValue(scrollbar);
									TrackControl(scrollbar,&myEvent.where,(Ptr)(-1l));
									y2=GetCtlValue(scrollbar);
									SetCtlValue(scrollbar,y1);
									dsplredraw(y2-y1);
									dsplcursor(line,column);
									}
								else TrackControl(scrollbar,&myEvent.where,
									(ProcPtr)myscroll);
								}
							else if (selControl==haltbutton) dsp_const.CTRLBR = 1;
							else if (selControl==stepbutton){
								key=MACFILE;
								(void) strcpy(macstring,"step");
								}
							else if (selControl==gobutton){
								key=MACFILE;
								(void) strcpy(macstring,"go");
								}
							}
						SetPort(saveport);
						break;
					}
				break;
			case activateEvt:
				break;

			case updateEvt:
				line=dsp_var->curline;
				column=dsp_var->curclm;
				macwrefresh();
				dsplcursor(line,column);
				key=REFRESH;
				break;
			}
		}
	return (key);
}
#endif


/*
* dsplgetch --- non-translated input from the keyboard
*
* This function gets keyboard input in a non-translated mode.  It's  
* not used much in the simulator - only when returning from a SYSTEM
* command before the screen parameters are re-initialized.
*/

dsplgetch()
{
   /* non-buffered type input */
#if VMS
   (void) getchar();
#else
#if macintosh
   (void) dsplgkey();
#else
   (void) getch();
#endif
#endif
}

/*
* dsplrefresh --- repaint the screen following screen buffering 
*
* Some operating systems provide screen buffering support.  The simulator
* keeps a counter call scrnest and will refresh the screen if it decrements
* from 1 to 0.  MSDOS really isn't supported for this - but maybe in a
* future update?
*/

dsplrefresh()
{
#if   MSDOS
   dsp_var->scrnest=0;
#endif
#if UNIX
   if (dsp_var->devindex==dsp_const.viewdev){
      refresh();
      }
   dsp_var->scrnest=0;
#endif
#if VMS
   if (dsp_var->devindex==dsp_const.viewdev){
      /* decrease buffering until screen is displayed */
      while (smg$end_display_update(&dispid)== SMG$_BATSTIPRO);
      }
   dsp_var->scrnest=0;
#endif
#if macintosh
   dsp_var->scrnest=0;
#endif
}

/*
* dsplscrnest --- increase screen buffer counter
*
* This function helps implement a nested screen buffering scheme.     
* VMS already has this capability, but this only allows the VAX to  
* nest one level ( on the 0 to 1 transition of the scrnest variable ).
* MSDOS isn't supported in this scheme, but the display output is fast
* enough that it doesn't seem to bother anyone there.
*/

dsplscrnest()
{
   dsp_var->scrnest++;
#if VMS
   if (dsp_var->scrnest == 1){
      /* increase buffering display output by 1 level*/
      smg$begin_display_update(&dispid);
      }
#endif
}

/*
* dsplunnest --- decrease screen buffering counter one level    
*
* This function and dsp_scrnest implement the nested screen buffering  
* scheme similar to what is already available on the VAX under VMS.  This
* function decreases the buffering one level and calls dsp_refresh if
* the level is less than 1 ( after the decrement).
*/

dsplunnest()
{
   if (dsp_var->scrnest <=1) dsplrefresh();
   else dsp_var->scrnest--;
}

/*
* dsplendwin --- reset all the window parameters
*
* This function is normally called by the simulator when returning to  
* the operating system level - either at the end of the simulation - or
* in response to a SYSTEM command.  It clears any special terminal processing
* that has been set up by the simulator and clears the screen.
*/

dsplendwin()
{
#if MSDOS||macintosh
   scr_erase();
#endif
#if UNIX
   scr_erase();
   endwin();
   resetty();
#endif
#if VMS
   /* restore normal keyboard and screen characteristics */
   void outofband();
   unsigned int obchars;
   scr_erase();
      smg$disable_unsolicited_input(&paste_id);
      smg$unpaste_virtual_display(&dispid,&paste_id);
      obchars=0; /* control-c, control-u, esc characters outofbound */
      smg$set_out_of_band_asts(&paste_id,&obchars,outofband,0);
      smg$delete_virtual_keyboard(&kbid);
      smg$delete_virtual_display(&dispid);
      smg$delete_pasteboard(&paste_id,NULL);
#endif
}

#if macintosh
char *
dsplmacfile()
{
return (macstring);
}
static
macwrefresh()
{
	int line,offset;
	GrafPtr saveport;	
	GetPort(&saveport);
	SetPort(myWindow);
	BeginUpdate(myWindow);
	DrawControls(myWindow);
	for (line=0,offset=dsp_var->dsp_win.head;line<21;line++){
        dsplputs(line,0,dsp_var->dsp_win.dbuf[offset],1);
		offset=(offset==99)?0:offset+1;
		}
	EndUpdate(myWindow);
	SetPort(saveport);
}
static
int	kbhit()
{
	otherevents();
	EventAvail(keyDownMask+autoKeyMask,&myEvent); /* get event */
	return(myEvent.what);
}
static
setupmenu()
{
	int i;
	static char helptop1[] =
	"asm;break;change;copy;disassemble;display;evaluate;go;help;history";
	static char helptop2[] =
	"input;load;log;output;path;quit;radix;reset;save;step;system;trace;wait";
	static char cmdtop1[] =
	"asm;break;disassemble;display;go;help;history;input;log;output;path;quit";
	static char cmdtop2[] =
	"radix;step;trace;wait";
	myMenus[0] = NewMenu(appleMenu, "\024");
	AddResMenu(myMenus[0], 'DRVR');
	myMenus[1] = NewMenu(fileMenu, "File");
	AppendMenu(myMenus[1], 
	"Execute Macro File;Load Object File;Load State File;Quit/Q");
	myMenus[2] = NewMenu(helpMenu, "Help");
	AppendMenu(myMenus[2], helptop1);
	AppendMenu(myMenus[2], helptop2);
	myMenus[3] = NewMenu(cmdMenu, "Commands");
	AppendMenu(myMenus[3], cmdtop1);
	AppendMenu(myMenus[3], cmdtop2);
	myMenus[4] = NewMenu(prevMenu, "History");
	AppendMenu(myMenus[4], cmdtop1);
	AppendMenu(myMenus[4], cmdtop2);
	for (i=0;i<NMENUS;i++)
		InsertMenu(myMenus[i], 0);
	DrawMenuBar();
}
pascal short lodfilter(pb)
ParmBlkPtr pb;
{
 short rv;
 int len;
 char *cp;
 OSType filetype;
 rv=TRUE;
 filetype=pb->fileParam.ioFlFndrInfo.fdType;
 if(filetype=='Fldr') rv=FALSE;
 else if (filetype=='TEXT'){
 	cp=ptoc (pb->fileParam.ioNamePtr);
	if((len=strlen(cp))>4 && !strcmp(&cp[len-4],macstring)) rv=FALSE;
 	ctop(cp);	
	}
 return(rv);
}

static
docommand(mResult)
long mResult;
{
	int theItem, theMenu;
	char name[40];
	Point where;
	SFReply freply;
	int key=0;
	char fname2[256];
	CInfoPBRec cb;
	OSErr operr;
	static char *opstr[]={"","load ","load s "};
	static char *flsufx[]={".cmd",".lod",".sim"};

	theMenu = mResult >> 16;
	theItem = mResult&0xffff;
	switch(theMenu) {
	case appleMenu:
		GetItem(myMenus[0], theItem, name);
		OpenDeskAcc(name);
		break;
	case fileMenu:
		switch(theItem) {
		case 1: /*macro file*/
		case 2: /*load obj */
		case 3: /*load state */
			where.h=85;
			where.v=100;
			(void) strcpy(macstring,flsufx[theItem-1]);
			SFGetFile(&where,NULL,(ProcPtr)lodfilter,1,"TEXT",NULL,&freply);
			if (freply.good ){
				key=MACFILE;
				(void) strcpy(macstring,ptoc(&freply.fName));
				ctop(&freply.fName);
				cb.dirInfo.ioNamePtr=(StringPtr) &freply.fName;
				cb.dirInfo.ioVRefNum=freply.vRefNum;
				cb.dirInfo.ioDrParID=0L;
				do{
					cb.dirInfo.ioFDirIndex=FIGNORENAME;
					cb.dirInfo.ioDrDirID=cb.dirInfo.ioDrParID;
					operr=PBGetCatInfo(&cb,FALSE);
					(void) strcpy(fname2,macstring);
					ptoc(&freply.fName); 	/* convert from p string to c string */
					(void) sprintf(macstring,"%s:%s",&freply.fName,fname2); /* insert parent directory */
					ctop(&freply.fName);/* convert back to c string */
					}while (operr==0 && cb.dirInfo.ioDrDirID != ROOTDIRID);	
				(void) strcpy(fname2,macstring);
				(void) sprintf(macstring,"%s\"%s\"",opstr[theItem-1],fname2);
				}
			break;
		case 4:
			exit(0);
		}
		break;
	case helpMenu:
		(void) strcpy (macstring,"help ");
		GetItem(myMenus[2], theItem,fname2);
		(void) strcat (macstring,fname2);
		key=MACFILE;
		break;
	case cmdMenu:
		GetItem(myMenus[3], theItem,macstring);
		key=MACFILE;
		break;
	case prevMenu:
		GetItem(myMenus[4], theItem,macstring);
		key=MACFILE;
		break;
	}
	HiliteMenu(0);
	return(key);
}
dsplstackcmd(cmdstr) /* replace command string in prevMenu */
char *cmdstr;
{
	char cmd[256];
	int i,j;

	/* check if command is already in list */
	for (i=1;i<17;i++){
		GetItem(myMenus[4], i,macstring);
		if (!strcmp(macstring,cmdstr))break;
		}
	/* push down stack */
	for(;i>1;i--){
		if(i<17){
			GetItem(myMenus[4], i-1,macstring);
			SetItem(myMenus[4], i,macstring);
			}
		}
	/* add new command on stack */
	SetItem(myMenus[4], 1,cmdstr);
}

#endif

dsplctrlbr() /* check for CTRLC signal */
{
#if MSDOS||macintosh
   if(kbhit()) (void) dsplgkey();
#endif
   if (dsp_const.CTRLBR){
      dsp_const.dsp_list[dsp_const.breakdev]->CTRLBR=1;
      dsp_const.CTRLBR=0;
      }
   return (dsp_var->CTRLBR);
}

