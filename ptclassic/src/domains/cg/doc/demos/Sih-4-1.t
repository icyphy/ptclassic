.\" $Id$
.NA Sih-4-1
.SD
Demonstration of the parallel scheduler
.SV $Revision$ $Date$
.AL "J. T. Buck"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg/demo"
.EQ
delim $$
.EN
.LD
This demo allows the properties of the parallel scheduler to be
investigated, by providing a universe in which the runtimes of
stars, the number of processors, and the communication cost between
processors can be varied.
.pp
The problem, as presented by the default parameters, is to schedule
a collection of dataflow actors on three processors with a shared
bus connecting them.
Executing the demo causes a Gantt chart display to appear, showing
the partitioning of the actors onto the three processors.
Clicking the left mouse button at various points in the schedule causes
the associated stars to be highlighted in the universe palette (for
details on how to use the Gantt chart display, see the CG domain
documentation).
After exiting from the Gantt chart display, code is written to a separate
file for each processor (here the "code" is simply a sequence of comments
written by the dummy CG stars).
.pp
It is interesting to explore the effects of varying the communication
costs, the number of processors, and the communication topology.
To do so, execute the
.i edit-target
command (keyboard shortcut 'T').
A display of possible targets comes up.
Of the available options, only
.c sharedBus-CG
and
.c fullConnect-CG
will use the parallel scheduler, so select one of them and click on ``Ok''.
Next, a display of target parameters will appear.
The interesting ones to vary are \fInprocs\fR, the number of processors,
and \fIsendTime\fR, the communication cost.
Try using two or four processors, for example.
Sometimes you will find that the scheduler will not use all the processors.
For example, if you make the communication cost very large, everything will
be placed on one processor.
If the communication cost is 1 (the default), and four processors are
specified, only three will actually be used.
.ES
