.\" $Id$
.NA sampler
.SD
This demo tests the
.c Sampler
star.
A counting process with regular arrivals at intervals of 5.0
is sampled at regular intervals of 1.0.
As expected, this produces 5 samples for each level of the counting process.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.IE Sampler
The "clock" input of the
.c Sampler
star is given a regular sequence of events with time interval 1.0.
Each time this input gets an event, the star produces an output.
The value of the output is simply the value of the most recently received
sample on the data input.
.SA
Sampler.
.ES
