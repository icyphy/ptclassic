.\" $Id$
.NA muxDeMux
.SD
Demonstrate the Mux and DeMux stars, which perform multiplexing and demultiplexing.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
This system shows how to use the
.c Mux
and
.c DeMux
stars.
.Se Mux
.Se DeMux
.Ie "multiplex"
.Ie "demultiplex"
The control signal for each is generated by the
.c WaveForm
star.  A ramp is demultiplexed, so the first two signals displayed
are the two outputs of the demultiplexer.  The control signal is chosen
to repeat the pattern "0 0 0 1 1 1 2 2 2", so the first three ramp outputs
go to the top demultiplexer output, the next three go to the bottom
demultiplexer output, and the final three go nowhere.
When the value of the control signal exceeds the number of outputs,
then all outputs of the demultiplexer produce 0.0.
The first control signal is periodic with period 9
and the second is periodic with period 6, so the overall control
pattern has period 18.
.SA
Distributor, Commutator, Switch, Select.
.ES
