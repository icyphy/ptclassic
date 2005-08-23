#ifndef _DEpropag_uldl_h
#define _DEpropag_uldl_h 1
// header file generated from DEpropag_uldl.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1996 - 1997 Dresden University of Technology,
                    Mobile Communications Systems
 */
#include <stdio.h>
#include <string.h>
#include <fstream.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <iomanip.h>
#include "pt_fstream.h"
#include "DEcell_list.h"
#include "StringState.h"

class DEpropag_uldl : public DEcell_list
{
public:
	DEpropag_uldl();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	/* virtual */ void begin();
	InDEPort MS_Position;
	InDEPort cellnumb;
	OutDEPort C_dBm;
	float BS_Stoerer (int I_number, int C_number, const char* topoFileName);
	float MS_Stoerer (int number, Complex Tposition, const char* topoFileName);

protected:
	/* virtual */ void go();
	StringState fileName;

private:
# line 45 "DEpropag_uldl.pl"
enum Umgebung { Fenster, Fenster1, Fenster2, Tuer, Tuer2, Metall_Tuer, Metall_Tuer1,
 			Glas_Tuer, Glas_Doppel_Tuer, Doppel_Tuer, Doppel_Tuer2, Fahrstuhltuer,
		 	Saeule, Ziegelwand1, Ziegelwand2, Ziegelwand3, Ziegelwand4, Ziegelwand5,
		 	Ziegelwand6, Ziegelwand7, Ziegelwand8, Ziegelwand9, Ziegelwand10,
 			Ziegelwand11, Ziegelwand12, Ziegelwand13, Ziegelwand14, Ziegelwand15,
 			Ziegelwand16, Ziegelwand17, Ziegelwand18,
			Gipswand1, Gipswand2, Gipswand3, Gipswand4, Gipswand5, Metallabschirm1,
			Metallabschirm2, Metallabschirm3};
	Umgebung Name;
        
        

        int     LI_number, LC_number, Bedingung, tunnel_flag;
	Complex position;

	int 	Gelesen, Durchg, SP_Anz, Art[50], Objekt_SP[39];
	double 	Mass, x_pos, y_pos, erg, d_m, Abst[50], Sum_OLoss, Prec_dBm;
	char 	EP_x[14], EP_y[14];

	Punkt 	FenAn[4], FenEn[4], Fen1An[4], Fen1En[4], Fen2An[4], Fen2En[4];
	Punkt  	TuerAn, TuerEn, Tuer2An, Tuer2En,
		GD_TuerAn, GD_TuerEn, D_TuerAn, D_TuerEn, D_Tuer2An, D_Tuer2En, FtuerAn, FtuerEn,
		SaeuleAn[4], SaeuleEn[4];
	Punkt   ZW1An[6], ZW1En[6], ZW2An[4], ZW2En[4], ZW3An[8], ZW3En[8], ZW4An[12], ZW4En[12],
		ZW5An[12], ZW5En[12], ZW6An[12], ZW6En[12], ZW7An[12], ZW7En[12], ZW8An[16],
 		ZW8En[16], ZW9An[6], ZW9En[6], ZW10An[16], ZW10En[16], ZW11An[16], ZW11En[16],
 		ZW12An[30], ZW12En[30], ZW13An[12], ZW13En[12], ZW14An[12], ZW14En[12], ZW15An[12],
 		ZW15En[12], ZW16An[12], ZW16En[12], ZW17An[12], ZW17En[12], ZW18An[16], ZW18En[16],
		GW1An[4], GW1En[4], GW2An[4], GW2En[4], GW3An[4], GW3En[4], GW4An[4], GW4En[4],
		GW5An[4], GW5En[4], MW1An[4], MW1En[4], MW2An[4], MW2En[4], MW3An[4], MW3En[4];
	Punkt 	HinAn[283],     HinEn[283];        		

	Punkt 	FenEP[20], Fen1EP[6], Fen2EP, TuerEP[7], Tuer2EP[2],
		M_TuerEP[4], M_Tuer1EP, G_TuerEP, GD_TuerEP, D_TuerEP, D_Tuer2EP, FtuerEP,
		SaeuleEP[4],
		ZW1EP, ZW2EP[13], ZW3EP, ZW4EP, ZW5EP, ZW6EP, ZW7EP, ZW8EP, ZW9EP,
		ZW10EP, ZW11EP, ZW12EP, ZW13EP, ZW14EP, ZW15EP, ZW16EP, ZW17EP, ZW18EP, GW1EP,
		GW2EP, GW3EP[2], GW4EP[2], GW5EP[2],
		MW1EP, MW2EP, MW3EP;
	Punkt 	EP[91];                      			

	Punkt 	A, E;
	Punkt 	S_P, S_Pr, SP[50];
	void Abzweig (const char* topoFileName);
	void entfernen (Punkt& P1, Punkt& P2, int& I1, int& I2, double& Z1, double& Z2);
	void tauschen (Punkt& P1, Punkt& P2, int& I1, int& I2, double& Z1, double& Z2);
	void File_lesen (const char *name);
	double Tuer_x_schliessen (double Tuerparam1, double Tuerparam2);
	double Tuer_y_schliessen (double Tuerparam1, double Tuerparam2);
	int Namenserfassung (char B_Name[14]);
	void Sortierung ();
	void Objekte_abtast ();
	void Koord_norm (int L1, int L2);
	void Runden (double nmbr);
	void SP_berechnung ();
	double Berechnung ();

};
#endif
