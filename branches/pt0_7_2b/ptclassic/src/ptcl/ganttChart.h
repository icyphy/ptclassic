/* Author: Matt Tavis */
/* Version:  $Id$ */

#ifndef _ganntChart_h
#define _ganttChart_h 1
#include "Error.h"

class GanttChart {
public:

	/* Functions added to allow cross compile with ptcl and pigi */
	static void beginGantt(ostream& out, char *argstring );
	static void writeGanttLabel(ostream& out, char *argstring);
	static void writeGanttBindings(ostream& out, char *argstring);
	static void endGantt(ostream& out);
	static void writeGanttProc(ostream& out, char *argstring);
};
#endif
