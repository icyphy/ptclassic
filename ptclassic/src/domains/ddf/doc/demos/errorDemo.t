.\" $Id$
.NA errorDemo
.SD
An example of an inconsistent DDF system.
.DE
.SV 1.1 "January 10, 1991"
.AL "S. Ha"
.LD
.ES
This demo shows what to expect when an inconsistent DDF program
is running.  Since an inconsistent DDF program is an erroneous case,
it should be detected with proper information without crashing the
system nor running forever.  The dynamic behaviour of the
DDF stars makes it very difficult to detect the errors at
compile-time.  Instead, the errors can be detected 
relatively easily at runtime.  However, the exact source of the error
is hard to be specified at runtime.
Currently, the DDF scheduler detects two situations : one is
infinite waiting usually from delay-free loop condition. The other
is an unbounded file-up of tokens in an arc. The latter may be resulted
from various sources, one of which is illustrated in this demo.
.pp
Here, the trueOutput of the
.c Switch
star is connected the the falseInput of the
.c Select 
star via the
.c FloatGain
star.  This must be an erroneous connection. 
Unless the control boolean to the
.c Switch 
and the
.c Select
stars is well balanced, one input arc of the
.c Select
star queues tokens unboundedly.  Therefore, the error can be
detected by examining the number of tokens stored in an arc.
If the number is larger than the limit (or default 1024), the
error is signaled.  The limit can be specified at user's will
by setting the state called "bufferSize".
In this demo, the bufferSize is set "5".  Since the control
boolean is always FALSE, the trueInput arc of the
.c Select
star will queue the tokens consistently at each run.
At the 6th run, the error is detected and signaled with some
message.
.SA
Switch,
Select,
inconsistency.
.ES

