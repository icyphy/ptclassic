.\" $Id$
.NA realTime
.SD
Demonstrate the use of the Synchronize and Timer blocks.
Input events from a Clock star are held by the Synchronize
star until their time stamp, multiplied by the universe
parameter "timeScale", is equal to or larger than the elapsed
real time since the start of the simulation.  The Timer
star then measures the actual (real) time at which the
Synchronize output is produced.  The closer the resulting
plot is to a straight line with a slope of one, the more
precise the timing of the Synchronize outputs are.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Id$ $Revision$
.AL "Tom Parks"
.LD
.Se Synchronize DE
.Se Timer DE
Time is measured in seconds, so with the default
value of \fItimeScale\fR, events are produced every 50ms.
.pp
The performance of this system depends on your processor
speed, as well as on the Unix scheduler.
In fact, this system can be used to measure the real-time performance
of your particular Unix system.
Irregularity in the output plot is most likely due to scheduling
artifacts, whereas slopes larger than unity are due to inadequate
processor speed.
Reducing the \fItimeScale\fR parameter increases the challenge.
For instance, if you reduce it to 0.01, then the 
.c Synchronize
star will be asked to produce an output every 10ms.
.SA
shave
.ES
