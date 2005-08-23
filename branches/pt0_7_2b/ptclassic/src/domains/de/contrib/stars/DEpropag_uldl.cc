static const char file_id[] = "DEpropag_uldl.pl";
// .cc file generated from DEpropag_uldl.pl by ptlang
/*
copyright (c) 1996 - 1997 Dresden University of Technology,
                    Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEpropag_uldl.h"

const char *star_nm_DEpropag_uldl = "DEpropag_uldl";

const char* DEpropag_uldl :: className() const {return star_nm_DEpropag_uldl;}

ISA_FUNC(DEpropag_uldl,DEcell_list);

Block* DEpropag_uldl :: makeNew() const { LOG_NEW; return new DEpropag_uldl;}

DEpropag_uldl::DEpropag_uldl ()
{
	setDescriptor("This star implements a path loss model. A specific enviroment is\n        read in from a AutoCad-file. All obstacles which are in a path are \n        searched for and their penetration loss is added to the path loss caused\n        by free space propagation. The resulting path loss is given out.\n        WARNING: I don't recommend anybody to trace down the following code. It\n        is kind of a quick hack and not very readable for others. Also, please \n        excuse some comments in German I left behind. They are to many to  \n        translate them all ...\n        Moreover, the implementation highly depends on the environment I use. \n        This is not a generic path-loss-model for any AutoCad-enviroment !!!");
	addPort(MS_Position.setPort("MS_Position",this,COMPLEX));
	addPort(cellnumb.setPort("cellnumb",this,INT));
	addPort(C_dBm.setPort("C_dBm",this,FLOAT));
	addState(fileName.setState("fileName",this,"$PTOLEMY/src/domains/de/contrib/stars/topo.dxf","File describing topology"));

# line 92 "DEpropag_uldl.pl"
Gelesen = 0;
                for (int i = 1; i < 50; i++) {
                    Art[i] = 0;
                }
}

void DEpropag_uldl::begin() {
# line 98 "DEpropag_uldl.pl"
tunnel_flag=0;
    DEcell_list::begin();
}

void DEpropag_uldl::go() {
# line 1694 "DEpropag_uldl.pl"
if (MS_Position.dataNew) 
       		{       
			tunnel_flag=0;
        		LC_number = cellnumb.get();
			position = MS_Position.get();		// MS: Transceiver-pos

			Abzweig(fileName);
			C_dBm.put(arrivalTime) << Berechnung();			
       		}
}


float DEpropag_uldl::BS_Stoerer (int I_number, int C_number, const char* topoFileName)
{
# line 110 "DEpropag_uldl.pl"
tunnel_flag=1;
	 LI_number=I_number;			// interf. BS-cellnumber
	 LC_number=C_number;		        // desired BS-Pos

	 Abzweig(topoFileName);
	 return (Berechnung());
}


float DEpropag_uldl::MS_Stoerer (int number, Complex Tposition, const char* topoFileName)
{
# line 126 "DEpropag_uldl.pl"
tunnel_flag=0;
	 LC_number=number;			// interf. BS-cellnumber
	 position=Tposition;		        // desired BS-Pos

	 Abzweig(topoFileName);
	 return (Berechnung());
}


void DEpropag_uldl::Abzweig (const char* topoFileName)
{
# line 143 "DEpropag_uldl.pl"
int 	j, m, n;
	double 	F1, F2, Kath_x, Kath_y;
	
	Mass=100;

	x_pos =real (zellen[LC_number].Trans_pos);		      // Receiver position (desired BS)
	y_pos =imag (zellen[LC_number].Trans_pos);

	if (tunnel_flag==0)
	{
		F1= (x_pos -real (position))/Mass;		
		F2= (y_pos -imag (position))/Mass;
	}
	else
	{
		F1 = (x_pos -real (zellen[LI_number].Trans_pos))/Mass;        
		F2 = (y_pos -imag (zellen[LI_number].Trans_pos))/Mass;
	}

	Kath_x= F1*F1; 
	Kath_y= F2*F2;
	d_m    = sqrt (Kath_x + Kath_y);
	if (d_m < 1)
	{
		d_m=1;				
	}
	if (Gelesen==1)  Objekte_abtast();    
	else
	{
		File_lesen(topoFileName);
		Sortierung();
		Objekte_abtast();
	}

	for (m=0; m<SP_Anz-1; m++)                		// SP einer Art ist doppelt (z.B. Hindernisecke)
	for (n=m+1; n<SP_Anz; n++)                		//  => SP[m] wird entfernt
	{
		if ((SP[m].x == SP[n].x) && (SP[m].y == SP[n].y) && (Art[m] == Art[n]))
		{
			for (j=m+1; j<SP_Anz; j++)                	// [m] wird entfernt
				entfernen (SP[j-1], SP[j], Art[j-1], Art[j], Abst[j-1], Abst[j]);
			Objekt_SP[Art[m]]--;                            // Rest rueckt nach
			SP_Anz--;
		}
	}
	for (m=0; m<SP_Anz-1; m++)                	// Sortieren des SP-Feldes,
	for (n=m+1; n<SP_Anz; n++)                	// beginnend mit dem kleinsten
	{                                    	  	// Abstandswert
		if (Abst[m] > Abst[n])
			tauschen (SP[m], SP[n], Art[m], Art[n], Abst[m], Abst[n]);
	}
	for (m=0; m<SP_Anz-1; m++)                // Sortiertes SP-Feld,
	for (n=m+1; n<SP_Anz; n++)                // beginnend mit dem kleinsten
	{                                    	  // Abstandswert+Typpaarung
		if ((Abst[m] == Abst[n]) && (Art[m] != Art[n]))
		{
			if (n==1)
				break;
			if (Art[n-2]==Art[n])
				tauschen (SP[m], SP[n], Art[m], Art[n], Abst[m], Abst[n]);
		}
	}
}


void DEpropag_uldl::entfernen (Punkt& P1, Punkt& P2, int& I1, int& I2, double& Z1, double& Z2)
{
# line 216 "DEpropag_uldl.pl"
P1 = P2;
	I1 = I2;
	Z1 = Z2;
}


void DEpropag_uldl::tauschen (Punkt& P1, Punkt& P2, int& I1, int& I2, double& Z1, double& Z2)
{
# line 229 "DEpropag_uldl.pl"
Punkt  temp1= P1;
	int    temp2= I1;
	double temp3= Z1;
	P1 = P2;
	I1 = I2;
	Z1 = Z2;
	P2 = temp1;
	I2 = temp2;
	Z2 = temp3;
}


void DEpropag_uldl::File_lesen (const char *name)
{
# line 250 "DEpropag_uldl.pl"
int 		v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12,v13,v14,v15,
		v16,v17,v18,v19,v20,v21,v22,v23,v24,v25,v26,v27,v28,
		w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,i,ok, Schalter;
   char 	Zeile[13], Blockname[14],
		VecAnx[14], VecAny[14], VecEnx[14], VecEny[14];
   double 	Tuerbog[5];
   
  
   pt_ifstream in;
   in.close();
   in.open(name);
   if (!in) {
	Error::abortRun(*this, "can't open dxf-file  ", fileName);
	}
   else
   {
   while (in)         	{
	in.getline (Zeile, sizeof(Zeile));
	if (!strcmp (Zeile, "BLOCK"))
	    {
	    for (i=0; i<14 ; i++)
		in.getline (Zeile, sizeof(Zeile));
	    strcpy (Blockname, Zeile);
 	  
	    Schalter= Namenserfassung (Blockname);
	    ok=((Schalter>=1) && (Schalter<40));
	    if (ok!=1)              	    			// Ausschliessen der Schraffurbloecke u. ae.
		continue;

	    v1=v2=v3=v4=v5=v6=v7=v8=v9=v10=v11=v12=v13=v14=v15=v16=v17=v18=v19=0;
	    v20=v21=v22=v23=v24=v25=v26=v27=v28=0;
	   
	    while (strcmp (Zeile, "ENDBLK"))
	    {
		in.getline (Zeile, sizeof(Zeile));
		if (!strcmp (Zeile, "ARC"))			// Tuerboegen einlesen
		    {
		    for (i=0; i<4; i++)
			in.getline (Zeile, sizeof(Zeile));
		    Tuerbog[0]= atof(Zeile);             // x_m
		    for (i=0; i<2; i++)
			in.getline (Zeile, sizeof(Zeile));
		    Tuerbog[1]= atof(Zeile);             // y_m
		    for (i=0; i<4; i++)
			in.getline (Zeile, sizeof(Zeile));
		    Tuerbog[2]= atof(Zeile);             // rad
		    for (i=0; i<2; i++)
			in.getline (Zeile, sizeof(Zeile));
		    Tuerbog[3]= atof(Zeile);             // start angle
		    for (i=0; i<2; i++)
			in.getline (Zeile, sizeof(Zeile));
		    Tuerbog[4]= atof(Zeile);             // end angle
		    Schalter= Namenserfassung (Blockname);

	switch (Schalter)
	{
	case 4:                                                 // = M_Tuer1
		TuerAn.x = 0;
		TuerAn.y = 0;
		TuerEn.x = Tuer_x_schliessen (Tuerbog[4], Tuerbog[2]);
		TuerEn.y = Tuer_y_schliessen (Tuerbog[4], Tuerbog[2]);
		break;
	case 5:                                                  // =M_Tuer =G_Tuer
		Tuer2An.x = 0;
		Tuer2An.y = 0;
		Tuer2En.x = Tuer_x_schliessen (Tuerbog[4], Tuerbog[2]);
		Tuer2En.y = Tuer_y_schliessen (Tuerbog[4], Tuerbog[2]);
		break;
	case 9:
		if ((Tuerbog[0]==0) && (Tuerbog[1]==0))
		{	GD_TuerAn.x = 0;
			GD_TuerAn.y = 0; }
		else
		{	GD_TuerEn.x = Tuerbog[0];
			GD_TuerEn.y = Tuerbog[1]; }
		break;
	case 10:
		if ((Tuerbog[0]==0) && (Tuerbog[1]==0))
		{	D_TuerAn.x = 0;
			D_TuerAn.y = 0; }
		else
		{	D_TuerEn.x = Tuerbog[0];
			D_TuerEn.y = Tuerbog[1]; }
		break;
	case 11:
		if ((Tuerbog[0]==0) && (Tuerbog[1]==0))
		{	D_Tuer2An.x = 0;
			D_Tuer2An.y = 0; }
		else
		{	D_Tuer2En.x = Tuerbog[0];
			D_Tuer2En.y = Tuerbog[1]; }
		break;
	case 12:
		if ((Tuerbog[0]==0) && (Tuerbog[1]==0))
		{	FtuerAn.x = 0;
			FtuerAn.y = 0; }
		else
		{	FtuerEn.x = Tuerbog[0];
			FtuerEn.y = Tuerbog[1]; }
		break;
	}
		    }
		if (!strcmp (Zeile, "LINE"))			// Blocklinien einlesen
		    {
		    for (i=0; i<4; i++)
			in.getline (Zeile, sizeof(Zeile));
		    strcpy (VecAnx, Zeile);
		    for (i=0; i<2; i++)
			in.getline (Zeile, sizeof(Zeile));
		    strcpy (VecAny, Zeile);
		    for (i=0; i<4; i++)
			in.getline (Zeile, sizeof(Zeile));
		    strcpy (VecEnx, Zeile);
		    for (i=0; i<2; i++)
		    	in.getline (Zeile, sizeof(Zeile));
		    strcpy (VecEny, Zeile);
		    Schalter= Namenserfassung (Blockname);
	switch (Schalter)
	{
	case 1:
		FenAn[v1].x = atof(VecAnx);
		FenAn[v1].y = atof(VecAny);
		FenEn[v1].x = atof(VecEnx);
		FenEn[v1].y = atof(VecEny);
		v1++;
		break;
	case 2:
		Fen1An[v2].x = atof(VecAnx);
		Fen1An[v2].y = atof(VecAny);
		Fen1En[v2].x = atof(VecEnx);
		Fen1En[v2].y = atof(VecEny);
		v2++;
		break;
	case 3:
		Fen2An[v3].x = atof(VecAnx);
		Fen2An[v3].y = atof(VecAny);
		Fen2En[v3].x = atof(VecEnx);
		Fen2En[v3].y = atof(VecEny);
		v3++;
		break;
	case 13:
		SaeuleAn[v4].x = atof(VecAnx);
		SaeuleAn[v4].y = atof(VecAny);
		SaeuleEn[v4].x = atof(VecEnx);
		SaeuleEn[v4].y = atof(VecEny);
		v4++;
		break;
	case 14:
		ZW1An[v5].x = atof(VecAnx);
		ZW1An[v5].y = atof(VecAny);
		ZW1En[v5].x = atof(VecEnx);
		ZW1En[v5].y = atof(VecEny);
		v5++;
		break;
	case 15:
		ZW2An[v6].x = atof(VecAnx);
		ZW2An[v6].y = atof(VecAny);
		ZW2En[v6].x = atof(VecEnx);
		ZW2En[v6].y = atof(VecEny);
		v6++;
		break;
	case 16:
		ZW3An[v7].x = atof(VecAnx);
		ZW3An[v7].y = atof(VecAny);
		ZW3En[v7].x = atof(VecEnx);
		ZW3En[v7].y = atof(VecEny);
		v7++;
		break;
	case 17:
		ZW4An[v8].x = atof(VecAnx);
		ZW4An[v8].y = atof(VecAny);
		ZW4En[v8].x = atof(VecEnx);
		ZW4En[v8].y = atof(VecEny);
		v8++;
		break;
	case 18:
		ZW5An[v9].x = atof(VecAnx);
		ZW5An[v9].y = atof(VecAny);
		ZW5En[v9].x = atof(VecEnx);
		ZW5En[v9].y = atof(VecEny);
		v9++;
		break;
	case 19:
		ZW6An[v10].x = atof(VecAnx);
		ZW6An[v10].y = atof(VecAny);
		ZW6En[v10].x = atof(VecEnx);
		ZW6En[v10].y = atof(VecEny);
		v10++;
		break;
	case 20:
		ZW7An[v11].x = atof(VecAnx);
		ZW7An[v11].y = atof(VecAny);
		ZW7En[v11].x = atof(VecEnx);
		ZW7En[v11].y = atof(VecEny);
		v11++;
		break;
	case 21:
		ZW8An[v12].x = atof(VecAnx);
		ZW8An[v12].y = atof(VecAny);
		ZW8En[v12].x = atof(VecEnx);
		ZW8En[v12].y = atof(VecEny);
		v12++;
		break;
	case 22:
		ZW9An[v13].x = atof(VecAnx);
		ZW9An[v13].y = atof(VecAny);
		ZW9En[v13].x = atof(VecEnx);
		ZW9En[v13].y = atof(VecEny);
		v13++;
		break;
	case 23:
		ZW10An[v14].x = atof(VecAnx);
		ZW10An[v14].y = atof(VecAny);
		ZW10En[v14].x = atof(VecEnx);
		ZW10En[v14].y = atof(VecEny);
		v14++;
		break;
	case 24:
		ZW11An[v15].x = atof(VecAnx);
		ZW11An[v15].y = atof(VecAny);
		ZW11En[v15].x = atof(VecEnx);
		ZW11En[v15].y = atof(VecEny);
		v15++;
		break;
	case 25:
		ZW12An[v16].x = atof(VecAnx);
		ZW12An[v16].y = atof(VecAny);
		ZW12En[v16].x = atof(VecEnx);
		ZW12En[v16].y = atof(VecEny);
		v16++;
		break;
	case 26:
		ZW13An[v17].x = atof(VecAnx);
		ZW13An[v17].y = atof(VecAny);
		ZW13En[v17].x = atof(VecEnx);
		ZW13En[v17].y = atof(VecEny);
		ZW16An[v17].x = -1*atof(VecAnx);     // ZW16=gespiegelte ZW13
		ZW16An[v17].y = 140+atof(VecAny);
		ZW16En[v17].x = -1*atof(VecEnx);
		ZW16En[v17].y = 140+atof(VecEny);
		v17++;
		break;
	case 27:
		ZW14An[v18].x = atof(VecAnx);
		ZW14An[v18].y = atof(VecAny);
		ZW14En[v18].x = atof(VecEnx);
		ZW14En[v18].y = atof(VecEny);
		ZW15An[v18].x = -1*atof(VecAnx);      // ZW15=gespiegelte ZW14
		ZW15An[v18].y = 140+atof(VecAny);
		ZW15En[v18].x = -1*atof(VecEnx);
		ZW15En[v18].y = 140+atof(VecEny);
		v18++;
		break;
	case 30:
		ZW17An[v19].x = atof(VecAnx);
		ZW17An[v19].y = atof(VecAny);
		ZW17En[v19].x = atof(VecEnx);
		ZW17En[v19].y = atof(VecEny);
		v19++;
		break;
	case 31:
		ZW18An[v20].x = atof(VecAnx);
		ZW18An[v20].y = atof(VecAny);
		ZW18En[v20].x = atof(VecEnx);
		ZW18En[v20].y = atof(VecEny);
		v20++;
		break;
	case 32:
		GW1An[v21].x = atof(VecAnx);
		GW1An[v21].y = atof(VecAny);
		GW1En[v21].x = atof(VecEnx);
		GW1En[v21].y = atof(VecEny);
		v21++;
		break;
	case 33:
		GW2An[v22].x = atof(VecAnx);
		GW2An[v22].y = atof(VecAny);
		GW2En[v22].x = atof(VecEnx);
		GW2En[v22].y = atof(VecEny);
		v22++;
		break;
	case 34:
		GW3An[v23].x = atof(VecAnx);
		GW3An[v23].y = atof(VecAny);
		GW3En[v23].x = atof(VecEnx);
		GW3En[v23].y = atof(VecEny);
		v23++;
		break;
	case 35:
		GW4An[v24].x = atof(VecAnx);
		GW4An[v24].y = atof(VecAny);
		GW4En[v24].x = atof(VecEnx);
		GW4En[v24].y = atof(VecEny);
		v24++;
		break;
	case 36:
		GW5An[v25].x = atof(VecAnx);
		GW5An[v25].y = atof(VecAny);
		GW5En[v25].x = atof(VecEnx);
		GW5En[v25].y = atof(VecEny);
		v25++;
		break;
	case 37:
		MW1An[v26].x = atof(VecAnx);
		MW1An[v26].y = atof(VecAny);
		MW1En[v26].x = atof(VecEnx);
		MW1En[v26].y = atof(VecEny);
		v26++;
		break;
	case 38:
		MW2An[v27].x = atof(VecAnx);
		MW2An[v27].y = atof(VecAny);
		MW2En[v27].x = atof(VecEnx);
		MW2En[v27].y = atof(VecEny);
		v27++;
		break;
	case 39:
		MW3An[v28].x = atof(VecAnx);
		MW3An[v28].y = atof(VecAny);
		MW3En[v28].x = atof(VecEnx);
		MW3En[v28].y = atof(VecEny);
		v28++;
		break;

	}

			}                       // Ende der LINE-Schleife
		}                               // Ende der while-Schleife
	    }                  			// Ende der BLOCK-Schleife


	   if (!strcmp (Zeile, "ENTITIES"))			// BlockeinfuegePunkt einlesen
	       {
	        w1=w2=w3=w4=w5=w6=w7=w8=w9=w10=0;
	       while (strcmp (Zeile, "ENDSEC"))
	       {
		    in.getline (Zeile, sizeof(Zeile));
		    if (!strcmp (Zeile, "INSERT"))
			{
			for (i=0; i<4; i++)
			    in.getline (Zeile, sizeof(Zeile));
			strcpy (Blockname, Zeile);
			for (i=0; i<2; i++)
			    in.getline (Zeile, sizeof(Zeile));
			strcpy (EP_x, Zeile);
			for (i=0; i<2; i++)
			    in.getline (Zeile, sizeof(Zeile));
			strcpy (EP_y, Zeile);
		Schalter= Namenserfassung (Blockname);
	switch (Schalter)
	{
	case 1:
	      FenEP[w1].x = atof(EP_x);             // x_in
	      FenEP[w1].y = atof(EP_y);             // y_in
	      w1++;
	      break;
	case 2:
	      Fen1EP[w2].x = atof(EP_x);             // x_in
	      Fen1EP[w2].y = atof(EP_y);             // y_in
	      w2++;
	      break;
	case 3:
	      Fen2EP.x = atof(EP_x);             // x_in
	      Fen2EP.y = atof(EP_y);             // y_in
	      break;
	case 4:
	      TuerEP[w3].x = atof(EP_x);             // x_in
	      TuerEP[w3].y = atof(EP_y);             // y_in
	      w3++;
	      break;
	case 5:
	      Tuer2EP[w4].x = atof(EP_x);             // x_in
	      Tuer2EP[w4].y = atof(EP_y);             // y_in
	      w4++;
	      break;
	case 6:
	      M_TuerEP[w5].x = atof(EP_x);             // x_in
	      M_TuerEP[w5].y = atof(EP_y);             // y_in
	      w5++;
	      break;
	case 7:
	      M_Tuer1EP.x = atof(EP_x);             // x_in
	      M_Tuer1EP.y = atof(EP_y);             // y_in
	      break;
	case 8:
	      G_TuerEP.x = atof(EP_x);             // x_in
	      G_TuerEP.y = atof(EP_y);             // y_in
	      break;
	case 9:
	      GD_TuerEP.x = atof(EP_x);             // x_in
	      GD_TuerEP.y = atof(EP_y);             // y_in
	      break;
	case 10:
	      D_TuerEP.x = atof(EP_x);             // x_in
	      D_TuerEP.y = atof(EP_y);             // y_in
	      break;
	case 11:
	      D_Tuer2EP.x = atof(EP_x);             // x_in
	      D_Tuer2EP.y = atof(EP_y);             // y_in
	      break;
	case 12:
	      FtuerEP.x = atof(EP_x);             // x_in
	      FtuerEP.y = atof(EP_y);             // y_in
	      break;
	case 13:
	      SaeuleEP[w6].x = atof(EP_x);             // x_in
	      SaeuleEP[w6].y = atof(EP_y);             // y_in
	      w6++;
	      break;
	case 14:
	      ZW1EP.x = atof(EP_x);             // x_in
	      ZW1EP.y = atof(EP_y);             // y_in
	      break;
	case 15:
	      ZW2EP[w7].x = atof(EP_x);             // x_in
	      ZW2EP[w7].y = atof(EP_y);             // y_in
	      w7++;
	      break;
	case 16:
	      ZW3EP.x = atof(EP_x);             // x_in
	      ZW3EP.y = atof(EP_y);             // y_in
	      break;
	case 17:
	      ZW4EP.x = atof(EP_x);             // x_in
	      ZW4EP.y = atof(EP_y);             // y_in
	      break;
	case 18:
	      ZW5EP.x = atof(EP_x);             // x_in
	      ZW5EP.y = atof(EP_y);             // y_in
	      break;
	case 19:
	      ZW6EP.x = atof(EP_x);             // x_in
	      ZW6EP.y = atof(EP_y);             // y_in
	      break;
	case 20:
	      ZW7EP.x = atof(EP_x);             // x_in
	      ZW7EP.y = atof(EP_y);             // y_in
	      break;
	case 21:
	      ZW8EP.x = atof(EP_x);             // x_in
	      ZW8EP.y = atof(EP_y);             // y_in
	      break;
	case 22:
	      ZW9EP.x = atof(EP_x);             // x_in
	      ZW9EP.y = atof(EP_y);             // y_in
	      break;
	case 23:
	      ZW10EP.x = atof(EP_x);             // x_in
	      ZW10EP.y = atof(EP_y);             // y_in
	      break;
	case 24:
	      ZW11EP.x = atof(EP_x);             // x_in
	      ZW11EP.y = atof(EP_y);             // y_in
	      break;
	case 25:
	      ZW12EP.x = atof(EP_x);             // x_in
	      ZW12EP.y = atof(EP_y);             // y_in
	      break;
	case 26:
	      ZW13EP.x = atof(EP_x);             // x_in
	      ZW13EP.y = atof(EP_y);             // y_in
	      break;
	case 27:
	      ZW14EP.x = atof(EP_x);             // x_in
	      ZW14EP.y = atof(EP_y);             // y_in
	      break;
	case 28:
	      ZW15EP.x = atof(EP_x);             // x_in
	      ZW15EP.y = atof(EP_y);             // y_in
	      break;
	case 29:
	      ZW16EP.x = atof(EP_x);             // x_in
	      ZW16EP.y = atof(EP_y);             // y_in
	      break;
	case 30:
	      ZW17EP.x = atof(EP_x);             // x_in
	      ZW17EP.y = atof(EP_y);             // y_in
	      break;
	case 31:
	      ZW18EP.x = atof(EP_x);             // x_in
	      ZW18EP.y = atof(EP_y);             // y_in
	      break;
	case 32:
	      GW1EP.x = atof(EP_x);             // x_in
	      GW1EP.y = atof(EP_y);             // y_in
	      break;
	case 33:
	      GW2EP.x = atof(EP_x);             // x_in
	      GW2EP.y = atof(EP_y);             // y_in
	      break;
	case 34:
	      GW3EP[w8].x = atof(EP_x);             // x_in
	      GW3EP[w8].y = atof(EP_y);             // y_in
	      w8++;
	      break;
	case 35:
	      GW4EP[w9].x = atof(EP_x);             // x_in
	      GW4EP[w9].y = atof(EP_y);             // y_in
	      w9++;
	      break;
	case 36:
	      GW5EP[w10].x = atof(EP_x);             // x_in
	      GW5EP[w10].y = atof(EP_y);             // y_in
	      w10++;
	      break;
	case 37:
	      MW1EP.x = atof(EP_x);             // x_in
	      MW1EP.y = atof(EP_y);             // y_in
	      break;
	case 38:
	      MW2EP.x = atof(EP_x);             // x_in
	      MW2EP.y = atof(EP_y);             // y_in
	      break;
	case 39:
	      MW3EP.x = atof(EP_x);             // x_in
	      MW3EP.y = atof(EP_y);             // y_in
	      break;
	}

			}                	// Verlassen d. if "INSERT"-Schl.
	    }                            	//     "        while-Schleife

	  }                              	// Ende der ENTITIES-Schleife


	}                                	// Ende der while(in)-Schleife
	 
   }                                     	// Ende else Zweig
   Gelesen=1;                      		// Flag, dass File gelesen wurde     
   in.close();
}


double DEpropag_uldl::Tuer_x_schliessen (double Tuerparam1, double Tuerparam2)
{
# line 790 "DEpropag_uldl.pl"
if (Tuerparam1== 0)       	return 0;
	if (Tuerparam1== 90)            return Tuerparam2;
	if (Tuerparam1== 180)     	return 0;
	if (Tuerparam1== 270)           return (0-Tuerparam2);
	return 0;               // Fehler !!
}


double DEpropag_uldl::Tuer_y_schliessen (double Tuerparam1, double Tuerparam2)
{
# line 805 "DEpropag_uldl.pl"
if (Tuerparam1== 0)            	return (0-Tuerparam2);
	if (Tuerparam1== 90)           	return 0;
	if (Tuerparam1== 180)           return Tuerparam2;
	if (Tuerparam1== 270)           return 0;
	return 0;               // Fehler !!
}


int DEpropag_uldl::Namenserfassung (char B_Name[14])
{
# line 820 "DEpropag_uldl.pl"
if (!strcmp (B_Name,"FENSTER"))    	return 1;
	if (!strcmp (B_Name,"FENSTER1"))     	return 2;
	if (!strcmp (B_Name,"FENSTER2"))     	return 3;
	if (!strcmp (B_Name,"TUER"))   		return 4;
	if (!strcmp (B_Name,"TUER2"))      	return 5;
	if (!strcmp (B_Name,"M_TUER"))       	return 6;
	if (!strcmp (B_Name,"M_TUER1"))    	return 7;
	if (!strcmp (B_Name,"G_TUER"))       	return 8;
	if (!strcmp (B_Name,"G_DTUER"))    	return 9;
	if (!strcmp (B_Name,"DTUER"))     	return 10;
	if (!strcmp (B_Name,"DTUER2"))   	return 11;
	if (!strcmp (B_Name,"FTUER"))   	return 12;
	if (!strcmp (B_Name,"SAEULE"))   	return 13;
	if (!strcmp (B_Name,"ZW1"))      	return 14;
	if (!strcmp (B_Name,"ZW2"))       	return 15;
	if (!strcmp (B_Name,"ZW3"))    		return 16;
	if (!strcmp (B_Name,"ZW4"))       	return 17;
	if (!strcmp (B_Name,"ZW5"))      	return 18;
	if (!strcmp (B_Name,"ZW6"))       	return 19;
	if (!strcmp (B_Name,"ZW7"))    		return 20;
	if (!strcmp (B_Name,"ZW8"))       	return 21;
	if (!strcmp (B_Name,"ZW9"))      	return 22;
	if (!strcmp (B_Name,"ZW10"))       	return 23;
	if (!strcmp (B_Name,"ZW11"))    	return 24;
	if (!strcmp (B_Name,"ZW12"))       	return 25;
	if (!strcmp (B_Name,"ZW13"))      	return 26;
	if (!strcmp (B_Name,"ZW14"))       	return 27;
	if (!strcmp (B_Name,"ZW15"))    	return 28;
	if (!strcmp (B_Name,"ZW16"))       	return 29;
	if (!strcmp (B_Name,"ZW17"))    	return 30;
	if (!strcmp (B_Name,"ZW18"))       	return 31;
	if (!strcmp (B_Name,"GW1"))      	return 32;
	if (!strcmp (B_Name,"GW2"))       	return 33;
	if (!strcmp (B_Name,"GW3"))    		return 34;
	if (!strcmp (B_Name,"GW4"))       	return 35;
	if (!strcmp (B_Name,"GW5"))      	return 36;
	if (!strcmp (B_Name,"MW1"))      	return 37;
	if (!strcmp (B_Name,"MW2"))       	return 38;
	if (!strcmp (B_Name,"MW3"))    		return 39;
	return 0;
}


void DEpropag_uldl::Sortierung ()
{
# line 869 "DEpropag_uldl.pl"
int i;

	for (i=0; i<91; i++)      		// Tabellierung aller EinfuegePunkte 
	{
		if ((i>=0)  && (i<20)) 	EP[i]= FenEP[i];
		if ((i>=20) && (i<26)) 	EP[i]= Fen1EP[i-20];
		if (i==26)	      	EP[i]= Fen2EP;
		if ((i>=27) && (i<34))	EP[i]= TuerEP[i-27];
		if ((i>=34) && (i<36))	EP[i]= Tuer2EP[i-34];
		if ((i>=36) && (i<40))  EP[i]= M_TuerEP[i-36];
		if (i==40) 		EP[i]= M_Tuer1EP;
		if (i==41)   	      	EP[i]= G_TuerEP;
		if (i==42)		EP[i]= GD_TuerEP;
		if (i==43)   	      	EP[i]= D_TuerEP;
		if (i==44)		EP[i]= D_Tuer2EP;
		if (i==45)              EP[i]= FtuerEP;
		if ((i>=46) && (i<50))  EP[i]= SaeuleEP[i-46];
		if (i==50)   	      	EP[i]= ZW1EP;
		if ((i>=51) && (i<64))	EP[i]= ZW2EP[i-51];
		if (i==64)   	      	EP[i]= ZW3EP;
		if (i==65)		EP[i]= ZW4EP;
		if (i==66)   	      	EP[i]= ZW5EP;
		if (i==67)		EP[i]= ZW6EP;
		if (i==68)   	      	EP[i]= ZW7EP;
		if (i==69)		EP[i]= ZW8EP;
		if (i==70)   	      	EP[i]= ZW9EP;
		if (i==71)		EP[i]= ZW10EP;
		if (i==72)   	      	EP[i]= ZW11EP;
		if (i==73)		EP[i]= ZW12EP;
		if (i==74)   	      	EP[i]= ZW13EP;
		if (i==75)		EP[i]= ZW14EP;
		if (i==76)   	      	EP[i]= ZW15EP;
		if (i==77)		EP[i]= ZW16EP;
		if (i==78)   	      	EP[i]= ZW17EP;
		if (i==79)		EP[i]= ZW18EP;
		if (i==80)   	      	EP[i]= GW1EP;
		if (i==81)		EP[i]= GW2EP;
		if ((i>=82) && (i<84))  EP[i]= GW3EP[i-82];
		if ((i>=84) && (i<86))	EP[i]= GW4EP[i-84];
		if ((i>=86) && (i<88))	EP[i]= GW5EP[i-86];
		if (i==88)		EP[i]= MW1EP;
		if (i==89)   	      	EP[i]= MW2EP;
		if (i==90)		EP[i]= MW3EP;
	}

	for (i=0; i<283; i++) 		// Tabellierung aller Block-Anf.&- EndPkte
	{
		if ((i>=0) && (i<4))  	      { HinAn[i]= FenAn[i];
						HinEn[i]= FenEn[i];}
		if ((i>=4) && (i<8))  	      { HinAn[i]= Fen1An[i-4];
						HinEn[i]= Fen1En[i-4];}
		if ((i>=8) && (i<12))  	      { HinAn[i]= Fen2An[i-8];
						HinEn[i]= Fen2En[i-8];}
		if (i==12)             	      {	HinAn[i]= TuerAn;
						HinEn[i]= TuerEn;}
		if (i==13)             	      {	HinAn[i]= Tuer2An;
						HinEn[i]= Tuer2En;}
		if (i==14)            	      {	HinAn[i]= Tuer2An;     // M_Tuer
						HinEn[i]= Tuer2En;}
		if (i==15)            	      {	HinAn[i]= TuerAn;      // M_Tuer1
						HinEn[i]= TuerEn;}
		if (i==16)            	      {	HinAn[i]= Tuer2An;     // G_Tuer
						HinEn[i]= Tuer2En;}
		if (i==17)            	      {	HinAn[i]= GD_TuerAn;
						HinEn[i]= GD_TuerEn;}
		if (i==18)            	      {	HinAn[i]= D_TuerAn;
						HinEn[i]= D_TuerEn;}
		if (i==19)            	      {	HinAn[i]= D_Tuer2An;
						HinEn[i]= D_Tuer2En;}
		if (i==20)                    {	HinAn[i]= FtuerAn;
						HinEn[i]= FtuerEn;}
		if ((i>=21) && (i<25))        {	HinAn[i]= SaeuleAn[i-21];
						HinEn[i]= SaeuleEn[i-21];}
		if ((i>=25) && (i<31))        {	HinAn[i]= ZW1An[i-25];
						HinEn[i]= ZW1En[i-25];}
		if ((i>=31) && (i<35)) 	      {	HinAn[i]= ZW2An[i-31];
						HinEn[i]= ZW2En[i-31];}
		if ((i>=35) && (i<43))	      {	HinAn[i]= ZW3An[i-35];
						HinEn[i]= ZW3En[i-35];}
		if ((i>=43) && (i<55))	      {	HinAn[i]= ZW4An[i-43];
						HinEn[i]= ZW4En[i-43];}
		if ((i>=55) && (i<67))	      {	HinAn[i]= ZW5An[i-55];
						HinEn[i]= ZW5En[i-55];}
		if ((i>=67) && (i<79))        {	HinAn[i]= ZW6An[i-67];
						HinEn[i]= ZW6En[i-67];}
		if ((i>=79) && (i<91)) 	      {	HinAn[i]= ZW7An[i-79];
						HinEn[i]= ZW7En[i-79];}
		if ((i>=91) && (i<107))	      {	HinAn[i]= ZW8An[i-91];
						HinEn[i]= ZW8En[i-91];}
		if ((i>=107)  && (i<113))     {	HinAn[i]= ZW9An[i-107];
						HinEn[i]= ZW9En[i-107];}
		if ((i>=113)  && (i<129))     {	HinAn[i]= ZW10An[i-113];
						HinEn[i]= ZW10En[i-113];}
		if ((i>=129)  && (i<145))     {	HinAn[i]= ZW11An[i-129];
						HinEn[i]= ZW11En[i-129];}
		if ((i>=145)  && (i<175))     {	HinAn[i]= ZW12An[i-145];
						HinEn[i]= ZW12En[i-145];}
		if ((i>=175)  && (i<187))     {	HinAn[i]= ZW13An[i-175];
						HinEn[i]= ZW13En[i-175];}
		if ((i>=187)  && (i<199))     {	HinAn[i]= ZW14An[i-187];
						HinEn[i]= ZW14En[i-187];}
		if ((i>=199)  && (i<211))     {	HinAn[i]= ZW15An[i-199];
						HinEn[i]= ZW15En[i-199];}
		if ((i>=211)  && (i<223))     {	HinAn[i]= ZW16An[i-211];
						HinEn[i]= ZW16En[i-211];}
		if ((i>=223)  && (i<235))     {	HinAn[i]= ZW17An[i-223];
						HinEn[i]= ZW17En[i-223];}
		if ((i>=235)  && (i<251))     {	HinAn[i]= ZW18An[i-235];
						HinEn[i]= ZW18En[i-235];}
		if ((i>=251)  && (i<255))     {	HinAn[i]= GW1An[i-251];
						HinEn[i]= GW1En[i-251];}
		if ((i>=255)  && (i<259))     {	HinAn[i]= GW2An[i-255];
						HinEn[i]= GW2En[i-255];}
		if ((i>=259)  && (i<263))     {	HinAn[i]= GW3An[i-259];
						HinEn[i]= GW3En[i-259];}
		if ((i>=263)  && (i<267))     {	HinAn[i]= GW4An[i-263];
						HinEn[i]= GW4En[i-263];}
		if ((i>=267)  && (i<271))     {	HinAn[i]= GW5An[i-267];
						HinEn[i]= GW5En[i-267];}
		if ((i>=271)  && (i<275))     {	HinAn[i]= MW1An[i-271];
						HinEn[i]= MW1En[i-271];}
		if ((i>=275)  && (i<279))     {	HinAn[i]= MW2An[i-275];
						HinEn[i]= MW2En[i-275];}
		if ((i>=279)  && (i<283))     {	HinAn[i]= MW3An[i-279];
						HinEn[i]= MW3En[i-279];}
	}
}


void DEpropag_uldl::Objekte_abtast ()
{
# line 1004 "DEpropag_uldl.pl"
int i, j;

	for (i=0; i<39; i++)
		Objekt_SP[i]=0;              	// Feld f. Anz. v. Objekt-SP
	SP_Anz= 0;

	for (i=0; i<91; i++)
	{
		if ((i>=0) && (i<20))     		// 20 Fenster EP
		{
			Name = Fenster;
			for (j=0; j<4; j++)          	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if ((i>=20) && (i<26))     		// 6 Fenster1 EP
		{
			Name = Fenster1;
			for (j=4; j<8; j++)          	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==26)     				// 1 Fenster2 EP
		{
			Name = Fenster2;
			for (j=8; j<12; j++) 		// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if ((i>=27) && (i<34))		        // 7 Tuer EP
		{
			Name = Tuer;
			j=12;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if ((i>=34) && (i<36))                  // 2 Tuer2 EP
		{
			Name = Tuer2;
			j=13;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if ((i>=36) && (i<40))     		// 4 M_Tuer EP
		{
			Name = Metall_Tuer;
			j=14;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if (i==40) 		     		// 1 M_Tuer1 EP
		{
			Name = Metall_Tuer1;
			j=15;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if (i==41)     				// 1 G_Tuer EP
		{
			Name = Glas_Tuer;
			j=16;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if (i==42)     				// 1 GD_Tuer EP
		{
			Name = Glas_Doppel_Tuer;
			j=17;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if (i==43)     				// 1 D_Tuer EP
		{
			Name = Doppel_Tuer;
			j=18;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if (i==44)     				// 1D_Tuer2 EP
		{
			Name = Doppel_Tuer2;
			j=19;			     	// 1 Blocklinie
			Koord_norm (i, j);
			SP_berechnung();
		}
		if (i==45)                              // 1 Ftuer EP
		{
			Name = Fahrstuhltuer;
			j=20;			   	// 1 Blocklinien
			Koord_norm (i, j);
			SP_berechnung();
		}
		if ((i>=46) && (i<50))			// 4 Saeule EP
		{
			Name = Saeule;
			for (j=21; j<25; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==50)                              // 1 ZW1 EP
		{
			Name = Ziegelwand1;
			for (j=25; j<31; j++)   	// 6 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if ((i>=51) && (i<64))     		// 13 ZW2 EP
		{
			Name = Ziegelwand2;
			for (j=31; j<35; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==64)     				// 1 ZW3 EP
		{
			Name = Ziegelwand3;
			for (j=35; j<43; j++)   	// 8 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==65)     				// 1 ZW4 EP
		{
			Name = Ziegelwand4;
			for (j=43; j<55; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==66)     				// 1 ZW5 EP
		{
			Name = Ziegelwand5;
			for (j=55; j<67; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==67)     				// 1 ZW6 EP
		{
			Name = Ziegelwand6;
			for (j=67; j<79; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==68)     				// 1 ZW7 EP
		{
			Name = Ziegelwand7;
			for (j=79; j<91; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==69)     				// 1 ZW8 EP
		{
			Name = Ziegelwand8;
			for (j=91; j<107; j++)   	// 16 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==70)     				// 1 ZW9 EP
		{
			Name = Ziegelwand9;
			for (j=107; j<113; j++)   	// 6 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==71)     				// 1 ZW10 EP
		{
			Name = Ziegelwand10;
			for (j=113; j<129; j++)   	// 16 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==72)     				// 1 ZW11 EP
		{
			Name = Ziegelwand11;
			for (j=129; j<145; j++)   	// 16 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==73)     				// 1 ZW12 EP
		{
			Name = Ziegelwand12;
			for (j=145; j<175; j++)   	// 30 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==74)     				// 1 ZW13 EP
		{
			Name = Ziegelwand13;
			for (j=175; j<187; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==75)     				// 1 ZW14 EP
		{
			Name = Ziegelwand14;
			for (j=187; j<199; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==76)     				// 1 ZW15 EP
		{
			Name = Ziegelwand15;
			for (j=199; j<211; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==77)     				// 1 ZW16 EP
		{
			Name = Ziegelwand16;
			for (j=211; j<223; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==78)     				// 1 ZW17 EP
		{
			Name = Ziegelwand17;
			for (j=223; j<235; j++)   	// 12 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==79)     				// 1 ZW18 EP
		{
			Name = Ziegelwand18;
			for (j=235; j<251; j++)   	// 16 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==80)     				// 1 GW1 EP
		{
			Name = Gipswand1;
			for (j=251; j<255; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==81)     				// 1 GW2 EP
		{
			Name = Gipswand2;
			for (j=255; j<259; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if ((i>=82) && (i<84))                  // 2 GW3 EP
		{
			Name = Gipswand3;
			for (j=259; j<263; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if ((i>=84) && (i<86))                  // 2 GW4 EP
		{
			Name = Gipswand4;
			for (j=263; j<267; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if ((i>=86) && (i<88))                  // 2 GW5 EP
		{
			Name = Gipswand5;
			for (j=267; j<271; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==88)				// 1 MW1 EP
		{
			Name = Metallabschirm1;
			for (j=271; j<275; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==89)				// 1 MW2 EP
		{
			Name = Metallabschirm2;
			for (j=275; j<279; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
		if (i==90)				// 1 MW3 EP
		{
			Name = Metallabschirm3;
			for (j=279; j<283; j++)   	// 4 Blocklinien
			{
				Koord_norm (i, j);
				SP_berechnung();
			}
		}
	    }
}


void DEpropag_uldl::Koord_norm (int L1, int L2)
{
# line 1356 "DEpropag_uldl.pl"
A.x= EP[L1].x+ HinAn[L2].x;               // Blocklinien auf ACAD 0-Punkt normieren
	A.y= EP[L1].y+ HinAn[L2].y;               // (  statt auf EP, wie aus DXF/File eingelesen)
	E.x= EP[L1].x+ HinEn[L2].x;
	E.y= EP[L1].y+ HinEn[L2].y;
}


void DEpropag_uldl::Runden (double nmbr)
{
# line 1370 "DEpropag_uldl.pl"
double down = floor(nmbr);
	double up = ceil(nmbr);

	if ((up-nmbr) < (nmbr-down))
		erg= up;
	else
		erg= down;
}


void DEpropag_uldl::SP_berechnung ()
{
# line 1386 "DEpropag_uldl.pl"
int 	Bed_1, Bed_2, Bed_3, Bed_4;
	double 	Fak1, Fak2, Rx, Ry, Tx, Ty, Kat_x, Kat_y, F1, F2, Z1, N1, Z2, N2;
	
	Rx= x_pos;
	Ry= y_pos;

	if (tunnel_flag==0)
	{
		Tx= real (position);		
		Ty= imag (position);
	}
	else
	{
		Tx = real (zellen[LI_number].Trans_pos);        
		Ty = imag (zellen[LI_number].Trans_pos);
	}

	// Test ob beide Vektoren (TR und AE) waagerecht (parallel => kein SP);
	if ((A.y==E.y) && (Ty==Ry))  goto Label_1;

	// Test ob beide Vektoren senkrecht (parallel => kein SP);
	if ((A.x==E.x) && (Tx==Rx))  goto Label_1;

	Z1= A.y*E.x*(Tx-Rx)-A.x*(Tx*(E.y-Ry)+Rx*(Ty-E.y))+E.x*(Rx*Ty-Tx*Ry);
	N1= A.y*(Tx-Rx)+A.x*(Ry-Ty)+E.x*(Ty-Ry)+E.y*(Rx-Tx);
	S_P.x= Z1/N1;

	F1= Ry-Ty;
	F2= S_P.x-Tx;
	Z2= F1*F2;
	N2= Rx-Tx;
	if (F1==0) S_P.y= Ty;   				// TR verlaeuft waagerecht
	if (N2==0)                                              // TR verlaeuft senkrecht
	{
		Z2= (E.y-A.y)*(S_P.x-A.x);
		N2= (E.x-A.x);
		if (Z2==0) 	S_P.y= A.y;          		// AE verlaeuft waagerecht
		else 		S_P.y= Z2/N2+A.y;
	}
	else S_P.y= Z2/N2+Ty;

	Runden(S_P.x); S_Pr.x= erg;
	Runden(S_P.y); S_Pr.y= erg;

	// Test ob S_P existiert
	Bed_1= ((S_Pr.x>=Tx) && (S_Pr.x<=Rx)) || ((S_Pr.x>=Rx) && (S_Pr.x<=Tx));
	Bed_2= ((S_Pr.y>=Ty) && (S_Pr.y<=Ry)) || ((S_Pr.y>=Ry) && (S_Pr.y<=Ty));
	Bed_3= ((S_Pr.x>=A.x) && (S_Pr.x<=E.x)) || ((S_Pr.x>=E.x) && (S_Pr.x<=A.x));
	Bed_4= ((S_Pr.y>=A.y) && (S_Pr.y<=E.y)) || ((S_Pr.y>=E.y) && (S_Pr.y<=A.y));

	if (Bed_1 && Bed_2 && Bed_3 && Bed_4)
	  {
		SP[SP_Anz]= S_Pr;
		// Hindernisabst. vom Sender
		Fak1 =  (S_P.x-Tx)/Mass;
		Fak2 =  (S_P.y-Ty)/Mass;
		Kat_x = Fak1*Fak1;
		Kat_y = Fak2*Fak2;
		Abst[SP_Anz] = sqrt (Kat_x+Kat_y);

		if (Name == Fenster)
		{ 		Art[SP_Anz]= 0;
				Objekt_SP[0]++;
		}
		if (Name == Fenster1)
		{		Art[SP_Anz]= 1;
				Objekt_SP[1]++;
		}
		if (Name == Fenster2)
		{		Art[SP_Anz]= 2;
				Objekt_SP[2]++;
		}
		if (Name == Tuer)
		{		Art[SP_Anz]= 3;
				Objekt_SP[3]++;
		}
		if (Name == Tuer2)
		{		Art[SP_Anz]= 4;
				Objekt_SP[4]++;
		}
		if (Name == Metall_Tuer)
		{ 		Art[SP_Anz]= 5;
				Objekt_SP[5]++;
		}
		if (Name == Metall_Tuer1)
		{	     	Art[SP_Anz]= 6;
				Objekt_SP[6]++;
		}
		if (Name == Glas_Tuer)
		{	      	Art[SP_Anz]= 7;
				Objekt_SP[7]++;
		}
		if (Name == Glas_Doppel_Tuer)
		{	      	Art[SP_Anz]= 8;
				Objekt_SP[8]++;
		}
		if (Name == Doppel_Tuer)
		{	      	Art[SP_Anz]= 9;
				Objekt_SP[9]++;
		}
		if (Name == Doppel_Tuer2)
		{	      	Art[SP_Anz]= 10;
				Objekt_SP[10]++;
		}
		if (Name == Fahrstuhltuer)
		{	      	Art[SP_Anz]= 11;
				Objekt_SP[11]++;
		}
		if (Name == Saeule)
		{	      	Art[SP_Anz]= 12;
				Objekt_SP[12]++;
		}
		if (Name == Ziegelwand1)
		{	      	Art[SP_Anz]= 13;
				Objekt_SP[13]++;
		}
		if (Name == Ziegelwand2)
		{	      	Art[SP_Anz]= 14;
				Objekt_SP[14]++;
		}
		if (Name == Ziegelwand3)
		{	      	Art[SP_Anz]= 15;
				Objekt_SP[15]++;
		}
		if (Name == Ziegelwand4)
		{	      	Art[SP_Anz]= 16;
				Objekt_SP[16]++;
		}
		if (Name == Ziegelwand5)
		{	      	Art[SP_Anz]= 17;
				Objekt_SP[17]++;
		}
		if (Name == Ziegelwand6)
		{	      	Art[SP_Anz]= 18;
				Objekt_SP[18]++;
		}
		if (Name == Ziegelwand7)
		{	      	Art[SP_Anz]= 19;
				Objekt_SP[19]++;
		}
		if (Name == Ziegelwand8)
		{	      	Art[SP_Anz]= 20;
				Objekt_SP[20]++;
		}
		if (Name == Ziegelwand9)
		{	      	Art[SP_Anz]= 21;
				Objekt_SP[21]++;
		}
		if (Name == Ziegelwand10)
		{	      	Art[SP_Anz]= 22;
				Objekt_SP[22]++;
		}
		if (Name == Ziegelwand11)
		{	      	Art[SP_Anz]= 23;
				Objekt_SP[23]++;
		}
		if (Name == Ziegelwand12)
		{	      	Art[SP_Anz]= 24;
				Objekt_SP[24]++;
		}
		if (Name == Ziegelwand13)
		{	      	Art[SP_Anz]= 25;
				Objekt_SP[25]++;
		}
		if (Name == Ziegelwand14)
		{	      	Art[SP_Anz]= 26;
				Objekt_SP[26]++;
		}
		if (Name == Ziegelwand15)
		{	      	Art[SP_Anz]= 27;
				Objekt_SP[27]++;
		}
		if (Name == Ziegelwand16)
		{	      	Art[SP_Anz]= 28;
				Objekt_SP[28]++;
		}
		if (Name == Ziegelwand17)
		{	      	Art[SP_Anz]= 29;
				Objekt_SP[29]++;
		}
		if (Name == Ziegelwand18)
		{	      	Art[SP_Anz]= 30;
				Objekt_SP[30]++;
		}
		if (Name == Gipswand1)
		{	      	Art[SP_Anz]= 31;
				Objekt_SP[31]++;
		}
		if (Name == Gipswand2)
		{	      	Art[SP_Anz]= 32;
				Objekt_SP[32]++;
		}
		if (Name == Gipswand3)
		{	      	Art[SP_Anz]= 33;
				Objekt_SP[33]++;
		}
		if (Name == Gipswand4)
		{	      	Art[SP_Anz]= 34;
				Objekt_SP[34]++;
		}
		if (Name == Gipswand5)
		{	      	Art[SP_Anz]= 35;
				Objekt_SP[35]++;
		}
		if (Name == Metallabschirm1)
		{	      	Art[SP_Anz]= 36;
				Objekt_SP[36]++;
		}
		if (Name == Metallabschirm2)
		{	      	Art[SP_Anz]= 37;
				Objekt_SP[37]++;
		}
		if (Name == Metallabschirm3)
		{	      	Art[SP_Anz]= 38;
				Objekt_SP[38]++;
		}
		SP_Anz++;
	  }
	Label_1:
	// SchnittPunktber. abgeschlossen
	SP_Anz*=1;		// Label- Dummy Befehl
}


double DEpropag_uldl::Berechnung ()
{
# line 1616 "DEpropag_uldl.pl"
int    i,n,z, Paar;
	double lambda_m, Loss_dB, Attenuat_dB = 0.0;

	lambda_m  = 0.3/(zellen[LC_number].dl_freq);

	if (tunnel_flag==0)
	{
		Loss_dB   = 20*log10(4*3.14159*1/lambda_m) + 10*1.2*log10(d_m/1)
		    	    - 3.0- zellen[LC_number].gain;
		Prec_dBm  = 4.0 - Loss_dB;		// MS sendet mit 4 dBm
	}
	else
	{
		Loss_dB   = 20*log10(4*3.14159*1/lambda_m) + 10*1.2*log10(d_m/1)
		    	    - zellen[LI_number].gain - zellen[LC_number].gain;
		Prec_dBm  = zellen[LI_number].power - Loss_dB;	
	}	

	Sum_OLoss=0;
	 if (SP_Anz !=0)
	 { 
	 for (i=0; i<39; i++) 						// Durchlauf durch Objekte 
	 { 
		if ((i>=3)&&(i<12)) 
				Durchg= Objekt_SP[i]; 			// Tuer kann nur jeweils 1 SP besitzen
	 	else 
	 	{ 
	 		z=1; 
	 		Paar=Durchg=0; 
	 		for (n=0; n<SP_Anz; n++) 			// Hindern.  muessen
	 		{ 						// gerade SP-Anz haben 
	 			if ((Art[n]==i) && (Art[n]==Art[n+1])) // und hintereinander liegen
	 			{ 
	 				if (n+1 > SP_Anz-1)
	 				break;
	 				z++;
	 			} 
	 		} 
	 		if ((z%2)==0)
	 			 Paar=z/2;
	 		else 
	 			Paar=(z-1)/2;
	 		Durchg= Paar;
	 	}
			// Daempfungswerte
			if ((i==0)||(i==1)||(i==2)||(i==7)||(i==8)) 	// Glas
			 { if (zellen[LC_number].dl_freq <= 5) 	Attenuat_dB=1;
			 else 			Attenuat_dB=2;}
			  
			if ((i>=31)&&(i<36)) 				// Gips
			 { if (zellen[LC_number].dl_freq  <= 5) 	Attenuat_dB=4; 
			 else 			Attenuat_dB=16;} 
			 
			if ((i==3)||(i==4)||(i==9)||(i==10)) 		// Holz 
			 { if (zellen[LC_number].dl_freq  <=5) 	Attenuat_dB=6;
			 else 			Attenuat_dB=18;}
			 
			if ((i>=13)&&(i<31)) 				// Ziegel 
			{ if (zellen[LC_number].dl_freq  <= 5) 	Attenuat_dB=40;
			 else 			Attenuat_dB=60;}
			  
			if ((i==5)||(i==6)||(i==11)||(i==36)||(i==37)||(i==38)) // Metall 
			{ if (zellen[LC_number].dl_freq  <= 5) 	Attenuat_dB=60;
			 else 			Attenuat_dB=60;} 
			 
			if (i==12) 					// Beton 
			{ if (zellen[LC_number].dl_freq  <= 5) 	Attenuat_dB=12;
			 else 			Attenuat_dB=60;} 
			 
			 Sum_OLoss += Durchg*Attenuat_dB;
			 } 
			 Prec_dBm -= Sum_OLoss;
		} // Ende if SP_Anz!=0 Schleife
	return (Prec_dBm);
}


// prototype instance for known block list
static DEpropag_uldl proto;
static RegisterBlock registerBlock(proto,"propag_uldl");
