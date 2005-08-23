defstar{
    name { CIR_uldl }  
    domain { DE }
    derivedfrom { DEcell_list }
    author { A. Wernicke, J. Voigt }
    version { @(#)DECIR_uldl.pl	1.6 01/07/99 }
    copyright { copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems 
    }
    desc { The interfences are calculated here by calling the appropriate 
    methods in the propagation-star. This star is called each time a mobile 
    station appears in the environment or one mobile station vanishes from the 
    environment. Both events of that kind cause a new interference situation 
    which is then calculated. Information and data are given out to animation 
    and data analysis stars.
    }
    location { The WiNeS-Library }   
    hinclude { <iostream.h>, <iomanip.h>, <fstream.h>, <complex.h>, 
    <string.h>, <math.h>, "DErrmanager_uldl.h", "DEpropag_uldl.h" }
    input {
        name { check }
        type { int }
    }
    output {
        name { CIR_value }
        type { float }
    }
    output {
        name{interferenz}
        type {Complex }
    }
    output {
        name{strength}
        type {int }
    }
    output {
        name{interferenz_bs}
        type {Complex }
    }
    output {
        name{strength_bs}
        type {int }
    }
    defstate {
	name { fileName }
	type { string }
	default { "$PTOLEMY/src/domains/de/contrib/stars/topo.dxf" }
	descriptor { File describing topology }
    }
    header { 
        //declare list_h for that file
        extern Handy *list_h;
    }
    private { 
        DEpropag_uldl prop_class;
        Handy *mobile, *interferer, *mob_count;    
	int slot_counter, slots_lower7, slots_lower23,ss;
	float CIR_min, CIR_max, CIR_aver, CIR_Sum, outage_7, outage_23; 
	double Wink_R, Wink_P[6], Wink_1[6], Wink_2[6];   
    }
    constructor { 
        mobile= interferer= mob_count= NULL;  
    } 
    destructor  {          
        while(list_h) {  
            interferer=list_h;
            list_h=list_h->next;
            delete interferer;
 	    interferer = 0;
        }
    }
    begin { 
        DEcell_list::begin();
        slot_counter= slots_lower23= slots_lower7= 0;
        CIR_max= CIR_Sum= 0;
        CIR_min= 140.0;
    }
    wrapup {
        while(list_h) {  
            interferer= list_h;
            list_h= list_h->next;
            delete interferer;
            interferer = 0;
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
                if (Wink_1[n] > 180)	Wink_1[n] -= 360;
                if (Wink_1[n] < -180)	Wink_1[n] += 360;
                Wink_2[n]= Wink_P[n]- zellen[Lv].beam_width[n]/2;			
                if (Wink_2[n] > 180)	Wink_2[n] -= 360;						
                if (Wink_2[n] < -180)   Wink_2[n] += 360;
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

            float Rx= real (mobile->MS_pos);
            float Ry= imag (mobile->MS_pos);
            for (n=0; n<6; n++)	im_Bereich[n]=0;
		
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
            if (a_flag == 1) return 1;
            else
            return 0;
        }
    } 
    method {
        name { scan_Handy }
	access { private }
	type { void }
	code {
            int b, s, n, knum, angle_flag, interf_flag, intf_flag, Bedingung1, Bedingung2;
            float t1, t2, tbg, tnd, ta, te, tj, t_value = 0.0, I, I_Sum, CIR, I_Sum_jens;
		
            for (b=0; b<cells; b++) {
		if (zellen[b].user==0) continue;
		
                mobile=list_h;
                while (mobile) {
                    if (mobile->cellnumber != b) {
                        mobile=mobile->next;
                        continue;       
                    }
                    for (n=0; n<mobile->B; n++) {
                        mobile->I_Tab[n].I_value=0;
                        mobile->I_Tab[n].t_value=0;
                    }   
                    for (t1=mobile->t_begin, s=0; s<mobile->B; t1+=0.15, s++) {       
                        slot_counter++;
                        I_Sum=knum=0;
                        interferer=list_h;
                        while (interferer) {
                            knum++;
                            if (zellen[mobile->cellnumber].freq_offset != zellen[interferer->cellnumber].freq_offset) {
                                interferer=interferer->next;
                                continue;
                            }
                            if (mobile->cellnumber==interferer->cellnumber) {
                                interferer=interferer->next;
                                continue;
                            }
                            angle_flag=0;
                            angle(interferer->cellnumber);
                            if (zellen[interferer->cellnumber].segments !=0) {
                                if (angle_check(interferer->cellnumber) !=1) {
                                    angle_flag=1;
                                }
                            }
                            if (angle_flag) {
                                interferer=interferer->next;
                                continue;
                            }
                            interf_flag=0;
                            t2=t1+0.15;
                            tbg=interferer->t_begin;
                            tnd=interferer->t_end;
                            Bedingung1= (tbg<=t1);
                            Bedingung2= (tnd>=t2);
                            if (Bedingung1 && Bedingung2) {
                                t_value=1.0;                          
                                interf_flag=1;
                            }
                            if ((tbg<t1) && (tnd>t1) && (tnd<(t1+0.15))) {
                                t_value=(tnd-t1)/0.15;                             
                                interf_flag=1;
                            }
                            if ((tbg>t1) && (tbg<(t1+0.15)) && (tnd>(t1+0.15))) {
                                t_value=(t1+0.15-tbg)/0.15;                            
                                interf_flag=1;
                            }
                            ta=interferer->t_begin+3.0;
                            te=interferer->t_end+3.0;
                            if ((ta<=t1) && (te>=(t1+0.15))) {
                                t_value=1.0;                          
                                interf_flag=1;
                            }
                            if ((ta<t1) && (te>t1) && (te<(t1+0.15))) {
                                t_value=(te-t1)/0.15;                             
                                interf_flag=1;
                            }
                            if ((ta>t1) && (ta<(t1+0.15)) && (te>(t1+0.15))) {
                                t_value=(t1+0.15-ta)/0.15;                            
                                interf_flag=1;
                            }    
                            if (interf_flag) {
                                I = prop_class.MS_Stoerer(b,
					 interferer->MS_pos, fileName); 
                                I_Sum+=t_value * pow(10.0, I/10);         // worst case, I[mW] 
                                if (t_value>mobile->I_Tab[s].t_value)
                                mobile->I_Tab[s].t_value=t_value;
                                // Jens
                                I_Sum_jens = t_value * pow(10.0, I/10);
                                I_Sum_jens = 10* log10(I_Sum_jens);
                                if (I_Sum_jens==0) I_Sum_jens= -174.4;
                                else {
                                    if (I_Sum_jens < -120.0) I_Sum_jens= -174.4;
                                }
                                CIR= mobile->C_dBm - I_Sum_jens;
                                ss = 1;
                                if (CIR < 20) ss++;
                                if (CIR < 10) ss++;
                                if (CIR < 40) {
                                    interferenz.put(arrivalTime) << Complex(mobile->startnum, interferer->startnum);
                                    strength.put(arrivalTime) << ss;
                                }
                            }
                            interferer=interferer->next;
                        }
                        // check base stations now
                        for (n=0; n<cells; n++) {                                        
                            if (n==b) {
                                // same cell
                                continue;
                            }
                            if (zellen[b].freq_offset != zellen[n].freq_offset) {
                                // another frequency
                                continue;
                            }
                            angle_flag=0;
                            angle(n);
                            if (zellen[n].segments !=0) {
                                if (angle_check(n) !=1) {
                                    angle_flag=1;
                                }
                            }
                            if (angle_flag) continue;
                            intf_flag=0;
                            // 1. DL-frame
                            if ((zellen[n].time_offset<=t1) && ((zellen[n].time_offset+S_time+1.2)>=(t1+0.15))) {
                                t_value=1.0;                         
                                intf_flag=1;
                            }
                            if ((zellen[n].time_offset<t1) && ((zellen[n].time_offset+S_time+1.2)>t1) && ((zellen[n].time_offset+S_time+1.2)<(t1+0.15))) {
                                t_value=(zellen[n].time_offset+S_time+1.2-t1)/0.15;                          
                                intf_flag=1;
                            }    
                            if ((zellen[n].time_offset>t1) && (zellen[n].time_offset<(t1+0.15)) && ((zellen[n].time_offset+S_time+1.2)>(t1+0.15))) {
                                t_value=(t1+0.15-zellen[n].time_offset)/0.15;                            
                                intf_flag=1;
                            }
                            // 2. DL-frame
                            tj= zellen[n].time_offset+3.0;
                            if ((tj<=t1) && ((tj+S_time+1.2)>=(t1+0.15))) {
                                t_value=1.0;                     
                                intf_flag=1;
                            }
                            if ((tj<t1) && ((tj+S_time+1.2)>t1) && ((tj+S_time+1.2)<(t1+0.15))) {
                                t_value=(tj+S_time+1.2-t1)/0.15;                          
                                intf_flag=1;
                            }
                            if ((tj>t1) && (tj<(t1+0.15)) && ((tj+S_time+1.2)>(t1+0.15))) {
                                t_value=(t1+0.15-tj)/0.15;                            
                                intf_flag=1;
                            }
                            if (intf_flag) {
                                I = prop_class.BS_Stoerer(n, b, fileName); 
                                I_Sum+=t_value * pow(10.0, I/10);          // worst case, I[mW] 
                                if (t_value>mobile->I_Tab[s].t_value)
                                mobile->I_Tab[s].t_value=t_value;
                                I_Sum_jens = t_value * pow(10.0, I/10);
                                I_Sum_jens = 10* log10(I_Sum_jens);
                                if (I_Sum_jens==0) I_Sum_jens= -174.4;
                                else  {
                                    if (I_Sum_jens < -120.0) I_Sum_jens= -174.4;
                                }
                                CIR= mobile->C_dBm - I_Sum_jens;
                                ss = 1;
                                if (CIR < 20) ss++;
                                if (CIR < 10) ss++;
                                if (CIR < 40) {
                                    interferenz_bs.put(arrivalTime+0.01) <<Complex(mobile->startnum, n);
                                    strength_bs.put(arrivalTime+0.01) << ss;
                                }
                            }
                        }              // for n    
                        if (I_Sum==0)       
                        mobile->I_Tab[s].I_value= -174.4;              
                        else  {
                            mobile->I_Tab[s].I_value= 10* log10(I_Sum);
                            if (mobile->I_Tab[s].I_value < -120.0)
                            mobile->I_Tab[s].I_value= -174.4;
                        }
                        CIR= mobile->C_dBm - mobile->I_Tab[s].I_value;           // C/I [dB]
                        if (CIR > CIR_max)
                        CIR_max= CIR;
                        if (CIR < CIR_min)
                        CIR_min= CIR;
                        CIR_Sum+=  CIR;
                        CIR_aver= (float) CIR_Sum/slot_counter;
                        CIR_value.put(arrivalTime + 0.01 * slot_counter) << CIR;
                    }   // for t1,s
                    mobile=mobile->next;
                }       // while (mobile)
            }         // for b
        }
    }
    go {
        if (check.dataNew) {
            scan_Handy();
            check.dataNew=FALSE;
        }
    }
}
 
