defstar {
	name { WirelessChannel }
	domain { SDF }
	version { $Id$ }
	author { John S. Davis, II }
	location { SDF user contribution library }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc {
Wireless Channel - Equivalent Complex Baseband Channel.
This model takes multipath proprogation into account.
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
		// Channel Paramters (From States Above)
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
