static const char file_id[] = "Wireless.cc";

/* ********************************************************************** *
Version identification:
$Id$ 

Copyright (c) 1990-1995 The Regents of the University of California.
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
    
 * ********************************************************************** */

#ifdef		__GNUG__
#pragma 	implementation
#endif

#include "Wireless.h"

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

	sample_cnt =+ 1;
	if( params.channel_type == 'i' | params.channel_type == 'I' )
	{
	     CorruptData();
	}
	else if( params.channel_type == 'o' | params.channel_type == 'O' )
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
	for( int path = 0; path < no_paths; path++ )
	{
	     // Determine Phase Travelled Through Distance
	     double new_phase = double( path/no_paths ) * params.Td * params.wc;

	     // Add An Additional Random Phase Based On Change In 
	     // Distance Between Tx and Rx Antennas.
	     new_phase += (*random)();

	     // Is There A Modulo 2 PI Function?
	     if( new_phase > (M_PI/2.0) ) {
		new_phase = ( new_phase/(M_PI/2.0) -
			      floor(new_phase/(M_PI/2.0)) ) * M_PI - M_PI/2.0;
	     }

	     corrupt_symbols[ path ] =
	        PhasorToRectangular( new_phase, abs( orig_symbols[ path ] ) );

	     corrupt_symbols[ path ] =
	        SetPathLoss( path, corrupt_symbols[ path ] );
	}
}


// Convert Phase/Magnitude To Real/Imaginery
Complex WirelessChannel::PhasorToRectangular( double phase, double mag )
{
	double tan_of_phs = tan( phase );
	double real = sqrt( mag*mag / (tan_of_phs*tan_of_phs + 1.0) );
	double imag = tan_of_phs*real;

	Complex result( real, imag );

	return result;
}


// Set The Magnitude According To Path Loss
Complex WirelessChannel::SetPathLoss( int path, Complex number )
{
	double tx_power_nat = norm( number );
	double tx_power_dB = 10.0*log10( tx_power_nat/0.001 );

	double one_meter_power = tx_power_dB - params.ref_power_loss;

	double distance = params.mean_distance 
		+ (double(path)/no_paths) * params.Td * LIGHT;

	double rx_power_dB = one_meter_power 
			- 10.0*params.loss_rate*log10( distance );
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










