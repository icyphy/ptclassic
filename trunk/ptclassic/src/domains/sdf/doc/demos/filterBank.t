.\" $Id$
.NA filterBank
.SD
This universe implements an eight-level perfect reconstruction one-dimensional
filter bank based on the biorthogonal wavelet decomposition.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Alan Peevers"
.LD
.pp
.Ie "filter bank"
.Ie "filter, multirate"
.Ie "perfect reconstruction"
.Ie "wavelet transform"
.Ie "wavelets"
The biorthogonal wavelet decomposition is described in [1].
.Ir "Herley, C."
.Ir "Vetterli, M."
This filter bank is a form of subband coder, in which the sample rate
decreases by powers of two, thereby forming a dyadic decomposition of the
frequency axis.
Each stage of the analysis section splits the incoming signal into
highpass and lowpass components via two (multirate) FIR filter stars.
These components are then decimated by a factor of two.
The decimated highpass signal proceeds directly to the synthesis filter bank,
whereas the decimated lowpass signal is sent to the next level of analysis.
The effect is to split the frequency domain progressively into finer bins,
with higher resolution at the lower frequencies.
This frequency decomposition can be viewed as a binary 'tree' structure, 
in which successive representations have progressively better frequency
resolution at the expense of progressively lower time resolution. 
Such octave-band frequency decompositions have been studied extensively
since the early 1970's [2-3].
.Ir "Smith, M. J. T."
.Ir "Barnwell, T. P."
.pp
The multi-resolution representation at the analysis bank outputs is actually
the Discrete Wavelet Transform (DWT) decomposition of the original signal.
The DWT is characterized by having this power-of-two resolution decomposition. 
.pp
The synthesis filter bank performs the inverse function of the analysis bank.
That is, incoming signals from two levels of the analysis section are each 
upsampled by 2, passed through a different set of FIR filters, and added 
together, yielding a new signal at twice the component signals' sampling rate.
This then gets combined with the next highest rate signal from the analysis
bank, and so on, until the original sampling rate is restored. 
.pp
For specially designed FIR filters, the reconstructed signal will be an exact
replica of the input (to within coefficient round-off error).
This property, known as the 'perfect reconstruction' property,
has been studied in great detail in the literature [4].
.Ir "Vaidyanathan, P. P."
.pp
All analysis stages are identical, as are all synthesis stages.
There are, in all, only four unique FIR filters used.
The impulse response of the analysis filters form a pair of biorthogonal
wavelets.
Biorthogonality is not quite as strong of a condition as is orthogonality,
and the reader is referred to Vetterli \fIet al.\fR for more details [1].
.pp
Concerning the schematic layout,
.c ana
is the basic analysis filter block, and
.c syn
is the corresponding synthesis block.
The source used is a sine wave burst.
Note the sharp edges in the output image at the precise onset and offset
times of the burst.
By typing 'e' in the image window, the color map can be edited to enhance
details.
The galaxy prescaler has gain and offset parameters which can be
adjusted to optimize the dynamic range of the image.
.pp
The \fIsfb\fR universe is a four-level decomposition using the same filters.
It runs much more quickly.
.SA
sfb
.UH References
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
