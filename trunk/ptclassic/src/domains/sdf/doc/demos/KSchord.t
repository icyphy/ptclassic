.\" $Id$
.NA KSchord
.SD
Simulation of plucked strings using the Karplus-Strong algorithm
.DE
.SV $Revision$ $Date$
.AL "Joseph T. Buck"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.pp
This demo models the plucking of three strings; each string is modeled
by an instance of the galaxy karplusStrongGal.
.pp
The Karplus-Strong algorithm models a string as a tapped delay line.
The initial state of the string is determined by the IIDGaussian source
and is random.  The FIR filter in the feedback loop is a lowpass filter,
so high-frequency energy is selectively reduced.  
.pp
The initial burst yields the fairly broad spectrum characteristic of the
onset of a complex sound, with high frequencies predominant.  Relative to
the fundamental and second harmonic, the higher frequencies die down
fairly rapidly, so that the underlying lower tones prevail to the end.
.Ir "Karplus-Strong algorithm"
.Ir "sound synthesis"
.Ir "plucked string"
.UH "REFERENCES"
.ip [1]
Richard Moore,
.i "Elements of Computer Music" ,
Prentice Hall, pp. 278-291, 1990
.ES
