defstar {
	name {FlipFlopJK}
	domain {DE}
	author { John S. Davis, II }
	location { DE main library }
	desc { JK Flip Flop Synchronous Sequential Circuit Component }
	input {
		name{J}
		type{float}
		desc{ J (set) Input }
	}
	input {
		name{K}
		type{float}
		desc{ K (clear) Input }
	}
	input {
		name{Clock}
		type{float}
		desc{ Clock Pulse }
	}
	outmulti {
		name{Q}
		type{float}
		desc{ Q Output }
	}
	outmulti {
		name{Qprime}
		type{float}
		desc{ Complement of Q Output }
	}
	defstate {
		name{InitQ}
		type{float}
		default{ "0" }
		desc{ Initial Value of Q Output }
	}
	defstate {
		name{PulseWidth}
		type{float}
		default{ "1" }
		desc{ Clock Pulse Interval 
		      This is based on a global parameter. }
	}
	private {
		float jValue, kValue;
		float qState;
		float beginning;
	}
	setup {
		qState = float( InitQ );
		beginning = TRUE;
	}
	go {
		if( J.dataNew )
		{
		     jValue = float( J.get() );
		     if( jValue > 1 || jValue < 0 )
		     {
			  Error::abortRun(*this, "Non-Binary Input!");
		     }
		}
		if( K.dataNew )
		{
		     kValue = float( K.get() );
		     if( kValue > 1 || kValue < 0 )
		     {
			  Error::abortRun(*this, "Non-Binary Input!");
		     }
		} 
		
		
		switch( int(qState) ) 
		{ 
		     case 0: 
			  if( jValue == 0.0 ) 
			  { 
			       qState = 0.0;
			  } 
			  else 
			  { 
			       qState = 1.0;
			  } 
			  break;
		     case 1: 
			  if( kValue == 1.0 ) 
			  { 
			       qState = 0.0; 
			  } 
			  else 
			  { 
			       qState = 1.0; 
			  } 
			  break;
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
		          completionTime = arrivalTime;
		          // completionTime = arrivalTime + float( PulseWidth );
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
				double( ( int(qState + 1.0) )%2 );
		     }
		}

	}

}





