.\" $Id$
.NA fibonnacci
.SD
Demonstrates the Self star.
.DE
.SV $Revision$ $Date$
.AL "S. Ha and E. A. Lee"
.LO "~ptolemy/src/domains/ddf/demo"
.LD
This demo shows how the
.c Self
star implements recursion.
.Se Self
.Ie recursion
The Fibonnacci sequence is generated using a rather inefficient recursion
that nonetheless is a good example
of how to realize recursion.
.pp
A recursion galaxy is defined as a wormhole in the DDF domain and
named "fib".  In the recursion galaxy, 
The incoming data is checked to see whether it is zero or one.
If it is, the
.c FloatThresh
star produces FALSE boolean to the
.c Case 
and
.c EndCase
stars, causing the
.c EndCase
star to send a constant (value 1)
to the output.  In case the incoming data is greater
than 1, the two previous Fibonnacci numbers are computed
recursively, and their sum is sent to the output.
To do this,
the incoming data is decremented by one twice after the
true output(output#2) of the
.c Case
star.  Each decremented data sample is now fed into the 
.c Self
star which is similar to a recursive function call.
Once fired, the
.c Self
star dynamically creates a cloned galaxy of the original recursion
galaxy "fib".  Since the original galaxy is searched by name, it can
be several levels up from the
.c Self
star (the
.c Self
star has a state called \fIrecurGal\fR which indicates the name of the
galaxy to be called).  The 
.c Self
star has its own scheduler and works with the cloned galaxy.
The dynamically created galaxy is destroyed after
the completion of the recursive computation.
Since the depth of the recursion is data dependent, all scheduling
decisions are made at runtime.
After the recursive calculation, the outputs from the
.c Self
stars are summed and sent to the output.
Note that this demo is slow: do not compute large Fibonnacci numbers.
This is because creating a cloned galaxy at runtime is expensive,
and the number of such creations is exponentially growing with the order
of the Fibonnacci number requested.
.pp
This mechanism for implementing recursion is intended only as
proof of feasibility.  Much more work would be required to get
a truly efficient recursive capability.
.SA
Case,
EndCase,
Self.
.ES
