.\" $Id$
.NA rateChange
.SD
Change the sampling rate of the input signal by a rational factor.
The input, filter, and output are plotted on a common frequency scale.
.SE
.SV $Revision$ $Date$
.AL "T. M. Parks"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.IH
.NE
\fIinput\fR (FLOAT): input signal.
.PE
.OH
.NE
\fIoutput\fR (FLOAT): output signal.
.PE
.SH
.NE
\fIcoef\fR (FLOATARRAY): Coefficients for the anti-aliasing filter.
.DF "0.5 0.5"
.NE
\fIinterpolation\fR (INT): Interpolation factor (numerator).
.DF "1"
.NE
\fIdecimation\fR (INT): Decimation factor (denominator).
.DF "1"
.NE
\fIblockSize\fR (INT): Number of input samples consumed.
.DF "256"
.NE
\fIgeometry\fR (STRING): Geometry specification for the plot.
.DF "+0+0"
.NE
\fIinputRate\fR (FLOAT): Sampling rate of the input singal.
.DF "1.0"
.ET
.LD
Change the sampling rate of \fIinput\fR by a rational factor,
\fIinterpolation\fR:\fIdecimation\fR.
The coefficients of the anti-aliasing filter are given by \fIcoef\fR.
The spectra of \fIinput\fR, \fIcoef\fR, and \fIoutput\fR are plotted
on a common frequency scale.
A block of input samples of size \fIblockSize\fR will be consumed to
produce a single plot.
.SA
blockFFT,
downSample,
upSample.
.ES
