.\" $Id$
.NA errorDemo
.SD
An example of an inconsistent DDF system.
.DE
.SV $Revision$ $Date$
.AL "S. Ha"
.LO "~ptolemy/src/domains/ddf/demo"
.LD
An inconsistent DDF program is one where the long term
average number of particles produced on an arc is not the same
as the average long term number of particles consumed.
This idea is elaborated in [1].
This demo shows what to expect when an inconsistent DDF program
is run.  Inconsistency is considered an error, so
it should be detected.  Pending implementation of the techniques
described in [1], these errors are detected indirectly through
their effect on buffer sizes at run time.
However, the exact source of the error
is hard to identify using this technique.
Currently, the DDF scheduler detects two error conditions: one is
infinite waiting usually due to a delay-free loop.  The other
is an unbounded build up of particles on an arc.  The latter can be
caused in many ways,
one of which is illustrated in this demo.
.Ie "inconsistent DDF system"
.Ie "DDF inconsistent system"
.pp
Here, the true output(output#2) of the
.c Case
star is connected the the false input(input#1) of the
.c EndCase 
star via the
.c FloatGain
star.  This is most likely an error, because
unless the control boolean to the
.c Case 
and the
.c EndCase
stars is well balanced, one input arc of the
.c EndCase
star queues particles unboundedly.  The error can be simply
detected by examining the number of particles stored in an arc.
If the number is larger than the limit (which defaults to 1024), the
error is signaled.  A lower or higher limit can be specified
by setting the state called \fImaxBufferSize\fR in the universe.
.Ir "maxBufferSize"
In this demo, the maxBufferSize is set to "5".  Since the control
boolean is always FALSE, the \fIinput#2\fR arc of the
.c EndCase
star will queue the particles
consistently on each run.
At the 6th iteration, the error is detected and signaled with a
message.
.UH References
.ip [1]
E. A. Lee, "Consistency in Dataflow Graphs,"\fR
revised from MEMO UCB/ERL M89/125, November 9, 1989,
to appear in \fIIEEE Transactions on Parallel & Distributed Computing\fR, 1991.
.SA
Case,
EndCase,
inconsistency.
.ES
