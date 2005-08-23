#ifndef _DEWiNeS_Tcl_Animation_h
#define _DEWiNeS_Tcl_Animation_h 1
// header file generated from DEWiNeS_Tcl_Animation.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1997 Dresden University of Technology,
            Mobile Communications Systems
 */
#include "DEStar.h"
#include "TclStarIfc.h"
#include "ptkTclCommands.h"
#include <complex.h>
#include "DErrmanager_uldl.h"
#include "IntState.h"
#include "StringState.h"
# line 76 "DEWiNeS_Tcl_Animation.pl"
// declare list_h for that file
            extern Handy* list_h;
            // Overload TclStarIfc for WiNeS-spezific behavior
            class TclStarIfcWiNeS : public TclStarIfc {
                public:
                TclStarIfcWiNeS();
                ~TclStarIfcWiNeS();
                InfString getInputValue_New ();
                InfString getInputValue_Del ();
		InfString getSetupValue ();
		InfString getInterferenzValue ();
		InfString getInterferenzValue_bs ();
                int mySetup ( Block*, const char*);
                //just to make callTclProc public
                inline int callTcl(const char *name) {
                    return (TclStarIfc::callTclProc(name));
                }
		int segmente,r1,i1,r2,i2,r,i,num,s;
		private:
                Complex mydata;
	    };

class DEWiNeS_Tcl_Animation : public DEStar
{
public:
	DEWiNeS_Tcl_Animation();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void wrapup();
	/* virtual */ void begin();
	InDEPort New_;
	InDEPort Delete_;
	InDEPort strength;
	InDEPort Interferenz;
	InDEPort strength_bs;
	InDEPort Interferenz_bs;

protected:
	/* virtual */ void go();
	StringState tcl_file;
	IntState Segments;
# line 345 "DEWiNeS_Tcl_Animation.pl"
TclStarIfcWiNeS tcl;

private:
# line 349 "DEWiNeS_Tcl_Animation.pl"
Handy *mobile;
	    Complex dummy;

};
#endif
