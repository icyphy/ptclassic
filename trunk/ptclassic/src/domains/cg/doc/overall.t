.VR $Revision$ $Date$
.TI "CG Domain"
.AU
E. A. Lee
J. T. Buck
.AE
.H1 "Introduction"
.pp
.b Warning:
this code is experimental and unfinished; at this point a demo
of an interesting parallel scheduler is supplied, with little else.
.pp
The
.c CG
domain and derivative domains are used to generate code rather
than run simulations.
Only the derivative domains are of practical use for generating
code (and none are included in this release).
The stars in the
.c CG
domain itself can be thought of as ``comment generators''; they
are useful for testing and debugging schedulers and for little else.
The
.c CG
domain is intended as a model and a collection of base classes
for derivative domains.
This section, therefore, documents the common features and general
structure of all code generation domains.
.pp
The principle mechanism is to define
.c codeblock s
that are processed and written to a code file, which can then
be compiled or assembled.
These code blocks can be parsed and edited in ways that depend
on the target processor.  In the generic
.c CG
domain, no such editing occurs.
.pp
A key feature of code generation domains is the notion of a target
architecture.
Every application must have a user-specified target
architecture, selected from a set of targets supported
by the user-selected domain.
Every target architecture is derived from the base class
.c Target ,
and controls such operations as scheduling, compiling, assembling,
and downloading code.
Since it controls scheduling, multiprocessor architectures can be
supported with automated task partitioning and synchronization.
.H1 "Codeblocks
.pp
The preprocessor
.c ptlang
supports
.c CG
domains.
Suppose you wish to define a star that generates assembly code for
the Motorola DSP56001 that adds two numbers stored in memory,
and then writes the result back to memory.
(Note that this would be a very poor code generator, since it takes
control of memory allocation.  But it serves as a simple example.)
Assume the memory locations used have addresses 10, 11, and 12,
respectively, and lie in the "x" memory bank.
This can be done as follows:
.(c
defstar {
	... \fIstar definition directives\fP ...
	codeblock (blockname) {
		move x:10,a
		move x:11,x0
		add x0,a
		move a,x:12
	}
	go {
		gencode(blockname);
	}
}
.)c
The
.c codeblock
directive defines a block of code with an associated identifying name
("blockname" in this case).
The go() method then generates code based on this code block.
In the generic
.c CG
domain (which is of little practical use directly) the gencode() method
produces the code block
verbatim.
In a practical code generation domain based on
.c CG,
the code would be processed in some way first.
For instance, memory and registers would be allocated, code would
be compacted, etc.
.pp
A codeblock consists of lines of code.
Within each line, spaces, tabs, and the
newline are important, and are preserved.
For this reason,
the brackets "{ }" should not be on the same lines with the code.
The following code, for example, will not yield the expected result:
.(c
	codeblock (blockname) { move x:10,a
		move x:11,x0
		add x0,a
		move a,x:12 }
.)c
It is bad form for two reasons.
First, spaces and tabs between the opening "{"
and the first non-space character will be lost.
Second, the code on the last line will be lost.
Anything preceding the closing "}" on the same line is discarded.
.pp
The above
.c codeblock
directive is translated by the preprocessor (ptlang) into a definition
of a static member in the ``.h'' file:
.(c
class CGStarname : public CGStar
{
	...
	static CGCodeBlock blockname;
	...
}
.)c
An associated constructor call will appear in the ``.cc'' file:
.(c
CGCodeBlock CGStarname :: blockname (
"	move x:10,a\n"
"	move x:11,x0\n"
"	add x0,a\n"
"	move a,x:12\n"
);
.)c
The argument is a single string, divided into pieces for convenience.
In a nontrivial code generator, the constructor for the CodeBlock class
could parse this string and create an internal data structure which
is later used by the gencode() method.
The details of this data structure depend highly on the target
processor or language of the generated code.
.pp
The "blockname" argument to the codeblock directive identifies the particular
block.  Any number of blocks can be defined, and conditional code
generation is easily accomplished, as in the following example:
.(c
	go {
		if (parameter == YES)
			gencode(yesblock);
		else
			gencode(noblock);
	}
.)c
.H1 "Targets"
.pp
A code generation
.c Domain
is specific to the language generated,
such as C, DSP56000 assembly code, and Sproc assembly code.
A
.c Target ,
by contrast, is specific to the hardware that will run the code.
A given language, particularly a generic language such as C,
may run on many target architectures.
Code generation functions are therefore cleanly divided
between the domain and the architecture.
.pp
All target architectures are derived from the base class
.c Target .
The special class
.c KnownTarget
is used to add targets to the known list of targets much as
.c KnownBlock
is used to add stars (and other blocks) to the known block list and
to assign names to them.
.pp
A
.c Target
object has methods for generating a schedule, adding code, running
(which may involve downloading code to target hardware and beginning
its execution).  There also may be child targets (for representing
multiprocessor targets) together with methods for scheduling communication
between them.  Targets have parameters that may be set by the user.
.H2 "Targets included with the CG domain"
.pp
Three targets are included in the current release for use with the CG
domain:
.c default-CG
(class
.c CGTarget ),
.c fullConnect-CG
(class
.c CGMultiTarget ),
and
.c sharedBus-CG
(class
.c SharedBus ).
.pp
.c default-CG ,
the default, represents a single processor.  The result of running a
universe of CG stars with this target is for all the source code to
be collected and printed on the standard output (under the interpreter)
or in a popup window (under the graphical interface).
.pp
The
.c fullConnect-CG
target represents a homogeneous collection of interconnected processors.
The number of processors and the communication cost are parameters of
the target and are user-selectable, as is the type of target used
as the child target (it is
.c default-CG
by default).  The target contains a parallel
scheduler that assigns stars to specific processors using algorithms
described in [1].  The class
.c CGMultiTarget
is intended to be the baseclass for all targets that represent
multiple, nearly homogeneous processors of any type.
.pp
The
.c sharedBus-CG
target is identical to
.c fullConnect-CG
except that communication takes place on a shared bus (the shared bus
is an abstraction \- the effect is that only one communication between
any pair of processors may take place at a time).
.H2 "The Gantt Chart Display"
.pp
Demos that use targets derived from
.c CGMultiTarget
can produce an interactive Gantt chart display for viewing the parallel
schedule.
.pp
The Gantt chart display involves two windows \- one for displaying
part of the Gantt chart (the display window), and one for controlling
how much of the Gantt chart is shown in the display window (the 
control window).
.pp
The control window represents the entire Gantt chart, and
it contains a shaded region which represents the part of the 
Gantt chart that is currently shown in the display window.  
This shaded region can be moved around inside the control window
by dragging the mouse with the middle button depressed and 
its size can be changed with the left or right mouse button. Moving the
shaded region horizontally changes the time from which the
Gantt chart display begins ; moving it vertically changes the
topmost processor displayed. The fraction of the control window's
width spanned by the shaded region is approximately
the fraction of the total duration of the schedule that is
currently being displayed, and the fraction of the control window's
height spanned by the shaded region is approximately
the fraction of the total number of processors which are currently
displayed.
.pp
The display window represents each star or "nops" in the schedule
as a box drawn through the time interval it is scheduled over.
If the name of a star can fit in its box, it is printed inside.
A vertical bar inside the window is available for identifying
stars which cannot be labeled. The names of the stars which
this bar passes through are printed alongside their respective
processors. The bar can be moved horizontally by pressing
the middle mouse button in the vicinity of the bar and then dragging
the mouse with the middle button down.
The stars which the bar passes through are identified by having
their icons highlighted.
.pp
The vertical bar used for identifying stars is sometimes
hard to find \- for example, it might be coincident with a
vertical line in the Gantt chart. Pressing the right button
in the display window locates the bar by moving the mouse on top
of it. Pressing the left button in the display window moves the
bar to the mouse and identifies the stars touched by the bar
in its new position.
.pp
Here is a summary of commands that can be used while the Gantt chart
display is active:
.pp
\fBTo change the size of shaded area inside the control window:\fR
.ip
Depress the left or right mouse button inside the shaded area and
drag the mouse with the left or right button depressed.
.pp
\fBTo move the shaded area around inside the control window:\fR
.ip   
Depress the middle mouse button inside the shaded area and
drag the mouse with the middle button depressed.
.pp   
\fBTo move the vertical bar inside the display window:\fR
.ip   
Depress the middle mouse button in the vicinity of the bar and
drag the mouse with the middle button depressed.
.pp
\fBTo locate the vertical bar used for identifying stars:\fR
.ip
Depress the right mouse button inside the display window.
.pp
\fBTo move the vertical bar to the mouse inside the display window:\fR
.ip
Depress the left mouse button inside the display window.
.pp
\fBTo redraw the Gantt chart:\fR
.ip
Type control-L inside the display window.
.pp
\fBTo exit the Gantt chart display program:\fR
.ip
Type control-D inside the display window.
.pp
There are a fixed (hardcoded) number of colors available for coloring
processors and highlighting icons. If the number of processors
exceeds the number of available colors, the Gantt chart will be 
displayed in black and white.
.pp
On monochrome displays, highlighting of stars does not work.


.UH "References"
.ip [1]
Gilbert Sih,
.i "Multiprocessor Scheduling To Account For Interprocessor Communication"
(Ph.D. Thesis),
College of Engineering, University of California, Berkeley, April 1991.


