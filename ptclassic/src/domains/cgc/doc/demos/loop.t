.\" $Id$
.NA loop
.SD
Demonstrates the code size reduction by loop scheduling.
.DE
.SV $Revision$ $Date$
.AL "S. Ha"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cgc/demo"
.EQ
delim $$
.EN
.LD
.Ie "loop scheduler"
This demo contains a feedback loop and initial tokens exist
on two arcs to prevent deadlock.
Since there is a sample rate change, the
.c Add
and the
.c Ramp
stars are executed 10 times per iteration.
Without loop scheduling, the generated code would contain 10 repetitions
of these star firings.
The looping option is controlled by the \fIloopingLevel\fR
.Ie "loopingLevel, parameter"
target parameter.
If the parameter is set to either 0 or 1, no looping is made in this demo.
When the parameter is set to 2, the star firings are put into a loop, thereby
reducing the code size drastically.
.ES
