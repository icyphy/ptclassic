.\" $Id$
.NA switch
.SD
This simple system demonstrates the use of the Switch
star.  A Poisson counting process is sent to one output
of the switch for the first 10 time units, and to the other
output of the switch for the remaining time.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
A Poisson counting process is sent through one path (the "false" path)
until time 10.0, and through another path (the "true" path) after that.
The
.c Switch
star is used to perform the routing.
It's control is generated by a clock feeding a
.c Ramp
star.
Notice that the output of type \fIfloat\fR is connected
to the control input of type \fIint\fR.
The automatic type conversion truncates the floating point number,
removing the fractional part.
Thus, until the output of the
.c Ramp
exceeds 1.0, the control input of the
.c Switch
is 0.
A control input of zero routes input events through the lower ("false")
output, while any other control input routes events through the upper
("true") path.
.SA
Poisson counting process,
Switch.
.ES
