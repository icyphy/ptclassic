defstar {
	name { WiNeS_Tcl_Animation }
	domain { DE }
	version { @(#)DEWiNeS_Tcl_Animation.pl	1.3 06/24/97 }
	author { J. Voigt }
	copyright {
            Copyright (c) 1997 Dresden University of Technology,
            Mobile Communications Systems
	}
	location { The WiNeS Library }
	desc { This is an tcl-animation star. A tcl-script is read in and data 
        containing positions of mobile stations, information about connections 
        to the base stations as well as information about existing interferences
        come to this star from CIR. Each time anything happens in the network 
        the animation will depict it. All mobile stations are shown at their 
        position, all connections are denoted by yellow arrows. Orange arrows 
        show interferences which are caused by other mobile stations, while red
        arrows show interferences which are caused by transmissions from other 
        base stations. All interferences are measured in the uplink, so the 
        receiving station is always the base station. 
        This animation can be used for debugging and demonstration reasons. 
        The most interesting and generic part of this star might be the code
        which I use to read in data from a specific input instead of an input 
        in a MultiPortHole as it is in the TclScript-star in the 
        Ptolemy-library.
        }
	explanation { My TclStarIfcWiNeS class contains some methods which 
        I just copied from Edward A. Lee's TclStarIfc. 
        }
	hinclude { "TclStarIfc.h", "ptkTclCommands.h",<complex.h>,
	"DErrmanager_uldl.h" }
	ccinclude { "ptk.h" }
	input {
            name { New_ }
            type { Complex }
            desc { a new handy's position }
	}
        input {
            name { Delete_ }
            type { int }
            desc { a dieing handy's reference number }
        }
	input {
            name { strength }
            type { int }
            desc { an interference' strength }
        }
	input {
            name { Interferenz }
            type { Complex }
            desc { interfering handy's numbers }
        }
	input {
            name { strength_bs }
            type { int }
            desc { an interference' strength }
        }
	input {
            name { Interferenz_bs }
            type { Complex }
            desc { interfering handy's numbers }
        }
	defstate {
            name { tcl_file }
            type { string }
            default { "$PTOLEMY/src/domains/de/contrib/stars/test.tcl" }
            desc { The file from which to read the Tcl script }
	}
	defstate {
            name { Segments }
            type { int }
            default { segments }
            desc { 0 = omnidiectional antennas }
	}
	header {
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
        }
	code {  
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
        } 
        protected {
	    TclStarIfcWiNeS tcl;
        }
	private {
           
	    Handy *mobile;
	    Complex dummy;
	}
	constructor {
	    mobile = NULL;
	}
	begin {
	    tcl.segmente = Segments;
            tcl.mySetup( this, (const char*) tcl_file );
        }
	go {
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
	wrapup {
            tcl.wrapup();
        }
}
