#ifndef _Wireless_h
#define _Wireless_h

/*
Version identification:
@(#)Wireless.h	1.4	9/17/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

Programmer: John S. Davis, II

 */

#ifdef __GNUG__
#pragma	interface
#endif

#include "miscFuncs.h"
#include "ComplexSubset.h"

#include <Uniform.h>

#define LIGHT 3e8

class ChannelParameters
{
   public:
	// No Constructor/Destructor Needed
	double wc;		// Carrier Frequency
	double Ts;		// Symbol (Sample) Period
	double Td;              // RMS Delay Spread
	double Fd;              // Doppler Spread
	double Tc;		// Coherence Time = 1/Doppler Spread
	double loss_rate;       // Path Loss Rate
	double ref_power_loss;	// Reference Power (1 Meter From Tx Antenna)
	double mean_distance;	// Tx/Rx Mean Separation Distance in Meters
	char channel_type; 	// Type Of Wireless Channel (Indoor, Outdoor Mobile) 
};

class WirelessChannel 
{
   public:
	// Setup Up Parameters
	void Setup( const ChannelParameters channelparams );

	// Input Data (Symbols)
	Complex Input( Complex input );

	// Constructor
	WirelessChannel();

	// Destructor 
	~WirelessChannel();

   private:
	Complex* orig_symbols;		// Data Sent Into Channel Unmodified
	Complex* corrupt_symbols;	// Corrupted Data Symbol After Channel

	ChannelParameters params;

	int sample_cnt;
	int no_paths;

	double PhaseFromDistance( int );
	Complex SetPathLoss( int path, Complex number );
	Complex PhasorToRectangular( double phase, double mag );

	// Declare Pointer To Random Number Generator Function 
	Uniform* random;

	// Shift Data By One Sample
	void ShiftData( Complex );

	// Corrupt Data 
	void CorruptData();
};

#endif
