.\" $Id$
.NA timeVarSpec
.SD
A time-varying spectrum is computed using the autocorrelation
method and displayed using a waterfall plot.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$	$Date$
.AL "Edward A. Lee"
.EQ
delim $$
.EN
.LD
A signal is generated by filtering white Gaussian noise and then modulating
it with a chirp (a sinusoid with steadily increasing frequency).
.Ie "chirp signal"
.Ie "spectral estimation"
.Ie "time-varying spectrum"
.Ie "spectrum, time-varying"
.Ie "waterfall plot"
.Se "Waterfall
Spectral estimates are repeatedly computed using the autocorrelation method,
and displayed using the
.c Waterfall
star.
Note that in the display, the first plot shows a roughly baseband spectrum,
while in subsequent displays the spectrum is centered at successively higher
frequencies.
Note also that hidden points are not displayed.
This can be changed by altering the parameters of the
.c Waterfall
star.
.SA
autocorrelation,
powerSpectrum,
Waterfall.
.ES