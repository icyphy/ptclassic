.\" $Id$
.NA SVD_MUSIC_2
.SD
 This demo shows the use of the Multiple-Signal Characterization (MUSIC)
algorithm to identify three sinusoids in noise which have frequencies very
close to each other.
.DE
.LO "$PTOLEMY/src/domains/sdf/matrix/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Mike J. Chen"
.Se SVD
.Se MUSIC
.Ir "SVD"
.Ir "singular-value decomposition"
.Ir "Multiple-Signal Characterization"
.Ir "MUSIC"
.LD
 The MUSIC algorithm takes advantage of the singular-value decomposition (SVD)
of a given data matrix to provide estimates of the frequencies of sinusoids
in the input.  Multiple sinusoids that are close in frequency can be very
difficult to distinguish.  The frequency resolving power of the MUSIC
algorithm can be increased by increasing the size of the \fIrows\fR and
\fIcols\fR parameters for the galaxy (good resolving power is obtained
when the parameters are set to values between 50-75).  The cost of the
greater resolving power is that the system becomes much slower.  The
\fIresolution\fR parameter controls how many points are plotted on the
frequency scale and does not directly correspond to the resolving
accuracy of the MUSIC algorithm.
.pp
 The demo plots the eigenspectrum \fIS(w)\fR output of the
.c
MUSIC_M
star which represents the sinusoids as peaks at their respective frequencies.
Note that since the demo has random noise, the plot of the eigenspectrum will
different for each run of the demo.  The demo also plots singular values
that are generated by the
.c
SVD_M
star.
.UH "References"
.ip [1]
S. Haykin, \fModern Filters\fR, pp. 330-331, 335-336,
Macmillan Publishing Company, New York, 1989.
.SA
SVD_M
MUSIC_M
.ES
