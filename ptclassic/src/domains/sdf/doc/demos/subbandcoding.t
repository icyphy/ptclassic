.\" $Id$
.NA subbandcoding
.SD
This universe performs four channel subband speech coding
with adaptive pulse code modulation at 16 kbps.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "William Chen"
.EQ
delim $$
.EN
.LD
.Ie "filter bank"
.Ie "filter, multirate"
The incoming speech signal is sampled at 8 kHz and divided into the following
four subbands: 0-500 Hz, 500-1000 Hz, 1000-2000 Hz, 2000-3000 Hz.
Each subband signal is then coded separately using APCM.  
Subband coding gives the flexibility of encoding each subband separately,
according to some perceptual criterion for that band.  In this universe,
a total of 16 kilobits per second are unequally distributed among the four
channels: the lower two channels are coded at 4 bits per second and the higher two 
channels are coded at 2 bits per second.  

APCM coding is used on the subbands for two reasons.  First, common encoders
for full band speech do not perform as well.  Techniques such as ADPCM
require that the signal be highly correlated from sample to sample.
In subband speech, the signals have low sample to sample correlation so that
adaptive pcm is used.  In APCM, the current step size is constantly adapting,
with the current step depending on the previous step, previous amplitude, and some
perceptual weighting.

On reconstruction, the subband signals are pass through reconstructing filters
and summed together to give a replica of the original signal. 

.SA
.UH REFERENCES
.ip [1]
R.E. Crochiere,
``On the Design of Subband Coders for Low Bit Rate Speech Communication,''
\fIBell System Technical Journal\fR,
vol. 56, no. 5, pp. 747-770, Nov, 1977.
.ES

