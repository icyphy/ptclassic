.\" $Id$
.NA lms
.SD
A universe showing the LMS adaptive filter.
.DE
.SV 1.1 "November 24, 1992"
.AL "Chih-Tsung Huang (ported from Gabriel)"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg56/demo/sim"
.EQ
delim $$
.EN
.LD
.pp
.Id "LMS adaptive filter"
A noise source is connected to an 8th-order LMS adaptive filter with
initial taps specifying a lowpass filter.
The taps adapt to a null filter (the impulse response is an impulse)
and the error signal is displayed.
.PP
The "sim" version of the demo simulates the universe.
The "s56x" version of the demo actually runs the code on
the S-56X SBus DSP board, and uses the "hread" program to
get the error signal from the DSP.
.ES
