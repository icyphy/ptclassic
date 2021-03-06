
defstar{
    name  { rrmanager_uldl }
    domain  { DE }   
    derivedfrom { DEcell_list } 
    author { A. Wernicke, J. Voigt }
    version { @(#)DErrmanager_uldl.pl	1.5 06/11/98 }
    copyright { copyright (c) 1996 - 1997 Dresden University of Technology,
    Mobile Communications Systems 
    }
    desc { Holds a linked list for all mobile stations. Each time a mobile 
    station is to be created or to be deleted, the list will be updated. 
    Also, each event which changes the list's contents causes a new calculation 
    of the interference situation. }
    location { The WiNeS-Library }   
    hinclude { "DEcell_list.h" }
    input 	{
        name { start }        
        type { int }
    }
    input 	{
        name { stop }  
        type { int }
    }
    input 	{ 
        name { Rx_Pos }
        type { Complex }
    }
    input 	{
        name { cellnum } 
        type { int }
    }
    input 	{
        name { bandw }
        type { int }
    }
    input 	{
        name { C_dBm }
        type { float }
    }
    output 	{
        name { check } 
        type { int }  
    }	
    output 	{
        name { MS_Position }
        type { Complex }
    }
    output 	{
        name { cellnumb } 
        type { int }
    }
    header {
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
    }
    code { 
        // must be scope file and external linkage 
        Handy *list_h, *list_t; 
    }    
    private { 
        Handy *handy, *active;
        int band, cellno, del_cellno, startvar, stopvar, shifter, hop, eject_user, ejected_user, usersum;
        float s_time, e_time;
    }
    begin   { 
        DEcell_list::begin();
        DEcell_list::time_offset();
        ejected_user= usersum= 0;
        list_h = NULL;
        list_t = NULL;
        active = NULL;
        handy = NULL;
    }
    constructor { 
        list_h= NULL;
        list_t= NULL;
        handy= NULL;
        active= NULL;
        Rx_Pos.before(C_dBm);
    }
    destructor  { 
        while (list_h) {
            handy=list_h;
            list_h=list_h -> next;
            delete handy;
            handy = 0;
        }
        list_h = NULL;
    }
    method {
        name { delete_handy }
	access { private }
	type { void }
	code {  
            if (list_t!=NULL && list_h != NULL) {    
                if ((list_h->next==NULL) && ((list_h->startnum)==stopvar)) {
                    // delete the only member
                    Handy *del_handy;
                    del_handy=list_h;
                    zellen[list_h->cellnumber].user--;
                    del_cellno=list_h->cellnumber;
                    list_h=NULL;
                    list_t=NULL;
                    delete del_handy;
                    del_handy = 0;
                }
                if ((list_t!=NULL) && (list_h->next!=NULL)) {
                    // list has at least two members
                    for (handy=list_h; handy->next!=NULL; handy=handy->next) {
                        if ((handy==list_h) && ((handy->startnum)==stopvar)) {  
                            // delete first member
                            Handy *del_handy;
                            list_h=handy->next;
                            del_handy=handy;
                            zellen[handy->cellnumber].user--;
                            del_cellno=handy->cellnumber;
                            delete del_handy;
                            del_handy = 0;
                            break;
                        }
                        if ((handy->next->startnum)==stopvar) {  
                            Handy *del_handy;
                            del_handy=handy->next;
                            if (handy->next->next==NULL) {
                                // delete last member
                                list_t=handy;
                                zellen[handy->next->cellnumber].user--;
                                del_cellno=handy->next->cellnumber;
                                handy->next=NULL;
                                delete del_handy;
                                del_handy  = 0;
                                break;
                            }
                            else {
                                // delete member in the middle
                                zellen[handy->next->cellnumber].user--;
                                del_cellno=handy->next->cellnumber;
                                handy->next=handy->next->next;
                                delete del_handy;
                                del_handy = 0;
                                break;
                            }
                        }
                    }
                }              
            }
        }
    }
    method {
        name { refresh_slot_man }
	access { private }
	type { void }
	code {
            s_time= float (zellen[cellno].time_offset+ S_time+ 1.2+ shifter* 0.15);
            e_time= float (s_time+ band* 0.15);
        }
    }
    method {
	name { initial_slot_man }
	access { private }
	type { void }
	code {
            eject_user=0;
            s_time= float (zellen[cellno].time_offset+ S_time+ 1.2+ shifter* 0.15);
            e_time= float (s_time+ band* 0.15);
            if (e_time>(zellen[cellno].time_offset+ S_time+ 1.2+ 1.5)) {
                ejected_user++;
                eject_user=1;
            }
        }
    }
    method {
        name { refresh }
	access { private }
	type { void }
	code {
            shifter=0;
            band=0;
            handy=list_h;
            while (handy) {
                if (handy->cellnumber==del_cellno) {
                    cellno=del_cellno;
                    shifter+=band;
                    band=  handy->B;
                    refresh_slot_man();
                    handy->t_begin = s_time;
                    handy->t_end   = e_time;	
                }
                handy=handy->next;
            }
        }
    }
    method {
        name { new_handy }
	access { private }
	type { "Handy *" }
	code {  
            int a;
            usersum++;
            cellno=   cellnum.get();
            band=     bandw.get();
            shifter=0;
            if (zellen[cellno].user==0)     
            initial_slot_man();
            else {
                handy=list_h;
                while (handy) {
                    if (handy->cellnumber==cellno)
                    shifter+=handy->B;
                    handy=handy->next;
                }
                initial_slot_man();
            }
            if (!eject_user) {
                handy= new Handy;
                handy->startnum	= startvar;
                handy->MS_pos      = Rx_Pos.get();
                handy->B            = band;
                handy->cellnumber   = cellno;
                handy->t_begin      = s_time;
                handy->t_end        = e_time;
                handy->C_dBm        = 0;
                for (a=0; a<5; a++) {
                    handy->I_Tab[a].I_value=0;
                    handy->I_Tab[a].t_value=0;
                }
                zellen[cellno].user++;
                if (!list_h)
                list_h=handy;
                else
                list_t->next=handy;
                list_t=handy;
                list_t->next=NULL;
                return (handy);
            }
            else 
            return (NULL);
        }
    }   
    go {
        if (stop.dataNew) {
            stopvar=  stop.get();
            delete_handy();
            refresh();
        }
        if (Rx_Pos.dataNew) {
            startvar= start.get();
            active=new_handy();
            if (!eject_user) {
                MS_Position.put(arrivalTime) << active->MS_pos;
                cellnumb.put(arrivalTime)   << active->cellnumber;  
            }
            Rx_Pos.dataNew=FALSE;
        }
        if ((C_dBm.dataNew) && (active !=NULL)) {
            active->C_dBm=C_dBm.get();
            if (active->C_dBm < -120) active->C_dBm = -70;
        }
        if (active !=NULL && list_h != NULL && list_t != NULL) {
            if ((list_h != list_t) && active->C_dBm!=0) {
                check.put(arrivalTime) << 1;   
            }
        }
    }
    wrapup  { 
        while (list_h) {
            handy=list_h;
            list_h=list_h -> next;
            delete handy;
            handy = 0;
        }
        list_h = NULL;
    }
}
