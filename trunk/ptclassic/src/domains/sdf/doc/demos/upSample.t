.\" $Id$
.NA "upSample"
.SD
Convert from the compact disc sampling rate (44.1 kHz) to the digital
audio tape sampling rate (48 kHz).
The conversion is performed in multiple stages for better performance.
.SE
.SV $Revision$ $Date$
.AL "T. M. Parks"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.LD
.EQ
delim $$
.EN
.pp
Converting sampling rates from 44.1 kHz to 48 kHz is a difficult problem.
A naive approach would be to interpolate (upsample) to a sampling frequency
which is the least common multiple of these two frequencies, filter to
prevent aliasing, then decimate (downsample) to the desired output rate.
Unfortunately the sampling rate ratio in this case is 160:147.
This would require interpolating to an intermediate frequency of 7.056 MHz
(160 x 44.1 kHz = 7.056 MHz).
Designing a lowpass filter with a pass band of 0-20 kHz and a stop band
of 22.05-3528 kHz would be very challenging.
Such a high-Q filter would require many, many coefficients to obtain
reasonable performance.
(High-Q means a high quality factor; i.e., the poles are close to the
unit circle, so the filter will be susceptible to oscillatory behavior
and instability.)
.pp
A more reasonable approach is to perform the rate conversion in
multiple stages.
Rate conversion ratios are chosen by examining the prime factorization
of the two sampling rates.
The prime factorizations of 48000 and 44100 are
$2 sup 7 ^times^ 3 ^times^ 5 sup 3$ and
$2 sup 2 ^times^ 3 sup 2 ^times^ 5 sup 2 ^times^ 7 sup 2$, respectively.
Thus, the ratio 48000:44100 is $2 sup 5 ^times^ 5 ~:~ 3 ^times^ 7 sup 2$
or 160:147.
In this example the conversion is performed in four stages--- 2:1, 4:3, 5:7,
and 4:7.
.pp
The first stage requires a filter with a relatively sharp cut-off with
a transition band from 20-22.05 kHz.
Because of this, the ratio for this stage was chosen to be 2:1.
With the smallest possible interpolation factor of 2, the cut-off frequency
of 20 kHz is as high as possible with respect to the intermediate sampling
rate (88.2 kHz in this case).
This means that the filter for this stage will require fewer coefficients
than if a higher interpolation factor had been chosen.
Unfortunately, no decimation can take place in this stage since the smallest
decimation factor, which is 3, would result in a loss of high-frequency
energy from the original signal.
.pp
The first filter, which has 173 taps, interpolates by a factor of 2 and
does not decimate.
The pass band is 0-20 kHz and the stop band is 22.05-44.1 kHz.
Note that the filter operates at a sampling rate of
$2 ^times^ 44.1 ~=~ 88.2$ kHz.
The output of this filter is a signal at a 88.2 kHz sampling rate with
no energy above 22.05 kHz.
.pp
The second filter, which has 31 taps, interpolates by a factor of 4 and
decimates by a factor of 3.
The pass band is 0-20 kHz and the stop bands are 44.1 kHz wide and are
centered at multiples of 88.2 kHz (the sampling rate of the input to
this stage).
More specifically, the stop bands are 66.15-110.25 kHz and 154.35-176.4 kHz.
Note that this filter operates at a sampling rate of
$4 ^times^ 88.2 ~=~ 352.8$ kHz.
The output of this filter is a signal at a 117.6 kHz sampling rate.
.pp
The third filter, which has 33 taps, interpolates by a factor of 5 and
decimates by a factor of 7.
The pass band is 0-20 kHz and the stop bands are 44.1 kHz wide and
are centered at multiples of 117.6 kHz (the sampling rate of the input
to this stage).
More specifically, the stop bands are 95.55-139.65 kHz and 213.15-257.25 kHz.
Note that this filter operates at a sampling rate of
$5 ^times^ 117.6 ~=~ 588$ kHz.
The output of this filter is a signal at a 84 kHz sampling rate.
.pp
The fourth filter, which has 33 taps, interpolates by a factor of 4 and
decimates by a factor of 7.
The pass band is 0-20 kHz and the stop bands are 44.1 kHz wide and
are centered at multiples of 84 kHz (the sampling rate of the input
to this stage).
More specifically, the stop bands are 61.95-106.05 kHz and 145.95-168 kHz.
Note that this filter operates at a sampling rate of
$4 ^times^ 84 ~=~ 336$ kHz.
The output of this filter is a signal at a 48 kHz sampling rate.
.pp
Because the second filter has the same interpolation factor as the
fourth and operates at a higher rate, it can actually use the same
filter coefficients.
This could be useful in an implementation where only
small amounts of memory are available for storing filter coefficients.
.SA
downSample,
rateChange.
.ES
