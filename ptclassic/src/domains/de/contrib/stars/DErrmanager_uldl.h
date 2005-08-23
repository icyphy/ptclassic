#ifndef _DErrmanager_uldl_h
#define _DErrmanager_uldl_h 1
// header file generated from DErrmanager_uldl.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1996 - 1997 Dresden University of Technology,
    Mobile Communications Systems
 */
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "DEcell_list.h"
#include "DEcell_list.h"
# line 54 "DErrmanager_uldl.pl"
typedef struct Tab {
            float I_value;
            float t_value;
        };
        typedef struct Handy {
            int 	startnum;
            Complex	MS_pos;		// Receiver position
            int 	B;	
            float	t_begin;
            float   t_end;
            int	cellnumber;
            float   C_dBm;
            Tab     I_Tab[5];           // to save CIR values of max. 5 slots
            Handy *next;
        };

class DErrmanager_uldl : public DEcell_list
{
public:
	DErrmanager_uldl();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void wrapup();
	/* virtual */ void begin();
	/* virtual */ ~DErrmanager_uldl();
	InDEPort start;
	InDEPort stop;
	InDEPort Rx_Pos;
	InDEPort cellnum;
	InDEPort bandw;
	InDEPort C_dBm;
	OutDEPort check;
	OutDEPort MS_Position;
	OutDEPort cellnumb;

protected:
	/* virtual */ void go();
private:
# line 75 "DErrmanager_uldl.pl"
Handy *handy, *active;
        int band, cellno, del_cellno, startvar, stopvar, shifter, hop, eject_user, ejected_user, usersum;
        float s_time, e_time;
	void delete_handy ();
	void refresh_slot_man ();
	void initial_slot_man ();
	void refresh ();
	Handy * new_handy ();

};
#endif
