static const char file_id[] = "DEWiNeS_Tcl_Animation.pl";
// .cc file generated from DEWiNeS_Tcl_Animation.pl by ptlang
/*
Copyright (c) 1997 Dresden University of Technology,
            Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEWiNeS_Tcl_Animation.h"
#include "ptk.h"

const char *star_nm_DEWiNeS_Tcl_Animation = "DEWiNeS_Tcl_Animation";

const char* DEWiNeS_Tcl_Animation :: className() const {return star_nm_DEWiNeS_Tcl_Animation;}

ISA_FUNC(DEWiNeS_Tcl_Animation,DEStar);

Block* DEWiNeS_Tcl_Animation :: makeNew() const { LOG_NEW; return new DEWiNeS_Tcl_Animation;}

# line 99 "DEWiNeS_Tcl_Animation.pl"
static int grabNew(ClientData tcl, Tcl_Interp*, int, char*[])
            {
                InfString inputs = ((TclStarIfcWiNeS*)tcl)->getInputValue_New();
                Tcl_SetResult(ptkInterp, (char*) inputs, TCL_VOLATILE);
                return TCL_OK;
            }
            static int grabDelete(ClientData tcl, Tcl_Interp*, int, char*[])
            {
                InfString inputs = ((TclStarIfcWiNeS*)tcl)->getInputValue_Del();
                Tcl_SetResult(ptkInterp, (char*) inputs, TCL_VOLATILE);
                return TCL_OK;
            }
	    static int Setup(ClientData tcl, Tcl_Interp*, int, char*[])
            {
                InfString inputs = ((TclStarIfcWiNeS*)tcl)->getSetupValue();
		Tcl_SetResult(ptkInterp, (char*) inputs, TCL_VOLATILE);
                return TCL_OK;
            }
	    static int grabInterf(ClientData tcl, Tcl_Interp*, int, char*[])
            {
                InfString inputs=((TclStarIfcWiNeS*)tcl)->getInterferenzValue();
		Tcl_SetResult(ptkInterp, (char*) inputs, TCL_VOLATILE);
                return TCL_OK;
            }
	    static int grabInterf_bs(ClientData tcl, Tcl_Interp*, int, char*[])
            {
                InfString inputs=((TclStarIfcWiNeS*)tcl)->getInterferenzValue_bs();
		Tcl_SetResult(ptkInterp, (char*) inputs, TCL_VOLATILE);
                return TCL_OK;
            }
            TclStarIfcWiNeS::TclStarIfcWiNeS() {
                starID = "tclStar";
                starID += unique++;
            }
            TclStarIfcWiNeS::~TclStarIfcWiNeS() {
               // exit immediately if there is no ptk interpreter.  This happens
               // if there are global TclStarIfc objects in the program but the
               // interpreter failed to set up.
                if (!ptkInterp) return;

                InfString buf;

               // First, invoke the Tcl desctructor procedure, if it was defined
                buf = "info procs destructorTcl_";
                buf += starID;
                if(Tcl_GlobalEval(ptkInterp, (char*)buf) == TCL_OK &&
                strlen(ptkInterp->result) != 0)
                callTclProc("destructorTcl");

                // Delete Tcl commands created for this star
                buf = "grabInputsNew_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);

                buf = "grabInputsDel_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);

		buf = "grabSetup_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);

		buf = "grabInterf_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);

		buf = "grabInterf_bs_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);

            }

            InfString TclStarIfcWiNeS::getInputValue_New() {
                InfString buffer;
                PortHole *which_one = myStar->portWithName("New_");
                Particle& MyParticle = (*which_one)%0; 
                mydata = MyParticle;
                buffer = "{";
                buffer += StringList (int(ceil(float(mydata.real()))));
                buffer += "} ";
                buffer += "{";
                buffer += StringList (int(ceil(float(mydata.imag()))));
                buffer += "} ";
                buffer += "{";
                buffer += StringList (num);
                buffer += "} "; 
		return buffer;
            } 
	    InfString TclStarIfcWiNeS::getInputValue_Del() {
                InfString buffer;
                PortHole *which_one = myStar->portWithName("Delete_");
                buffer = "{";
                buffer += StringList (((*which_one)%0).print());
                buffer += "} "; 
                return buffer;
            }
	    InfString TclStarIfcWiNeS::getSetupValue() {
                InfString buffer;
		buffer = "{";
                buffer += StringList (segmente);
                buffer += "} "; 
                return buffer;
            } 
	    InfString TclStarIfcWiNeS::getInterferenzValue() {
                InfString buffer;
		buffer = "{";
                buffer += StringList (r1);
                buffer += "} ";
	        buffer += "{";
                buffer += StringList (r2);
                buffer += "} ";
                buffer += "{";
                buffer += StringList (i2);
                buffer += "} ";
		buffer += "{";
		buffer += StringList (r);
                buffer += "} ";
                buffer += "{";
                buffer += StringList (i);
                buffer += "} ";
		buffer += "{";
                buffer += StringList (s);
                buffer += "} ";
		return buffer;
            }
	    InfString TclStarIfcWiNeS::getInterferenzValue_bs() {
                InfString buffer;
		buffer = "{";
                buffer += StringList (r1);
                buffer += "} ";
		buffer += "{";
		buffer += StringList (r);
                buffer += "} ";
                buffer += "{";
                buffer += StringList (i);
                buffer += "} ";
		buffer += "{";
                buffer += StringList (s);
                buffer += "} ";
		return buffer;
            }
            int TclStarIfcWiNeS::mySetup(Block *star, const char *tcl_file) {
                myStar = star;
                InfString buffer;
                synchronous = 1; 

                buffer = "grabInputsNew_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, grabNew, (ClientData) this , NULL);
              
                buffer = "grabInputsDel_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, grabDelete, (ClientData) this, NULL);

		
                buffer = "grabSetup_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, Setup, (ClientData) this, NULL);

		buffer = "grabInterf_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, grabInterf, (ClientData) this, NULL);

		buffer = "grabInterf_bs_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, grabInterf_bs, (ClientData) this, NULL);
                // Set entries in the $starID array
                buffer = (int) 2;
                Tcl_SetVar2(ptkInterp, (char*)starID, "numInputs",
                (char*)buffer, TCL_GLOBAL_ONLY);

                buffer = (int) 0;
                Tcl_SetVar2(ptkInterp, (char*)starID, "numOutputs",
                (char*)buffer, TCL_GLOBAL_ONLY);

                buffer = star->fullName();
                Tcl_SetVar2(ptkInterp, (char*)starID, "fullName",
                (char*)buffer, TCL_GLOBAL_ONLY);

            
                // Step through all the states
                State* s;
                BlockStateIter next(*star);
                while ((s = next++) != 0) {
                    InfString val = s->currentValue();
                    InfString name = s->name();
                    // In case the currentValue is null, use an empty string.
                    char* v = (char*)val;
                    if (v == NULL) { v = ""; }
                    Tcl_SetVar2(ptkInterp, (char*)starID, (char*)name,
                    v, TCL_GLOBAL_ONLY);
                }
            
                // Add the full name of the star to the $starID array
                InfString fullname = star->fullName();
                Tcl_SetVar2(ptkInterp, (char*)starID, "fullName",
                (char*)fullname, TCL_GLOBAL_ONLY);
            
                // Before sourcing the Tcl script, we set the global variable
                // starID, which will be used by the Tcl script.
                if(Tcl_SetVar(ptkInterp, "starID", (char*)starID, TCL_GLOBAL_ONLY)
                == NULL) {
                    Error::abortRun(*star, "Failed to set starID");
                    return FALSE;
                }
            
                // Source the Tcl script.
                if(tcl_file[0] == '$') {
                    buffer = "source [ptkExpandEnvVar \\";
                    buffer += tcl_file;
                    buffer += "]";
                } else { 
                    buffer = "source ";
                    buffer += tcl_file;
                }
            
                if(Tcl_GlobalEval(ptkInterp, (char*)buffer) != TCL_OK) {
                    char tkErrorCmd[ sizeof(PTK_DISPLAY_ERROR_INFO) ];
                    strcpy(tkErrorCmd, PTK_DISPLAY_ERROR_INFO);
                    Tcl_GlobalEval(ptkInterp, tkErrorCmd);
                    Error::abortRun(*star, "Cannot source tcl script ", tcl_file);
                    return FALSE;
                }
                // Determine whether the star is synchronous by checking to
                // see whether the goTcl procedure has been defined by the
                // Tcl script.
                buffer = "info procs goTcl_";
                buffer += starID;
                if(Tcl_GlobalEval(ptkInterp, (char*)buffer) != TCL_OK ||
                strlen(ptkInterp->result) == 0)
                synchronous = 0;
                else 
                synchronous = 1;
           
                // Determine whether Tk has been loaded into the system
                // (if so, then its event loop will need to be updated).
                char tkExistsCmd[ sizeof(PTK_TK_EXISTS) ];
                strcpy(tkExistsCmd, PTK_TK_EXISTS);
                Tcl_GlobalEval(ptkInterp, tkExistsCmd);
                if (*(ptkInterp->result)=='1') tkExists = 1;
                else tkExists = 0;
                
                return TRUE;
            }

DEWiNeS_Tcl_Animation::DEWiNeS_Tcl_Animation ()
{
	setDescriptor("This is an tcl-animation star. A tcl-script is read in and data \n        containing positions of mobile stations, information about connections \n        to the base stations as well as information about existing interferences\n        come to this star from CIR. Each time anything happens in the network \n        the animation will depict it. All mobile stations are shown at their \n        position, all connections are denoted by yellow arrows. Orange arrows \n        show interferences which are caused by other mobile stations, while red\n        arrows show interferences which are caused by transmissions from other \n        base stations. All interferences are measured in the uplink, so the \n        receiving station is always the base station. \n        This animation can be used for debugging and demonstration reasons. \n        The most interesting and generic part of this star might be the code\n        which I use to read in data from a specific input instead of an input \n        in a MultiPortHole as it is in the TclScript-star in the \n        Ptolemy-library.");
	addPort(New_.setPort("New_",this,COMPLEX));
	addPort(Delete_.setPort("Delete_",this,INT));
	addPort(strength.setPort("strength",this,INT));
	addPort(Interferenz.setPort("Interferenz",this,COMPLEX));
	addPort(strength_bs.setPort("strength_bs",this,INT));
	addPort(Interferenz_bs.setPort("Interferenz_bs",this,COMPLEX));
	addState(tcl_file.setState("tcl_file",this,"$PTOLEMY/src/domains/de/contrib/stars/test.tcl","The file from which to read the Tcl script"));
	addState(Segments.setState("Segments",this,"segments","0 = omnidiectional antennas"));

# line 353 "DEWiNeS_Tcl_Animation.pl"
mobile = NULL;
}

void DEWiNeS_Tcl_Animation::wrapup() {
# line 429 "DEWiNeS_Tcl_Animation.pl"
tcl.wrapup();
}

void DEWiNeS_Tcl_Animation::begin() {
# line 356 "DEWiNeS_Tcl_Animation.pl"
tcl.segmente = Segments;
            tcl.mySetup( this, (const char*) tcl_file );
}

void DEWiNeS_Tcl_Animation::go() {
# line 360 "DEWiNeS_Tcl_Animation.pl"
if (New_.dataNew) {
		int flag;
		dummy = New_%0;
		flag = 0;
		mobile = list_h;
		while (mobile) {
		    if (mobile->MS_pos == dummy) {
			tcl.num = mobile->cellnumber;
			flag++;
		    }
		    if ( flag ) break;
		    else mobile = mobile->next;
		}
		tcl.callTcl("new");
                New_.dataNew = FALSE;
            }
            if (Delete_.dataNew) {
                tcl.callTcl("loeschen");
                Delete_.dataNew = FALSE;
            }
	    if (Interferenz.dataNew) {
		dummy = Interferenz%0;
		int r, i, flag;
		r = int(real(dummy));
		i = int(imag(dummy));
		tcl.s = strength.get();
		flag = 0;
		mobile = list_h;
		while (mobile) {
		   if (mobile->startnum == r) {
			tcl.r1 = mobile->cellnumber;
			tcl.r = mobile->startnum;
			flag++;
		    }  
		    if (mobile->startnum == i) {
			tcl.r2 = int(real(mobile->MS_pos));
			tcl.i2 = int(imag(mobile->MS_pos));
			tcl.i = mobile->startnum;
			flag++;
		    }
		    if (flag == 2) break;
		    else mobile = mobile->next;
		}
		tcl.callTcl("interferenz");
		Interferenz.dataNew = FALSE;
	    }
	    if (Interferenz_bs.dataNew) {
		dummy = Interferenz_bs%0;
		int r, i, flag;
		r = int(real(dummy));
		i = int(imag(dummy));
		tcl.s = strength_bs.get();
		tcl.i = i;
		flag = 0;
		mobile = list_h;
		while (mobile) {
		    if (mobile->startnum == r) {
                        tcl.r1 = mobile->cellnumber;
			tcl.r = mobile->startnum;
			flag++;
		    }
		    if (flag) break;
		    else mobile = mobile->next;
		}
		tcl.callTcl("interferenz_bs");
		Interferenz_bs.dataNew = FALSE;
	    }
}

// prototype instance for known block list
static DEWiNeS_Tcl_Animation proto;
static RegisterBlock registerBlock(proto,"WiNeS_Tcl_Animation");
