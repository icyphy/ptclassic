defstar {
	name { ZeroSubstitution }
	domain { DE }
	author { GSWalter }
	version { $Id$ }
	copyright { 1992 (c) U. C. Regents }
	location { DE main palette }
	desc {
This star reads in a sequence of BitArray types. If
it finds a BitArray missing by checking sequence
numbers, it will substitute all 0-bits for the missing
bits. All bits are unloaded from their BitArray
before exiting the output port.
	}

	input { name { input } type { message } }
	output { name { output } type { int } }
	defstate {
		name { numInfoBits }
		type { int }
		default { 384 }
		desc { number of information bits per cell }
	}

	hinclude { "BitArray.h" }

	protected { int count; }

	start {
		if ( numInfoBits > 384 )
			Error::abortRun( *this,
				"numInfoBits must be <= 384" );
		count = 0;
	}

	go {
		if ( input.dataNew ) {
			Envelope inPkt;
			input.get().getMessage( inPkt );
			TYPE_CHECK( inPkt, "BitArray" );
			count %= 8;
			const BitArray* voiceCell = ( const BitArray* )
					inPkt.myData();

			// if packet missing, fill in with all 0s
			while ( ( voiceCell->readSeq() ) != count ) {
				for ( int i = 0; i < numInfoBits; ++i )
					output.put( arrivalTime ) << 0;
				count++;
				count %= 8;
			}

			// output the arrived packet's bits
			for ( int j = 40; j < int( 40 + numInfoBits ); ++j ) {
				if ( voiceCell->isON( j ) )
					output.put( arrivalTime ) << 1;
				else
					output.put( arrivalTime ) << 0;
			}

			count++;
		} // end if
	} // end go
} // end defstar
