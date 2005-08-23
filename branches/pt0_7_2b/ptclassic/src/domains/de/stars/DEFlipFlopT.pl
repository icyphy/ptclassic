defstar {
	name {FlipFlopT}
	domain {DE}
	author { John S. Davis, II }
	location { DE main library }
	desc { T Flip Flop Synchronous Sequential Circuit Component }
	input {
		name{T}
		type{float}
		desc{ T (set) Input }
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
		float tValue;
		float qState;
		float beginning;
	}
	setup {
		qState = float( InitQ );
		beginning = TRUE;
	}
	go {
		if( T.dataNew )
		{
		     tValue = float( T.get() );
		     if( tValue > 1 || tValue < 0 )
		     {
			  Error::abortRun(*this, "Non-Binary Input!");
		     }
		}
		
		
		switch( int( qState ) ) 
		{ 
		     case 0: 
			  if( tValue == 0 ) 
			  { 
			       qState = 0;
			  } 
			  else 
			  { 
			       qState = 1;
			  } 
			  break;
		     case 1: 
			  if( tValue == 1 ) 
			  { 
			       qState = 0; 
			  } 
			  else 
			  { 
			       qState = 1; 
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
			  outportQ->put(arrivalTime) << qState;
		     }
		     while ((outportQprime = nextpQprime++) != 0)
		     { 
			  outportQprime->put(arrivalTime) << 
				double( ( int(qState + 1.0) )%2 );
		     }
		}

	}

}





