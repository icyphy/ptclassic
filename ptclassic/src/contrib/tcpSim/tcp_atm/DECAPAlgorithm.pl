 defstar {
 	name {CAPAlgorithm }
	domain { DE }
	author { Dorgham Sisalem}
        copyright { SEE $PTOLEMY_SIMULATIONS/copyright }

	desc { This star simulates the switch behavior of the CAPC algorithm
	       as was specified by Barnhart in ATM-Forum94-0983}

	input { name { IN_RM}
                type { message}
	        desc { incoming RM Cells}
	}

	inmulti { name { RET_RM}
                type { message}
	        desc { Returning RM Cells}
	}

	input { name { Congestion}
                type { int}
	        desc { indicate the congestion state}
	}

	output { name { OUT_RM}
                 type { message}
	         desc { Send RM Cells to the Destination}
        }

	output { name { NEW_RM}
                 type { message}
	         desc { updated RM Cells}
        }

	defstate {
		name { PCR}
		type { float}
		default {0 }
		desc { Peak Cell Rate}
	}
	defstate {
		name { target_rate}
		type { float}
		default {0 }
	}

	defstate {
		name { Rup}
		type { float}
		default {0.05 }
		desc { Initial value for the slope}
	}

	defstate {
		name { ERU}
		type { float}
		default { 1.5}
		desc { maximum increase allowed}
	}

	defstate {
		name { Rdn}
		type { float}
		default { 0.5}
		desc { slope parameter}
	}
	defstate {
		name { ERF}
		type { float}
		default { 0.5}
		desc { slope parameter}
	}

	defstate {
		name { DQT}
		type { int}
		default { 200}
		desc { High Queue Limit}
	}

	defstate {
		name { max_count}
		type { int}
		default { 100}
		desc { counting interval}
	}
 	defstate {
		name { interval}
		type { float}
		default { 1}
		desc { timing interval}
	}


	defstate {
		name { mode}
		type { int}
		default {0 }
		desc { 0=Explicit,1=Binary}
	}
 
        defstate {
                name { RM_Proc}
                type { intarray}
        }
        defstate {
                name { debug}
                type { int}
                default {0 }
        }
 

	hinclude { "ATMCell.h" ,<math.h>,<minmax.h>}
	defstate {
		name { fileName }
		type { string }
		default { "<stdout>" }
		desc { Filename for output }
	}
	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream *p_out;
	}

	protected { 
			int congested;
			float ACR,MACR;
			float start_time;
			int count;
			float ERS;
		  }

//*****************************************************************************/
// initialization: 
//*****************************************************************************/

	setup { 
		congested=0;
		count=0;
		start_time=0;
		ERS=target_rate/(float)10;
              }

//*****************************************************************************/
//*****************************************************************************/

	begin  {  
                LOG_DEL; delete p_out;
		LOG_NEW; p_out = new pt_ofstream(fileName);
		}

//****************************************************************************/
// set scheduling priorities
//****************************************************************************/

	constructor { 
                      p_out = 0;
               	    }
        wrapup {
	        LOG_DEL; delete p_out;
		p_out = 0;
		}
	destructor { LOG_DEL; delete p_out;}

	go {
                pt_ofstream& OUTPUT = *p_out;
                completionTime= arrivalTime;

		InDEMPHIter inIter(RET_RM);
		InDEPort* inPtr;
		if(Congestion.dataNew)
		{
			Congestion.dataNew=FALSE;
			float cong=Congestion%0;
			congested= (int) cong;
		} //end Congestion

		if(IN_RM.dataNew)
	        {
			Envelope Envp;
	                IN_RM.get().getMessage(Envp);
       	            	ATMCell* cellPtr=(ATMCell *)
                    	  	Envp.myData();
                  	if(cellPtr->readRM()==0)
			{
				if((++count>=max_count)||
					(completionTime>start_time+interval))
				{
					float rate=(float)count/(float)
					   (completionTime-(float)start_time);
					float delta=(float)1-((float)rate/
							 (float)target_rate);
					float ERX;

					if(delta>=0)
					{
						ERX=(float)1+((float)delta*
							      (float)Rup);
						ERX=min((float)ERX,(float)ERU);
					}
					else
					{
						ERX=(float)1+((float)delta*
							      (float)Rdn);
						ERX=max((float)ERX,(float)ERF);
					}
					ERS=ERS*ERX;
					count=0;
					start_time=completionTime;
				}
			}
			OUT_RM.put(completionTime)<<Envp;
		}

//******************************************************************
		while ((inPtr = inIter++) != 0) 
		{
		     while (inPtr->dataNew) 
		     {
			Envelope Envp;
			inPtr->get().getMessage(Envp);
       	            	ATMCell* cellPtr=(ATMCell *)
                    	  	Envp.myData();
			int VP=cellPtr->readDestination();
			int VC=cellPtr->readVC();
		       if((VP!=RM_Proc[0])&&(VP!=RM_Proc[1])&&(VP!=RM_Proc[2])
			  &&(VP!=RM_Proc[3]))
		       {	           
                            Envelope envp( *cellPtr);
			    NEW_RM.put(completionTime)<<envp;
		            return;
			}
			float ACR=cellPtr->readACR();
                        LOG_NEW;
                        ATMCell* RM_Cell = new ATMCell(0);
                        RM_Cell->setRM(1);
                        RM_Cell->setSource(cellPtr->readSource());
                        RM_Cell->setVC(cellPtr->readVC());
                        RM_Cell->setDestination(cellPtr->readDestination());
                        RM_Cell->setDIR(0);
                        RM_Cell->setACR(ACR);
                        RM_Cell->setCI(cellPtr->readCI());
                        RM_Cell->setER(cellPtr->readER());
                        RM_Cell->setCI(cellPtr->readCI());
if(debug)
OUTPUT<<" cong "<<congested<<" ER "<<cellPtr->readER()<<" ERS "<<ERS<<" at "<<completionTime<<"\n";
			if(cellPtr->readER()>ERS)
				RM_Cell->setER(ERS);
			if(congested>DQT)
				RM_Cell->setCI(1);
if(debug)
OUTPUT<<" VP "<<VP<<" VC "<<VC<<" ER "<<RM_Cell->readER()<<" CI "<<RM_Cell->readCI()<<" at "<<completionTime<<"\n";
                        Envelope envp( *RM_Cell);
			NEW_RM.put(completionTime)<<envp;
		    }
		}// end RET_RM
	   } // end go
} // end defstar
