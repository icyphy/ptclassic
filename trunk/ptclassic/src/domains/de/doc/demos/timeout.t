.\" $Id$
.NA timeout
.SD
Demonstrate the use of the
.c Timeout
star.
Every one time unit, a timer is set.
If after another 0.5 time units have elapsed the timer is not cleared,
an output is produced to indicate that the timer has expired.
The signal that clears the timer is a
.c Poisson
process with a mean interarrival time of one time unit.
Thus, the timer expires quite often.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "Tom Parks"
.LD
.Se Timeout DE
The "expired" output events always lag the input 
"set" events by 0.5 time units.
But they are only produced if
the timer expires before a "clear" event arrives.
.SA
Test.
.ES
