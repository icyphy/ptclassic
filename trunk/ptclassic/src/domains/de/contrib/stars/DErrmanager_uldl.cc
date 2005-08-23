static const char file_id[] = "DErrmanager_uldl.pl";
// .cc file generated from DErrmanager_uldl.pl by ptlang
/*
copyright (c) 1996 - 1997 Dresden University of Technology,
    Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DErrmanager_uldl.h"

const char *star_nm_DErrmanager_uldl = "DErrmanager_uldl";

const char* DErrmanager_uldl :: className() const {return star_nm_DErrmanager_uldl;}

ISA_FUNC(DErrmanager_uldl,DEcell_list);

Block* DErrmanager_uldl :: makeNew() const { LOG_NEW; return new DErrmanager_uldl;}

# line 71 "DErrmanager_uldl.pl"
// must be scope file and external linkage 
        Handy *list_h, *list_t;

DErrmanager_uldl::DErrmanager_uldl ()
{
	setDescriptor("Holds a linked list for all mobile stations. Each time a mobile \n    station is to be created or to be deleted, the list will be updated. \n    Also, each event which changes the list's contents causes a new calculation \n    of the interference situation.");
	addPort(start.setPort("start",this,INT));
	addPort(stop.setPort("stop",this,INT));
	addPort(Rx_Pos.setPort("Rx_Pos",this,COMPLEX));
	addPort(cellnum.setPort("cellnum",this,INT));
	addPort(bandw.setPort("bandw",this,INT));
	addPort(C_dBm.setPort("C_dBm",this,FLOAT));
	addPort(check.setPort("check",this,INT));
	addPort(MS_Position.setPort("MS_Position",this,COMPLEX));
	addPort(cellnumb.setPort("cellnumb",this,INT));

# line 89 "DErrmanager_uldl.pl"
list_h= NULL;
        list_t= NULL;
        handy= NULL;
        active= NULL;
        Rx_Pos.before(C_dBm);
}

void DErrmanager_uldl::wrapup() {
# line 280 "DErrmanager_uldl.pl"
while (list_h) {
            handy=list_h;
            list_h=list_h -> next;
            delete handy;
            handy = 0;
        }
        list_h = NULL;
}

void DErrmanager_uldl::begin() {
# line 80 "DErrmanager_uldl.pl"
DEcell_list::begin();
        DEcell_list::time_offset();
        ejected_user= usersum= 0;
        list_h = NULL;
        list_t = NULL;
        active = NULL;
        handy = NULL;
}

DErrmanager_uldl::~DErrmanager_uldl() {
# line 96 "DErrmanager_uldl.pl"
while (list_h) {
            handy=list_h;
            list_h=list_h -> next;
            delete handy;
            handy = 0;
        }
        list_h = NULL;
}

void DErrmanager_uldl::go() {
# line 255 "DErrmanager_uldl.pl"
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


void DErrmanager_uldl::delete_handy ()
{
# line 109 "DErrmanager_uldl.pl"
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


void DErrmanager_uldl::refresh_slot_man ()
{
# line 168 "DErrmanager_uldl.pl"
s_time= float (zellen[cellno].time_offset+ S_time+ 1.2+ shifter* 0.15);
            e_time= float (s_time+ band* 0.15);
}


void DErrmanager_uldl::initial_slot_man ()
{
# line 177 "DErrmanager_uldl.pl"
eject_user=0;
            s_time= float (zellen[cellno].time_offset+ S_time+ 1.2+ shifter* 0.15);
            e_time= float (s_time+ band* 0.15);
            if (e_time>(zellen[cellno].time_offset+ S_time+ 1.2+ 1.5)) {
                ejected_user++;
                eject_user=1;
            }
}


void DErrmanager_uldl::refresh ()
{
# line 191 "DErrmanager_uldl.pl"
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


Handy * DErrmanager_uldl::new_handy ()
{
# line 212 "DErrmanager_uldl.pl"
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


// prototype instance for known block list
static DErrmanager_uldl proto;
static RegisterBlock registerBlock(proto,"rrmanager_uldl");
