.\" $Id$
.NA image2trans
.SD
Illustrates the improvement in received signal quality
using matched filtering.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo" 
.DM SDF Universe 
.SV $Revision$ $Date$
.AL "KangNgee Chia and Farhad Jalilvand"
.EQ
delim $$
.EN
.LD
.IE "matched filtering binary image transmission"
This universe uses
.c match_filter
galaxy to illustrate the improvement offered by the matched filtering in
noisy conditions.
The power of the additive white Gaussian noise introduced by the
channel is controlled by the \fINoise_Power\fR parameters, which
is set to 0.025.
The input file is in PGM format, and is a small piece of a larger picture
of a face.
The PGM input file is converted into a binary bit stream,
which is passed through pulse shaping and then a noisy
.c AWGN 
channel.
The output of the
.c AWGN
is then passed through two sets of receivers.
In one case, matched filtering is performed but not in the other case.
The universe then displays the two images and estimates the power of the
difference between the two received images. 
.UH REFERENCES
.ip [1]
Simon Haykin, \fICommunication Systems\fR,
Wiley & Sons, 3rd ed., 1994, ISBN 0-471-57176-8.
.SA
match_filter.
.ES
