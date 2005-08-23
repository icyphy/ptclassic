static const char file_id[] = "Wireless.cc";

/*
Version identification:
@(#)Wireless.cc	1.5	9/17/96 

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
    
Bug fixes by Uwe Trautwein (Uwe.Trautwein@E-Technik.TU-Ilmenau.DE)
and Brian L. Evans.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "Wireless.h"

#include <math.h>		// Define cos and sin
#include <float.h>		// Define DBL_MIN
#include <Random.h>		// Define random number generators
#include <ACG.h>

// Declare Random Number Generator Seed
extern ACG* gen;

// Constructor
WirelessChannel::WirelessChannel()
{
	orig_symbols = 0;
	corrupt_symbols = 0;
	random = 0;
}

// Setup Parameters
void WirelessChannel::Setup( const ChannelParameters channelparams )
{
	// Assign Channel Parameters
	params = channelparams;
	params.Td /= 1.0e9;
	params.Ts = 1.0 / ( params.Ts * 1.0e6 );
	params.wc *= 1.0e6;

	// Set The Number Of Filter Taps
	double number_paths = params.Td/params.Ts;
	no_paths = int( floor(number_paths) ) + 1;	

	// Set Up Path Storage Bins
	delete [] orig_symbols;
	delete [] corrupt_symbols;
	orig_symbols = new Complex[ no_paths ];
	corrupt_symbols = new Complex[ no_paths ];

	// Initialize For Beginning Of Simulation
	sample_cnt = 0;
	Complex initial_value( 1.0e-20, 1.0e-20 );
	for( int i = 0; i < no_paths; i++ )
	{
	     orig_symbols[ i ] = initial_value;
	}

	// Define Pointer To Uniform Distribution
	delete random;
	random = new Uniform( -0.5 * M_PI, 0.5 * M_PI, gen );
}


// Shift Data By One Sample
void WirelessChannel::ShiftData( Complex new_input )
{
	// NOTE: The greater the filter tap, the older the data
	for( int i = no_paths - 1; i > 0; i-- )
	{
	     orig_symbols[ i ] = orig_symbols[ i - 1 ];
	}
	orig_symbols[ 0 ] = new_input; 
}


// Accept Input and Create Output Data
Complex WirelessChannel::Input( Complex input )
{
	ShiftData( input );

	sample_cnt++;
	if( params.channel_type == 'i' || params.channel_type == 'I' )
	{
	     CorruptData();
	}
	else if( params.channel_type == 'o' || params.channel_type == 'O' )
	{
	     if( sample_cnt * params.Ts > params.wc )
	     {
		  sample_cnt = 0;
	          CorruptData();
	     }
	}

	// Sum All Received Paths
	Complex sum(0.0, 0.0);
	for( int path = 0; path < no_paths; path++ )
	{
	     sum += corrupt_symbols[ path ];
	}

	return sum;
}


// Corrupt Data
void WirelessChannel::CorruptData()
{
	for(int path = 0; path < no_paths; path++ )
	{
	     // Determine Phase Travelled Through Distance
	     double new_phase = double( path/no_paths ) * params.Td * params.wc;

	     // Add An Additional Random Phase Based On Change In 
	     // Distance Between Tx and Rx Antennas.
	     new_phase += (*random)();

	     // Compute the Phase Modulo 2 PI To Keep It In [-PI, PI]
	     new_phase -= floor((new_phase+M_PI) / (2.0 * M_PI)) * 2.0 * M_PI;

	     // abs Is An Overloaded Operator: see ComplexSubset.h
	     Complex corruptSymbolPhase =
	        PhasorToRectangular( new_phase, abs( orig_symbols[ path ] ) );

	     corrupt_symbols[ path ] = SetPathLoss( path, corruptSymbolPhase );
	}
}


// Convert Phase/Magnitude To Real/Imaginary
Complex WirelessChannel::PhasorToRectangular( double phase, double mag )
{
	Complex result( mag * cos(phase), mag * sin(phase) );
	return result;
}


// Set The Magnitude According To Path Loss
Complex WirelessChannel::SetPathLoss( int path, Complex number )
{
	// FIXME: Prevent the log10(0) from being taken
	// On Linux machines, log10(0) yields Inf instead of -Inf,
	// and power(10,inf) -NaN instead of Inf.  We increment the
	// value to make sure that it is always positive.
	// This is being applied to all platforms
	double tx_power_nat = norm( number ) + DBL_MIN;
	double tx_power_dB = 10.0*log10( tx_power_nat/0.001 );
	double one_meter_power = tx_power_dB - params.ref_power_loss;

	double distance = params.mean_distance +
			  (double(path)/no_paths) * params.Td * LIGHT;

	double rx_power_dB = one_meter_power -
			     10.0*params.loss_rate*log10( distance );
	double rx_power_nat = 0.001*power( 10.0, rx_power_dB/10.0 );
	
	double mag = sqrt( rx_power_nat );
	double phase = arg( number );

	Complex result = PhasorToRectangular( phase, mag );
	return result;
}


// Destructor
WirelessChannel::~WirelessChannel()
{
	delete [] orig_symbols;
	delete [] corrupt_symbols;
	delete random;
}
