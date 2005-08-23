#ifndef _DEMobile_Station_h
#define _DEMobile_Station_h 1
// header file generated from DEMobile_Station.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems
 */
#include "DEcell_list.h"
#include <complex.h>
#include <fstream.h>
#include <iomanip.h>
#include <math.h>

class DEMobile_Station : public DEcell_list
{
public:
	DEMobile_Station();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void begin();
	InDEPort R_Pos;
	InDEPort start;
	OutDEPort restart;
	OutDEPort Rx_Pos;
	OutDEPort cellnum;
	OutDEPort bandw;

protected:
	/* virtual */ void go();
private:
# line 42 "DEMobile_Station.pl"
int i, Bed_x1, Bed_x2, Bed_y1, Bed_y2;
        int flag;
        double Var1, Var2, Var3, Var4, Rx, Ry, Wink_R, Wink_P[6], Wink_1[6], Wink_2[6];
        Complex Rec;
	void cell_verif ();
	void angle (int Lv);
	int angle_check (int Lv);

};
#endif
