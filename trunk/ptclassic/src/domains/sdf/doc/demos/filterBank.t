.\" $Id$
.NA filterBank
.SD
Discrete Wavelet Transform Filter Bank
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Date$ "November 18, 1992"
.AL "Alan Peevers"
.LD
.pp
.Ie "filter bank"
.Ie "filter, multirate"
.Ie "perfect reconstruction"
.Ie "wavelet transform"
.Ie "wavelets"
This universe implements an 8-level perfect reconstruction filter bank
based on a biorthogonal wavelet decomposition. It is inspired by the paper
"Wavelets and Filter Banks: Theory and Design", \fIIEEE Transactions on
Signal Processing\fR, \fB40(9)\fR, September, 1992.
.Ir "M. Vetterli and C. Herley"
This is a form of sub-band coder, where the sample rate is decreasing by
powers of two, forming a dyadic decomposition of the frequency axis.
At each stage of the analysis section, the incoming signal is split into
highpass and lowpass components via two FIR filters. These components are
then decimated by a factor of two. The decimated high-pass signal proceeds
directly to the synthesis filterbank, while the decimated low-pass signal
gets sent down to the next level of analysis. The effect is to split the
frequency domain into progressively finer bins, with the highest resolution
at the lowest frequencies.This can be seen as a binary 'tree' structure, 
where successive representations have progressively better frequency resolution
at the expense of progressively lower time resolution. 
.pp
The multi-resolution representation at the analysis bank outputs is actually
the Discrete Wavelet Transform (DWT) decomposition of the original signal. The 
DWT is characterized by having this power-of-2 resolution decomposition. 
.pp
The synthesis filter bank performs the inverse function of the analysis bank.
That is, incoming signals from two levels of the analysis section are each 
upsampled by 2, passed through a different set of FIR filters, and added 
together, yielding a new signal at twice the component signals' sampling rate.
This then gets combined with the next highest rate signal from the analysis
bank, and so on, until the original sampling rate is restored. 
.pp
For specially designed FIR filters, the reconstructed signal will be an exact
replica of the input (to within coefficient roundoff error). This property,
known as the 'perfect reconstruction' property, has been studied in great
detail in the literature. See, for example, "Quadrature Mirror Filter Banks,
M-Band Extensions, and Perfect-Reconstruction Techniques", \fIIEEE ASSP Magazine
\fR, \fB37(12)\fR, July, 1987. 
.pp
All analysis stages are identical, as are all synthesis stages. There are, in 
all, only 4 unique FIR filters used. The impulse response of the analysis 
filters form a pair of biorthogonal wavelets. Biorthogonality is not quite as
strong as orthogonality, and the reader is referred to Vetterli et al for
more details.
.pp
.c ana is the basic analysis filter block, and .c syn is the corresponding
synthesis block. The source used is a sinewave burst. Note the sharp edges
in the output image at the precise onset and offset times of the burst.
By typing 'e' in the image window, the color map can be edited to enhance
details. The galaxy prescaler has gain and offset parameters which can be
adjusted to optimize the dynamic range of the image.
.pp
The \fIsfb\fR universe is a four-level decomposition using the same filters.
It runs much more quickly.
.SA
sfb
.ES
