.\" $Id$
.NA spread
.SD
Demonstrates the Spread and the Collect stars
.DE
.SV $Revision$	$Date$
.AL "S. Ha"
.LO "~ptolemy/src/domains/cgc/demo"
.LD
.pp
.Se "Spread"
.Se "Collect"
This demo shows how multiple invocations of a star can be scheduled onto
more than one processors (in this demo, 2). The number of invocation of the
.c Gain
star between the
.c UpSample
and the
.c DownSample
is 3. The automatic scheduler (Hu's level scheduler) schedules two instances
of the 
.c Gain
star to the first processor and the last invocation to the second processor.
A 
.c Scatter 
star is automatically inserted after the 
.c UpSample 
star in the first processor to scatter the outputs of the
.c UpSample
star to the 
.c Gain
star (the first two outputs) and a
.c Send
star (the third output). On the other hand, a
.c Collect
star is automatically inserted before the
.c DownSample 
star to collect the inputs of the
.c DownSample
star from two
.c Receive
stars (the first two inputs) and the
.c Gain
star (the third input).
Note that there is a delay between the 
.c Gain
star and the
.c DownSample
star. That delay causes an addition copy of data between the 
.c Collect
star and the 
.c DownSample
star. The copy operations are coded automatically.
.pp
Once codes are generated, they are shipped to the specified
machines (
.c machineNames
target parameter ). compiled, and run by remote shell command "xon".
.Ir "xon, shell command"
.ES
