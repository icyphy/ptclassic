defstar {
	name {FlipFlopD}
	domain {DE}
	author { John S. Davis, II }
	location { DE main library }
	desc { D Flip Flop Synchronous Sequential Circuit Component }
	input {
		name{D}
		type{int}
		desc{ D Input }
	}
	input {
		name{Clock}
		type{int}
		desc{ Clock Pulse }
	}
	outmulti {
		name{Q}
		type{int}
		desc{ Q Output }
	}
	outmulti {
		name{Qprime}
		type{int}
		desc{ Complement of Q Output }
	}
	defstate {
		name{InitQ}
		type{int}
		default{ "0" }
		desc{ Initial Value of Q Output }
	}
	defstate {
		name{PulseWidth}
		type{int}
		default{ "1" }
		desc{ Clock Pulse Interval 
		      This is based on a global parameter. }
	}
	private {
		int dValue;
		int qState;
		int beginning;
	}
	setup {
		qState = int( InitQ );
		beginning = TRUE;
	}
	go {
		if( D.dataNew )
		{
		     dValue = int( D.get() );
		     if( dValue > 1 || dValue < 0 )
		     {
			  Error::abortRun(*this, "Non-Binary Input!");
		     } 
		     
		     qState = dValue;
		}

		
		if( Clock.dataNew )
		{ 
		     Clock.get();
		     if( beginning )
		     {
			  beginning = FALSE;
			  completionTime = arrivalTime;
		     }
		     else
		     {
		          completionTime = arrivalTime + int( PulseWidth );
		     }

		     OutDEMPHIter nextpQ(Q), nextpQprime(Qprime); 
		     OutDEPort *outportQ, *outportQprime; 
		     while ((outportQ = nextpQ++) != 0)
		     { 
			  outportQ->put(completionTime) << qState;
		     }
		     while ((outportQprime = nextpQprime++) != 0)
		     { 
			  outportQprime->put(completionTime) << 
				(qState + 1)%2;
		     }
		}

	}

}





