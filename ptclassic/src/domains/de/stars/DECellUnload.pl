defstar {
	name { CellUnload }
	domain { DE }
	author { GSWalter }
	version { $Id$ }
	copyright { 1992 (c) U. C. Regents }
	location { DE main palette }
	desc {
Removes Message from NetworkCell in DE domain.
	}

	input { name { input } type { message } }
	output { name { output } type { message } }

	hinclude { "NetworkCell.h" }

	go {
		if ( input.dataNew ) {
			Envelope inEnvlp;
			input.get().getMessage( inEnvlp );
			TYPE_CHECK( inEnvlp, "NetworkCell" );
			const NetworkCell* cellPtr = ( const NetworkCell* )
							inEnvlp.myData();
			const Message* outMssg = ( const Message* )
							( cellPtr->data() );
			Envelope outEnvlp( *outMssg );
			output.put( arrivalTime ) << outEnvlp;
		} // end if
	} // end go
} // end defstar
