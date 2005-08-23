defstar{
    name { cell_list }  
    domain { DE }
    author { A. Wernicke, J. Voigt }
    version { @(#)DEcell_list.pl	1.2 06/24/97 }
    copyright { copyright (c) 1996 - 1997 Dresden University of Technology,
                Mobile Communications Systems 
    }
    location { The WiNeS-Library }
    desc { This star is the master for some other stars in this library. 
           It contains the room's descriptions and asks for the frequency reuse
           factor (4 choices), the antenna beam pattern (2 choices) as well as 
           for the time offset. Refer to the defstates for further explanations.
    }
    hinclude { <complex.h>, <iostream.h>, <Uniform.h> }
    ccinclude  { <ACG.h> }  
    defstate {
        name { offset } 
        type { float }
        default { "100.0" }
        desc { The time_offset ist a possible offset in the starts of the 
        TDMA-frames in all cells. This parameter takes a percent value, where 
        "100" is the entire frame duration. For "0" at this parameter, all 
        frames in all cells will start at the same time exactly. }     
    }
    defstate {
        name { segments } 
        type { int }
        default { "0" }
        desc { omnidirectional antennas, if 0,
               some adapted antennas otherwise }     
    }
    defstate {
        name { frequency_reuse_factor } 
        type { int }
        default { "0" }
        desc { 0,3,5,9 are allowed }     
    }
    header {
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
    }
    protected {
           static cell zellen[cells];
	   float S_time;
           Uniform *random;
    }  
    constructor { 
        random = NULL;
    }
    destructor  { if (random) delete random;
    }
    begin {	      
	S_time=0.3;
	if (segments) {
	    zellen[0].Trans_pos=    Complex (3930.0, 1750.0);
	    zellen[0].power    =    3.8;
	    zellen[0].gain     =    6.0;
	    zellen[0].segments =    2;
	    zellen[0].direction[0] =    Complex (3930.0, 2000.0);
	    zellen[0].beam_width[0]=    170.0;
	    zellen[0].direction[1] =    Complex (3930.0, 1200.0);
	    zellen[0].beam_width[1]=    170.0;

	    zellen[1].Trans_pos=    Complex (2985.0, 2190.0);
	    zellen[1].power    =    2.3;
	    zellen[1].gain     =	3.0;
	    zellen[1].segments =    0;

	    zellen[2].Trans_pos=    Complex (2985.0, 1310.0); 
	    zellen[2].power    =    2.3;
	    zellen[2].gain     =	3.0;
	    zellen[2].segments =    0;
	    
	    zellen[3].Trans_pos=    Complex (3275.0, 1750.0);
	    zellen[3].power    =    10.0;
	    zellen[3].gain     =	6.0;
	    zellen[3].segments =    1;
	    zellen[3].direction[0] =    Complex (2000.0, 1750.0);
	    zellen[3].beam_width[0]=    60.0;

	    zellen[4].Trans_pos=    Complex (2500.0, 2190.0); 
	    zellen[4].power    =    0.8;
	    zellen[4].gain     =	3.0;
	    zellen[4].segments =    0;

	    zellen[5].Trans_pos=    Complex (2010.0, 1055.0); 
	    zellen[5].power    =    3.0;
	    zellen[5].gain     =	6.0;
	    zellen[5].segments =    1;
	    zellen[5].direction[0] =    Complex (2645.0, 1570.0);
	    zellen[5].beam_width[0]=    80.0;

	    zellen[6].Trans_pos=    Complex (2175.0, 2190.0); 
	    zellen[6].power    =    0.8;
	    zellen[6].gain     =	3.0;
	    zellen[6].segments =    0;

	    zellen[7].Trans_pos=    Complex (1855.0, 2190.0); 
	    zellen[7].power    =    0.8;
	    zellen[7].gain     =	3.0;
	    zellen[7].segments =    0;

	    zellen[8].Trans_pos=    Complex (1075.0, 1935.0); 
	    zellen[8].power    =    3.0;
	    zellen[8].gain     =	6.0;
	    zellen[8].segments =    1;
	    zellen[8].direction[0] =    Complex (1685.0, 2450.0);
	    zellen[8].beam_width[0]=    80.0;

	    zellen[9].Trans_pos=    Complex (800.0, 2190.0); 
	    zellen[9].power    =    2.3;
	    zellen[9].gain     =	3.0;
	    zellen[9].segments =    0;

	    zellen[10].Trans_pos=    Complex (440.0, 1875.0); 
	    zellen[10].power    =    10.0;
	    zellen[10].gain     =	 6.0;
	    zellen[10].segments =    1;
	    zellen[10].direction[0] =    Complex (1140.0, 1285.0);
	    zellen[10].beam_width[0]=    80.0;

	    zellen[11].Trans_pos=    Complex (810.0, 1040.0); 
	    zellen[11].power    =    0.8;
	    zellen[11].gain     =	 3.0;
	    zellen[11].segments =    0;

	    zellen[12].Trans_pos=    Complex (1250.0, 1230.0); 
	    zellen[12].power    =    10.0;
	    zellen[12].gain     =	 6.0;
	    zellen[12].segments =    1;
	    zellen[12].direction[0] =    Complex (1250.0, 800.0);
	    zellen[12].beam_width[0]=    60.0;

	    zellen[13].Trans_pos=    Complex (1690.0, 880.0); 
	    zellen[13].power    =    2.3;
	    zellen[13].gain     =	 3.0;
	    zellen[13].segments =    0;

	    zellen[14].Trans_pos=    Complex (810.0, 660.0); 
	    zellen[14].power    =    0.8;
	    zellen[14].gain     =	 3.0;
	    zellen[14].segments =    0;

	    zellen[15].Trans_pos=    Complex (810.0, 335.0); 
	    zellen[15].power    =    0.8;
	    zellen[15].gain     =	 3.0;
	    zellen[15].segments =    0;

	    zellen[16].Trans_pos=    Complex (1690.0, 335.0); 
	    zellen[16].power    =    0.8;
	    zellen[16].gain     =	 3.0;
	    zellen[16].segments =    0;      
	}
	else {
	    zellen[0].Trans_pos=    Complex (3930.0, 1750.0);
	    zellen[0].power    =    13.0;
	    zellen[0].gain     =	3.0;
	    zellen[0].segments =    0; 
	       		    
	    zellen[1].Trans_pos=    Complex (2985.0, 2190.0);
	    zellen[1].power    =    13.0;
	    zellen[1].gain     =	3.0;
	    zellen[1].segments =    0;
       	
	    zellen[2].Trans_pos=    Complex (2985.0, 1310.0); 
	    zellen[2].power    =    13.0;
	    zellen[2].gain     =	3.0;
	    zellen[2].segments =    0;
  
	    zellen[3].Trans_pos=    Complex (2550.0, 1750.0);
	    zellen[3].power    =    13.0;
	    zellen[3].gain     =	3.0;
	    zellen[3].segments =    0;
       	
	    zellen[4].Trans_pos=    Complex (2500.0, 2190.0); 
	    zellen[4].power    =    13.0;
	    zellen[4].gain     =	3.0;
	    zellen[4].segments =    0;

	    zellen[5].Trans_pos=    Complex (2320.0, 1310.0); 
	    zellen[5].power    =    13.0;
	    zellen[5].gain     =	3.0;
	    zellen[5].segments =    0;
		
	    zellen[6].Trans_pos=    Complex (2175.0, 2190.0); 
	    zellen[6].power    =    13.0;
	    zellen[6].gain     =	3.0;
	    zellen[6].segments =    0;
		       
	    zellen[7].Trans_pos=    Complex (1855.0, 2190.0); 
	    zellen[7].power    =    13.0;
	    zellen[7].gain     =	3.0;
	    zellen[7].segments =    0;

	    zellen[8].Trans_pos=    Complex (1375.0, 2190.0); 
	    zellen[8].power    =    13.0;
	    zellen[8].gain     =	3.0;
	    zellen[8].segments =    0;
	
	    zellen[9].Trans_pos=    Complex (800.0, 2190.0); 
	    zellen[9].power    =    13.0;
	    zellen[9].gain     =	3.0;
	    zellen[9].segments =    0;

	    zellen[10].Trans_pos=    Complex (1095.0, 1585.0); 
	    zellen[10].power    =    13.0;
	    zellen[10].gain     =	 3.0;
	    zellen[10].segments =    0;

	    zellen[11].Trans_pos=    Complex (810.0, 1040.0); 
	    zellen[11].power    =    13.0;
	    zellen[11].gain     =	 3.0;
	    zellen[11].segments =    0;

	    zellen[12].Trans_pos=    Complex (1250.0, 670.0); 
	    zellen[12].power    =    13.0;
	    zellen[12].gain     =	 3.0;
	    zellen[12].segments =    0;

	    zellen[13].Trans_pos=    Complex (1690.0, 880.0); 
	    zellen[13].power    =    13.0;
	    zellen[13].gain     =	 3.0;
	    zellen[13].segments =    0;

	    zellen[14].Trans_pos=    Complex (810.0, 660.0); 
	    zellen[14].power    =    13.0;
	    zellen[14].gain     =	 3.0;
	    zellen[14].segments =    0;

	    zellen[15].Trans_pos=    Complex (810.0, 335.0); 
	    zellen[15].power    =    13.0;
	    zellen[15].gain     =	 3.0;
	    zellen[15].segments =    0;

	    zellen[16].Trans_pos=    Complex (1690.0, 335.0); 
	    zellen[16].power    =    13.0;
	    zellen[16].gain     =	 3.0;
	    zellen[16].segments =    0;        
	}
	switch (frequency_reuse_factor) {
	    case 0: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  0;
		zellen[2].freq_offset=  0;
		zellen[3].freq_offset=  0;
		zellen[4].freq_offset=  0;
		zellen[5].freq_offset=  0;
		zellen[6].freq_offset=  0;
		zellen[7].freq_offset=  0;
		zellen[8].freq_offset=  0;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  0;
		zellen[11].freq_offset=  0;
		zellen[12].freq_offset=  0;
		zellen[13].freq_offset=  0;
		zellen[14].freq_offset=  0;
		zellen[15].freq_offset=  0;
		zellen[16].freq_offset=  0;
		break;
	    }
	    case 3: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  1;
		zellen[2].freq_offset=  2;
		zellen[3].freq_offset=  0;
		zellen[4].freq_offset=  1;
		zellen[5].freq_offset=  2;
		zellen[6].freq_offset=  0;
		zellen[7].freq_offset=  1;
		zellen[8].freq_offset=  2;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  1;
		zellen[11].freq_offset=  2;
		zellen[12].freq_offset=  0;
		zellen[13].freq_offset=  1;
		zellen[14].freq_offset=  2;
		zellen[15].freq_offset=  0;
		zellen[16].freq_offset=  1;
		break;
	    }
	    case 5: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  1;
		zellen[2].freq_offset=  2;
		zellen[3].freq_offset=  3;
		zellen[4].freq_offset=  4;
		zellen[5].freq_offset=  0;
		zellen[6].freq_offset=  1;
		zellen[7].freq_offset=  2;
		zellen[8].freq_offset=  3;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  4;
		zellen[11].freq_offset=  1;
		zellen[12].freq_offset=  2;
		zellen[13].freq_offset=  3;
		zellen[14].freq_offset=  4;
		zellen[15].freq_offset=  0;
		zellen[16].freq_offset=  1;
		break;
	    }
	    case 9: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  1;
		zellen[2].freq_offset=  2;
		zellen[3].freq_offset=  3;
		zellen[4].freq_offset=  4;
		zellen[5].freq_offset=  5;
		zellen[6].freq_offset=  6;
		zellen[7].freq_offset=  7;
		zellen[8].freq_offset=  8;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  1;
		zellen[11].freq_offset=  2;
		zellen[12].freq_offset=  3;
		zellen[13].freq_offset=  4;
		zellen[14].freq_offset=  5;
		zellen[15].freq_offset=  6;
		zellen[16].freq_offset=  7;
		break;
	    }
	}

	zellen[0].number   =   	0;
	zellen[0].user     =    0;
	zellen[0].cov_area1=    Complex (3305.0, 1045.0);
	zellen[0].cov_area2=    Complex (4550.0, 2450.0);
	zellen[0].dl_freq  =	60.0;
	zellen[0].ul_freq  =	60.0;
		     			       		
	zellen[1].number   =   	1;
	zellen[1].user     =    0;
	zellen[1].cov_area1=    Complex (2675.0, 1910.0);
	zellen[1].cov_area2=    Complex (3295.0, 2450.0);
	zellen[1].dl_freq  =	60.0;
	zellen[1].ul_freq  =	60.0;
	       		
	zellen[2].number   =   	2;
	zellen[2].user     =    0;
	zellen[2].cov_area1=    Complex (2675.0, 1045.0);
	zellen[2].cov_area2=    Complex (3295.0, 1585.0);
	zellen[2].dl_freq  =	60.0;
	zellen[2].ul_freq  =	60.0;
	       	  
	zellen[3].number   =   	3;
	zellen[3].user     =    0;
	zellen[3].cov_area1=    Complex (1810.0, 1610.0);
	zellen[3].cov_area2=    Complex (3295.0, 1885.0);
	zellen[3].dl_freq  =	60.0;
	zellen[3].ul_freq  =	60.0;
	       	
	zellen[4].number   =   	4;
	zellen[4].user     =    0;
	zellen[4].cov_area1=    Complex (2355.0, 1925.0);
	zellen[4].cov_area2=    Complex (2645.0, 2450.0);
	zellen[4].dl_freq  =	60.0;
	zellen[4].ul_freq  =	60.0;
	       	
	zellen[5].number   =   	5;
	zellen[5].user     =    0;
	zellen[5].cov_area1=    Complex (2000.0, 1045.0);
	zellen[5].cov_area2=    Complex (2645.0, 1570.0);
	zellen[5].dl_freq  =	60.0;
	zellen[5].ul_freq  =	60.0;
	       			
	zellen[6].number   =   	6;
	zellen[6].user     =    0;
	zellen[6].cov_area1=    Complex (2025.0, 1925.0);
	zellen[6].cov_area2=    Complex (2325.0, 2450.0);
	zellen[6].dl_freq  =	60.0;
	zellen[6].ul_freq  =	60.0;
	       			       
	zellen[7].number   =   	7;
	zellen[7].user     =    0;
	zellen[7].cov_area1=    Complex (1715.0, 1925.0);
	zellen[7].cov_area2=    Complex (1995.0, 2450.0);
	zellen[7].dl_freq  =	60.0;
	zellen[7].ul_freq  =	60.0;
	       	
	zellen[8].number   =   	8;
	zellen[8].user     =    0;
	zellen[8].cov_area1=    Complex (1065.0, 1925.0);
	zellen[8].cov_area2=    Complex (1685.0, 2450.0);
	zellen[8].dl_freq  =	60.0;
	zellen[8].ul_freq  =	60.0;
	       			       
	zellen[9].number   =   	9;
	zellen[9].user     =    0;
	zellen[9].cov_area1=    Complex (550.0, 1925.0);
	zellen[9].cov_area2=    Complex (1055.0, 2450.0);
	zellen[9].dl_freq  =	60.0;
	zellen[9].ul_freq  =	60.0;
	       	
	zellen[10].number   =    10;
	zellen[10].user     =    0;
	zellen[10].cov_area1=    Complex (430.0, 1285.0);
	zellen[10].cov_area2=    Complex (1760.0, 1885.0);
	zellen[10].dl_freq  =	 60.0;
	zellen[10].ul_freq  =	 60.0;
		       	
	zellen[11].number   =    11;
	zellen[11].user     =    0;
	zellen[11].cov_area1=    Complex (550.0, 830.0);
	zellen[11].cov_area2=    Complex (1075.0, 1245.0);
	zellen[11].dl_freq  =	 60.0;
	zellen[11].ul_freq  =	 60.0;
		       	
	zellen[12].number   =    12;
	zellen[12].user     =    0;
	zellen[12].cov_area1=    Complex (1115.0, 100.0);
	zellen[12].cov_area2=    Complex (1385.0, 1245.0);
	zellen[12].dl_freq  =	 60.0;
	zellen[12].ul_freq  =	 60.0;
		       	
	zellen[13].number   =    13;
	zellen[13].user     =    0;
	zellen[13].cov_area1=    Complex (1425.0, 510.0);
	zellen[13].cov_area2=    Complex (1950.0, 1245.0);
	zellen[13].dl_freq  =	 60.0;
	zellen[13].ul_freq  =	 60.0;
		       	
	zellen[14].number   =    14;
	zellen[14].user     =    0;
	zellen[14].cov_area1=    Complex (550.0, 510.0);
	zellen[14].cov_area2=    Complex (1075.0, 820.0);
	zellen[14].dl_freq  =	 60.0;
	zellen[14].ul_freq  =	 60.0;
	       	
	zellen[15].number   =    15;
	zellen[15].user     =    0;
	zellen[15].cov_area1=    Complex (550.0, 190.0);
	zellen[15].cov_area2=    Complex (1075.0, 480.0);
	zellen[15].dl_freq  =	 60.0;
	zellen[15].ul_freq  =	 60.0;
	       	
	zellen[16].number   =    16;
	zellen[16].user     =    0;
	zellen[16].cov_area1=    Complex (1425.0, 190.0);
	zellen[16].cov_area2=    Complex (1950.0, 480.0);
	zellen[16].dl_freq  =	 60.0;
	zellen[16].ul_freq  =	 60.0;  
    }
    code {  
	extern ACG* gen;
	cell DEcell_list::zellen[cells];
    }
    method {    
	name { time_offset }
	access { public }
	type { void }
	code {                  
	    int j;
	    for (j=0; j<cells ;j++) {
		    zellen[j].time_offset= float( (*random) () );
		
	    }
	}
    }     
    setup {
	if (random) delete random;
	double dummy;
        dummy = offset; 
        dummy *= 0.015;
	random = new Uniform(-dummy, dummy, gen);
    }
    wrapup {
	S_time=0.3;
	if (segments) {
	    zellen[0].Trans_pos=    Complex (3930.0, 1750.0);
	    zellen[0].power    =    3.8;
	    zellen[0].gain     =	6.0;
	    zellen[0].segments =    2;
	    zellen[0].direction[0] =    Complex (3930.0, 2000.0);
	    zellen[0].beam_width[0]=    170.0;
	    zellen[0].direction[1] =    Complex (3930.0, 1200.0);
	    zellen[0].beam_width[1]=    170.0;

	    zellen[1].Trans_pos=    Complex (2985.0, 2190.0);
	    zellen[1].power    =    2.3;
	    zellen[1].gain     =	3.0;
	    zellen[1].segments =    0;

	    zellen[2].Trans_pos=    Complex (2985.0, 1310.0); 
	    zellen[2].power    =    2.3;
	    zellen[2].gain     =	3.0;
	    zellen[2].segments =    0;
	    
	    zellen[3].Trans_pos=    Complex (3275.0, 1750.0);
	    zellen[3].power    =    10.0;
	    zellen[3].gain     =	6.0;
	    zellen[3].segments =    1;
	    zellen[3].direction[0] =    Complex (2000.0, 1750.0);
	    zellen[3].beam_width[0]=    60.0;

	    zellen[4].Trans_pos=    Complex (2500.0, 2190.0); 
	    zellen[4].power    =    0.8;
	    zellen[4].gain     =    3.0;
	    zellen[4].segments =    0;

	    zellen[5].Trans_pos=    Complex (2010.0, 1055.0); 
	    zellen[5].power    =    3.0;
	    zellen[5].gain     =	6.0;
	    zellen[5].segments =    1;
	    zellen[5].direction[0] =    Complex (2645.0, 1570.0);
	    zellen[5].beam_width[0]=    80.0;

	    zellen[6].Trans_pos=    Complex (2175.0, 2190.0); 
	    zellen[6].power    =    0.8;
	    zellen[6].gain     =	3.0;
	    zellen[6].segments =    0;

	    zellen[7].Trans_pos=    Complex (1855.0, 2190.0); 
	    zellen[7].power    =    0.8;
	    zellen[7].gain     =	3.0;
	    zellen[7].segments =    0;

	    zellen[8].Trans_pos=    Complex (1075.0, 1935.0); 
	    zellen[8].power    =    3.0;
	    zellen[8].gain     =	6.0;
	    zellen[8].segments =    1;
	    zellen[8].direction[0] =    Complex (1685.0, 2450.0);
	    zellen[8].beam_width[0]=    80.0;

	    zellen[9].Trans_pos=    Complex (800.0, 2190.0); 
	    zellen[9].power    =    2.3;
	    zellen[9].gain     =	3.0;
	    zellen[9].segments =    0;

	    zellen[10].Trans_pos=    Complex (440.0, 1875.0); 
	    zellen[10].power    =    10.0;
	    zellen[10].gain     =	 6.0;
	    zellen[10].segments =    1;
	    zellen[10].direction[0] =    Complex (1140.0, 1285.0);
	    zellen[10].beam_width[0]=    80.0;

	    zellen[11].Trans_pos=    Complex (810.0, 1040.0); 
	    zellen[11].power    =    0.8;
	    zellen[11].gain     =	 3.0;
	    zellen[11].segments =    0;

	    zellen[12].Trans_pos=    Complex (1250.0, 1230.0); 
	    zellen[12].power    =    10.0;
	    zellen[12].gain     =	 6.0;
	    zellen[12].segments =    1;
	    zellen[12].direction[0] =    Complex (1250.0, 800.0);
	    zellen[12].beam_width[0]=    60.0;

	    zellen[13].Trans_pos=    Complex (1690.0, 880.0); 
	    zellen[13].power    =    2.3;
	    zellen[13].gain     =	 3.0;
	    zellen[13].segments =    0;

	    zellen[14].Trans_pos=    Complex (810.0, 660.0); 
	    zellen[14].power    =    0.8;
	    zellen[14].gain     =	 3.0;
	    zellen[14].segments =    0;

	    zellen[15].Trans_pos=    Complex (810.0, 335.0); 
	    zellen[15].power    =    0.8;
	    zellen[15].gain     =	 3.0;
	    zellen[15].segments =    0;

	    zellen[16].Trans_pos=    Complex (1690.0, 335.0); 
	    zellen[16].power    =    0.8;
	    zellen[16].gain     =	 3.0;
	    zellen[16].segments =    0;      
	}
	else {
	    zellen[0].Trans_pos=    Complex (3930.0, 1750.0);
	    zellen[0].power    =    13.0;
	    zellen[0].gain     =	3.0;
	    zellen[0].segments =    0; 
	       		    
	    zellen[1].Trans_pos=    Complex (2985.0, 2190.0);
	    zellen[1].power    =    13.0;
	    zellen[1].gain     =	3.0;
	    zellen[1].segments =    0;
       	
	    zellen[2].Trans_pos=    Complex (2985.0, 1310.0); 
	    zellen[2].power    =    13.0;
	    zellen[2].gain     =	3.0;
	    zellen[2].segments =    0;
  
	    zellen[3].Trans_pos=    Complex (2550.0, 1750.0);
	    zellen[3].power    =    13.0;
	    zellen[3].gain     =	3.0;
	    zellen[3].segments =    0;
       	
	    zellen[4].Trans_pos=    Complex (2500.0, 2190.0); 
	    zellen[4].power    =    13.0;
	    zellen[4].gain     =	3.0;
	    zellen[4].segments =    0;

	    zellen[5].Trans_pos=    Complex (2320.0, 1310.0); 
	    zellen[5].power    =    13.0;
	    zellen[5].gain     =	3.0;
	    zellen[5].segments =    0;
		
	    zellen[6].Trans_pos=    Complex (2175.0, 2190.0); 
	    zellen[6].power    =    13.0;
	    zellen[6].gain     =	3.0;
	    zellen[6].segments =    0;
		       
	    zellen[7].Trans_pos=    Complex (1855.0, 2190.0); 
	    zellen[7].power    =    13.0;
	    zellen[7].gain     =	3.0;
	    zellen[7].segments =    0;

	    zellen[8].Trans_pos=    Complex (1375.0, 2190.0); 
	    zellen[8].power    =    13.0;
	    zellen[8].gain     =	3.0;
	    zellen[8].segments =    0;
	
	    zellen[9].Trans_pos=    Complex (800.0, 2190.0); 
	    zellen[9].power    =    13.0;
	    zellen[9].gain     =	3.0;
	    zellen[9].segments =    0;

	    zellen[10].Trans_pos=    Complex (1095.0, 1585.0); 
	    zellen[10].power    =    13.0;
	    zellen[10].gain     =	 3.0;
	    zellen[10].segments =    0;

	    zellen[11].Trans_pos=    Complex (810.0, 1040.0); 
	    zellen[11].power    =    13.0;
	    zellen[11].gain     =	 3.0;
	    zellen[11].segments =    0;

	    zellen[12].Trans_pos=    Complex (1250.0, 670.0); 
	    zellen[12].power    =    13.0;
	    zellen[12].gain     =	 3.0;
	    zellen[12].segments =    0;

	    zellen[13].Trans_pos=    Complex (1690.0, 880.0); 
	    zellen[13].power    =    13.0;
	    zellen[13].gain     =	 3.0;
	    zellen[13].segments =    0;

	    zellen[14].Trans_pos=    Complex (810.0, 660.0); 
	    zellen[14].power    =    13.0;
	    zellen[14].gain     =	 3.0;
	    zellen[14].segments =    0;

	    zellen[15].Trans_pos=    Complex (810.0, 335.0); 
	    zellen[15].power    =    13.0;
	    zellen[15].gain     =	 3.0;
	    zellen[15].segments =    0;

	    zellen[16].Trans_pos=    Complex (1690.0, 335.0); 
	    zellen[16].power    =    13.0;
	    zellen[16].gain     =	 3.0;
	    zellen[16].segments =    0;        
	}
	switch (frequency_reuse_factor) {
	    case 0: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  0;
		zellen[2].freq_offset=  0;
		zellen[3].freq_offset=  0;
		zellen[4].freq_offset=  0;
		zellen[5].freq_offset=  0;
		zellen[6].freq_offset=  0;
		zellen[7].freq_offset=  0;
		zellen[8].freq_offset=  0;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  0;
		zellen[11].freq_offset=  0;
		zellen[12].freq_offset=  0;
		zellen[13].freq_offset=  0;
		zellen[14].freq_offset=  0;
		zellen[15].freq_offset=  0;
		zellen[16].freq_offset=  0;
		break;
	    }
	    case 3: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  1;
		zellen[2].freq_offset=  2;
		zellen[3].freq_offset=  0;
		zellen[4].freq_offset=  1;
		zellen[5].freq_offset=  2;
		zellen[6].freq_offset=  0;
		zellen[7].freq_offset=  1;
		zellen[8].freq_offset=  2;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  1;
		zellen[11].freq_offset=  2;
		zellen[12].freq_offset=  0;
		zellen[13].freq_offset=  1;
		zellen[14].freq_offset=  2;
		zellen[15].freq_offset=  0;
		zellen[16].freq_offset=  1;
		break;
	    }
	    case 5: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  1;
		zellen[2].freq_offset=  2;
		zellen[3].freq_offset=  3;
		zellen[4].freq_offset=  4;
		zellen[5].freq_offset=  0;
		zellen[6].freq_offset=  1;
		zellen[7].freq_offset=  2;
		zellen[8].freq_offset=  3;
		zellen[9].freq_offset=  4;
		zellen[10].freq_offset=  0;
		zellen[11].freq_offset=  1;
		zellen[12].freq_offset=  2;
		zellen[13].freq_offset=  3;
		zellen[14].freq_offset=  4;
		zellen[15].freq_offset=  0;
		zellen[16].freq_offset=  1;
		break;
	    }
	    case 9: {
		zellen[0].freq_offset=  0;
		zellen[1].freq_offset=  1;
		zellen[2].freq_offset=  2;
		zellen[3].freq_offset=  3;
		zellen[4].freq_offset=  4;
		zellen[5].freq_offset=  5;
		zellen[6].freq_offset=  6;
		zellen[7].freq_offset=  7;
		zellen[8].freq_offset=  8;
		zellen[9].freq_offset=  0;
		zellen[10].freq_offset=  1;
		zellen[11].freq_offset=  2;
		zellen[12].freq_offset=  3;
		zellen[13].freq_offset=  4;
		zellen[14].freq_offset=  5;
		zellen[15].freq_offset=  6;
		zellen[16].freq_offset=  7;
		break;
	    }
	}

	zellen[0].number   =   	0;
	zellen[0].user     =    0;
	zellen[0].cov_area1=    Complex (3305.0, 1045.0);
	zellen[0].cov_area2=    Complex (4550.0, 2450.0);
	zellen[0].dl_freq  =	60.0;
	zellen[0].ul_freq  =	60.0;
		     			       		
	zellen[1].number   =   	1;
	zellen[1].user     =    0;
	zellen[1].cov_area1=    Complex (2675.0, 1910.0);
	zellen[1].cov_area2=    Complex (3295.0, 2450.0);
	zellen[1].dl_freq  =	60.0;
	zellen[1].ul_freq  =	60.0;
	       		
	zellen[2].number   =   	2;
	zellen[2].user     =    0;
	zellen[2].cov_area1=    Complex (2675.0, 1045.0);
	zellen[2].cov_area2=    Complex (3295.0, 1585.0);
	zellen[2].dl_freq  =	60.0;
	zellen[2].ul_freq  =	60.0;
	       	  
	zellen[3].number   =   	3;
	zellen[3].user     =    0;
	zellen[3].cov_area1=    Complex (1810.0, 1610.0);
	zellen[3].cov_area2=    Complex (3295.0, 1885.0);
	zellen[3].dl_freq  =	60.0;
	zellen[3].ul_freq  =	60.0;
	       	
	zellen[4].number   =   	4;
	zellen[4].user     =    0;
	zellen[4].cov_area1=    Complex (2355.0, 1925.0);
	zellen[4].cov_area2=    Complex (2645.0, 2450.0);
	zellen[4].dl_freq  =	60.0;
	zellen[4].ul_freq  =	60.0;
	       	
	zellen[5].number   =   	5;
	zellen[5].user     =    0;
	zellen[5].cov_area1=    Complex (2000.0, 1045.0);
	zellen[5].cov_area2=    Complex (2645.0, 1570.0);
	zellen[5].dl_freq  =	60.0;
	zellen[5].ul_freq  =	60.0;
	       			
	zellen[6].number   =   	6;
	zellen[6].user     =    0;
	zellen[6].cov_area1=    Complex (2025.0, 1925.0);
	zellen[6].cov_area2=    Complex (2325.0, 2450.0);
	zellen[6].dl_freq  =	60.0;
	zellen[6].ul_freq  =	60.0;
	       			       
	zellen[7].number   =   	7;
	zellen[7].user     =    0;
	zellen[7].cov_area1=    Complex (1715.0, 1925.0);
	zellen[7].cov_area2=    Complex (1995.0, 2450.0);
	zellen[7].dl_freq  =	60.0;
	zellen[7].ul_freq  =	60.0;
	       	
	zellen[8].number   =   	8;
	zellen[8].user     =    0;
	zellen[8].cov_area1=    Complex (1065.0, 1925.0);
	zellen[8].cov_area2=    Complex (1685.0, 2450.0);
	zellen[8].dl_freq  =	60.0;
	zellen[8].ul_freq  =	60.0;
	       			       
	zellen[9].number   =   	9;
	zellen[9].user     =    0;
	zellen[9].cov_area1=    Complex (550.0, 1925.0);
	zellen[9].cov_area2=    Complex (1055.0, 2450.0);
	zellen[9].dl_freq  =	60.0;
	zellen[9].ul_freq  =	60.0;
	       	
	zellen[10].number   =    10;
	zellen[10].user     =    0;
	zellen[10].cov_area1=    Complex (430.0, 1285.0);
	zellen[10].cov_area2=    Complex (1760.0, 1885.0);
	zellen[10].dl_freq  =	 60.0;
	zellen[10].ul_freq  =	 60.0;
		       	
	zellen[11].number   =    11;
	zellen[11].user     =    0;
	zellen[11].cov_area1=    Complex (550.0, 830.0);
	zellen[11].cov_area2=    Complex (1075.0, 1245.0);
	zellen[11].dl_freq  =	 60.0;
	zellen[11].ul_freq  =	 60.0;
		       	
	zellen[12].number   =    12;
	zellen[12].user     =    0;
	zellen[12].cov_area1=    Complex (1115.0, 100.0);
	zellen[12].cov_area2=    Complex (1385.0, 1245.0);
	zellen[12].dl_freq  =	 60.0;
	zellen[12].ul_freq  =	 60.0;
		       	
	zellen[13].number   =    13;
	zellen[13].user     =    0;
	zellen[13].cov_area1=    Complex (1425.0, 510.0);
	zellen[13].cov_area2=    Complex (1950.0, 1245.0);
	zellen[13].dl_freq  =	 60.0;
	zellen[13].ul_freq  =	 60.0;
		       	
	zellen[14].number   =    14;
	zellen[14].user     =    0;
	zellen[14].cov_area1=    Complex (550.0, 510.0);
	zellen[14].cov_area2=    Complex (1075.0, 820.0);
	zellen[14].dl_freq  =	 60.0;
	zellen[14].ul_freq  =	 60.0;
	       	
	zellen[15].number   =    15;
	zellen[15].user     =    0;
	zellen[15].cov_area1=    Complex (550.0, 190.0);
	zellen[15].cov_area2=    Complex (1075.0, 480.0);
	zellen[15].dl_freq  =	 60.0;
	zellen[15].ul_freq  =	 60.0;
	       	
	zellen[16].number   =    16;
	zellen[16].user     =    0;
	zellen[16].cov_area1=    Complex (1425.0, 190.0);
	zellen[16].cov_area2=    Complex (1950.0, 480.0);
	zellen[16].dl_freq  =	 60.0;
	zellen[16].ul_freq  =	 60.0; 
    }
}

