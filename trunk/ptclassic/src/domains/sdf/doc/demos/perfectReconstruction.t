.\" $Date$ $Id$
.NA perfectReconstruction 
.SD
This universe implements an eight-channel perfect reconstruction one-dimensional
filterbank.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Id$ 
.AL "William Chen"
.LD
.EQ
delim $$
.EN
.pp
.Ie "filter bank"
.Ie "filter, multirate"
.Ie "perfect reconstruction"
The following represents an eight-channel perfect reconstruction filterbank,
consisting of an analysis section and a synthesis section.

The analysis section here divides a signal according to its frequency content.
The analysis section consists of three stages, and each stage is composed of 
two multirate FIR filter stars.  The pair of FIR filters perfectly split an 
incoming signal into highpass and lowpass components and then downsamples each
component.  By chaining three stages together, the analysis section divides 
the frequency axis into 8 equal bins.
.Ir "Smith, M. J. T."
.Ir "Barnwell, T. P."
.pp
The synthesis filterbank performs the inverse function of the analysis filterbank,
taking the frequency components and reconstructing the original signal.
In a similar way, the synthesis section consists of three stages.
In each stage, the incoming components from the analysis section are
upsampled by 2, passed through reconstructing FIR filters, and added 
together.  By chaining three stages together, the synthesis section mirrors
that of the analysis to reconstruct the original signal.
.pp
For specially designed FIR filters, the reconstructed signal will be an exact
replica of the input (to within coefficient round-off error and delay).
There are, in all, only four unique FIR filters used, known as the
conjugate quadrature mirror filters.  These filters preserve  
the property of 'perfect reconstruction', which 
has been studied in great detail in the literature [4].
.Ir "Vaidyanathan, P. P."
.pp
Concerning the schematic layout,
.c ana
is the basic analysis filter block, and
.c syn
is the corresponding synthesis block.
The source is a brief segment of speech, which is compared to the
reconstructed one.
.SA
filterBankNonUniform
.UH REFERENCES
.ip [1]
M. Vetterli and C. Herley,
``Wavelets and Filter Banks: Theory and Design,''
\fIIEEE Transactions on Signal Processing\fR,
Sep., 1992, \fB40(9)\fR, pp. 2207-2232.
.ip [2]
R. E. Crochiere and L. R. Rabiner, 
\fIMultirate Digital Signal Processing\fR,
Prentice-Hall, Englewood Cliffs, NJ, 1983.
.ip [3]
M. J. T. Smith and T. P. Barnwell,
``A Procedure for Designing Exact Reconstruction Filter Banks
for Tree-Structured Subband Coders,'' 
\fIInt. Conf. on Acoustics, Speech, and Signal Processing\fR,
Mar., 1984, vol. 2, pp. 27.1/1-4.
.ip [4]
P. P. Vaidyanathan,
``Quadrature Mirror Filter Banks, $M$-Band Extensions, and
Perfect Reconstruction Techniques,''
\fIIEEE ASSP Magazine \fR, \fB37(12)\fR,
July, 1987, \fB4(3)\fR, pp. 652-663.
.ES
