defstar {
	name { WirelessChannel }
	domain { SDF }
	version { $Id$ }
	author { John S. Davis, II }
	acknowledge { Brian L. Evans }
	location { SDF user contribution library }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc {
Wireless Channel - Equivalent Complex Baseband Channel.
This model takes multipath propagation into account.
	}

        explanation {
The Wireless Channel Star assumes complex baseband input.
The two "most" important parameters are \fIRMSDelaySpread\fR and
the \fISymbolRate\fR (sample rate).
Symbol \fIPeriods\fR (1/\fISymbolRate\fR) which are not significantly
less than the \fIRMSDelaySpread\fR will result in an environment
that is essentially not a multipath channel.

We make the following assumptions.
First, that the signal velocity is equal to the speed of light.
Second, the signal propagation distance varies about the
\fIMeanSeparationDistance\fR random phase variation.
Note that this Star does not consider shadowing effects.

This star is designed primarily for modeling indoor wireless 
channels but can be adapted to outdoor mobile communication
channels by changing the \fIChannelType\fR parameter from the 
default of "Indoor" to "Outdoor."
Given this change, the \fIDopplerSpread\fR (which is otherwise unused)
will be incorporated via the coherence bandwidth to model the degree
of time variation within the channel.
One problem with this scheme is that high symbol rates will require
very long simulations (millions of iterations with each iteration
representing perhaps millionths of a second) in order to see the
effects of the time variation.
	}

	hinclude { <stdlib.h>, "Wireless.h" }

	input { 
		name { DataIn }
		type { complex }
	}

	output {
		name { DataOut }
		type { complex }
	}

	defstate {
		name { CarrierFrequency }
		type { float }
		default { 1000.0 }
		desc { Carrier Frequency In MHz }
	}

	defstate {
		name { SymbolRate }
		type { float }
		default { 1500.0 }
		desc { Symbol (Sample) Rate In MHz }
	}

	defstate {
		name { RMSDelaySpread }
		type { float }
		default { 40.0 }
		desc { Root Mean Square Delay Spread In Nanoseconds }
	}

	defstate {
		name { DopplerSpread }
		type { float }
		default { 4.0 }
		desc { Frequency Spreading In Hertz }
	}

	defstate {
		name { PathLossRate }
		type { float }
		default { 2.18 }
		desc { Large Scale Signal Variation }
	}

	defstate {
		name { ReferencePowerLoss }
		type { float }
		default { 55.0 }
		desc { The Power Loss At 1 Meter From Transmitter in dB.
		       Note That The Default Is Based Upon Experiments 
		       In Room 550, Cory Hall (UC Berkeley) With Biconical 
		       Antenna And Carrier Frequency At 2.4 GHz.
		}
	}

	defstate {
		name { ChannelType }
		type { string }
		default { "Indoor" }
		desc { Indoor(i;I), OutdoorMobile(o;O) }
	}

	defstate {
		name { MeanSeparationDistance }
		type { float }
		default { 10.0 }
		desc { Mean Separation Distance Between Antennas In Meters }
	}

	protected {
		// Channel Parameters (From States Above)
		ChannelParameters params;

		// Channel Filter
		WirelessChannel Channel;  
	}

	setup {
		// Setup Channel Parameters
		params.wc = CarrierFrequency;
		params.Td = RMSDelaySpread;
		params.Ts = SymbolRate;
		params.Fd = DopplerSpread;
		params.loss_rate = PathLossRate;
		params.ref_power_loss = ReferencePowerLoss;
		params.mean_distance = MeanSeparationDistance;
		const char* type = ChannelType;
		params.channel_type = type[0];

		Channel.Setup(params);  
	}

	go {
		DataOut%0 << Channel.Input( Complex(DataIn%0) );
	}

}
