.\" $Date$	$Id$
.NA filterBankNonUniform
.SD
Two-channel non-uniform filter bank.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Id$
.AL "Brian Evans"
.LD
.pp
.Ie "filter bank"
.Ie "filter, multirate"
.Ie "perfect reconstruction"
 This universe implements a simple split of the frequency domain into
two non-uniform frequency bands.
 The upper channel contains the lower frequencies $omega$ over $(0, 2/3 pi)$
and the lower channel contains the upper frequencies $omega$ over 
$(2/3 pi, pi)$.
 The gain after the adder on the output compensates for the
downsampling by 3 in the analysis bank of filters.
 The analysis/synthesis filters, taken from [1], have been designed so that
the overall filter bank exhibits minimal delay.
.Ir "Nayebi, K."
.Ir "Smith, M. J. T."
.Ir "Barnwell, T."
 The code for the layout of the filter bank was originally generated for
the Ptolemy interpreter by the signal processing packages for
Mathematica [2].
.Ir "Evans, B."
.SA
filterBank
.UH References
.ip [1]
K. Nayebi, T. P. Barnwell, and M. J. T. Smith,
``Nonuniform Filter Banks: A Reconstruction and Design Theory,''
\fITransactions on Signal Processing\fR,
vol. 41, no. 3, pp. 1114-1127, March, 1993.
.ip [2]
B. L. Evans,
\fIA Knowledge-Based Environment for the Design and Analysis of
Multidimensional Multirate Signal Processing Algorithms\fR,
Ph. D. Thesis, Georgia Institute of Technology,
June, 1993.
.ES
