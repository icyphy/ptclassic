 defstar {
 	name {CountX10 }
	domain { DE }
	author { Dorgham Sisalem}
        copyright { SEE $PTOLEMY_SIMULATIONS/copyright }

	desc {
	}

	input { name {count }
                type { int}
	        desc { }
	}

	output { name { atmcount}
                 type { =count}
	         desc { }
        }


	hinclude { "TCPPacket.h" ,<math.h>}

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
		  }

//*****************************************************************************/
// initialization: 
//*****************************************************************************/

	setup { 
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
		if(count.dataNew)
		{
                        int size=count%0;
			count.dataNew=FALSE;
			size=10*(int)size;
//OUTPUT<<" SIZE*10 "<<size<<"\n";
			atmcount.put(completionTime)=&size;
		}

	   } // end go
} // end defstar
