static const char file_id[] = "DEMobile_Station.pl";
// .cc file generated from DEMobile_Station.pl by ptlang
/*
copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEMobile_Station.h"

const char *star_nm_DEMobile_Station = "DEMobile_Station";

const char* DEMobile_Station :: className() const {return star_nm_DEMobile_Station;}

ISA_FUNC(DEMobile_Station,DEcell_list);

Block* DEMobile_Station :: makeNew() const { LOG_NEW; return new DEMobile_Station;}

DEMobile_Station::DEMobile_Station ()
{
	setDescriptor("That star receives the demands for creating a new mobile station \n    from the activation. It checks the position and places the mobile in the \n    right cell. If the the received position is not inside of one of the rooms, \n    it demands a new position rather than dropping that mobile away.");
	addPort(R_Pos.setPort("R_Pos",this,COMPLEX));
	addPort(start.setPort("start",this,INT));
	addPort(restart.setPort("restart",this,INT));
	addPort(Rx_Pos.setPort("Rx_Pos",this,COMPLEX));
	addPort(cellnum.setPort("cellnum",this,INT));
	addPort(bandw.setPort("bandw",this,INT));


}

void DEMobile_Station::begin() {
# line 48 "DEMobile_Station.pl"
DEcell_list::begin();
}

void DEMobile_Station::go() {
# line 150 "DEMobile_Station.pl"
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


void DEMobile_Station::cell_verif ()
{
# line 55 "DEMobile_Station.pl"
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


void DEMobile_Station::angle (int Lv)
{
# line 90 "DEMobile_Station.pl"
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


int DEMobile_Station::angle_check (int Lv)
{
# line 111 "DEMobile_Station.pl"
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


// prototype instance for known block list
static DEMobile_Station proto;
static RegisterBlock registerBlock(proto,"Mobile_Station");
