#ifndef _DECIR_uldl_h
#define _DECIR_uldl_h 1
// header file generated from DECIR_uldl.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems
 */
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <complex.h>
#include <string.h>
#include <math.h>
#include "DErrmanager_uldl.h"
#include "DEpropag_uldl.h"
#include "DEcell_list.h"
#include "StringState.h"
# line 51 "DECIR_uldl.pl"
//declare list_h for that file
        extern Handy *list_h;

class DECIR_uldl : public DEcell_list
{
public:
	DECIR_uldl();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void wrapup();
	/* virtual */ void begin();
	/* virtual */ ~DECIR_uldl();
	InDEPort check;
	OutDEPort CIR_value;
	OutDEPort interferenz;
	OutDEPort strength;
	OutDEPort interferenz_bs;
	OutDEPort strength_bs;

protected:
	/* virtual */ void go();
	StringState fileName;

private:
# line 55 "DECIR_uldl.pl"
DEpropag_uldl prop_class;
        Handy *mobile, *interferer, *mob_count;    
	int slot_counter, slots_lower7, slots_lower23,ss;
	float CIR_min, CIR_max, CIR_aver, CIR_Sum, outage_7, outage_23; 
	double Wink_R, Wink_P[6], Wink_1[6], Wink_2[6];
	void angle (int Lv);
	int angle_check (int Lv);
	void scan_Handy ();

};
#endif
