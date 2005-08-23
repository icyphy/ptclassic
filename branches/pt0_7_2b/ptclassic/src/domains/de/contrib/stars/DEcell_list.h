#ifndef _DEcell_list_h
#define _DEcell_list_h 1
// header file generated from DEcell_list.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems
 */
#include "DEStar.h"
#include <complex.h>
#include <iostream.h>
#include <Uniform.h>
#include "IntState.h"
#include "FloatState.h"
# line 40 "DEcell_list.pl"
#define cells 17 
	typedef struct cell {
	    int 	number;         // cell number
	    int		user;		// # user per cell
	    Complex   	cov_area1;      // cell area (window down left)
	    Complex   	cov_area2;      // cell area (   "   up right)
	    double  	dl_freq;        // down link frequency [GHz]
	    double  	ul_freq;	// up link frequency [GHz]
	    float       freq_offset;    // 
	    float   	time_offset;    // +/- 1.5 ms max.
	    Complex	Trans_pos;      // transceiver position
	    double	power;		//      "      power [dBm]
	    double	gain;		//      "      antenna gain
	    int		segments;	//      "          "   segments (0=omni)
	    Complex   	direction[6];	// segment beam direction
	    double	beam_width[6];	//     "   3-dB angle
	};
	struct Punkt {	
	    float x;   
	    float y;
	};

class DEcell_list : public DEStar
{
public:
	DEcell_list();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void wrapup();
	/* virtual */ void begin();
	/* virtual */ ~DEcell_list();
	void time_offset ();

protected:
	/* virtual */ void setup();
	FloatState offset;
	IntState segments;
	IntState frequency_reuse_factor;
# line 63 "DEcell_list.pl"
static cell zellen[cells];
	   float S_time;
           Uniform *random;

};
#endif
