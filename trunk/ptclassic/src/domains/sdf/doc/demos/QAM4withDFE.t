.\" $Id$
.NA QAM4withDFE
.SD
This is a model of a digital communication system that uses quadrature
amplitude modulation and a fractionally spaced decision feedback
equalizer.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF universe
.IE QAM4withDFE
.SV $Revision$ $Date$
.AL "J. T. Buck"
.EQ
delim $$
.EN
.LD
.Ie "modulation, quadrature amplitude"
.Ie "quadrature amplitude modulation"
.Ie "decision feedback equalizer"
.pp
Transversal equalizers, first proposed by Lucky [1], permit the
removal of intersymbol interference in digital communication systems.
A particularly robust structure for accomplishing this is the
fractionally spaced decision feedback equalizer (FS/DFE).
Qureshi and Forney [2] analyze its performance and describe
what they call its most important property, its almost complete
insensitivity to timing phase, provided that the tap spacing
is less than the inverse of twice the highest frequency of
the baseband signal.
.pp
Three rates are required to simulate the FS/DFE: the symbol rate, twice
the symbol rate, and the Nyquist rate of the channel.
This demo exploits the multirate abilities of the Ptolemy SDF domain
to achieve these three rates, with appropriate rate conversions, with
little effort.
.pp
This universe consists of a 4-QAM passband PAM system with a fractionally
spaced decision feedback equalizer.
Reading from left to right, a bit sequence is generated, and is coded by the
.c QAM4
galaxy into complex-valued symbols for transmission.
The symbols are converted into shaped pulses by the
.c CxRaisedCos
block.
The parameters of this block are set so as to use 100% excess bandwidth
and to upsample by a factor of 16 (so the sample rate used to simulate
the channel is 16 times higher than the symbol rate).
.pp
The complex exponential that modulates the shaped pulses has a period
of eight samples (at the channel sample rate).
The modulated signal's imaginary part is dropped and the real part is
scaled up by $sqrt 2$ to preserve the same power.
The resultant signal is the one that is output by the transmitter.
.pp
The transmitted signal is processed by a galaxy that simulates the
effect of a channel with additive Gaussian noise (noise variance is 0.01,
as compared to signal variance of 2).
Next comes the receiver front end: a band pass filter (whose taps were
designed using a program implementing the Parks-McClellan algorithm)
and a phase splitter.
Here the demodulation is accomplished perfectly
(the demodulating signal is the same as the modulating signal); 
the reader may wish to experiment with the effect of demodulating
with a slightly different complex exponential.
.pp
The
.c DownSample
star downsamples the result to twice the symbol rate.
Next comes the forward equalizer.
This equalizer (a
.c CxLMSPlot
star), in addition to implementing the stochastic gradient (or LMS)
algorithm, also does decimation by a factor of two to the symbol rate.
It correctly deals with the differing sample rates of the data input
and error input signals.
This is the ``fractionally spaced'' part of the FS/DFE.
.pp
The forward equalizer is followed by a feedback loop containing
a slicer and another
.c CxLMSPlot
star, which implements the feedback equalizer.  Note that, instead
of just using the decisions from the slicer in the feedback loop,
we use the
.c Trainer
star.
This star passes the training sequence (a delayed version
of the original symbols) for the first 50 symbols (a parameter),
and then passes the decisions from the slicer after that.
This turns out to be crucial to making the receiver work reliably.
.pp
The remaining blocks are present to provide a view of what is going on.
At the top of the figure, baud rate samples are obtained at
the slicer input and the real and imaginary parts are plotted.
At the far right, the ``signal'' (actually, the slicer output) and
``noise'' (the error signal used for adapting the equalizers)
are each fed into a galaxy that is designed for power estimation.
This galaxy,
.c CxPower ,
has a leaky integrator to form an exponential average of the power;
here we use a weight of 0.99 (hence it takes about 100 samples for
the integrator to saturate).
.pp
Executing the simulation produces six plots.
The top left plot shows the signal and noise powers in dB.
The signal power is a constant 3 dB, as the power from the 4-QAM
constellation is 2.
The noise power decreases as the adaptive filters converge, until a
SNR of 26 dB at the slicer is achieved.
Because of the smoothing in the integrator used for this plot, the equalizers
converge somewhat faster than this plot would suggest, as can be seen
by looking at the top right plot.
.pp
The top right plot shows the real and imaginary parts of the first hundred
symbols received at the slicer.
Notice that it only takes about ten samples before substantial separation
occurs (the first few symbols are zero because of the delay in the system).
.pp
The remaining four plots show the magnitudes and phases of the
taps in the forward and feedback equalizers as they adapt.
The phase plots do phase unwrapping so a continuous phase trajectory
is always shown.
.UH REFERENCES
.ip [1]
R. W. Lucky,
``Signal Filtering With the Transversal Equalizer,''
.i "Proc. 7th Annual Allerton Conf. on Circuits and System Theory" ,
pp. 792-804, October 1969
.ip [2]
S. U. H. Qureshi and G. D. Forney, Jr.,
``Performance Properties of a T/2 Equalizer,''
.i NTC '77 Proceedings ,
p. 11:1-1, 1977
.SA
CxLMSPlot,
Trainer,
qam,
Hilbert.
.ES
