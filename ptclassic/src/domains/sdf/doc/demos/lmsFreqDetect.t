.\" $Id$
.NA lmsFreqDetect
.SD
Illustrate the use of the least-mean square (LMS) algorithm to estimate
the dominant sinusoidal frequency in the input signal.
.EQ
delim off
.EN
.LO "SDF Basic Palette"
.DM SDF
.SV $Revision$	$Date$
.AL "Brian L. Evans"
.LD
.Ir "DTMF"
An estimate of the cosine of the frequency (in radians) of the
dominant sinusoidal component is estimated by the
.c LMSOscDet
star.  The error of the LMS filter is fed back.  Of the three
taps of The LMS filter, the first and third coefficients are
fixed at one.  The second coefficient is adapted.  It is a normalized
version of the Direct Adaptive Frequency Estimation Technique.
.UH REFERENCES
.IP [1]
G. Arslan, B. L. Evans, F. A. Sakarya, and J. L. Pino,
"Performance Evaluation and Real-Time Implementation of Subspace, Adaptive,
and DFT Algorithms for Multi-Tone Detection,"
Proc. Int. Conf. on Telecommunications, Istanbul, Turkey, April 15-17, 1996.
http://ptolemy.eecs.berkeley.edu/papers/96/dtmf_ict
.SA
LMSOscDet,
lmsDualTone.
.ES
