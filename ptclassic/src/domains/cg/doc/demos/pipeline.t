.\" $Id$
.NA pipeline
.SD
Demonstration of generation of pipelined schedules by retiming
.DE
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
This demo demonstrates a technique for generation of pipelined schedules
with Ptolemy's parallel schedulers, even though Ptolemy's parallel
schedulers attempt to minimize the
.i makespan
rather than the throughput.
The makespan is the time to compute one iteration of the schedule,
and the throughput is the time for each iteration in the execution
of a very large number of iterations.
.pp
To
.i retime
a graph, we simply add delays on all feedforward arcs (arcs that are not
part of feedback loops).
We must not add delays in feedback loops as that will change the semantics.
The effect of the added delays is to cause the generation of a pipelined
schedule.
.pp
The delays marked as ``(conditional)'' in the demo are parameterized delays;
the delay value is zero if the universe parameter \fIretime\fR is set to
NO, and is 100 if the universe parameter is set to YES.
The delay in the feedback loop is always one.
Schedules are generated in either case for a three-processor system with
no inter-processor communication costs.
.pp
If this were a real-life example, the programmer would next attempt to
reduce the "100" values to the minimum values that enable the retimed
schedule to run; there are other constraints that apply as well when there
are parallel paths, so that corresponding tokens arrive at the same star.
If the system will function correctly with zero values for initial values
at points where the retiming delays are added, the generated schedule can
be used directly.
Otherwise, a
.i preamble ,
or partial schedule, can be prepended to provide initial values.
.ES
