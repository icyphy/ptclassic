/* $Id$ */
#include "flowgraph.h"
#include "xFlow2C.h"

#define INFINITY 1000000
#define INFSYMB "?"
#define HIGH_LEVEL "HIGH LEVEL"
#define BIT_TRUE "BIT TRUE"
#define ALL_MODES "ALL"
#define MAG_ONLY "MAGNITUDE"
#define PHASE_ONLY "PHASE"
#define MAG_PHASE "MAG/PHASE"
#define LINEAR "LINEAR"
#define LOG "LOG"

/*
 * static variables - needed as widget references
 */

static Widget topWidget;
static Widget *inputFiles;
int *outputFiles;

static int NrOfCycles = -1;
static char SimulationMode[16] = HIGH_LEVEL ;
static char Magnitude[16] = MAG_ONLY;
static char Linear[16] = LINEAR;

/* Simulation Control flags */
extern int FFTFlag;
extern int HighLevelFlag;
extern int LinearFlag;
extern int MagnitudeFlag;
extern ListPointer ListOfInputs;
extern ListPointer ListOfDisplays;

void DoQuit(), DoFreq(), DoCancel(), DoSelect();
void SetSimulationMode(), SetMagnitude(), SetCycles(), SetLinear();
Widget MakeCommandButton(), MakeTextButton(), *CreateInputFileListWidget();
ListPointer SetAttribute(), new_list(), ListAppend();
int *CreateOutputFileListWidget();
char *GetString();
char *getenv();

extern FILE *LogFile;
extern GraphPointer Root;
extern char *FlowLib;

/* Routines to interactively query for simulation in- and outputs */

int
RunInInteractiveMode(argc, argv)
int *argc;
char **argv;
{
    /* Check if interactive flag is set - check next if we can run an
     * X-application. 
     */
    int NrArguments, found;
    char **Ptr, *s;
    char PathCmd[WORDLENGTH];
    char *ApplDir;

    NrArguments = *argc;
    Ptr = argv;
    found = FALSE;
    while (--NrArguments > 0 && (*++Ptr)[0] == '-') {
        for (s = Ptr[0]+1; *s !='\0'; s++) {
	    if (*s == 'm') {
		found = TRUE;
		break;
	    }
	}
	if (found)
	    break;
    }
    if (found == FALSE)
        return (FALSE); 

    /* Try to open the x-environment - parse input arguments at the same time */

    /* check first where the resources can be found */
    ApplDir = getenv("XHYPERRESDIR");
    if (ApplDir == NULL) {
        sprintf(PathCmd, "XAPPLRESDIR=%s/app-defaults", FlowLib);
        putenv(PathCmd);
    }
    else {
        sprintf(PathCmd, "XAPPLRESDIR=%s", ApplDir);
        putenv(PathCmd);
    }

    topWidget = XtInitialize( "Flow2C", "XFlow2C", NULL, 0, argc, argv);

    return ((topWidget == NULL) ? FALSE : TRUE);
}

int ReadInteractive()
{
    /* Create an X-window widget to query the command file entries */
    Widget outer;

    outer = XtCreateManagedWidget("paned", panedWidgetClass, topWidget,
        NULL, ZERO);

    XtCreateManagedWidget("simulatelabel", labelWidgetClass, outer, NULL, ZERO);

    CreateFileWidget(outer);
    CreateTimeWidget(outer);
    CreateFreqWidget(outer);
    CreateCommandWidget(outer);

    XtRealizeWidget(topWidget);

    Flow2CEventLoop();

    return (NrOfCycles);
}

Flow2CEventLoop()
{
    XEvent ev;

    for(;;) {
        XtNextEvent(&ev);
	(void) XtDispatchEvent(&ev);

	if (topWidget == NULL)
	    break;
    }
}

CreateFileWidget(outer)
Widget outer;
{
    Widget file;

    file = XtCreateManagedWidget("file", formWidgetClass, outer, NULL, ZERO);

    /* Queries file names for inputs and outputs */
    inputFiles = CreateInputFileListWidget(file, Root);
    outputFiles = CreateOutputFileListWidget(file, Root);
}

CreateTimeWidget(outer)
Widget outer;
{
    Widget time, cycles, mode;
    Widget labelW, commandW;
    Arg args[1];

    time = XtCreateManagedWidget("time", formWidgetClass, outer, NULL, ZERO);
    labelW = XtCreateManagedWidget("timeLabel", labelWidgetClass, time,
	NULL, ZERO);
    ReverseVideo(labelW);

    cycles = XtCreateManagedWidget("cycles", formWidgetClass, time, NULL, ZERO);
    labelW = XtCreateManagedWidget("cycleLabel", labelWidgetClass, cycles,
	    NULL, ZERO);

    XtSetArg(args[0], XtNlabel, INFSYMB);
    commandW = XtCreateManagedWidget("cycleBox", commandWidgetClass, cycles,
	    args, 1);
    XtAddCallback(commandW, XtNcallback, SetCycles, (caddr_t) NULL);

    mode = XtCreateManagedWidget("mode", formWidgetClass, time, NULL, ZERO);
    labelW = XtCreateManagedWidget("modeLabel", labelWidgetClass, mode,
	    NULL, ZERO);
    XtSetArg(args[0], XtNlabel, HIGH_LEVEL);
    commandW = XtCreateManagedWidget("modeBox", commandWidgetClass, mode,
	    args, 1);
    XtAddCallback(commandW, XtNcallback, SetSimulationMode, (caddr_t) NULL);
}

CreateFreqWidget(outer)
Widget outer;
{
    Widget freq, linear, mag;
    Widget labelW, commandW;
    Arg args[1];

    freq = XtCreateManagedWidget("freq", formWidgetClass, outer, NULL, ZERO);
    labelW = XtCreateManagedWidget("freqLabel", commandWidgetClass, freq,
	NULL, ZERO);
    XtAddCallback(labelW, XtNcallback, DoFreq, (caddr_t) NULL);

    linear = XtCreateManagedWidget("linear", formWidgetClass, freq, NULL, ZERO);
    labelW = XtCreateManagedWidget("linearLabel", labelWidgetClass, linear,
	    NULL, ZERO);

    XtSetArg(args[0], XtNlabel, LINEAR);
    commandW = XtCreateManagedWidget("linearBox", commandWidgetClass, linear,
	    args, 1);
    XtAddCallback(commandW, XtNcallback, SetLinear, (caddr_t) NULL);

    mag = XtCreateManagedWidget("magnitude", formWidgetClass, freq, NULL, ZERO);
    labelW = XtCreateManagedWidget("magLabel", labelWidgetClass, mag,
	    NULL, ZERO);
    XtSetArg(args[0], XtNlabel, MAG_ONLY);
    commandW = XtCreateManagedWidget("magBox", commandWidgetClass, mag,
	    args, 1);
    XtAddCallback(commandW, XtNcallback, SetMagnitude, (caddr_t) NULL);
}

CreateCommandWidget(outer)
Widget outer;
{
    Widget command;
    command = XtCreateManagedWidget("control", formWidgetClass, outer,
                                  NULL, ZERO);
    MakeCommandButton(command, "ok", DoQuit);
    MakeCommandButton(command, "cancel", DoCancel);
}

static Widget Current = NULL;

Widget *
CreateInputFileListWidget(container, Graph)
Widget container;
GraphPointer Graph;
{
    Widget textlist;
    Widget *filelist;
    Widget labelW;
    int Count;
    char Buffer[128], *p, *index();

    ListPointer Ptr;
    EdgePointer Edge;

    textlist = XtCreateManagedWidget("input", formWidgetClass, container,
                                  NULL, ZERO);

    labelW = XtCreateManagedWidget("title", labelWidgetClass, textlist,
	NULL, ZERO);
    ReverseVideo(labelW);
    
    GenerateListOfInputs(Graph);
    /* Count the edges */
    for (Count = 0, Ptr = ListOfInputs; Ptr != NULL; Count++,Ptr = Ptr->Next);

    filelist = (Widget *) calloc (Count, sizeof(Widget));

    Current = labelW;

    for (Count = 0, Ptr = ListOfInputs; Ptr != NULL; Count++,Ptr = Ptr->Next){
	if (Ptr->Type != EdgeNode)
	    continue;
	Edge = (EdgePointer) Ptr->Entry;
	strcpy(Buffer, Edge->Name);
	if (IsArray(Edge)) {
            p = index(Buffer, '[');
            *p = '\0';
	}
	filelist[Count] = MakeTextButton(textlist, "text", 
	    Buffer, Buffer, Current);
    }

    return (filelist);
}

int *
CreateOutputFileListWidget(container, Graph)
Widget container;
GraphPointer Graph;
{
    int *selectlist;
    int Count;
    Widget select, labelW;
    char Buffer[128], *p, *index();

    ListPointer Ptr;
    EdgePointer Edge;

    select = XtCreateManagedWidget("output", formWidgetClass, container,
                                  NULL, ZERO);
    
    labelW = XtCreateManagedWidget("title", labelWidgetClass, select,
	NULL, ZERO);
    ReverseVideo(labelW);

    GenerateListOfDisplays(Graph);
    /* Count the edges */
    for (Count = 0, Ptr = ListOfDisplays; Ptr != NULL;
	 Count++, Ptr = Ptr->Next);

    selectlist = (int *) calloc(Count, sizeof (int));

    Current = labelW;

    for (Count = 0, Ptr = ListOfDisplays; 
	 Ptr != NULL; Count++, Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode)
	    continue;
	Edge = (EdgePointer) Ptr->Entry;
	strcpy(Buffer, Edge->Name);
        if (IsArray(Edge)) {
	    p = index(Buffer, '[');
	    *p = '\0';
	}

	MakeSelectButton(select, "select", Buffer, Count, TRUE,
	    Current, DoSelect);
	selectlist[Count] = TRUE;
    }

    return (selectlist);
}

GenerateListOfInputs(Graph)
GraphPointer Graph;
{
   NodePointer node;
   EdgePointer edge;
   GraphPointer SubGraph;

   for (node = Graph->NodeList; node != NULL; node = node->Next) {
       if (IsHierarchy(node)) {
            SubGraph = (GraphPointer)node->Master;
            GenerateListOfInputs(SubGraph);
       }

       if (IsInputNode(node)) {
           edge = (node->OutEdges == NULL) ? 
		(EdgePointer)(node->OutControl->Entry) :
                (EdgePointer)(node->OutEdges->Entry);
	   ListOfInputs = ListAppend(new_list(EdgeNode, edge), ListOfInputs);
       }
   }
}

GenerateListOfDisplays(Graph)
GraphPointer Graph;
{
   NodePointer node;
   EdgePointer edge;
   GraphPointer SubGraph;

   for (node = Graph->NodeList; node != NULL; node = node->Next) {
       if (IsHierarchy(node)) {
            SubGraph = (GraphPointer)node->Master;
            GenerateListOfDisplays(SubGraph);
       }

       if (IsOutputNode(node)) {
           edge = (node->InEdges == NULL) ? 
		(EdgePointer)(node->InControl->Entry) :
                (EdgePointer)(node->InEdges->Entry);
	   ListOfDisplays =ListAppend(new_list(EdgeNode, edge), ListOfDisplays);
       }
   }
}

Widget
MakeCommandButton(box, name, function)
Widget box;
char *name;
XtCallbackProc function;
{
  Widget w = XtCreateManagedWidget(name, commandWidgetClass, box, NULL, ZERO);
  if (function != NULL)
    XtAddCallback(w, XtNcallback, function, (caddr_t) NULL);
  return w;
}

Widget
MakeTextButton(parent, name, label, value, previous)
Widget parent, previous;
String name, value, label;
{
    Widget text, labelW, textW;
    Widget MakeStringBox();
    Arg args[5];
    Cardinal numargs = 0;

    if (previous != NULL)
        XtSetArg(args[numargs], XtNfromVert, previous); numargs++;

    text = XtCreateManagedWidget(name, formWidgetClass, parent, args, numargs);
    Current = text;

    numargs = 0;
    if (label != NULL) 
	XtSetArg(args[numargs], XtNlabel, label); numargs++;

    labelW = XtCreateManagedWidget("textLabel", labelWidgetClass, text,
	args, numargs);

    numargs = 0;
    XtSetArg(args[numargs], XtNeditType, XawtextEdit); numargs++;
    XtSetArg(args[numargs], XtNstring, value); numargs++;
    XtSetArg(args[numargs], XtNfromHoriz, labelW); numargs++;

    textW = XtCreateManagedWidget("textString", asciiTextWidgetClass, 
				  text, args, numargs);
    return (textW);
}

MakeSelectButton(parent, name, label, number, defaultValue, previous, CallBack)
Widget parent, previous;
String name, label;
int number;
int defaultValue;
XtPointer CallBack;
{
    Widget selectItem, labelW, commandW;
    Arg args[1];
    Cardinal numargs = 0;

    if (previous != NULL)
        XtSetArg(args[numargs], XtNfromVert, previous); numargs++;

    selectItem =
	XtCreateManagedWidget(name, formWidgetClass, parent, args, numargs);

    numargs = 0;
    if (label != NULL)
        XtSetArg(args[numargs], XtNlabel, label); numargs++;

    labelW = XtCreateManagedWidget("selectLabel", labelWidgetClass, selectItem,
	args, numargs);

    numargs = 0;
    XtSetArg(args[numargs], XtNfromHoriz, labelW); numargs++;
    commandW = XtCreateManagedWidget("selectBox", commandWidgetClass, 
				  selectItem, args, numargs);
    XtAddCallback(commandW, XtNcallback, CallBack, (XtPointer) number);

    if (defaultValue == 1)
	ReverseVideo(commandW);

    Current = selectItem;
}

/*****************************************************************************/
/* Action Functions                                                          */
/*****************************************************************************/

void
DoQuit()
{
    /* Gather all the available data and backannotate onto the database */
    int fatal = FALSE, Count;
    ListPointer Ptr;
    EdgePointer Edge;
    char *CycleNumber;
   

    /* Check first on input files */
    for (Count = 0, Ptr = ListOfInputs; Ptr != NULL; 
	 Count++, Ptr = Ptr->Next) {

	if (Ptr->Type != EdgeNode)
	    continue;
	Edge = (EdgePointer) Ptr->Entry;

	if (CheckFile(Edge, GetString(inputFiles[Count])) == FALSE)
	    fatal = TRUE;
    }

    if (fatal == TRUE)
	return;

    /* Set the output variables */
    for (Count = 0, Ptr = ListOfDisplays; Ptr != NULL;
	 Count++, Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode)
	    continue;
	Edge = (EdgePointer) Ptr->Entry;

	if (outputFiles[Count] == TRUE) {
	    Edge->Attributes = SetAttribute("IsDisplay", IntNode, (char *) TRUE,
                        Edge->Attributes);
	}
    }

    /* Set the flags */
    if (!strcmp(SimulationMode, HIGH_LEVEL))
	HighLevelFlag = 0;
    else if (!strcmp(SimulationMode, BIT_TRUE))
	HighLevelFlag = 1;
    else
	HighLevelFlag = 2;

    if (!strcmp(Magnitude, MAG_ONLY))
	MagnitudeFlag = 0;
    else if (!strcmp(Magnitude, PHASE_ONLY))
	MagnitudeFlag = 1;
    else
	MagnitudeFlag = 2;

    if (!strcmp(Linear, LINEAR))
	LinearFlag = 1;
    else
	LinearFlag = 0;

   

    /* Get number of cycles */
    if (NrOfCycles == -1)
        NrOfCycles = INFINITY;

    /* Kill the widget */
	
    XtDestroyWidget(topWidget);
    topWidget = NULL;
}

int
CheckFile(Edge, Name)
EdgePointer Edge;
char *Name;
{
    char FileName[WORDLENGTH], Token[16];
    int IsConstant = 0;
    FILE *File;

    /* Check if this is a constant variable */
    if (sscanf(Name, "%s %s", FileName, Token) == 2) {
            if (!strcmp(Token, "C") || !strcmp(Token, "CONSTANT"))
                IsConstant = 1;
    }
    else {
        IsConstant = 0;
        strcpy(FileName, Name);
    }

    File = fopen(FileName, "r");
    if (File == NULL) {
        printmsg(NULL, "Input File %s does not exist ... Please open\n",
                FileName);
        return (FALSE);
    }
    else {
	if (IsConstant)
            Edge->Attributes = SetAttribute("IsConstant", IntNode, IsConstant,
                                             Edge->Attributes);
        Edge->Attributes = SetAttribute("InFile", CharNode,
                      Intern(FileName), Edge->Attributes);
        fclose(File);
    }
    return (TRUE);
}

void
DoCancel()
{
    exit (-1);
}

void
DoFreq(widget, number, callData)
Widget widget;
int number;
caddr_t callData;
{
    FFTFlag = (FFTFlag == 1) ? 0 : 1;
    ReverseVideo(widget);
}

void
SetCycles(widget, Ptr, callData)
Widget widget;
XtPointer Ptr;
caddr_t callData;
{
    Arg args[1];
    char Label[16];

    if (NrOfCycles == -1)
	NrOfCycles = 16;
    else if (NrOfCycles >= 2048)
	NrOfCycles = -1;
    else
	NrOfCycles *= 2;

    if (NrOfCycles == -1)
	sprintf(Label, "%s", INFSYMB);
    else
	sprintf(Label, "%d", NrOfCycles);

    XtSetArg(args[0], XtNlabel, Label);
    XtSetValues(widget, args, 1);
}

void
SetSimulationMode(widget, Ptr, callData)
Widget widget;
XtPointer Ptr;
caddr_t callData;
{
    Arg args[1];
    
    if (!strcmp(SimulationMode, HIGH_LEVEL))
	strcpy(SimulationMode, BIT_TRUE);
    else if (!strcmp(SimulationMode, BIT_TRUE))
	strcpy(SimulationMode, ALL_MODES);
    else
	strcpy(SimulationMode, HIGH_LEVEL);

    XtSetArg(args[0], XtNlabel, SimulationMode);
    XtSetValues(widget, args, 1);
}

void
SetLinear(widget, Ptr, callData)
Widget widget;
XtPointer Ptr;
caddr_t callData;
{
    Arg args[1];
    
    if (!strcmp(Linear, LINEAR))
	strcpy(Linear, LOG);
    else
	strcpy(Linear, LINEAR);

    XtSetArg(args[0], XtNlabel, Linear);
    XtSetValues(widget, args, 1);
}


void
SetMagnitude(widget, Ptr, callData)
Widget widget;
XtPointer Ptr;
caddr_t callData;
{
    Arg args[1];
    
    if (!strcmp(Magnitude, MAG_ONLY))
	strcpy(Magnitude, PHASE_ONLY);
    else if (!strcmp(Magnitude, PHASE_ONLY))
	strcpy(Magnitude, MAG_PHASE);
    else
	strcpy(Magnitude, MAG_ONLY);

    XtSetArg(args[0], XtNlabel, Magnitude);
    XtSetValues(widget, args, 1);
}

void
DoSelect(widget, number, callData)
Widget widget;
int number;
caddr_t callData;
{
    outputFiles[number] = (outputFiles[number] == 1) ? 0 : 1;
    ReverseVideo(widget);
}

/*****************************************************************************/
/* Support Functions                                                         */
/*****************************************************************************/

/*	Function Name: GetString
 *	Description: retrieves the string from a asciiText widget.
 *	Arguments: w - the ascii text widget.
 *	Returns: the filename.
 */

String
GetString(w)
Widget w;
{
  String str;
  Arg arglist[1];
  
  XtSetArg(arglist[0], XtNstring, &str);
  XtGetValues( w, arglist, ONE);
  return(str);
}

SetString(w, str)
Widget w;
String str;
{
  Arg arglist[1];

  XtSetArg(arglist[0], XtNstring, str);
  XtSetValues( w, arglist, ONE);
}

ReverseVideo(widget)
Widget widget;
{
    XtPointer foreground, background;
    Arg arg[2];

    XtSetArg(arg[0], XtNforeground, &foreground);
    XtSetArg(arg[1], XtNbackground, &background);
    XtGetValues(widget, arg, 2);
    XtSetArg(arg[0], XtNforeground, background);
    XtSetArg(arg[1], XtNbackground, foreground);
    XtSetValues(widget, arg, 2);
    
}
