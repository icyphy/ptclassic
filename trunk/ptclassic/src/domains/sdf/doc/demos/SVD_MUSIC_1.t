.\" $Id$
.NA SVD_MUSIC_1
.SD
This demo shows the use of singular-value decomposition (SVD) and
the Multiple-Signal Characterization (MUSIC) algorithm to identify
the frequency of a single sinusoid in a signal that has two different
signal to noise ratios.
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
in the input.  A higher signal to noise ratio makes the task of identifying
the frequency of the sinusoid much easier, as the output plot of the 
.c
MUSIC_M
stars show.
.pp
This demo also plots the various right and left singular matrices and
the vector of singular values for the two different signals.  The data
matrices were taken from pages 334-335 of reference [1].
.UH "References"
.ip [1]
S. Haykin, \fIModern Filters\fR, pp. 333-335, 
Macmillan Publishing Company, New York, 1989.
.SA
SVD_M
MUSIC_M
.ES
