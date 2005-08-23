defstar {
    name { Mobile_Station }
    domain { DE }
    derivedfrom { DEcell_list }
    author { A. Wernicke, J. Voigt }
    version { @(#)DEMobile_Station.pl	1.3 12/08/97 }
    copyright { copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems 
    }
    location { The WiNeS-Library }
    desc { That star receives the demands for creating a new mobile station 
    from the activation. It checks the position and places the mobile in the 
    right cell. If the the received position is not inside of one of the rooms, 
    it demands a new position rather than dropping that mobile away.
    }
    hinclude { <complex.h>, <fstream.h>, <iomanip.h>, <math.h>        }
    input {
        name { R_Pos }
        type { Complex }
    }
    input {
        name { start }
        type { int }
    }
    output {
        name { restart }
        type { int }
    } 
    output {
        name { Rx_Pos }
        type { Complex }
    } 
    output {
        name { cellnum }
        type { int }
    } 
    output {
        name { bandw }
        type { int }
    } 	       
    private {  
        int i, Bed_x1, Bed_x2, Bed_y1, Bed_y2;
        int flag;
        double Var1, Var2, Var3, Var4, Rx, Ry, Wink_R, Wink_P[6], Wink_1[6], Wink_2[6];
        Complex Rec;	      
    }
    begin { 
        DEcell_list::begin(); 
    }
    method {
        name { cell_verif }
        access { private }
        type { void }
        code {
            int angle_flag;
            angle_flag=0;
            flag=0;
            for (i=0; i<cells; i++) {
                Var1= real(zellen[i].cov_area1);
                Var2= real(zellen[i].cov_area2);
                Var3= imag(zellen[i].cov_area1);
                Var4= imag(zellen[i].cov_area2);

                Bed_x1= (Rx >= Var1);
                Bed_x2= (Rx <= Var2);
                Bed_y1= (Ry >= Var3);
                Bed_y2= (Ry <= Var4);
                if (Bed_x1 && Bed_x2 && Bed_y1 && Bed_y2) {
                    angle(i);
                    if (zellen[i].segments!=0) {
                        if (angle_check(i) != 1) {
                            angle_flag=1;
                        }
                    }
                    if (angle_flag==1)
                    break;
                    flag=1;
                }
                if ((flag==1) || (angle_flag==1))
                break;
            }
        }
    }
    method {
        name { angle } 
	access { private }
	arglist { "(int Lv)" }
	type { void }
	code {
            double 	Z, N;
            const 	double Faktor= 57.29577951;     
            for (int n=0; n<zellen[Lv].segments; n++) {
                Z= imag (zellen[Lv].direction[n]) - imag (zellen[Lv].Trans_pos);
                N= real (zellen[Lv].direction[n]) - real (zellen[Lv].Trans_pos);
                Wink_P[n]= Faktor* atan2 (Z,N);				
                Wink_1[n]= Wink_P[n]+ zellen[Lv].beam_width[n]/2;
                if (Wink_1[n] > 180) Wink_1[n] -= 360;
                if (Wink_1[n] < -180) Wink_1[n] += 360;
                Wink_2[n]= Wink_P[n]- zellen[Lv].beam_width[n]/2;	
                if (Wink_2[n] > 180) Wink_2[n] -= 360;			
                if (Wink_2[n] < -180) Wink_2[n] += 360;
            }
        }
    }
    method {
        name { angle_check } 
	access { private }
	arglist { "(int Lv)" }
	type { int }
	code {
		int n, a_flag = 0, im_Bereich[6];
		double Z, N;
		const double Faktor= 57.29577951;     

		for (n=0; n<6; n++) im_Bereich[n]=0;
		
		Z= Ry - imag (zellen[Lv].Trans_pos);
		N= Rx - real (zellen[Lv].Trans_pos);
		Wink_R= Faktor* atan2(Z,N);

		for (n=0; n<zellen[Lv].segments; n++) {
                    if ((Wink_1[n] < 0) && (Wink_2[n] > 0)) {
                        if ((Wink_R <= Wink_1[n]) || (Wink_R >= Wink_2[n]))
                        im_Bereich[n]=1;
                        else
                        im_Bereich[n]=0;
                    }
                    else {
                        if ((Wink_R <= Wink_1[n]) && (Wink_R >= Wink_2[n]))
                        im_Bereich[n]=1;
                        else
                        im_Bereich[n]=0;
                    }
                }
                for (n=0; n<zellen[Lv].segments; n++) {
                    if (im_Bereich[n]==1) {
                        a_flag= 1;
                        break;
                    }
                    else
                    a_flag= 0;
                }
                if (a_flag == 1)
                return 1;
                else
                return 0;
            }
        }
        go {
            if (start.dataNew) {
                restart.put(arrivalTime) << 1;
		Rec= R_Pos.get();
                Rx= real (Rec);
                Ry= imag (Rec);
                cell_verif ();
                if (flag==1) {
                    start.dataNew=FALSE;
                    cellnum.put(arrivalTime) << i;
                    Rx_Pos.put(arrivalTime) << Rec;
                    bandw.put(arrivalTime) << 1; //FIXME: is currently not used 
                }
            }
        }
    }	
			
		
		
