.\" $Id$
.NA downSample
.SD
Convert from the digital audio tape sampling rate (48 kHz) to the
compact disc sampling rate (44.1 kHz).
The conversion is performed in multiple stages for better performance.
.SE
.SV $Revision$ $Date$
.AL "T. M. Parks"
.EQ
delim off
.EN
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.EQ
delim $$
.EN
.LD
.EQ
delim $$
.EN
.pp
Converting sampling rates from 48 kHz to 44.1 kHz is a difficult problem.
A naive approach would be to interpolate (upsample) to a sampling frequency
which is the least common multiple of these two frequencies, filter to
prevent aliasing, and then decimate (downsample) to the desired output rate.
Unfortunately, the sampling rate ratio in this case is 147:160.
This would require interpolating to an intermediate frequency of 7.056 MHz
(147 x 48 kHz = 7.056 MHz).
Designing a lowpass filter with a passband of 0-20 kHz and a stop band of
24-3528 kHz would be very challenging.
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
The prime factorizations of 44100 and 48000 are
$2 sup 2 ^times^ 3 sup 2 ^times^ 5 sup 2 ^times^ 7 sup 2$
and $2 sup 7 ^times^ 3 ^times^ 5 sup 3$, respectively.
Thus, the ratio 44100:48000 is $3 ^times^ 7 sup 2 ~:~ 2 sup 5 ^times^ 5$ or
147:160.
In this example the conversion is performed in three stages ---
3:2, 7:5, and 7:16.
.pp
The first stage requires a filter with a relatively sharp cut-off with
a transition band from 20-24 kHz.
Because of this, the ratio for this stage was chosen to be 3:2.
With the smallest possible interpolation factor of 3, the cut-off frequency
of 20 kHz is as high as possible with respect to the intermediate sampling
rate (144 kHz in this case).
This means that the filter for this stage will require fewer
coefficients than if a higher interpolation factor had been chosen.
The two remaining stages must each interpolate by a factor of 7, and
only in the final stage is the sampling rate actually reduced.
.pp
The first filter, which has 147 taps, interpolates by a factor of 3 and
decimates by a factor of 2.
The pass band is 0-20 kHz and the stop band is 24-72 kHz.
Note that the filter operates at a sampling rate of
$3 ^times^ 48 ~=~ 144$ kHz.
The output of this filter is a signal at a 72 kHz sampling rate with no
energy above 24 kHz.
.pp
The second filter, which has 75 taps, interpolates by a factor of 7 and
decimates by a factor of 5.
The passband is 0-20 kHz and the stop bands are 48 kHz wide and are centered
at multiples of 72 kHz (the sampling rate of the input to this stage).
More specifically, the stop bands are 48-96 kHz, 120-168 kHz, and 192-240 kHz.
Note that this filter operates at a sampling rate of
$7 ^times^ 72 ~=~ 504$ kHz.
The output of this filter is a signal at a 100.8 kHz sampling rate.
.pp
The third filter, which has 53 taps, interpolates by a factor of 7 and
decimates by a factor of 16.
The pass band is 0-20 kHz and the stop bands are 48 kHz wide and are
centered at multiples of 100.8 kHz (the sampling rate of the input to
this stage).
More specifically, the stop bands are 76.8-124.8 kHz, 177.6-225.6 kHz,
and 278.4-326.4 kHz.
Note that this filter operates at a sampling rate of
$7 ^times^100.8 ~=~ 705.6$ kHz.
The output of this filter is a signal at a 44.1 kHz sampling rate.
.pp
Because the third filter has the same interpolation factor as the
second and operates at a higher rate, it can actually use the same
filter coefficients.
This could be useful in an implementation where only small amounts of
memory are available for storing filter coefficients.
.SA
upSample,
rateChange.
.ES
