defstar {
	name { SeqATMZero }
	domain { DE }
	author { GSWalter }
	version { $Id$ }
WARNING: CHECK COPYRIGHT - not UC
	copyright { 1992 (c) U. C. Regents }
	location { DE main palette }
	desc {
This star reads in a sequence of SeqATMCell objects.
For each object input correctly in sequence, 'headerLength'
bits are skipped over and the next 'numInfoBits' bits in the
cell are output.

If this star finds that a SeqATMCell is missing by checking sequence
numbers, it will substitute 'numInfoBits' 0-bits for the missing
bits.
	}


	input { name { input } type { message } }
	output { name { output } type { int } }
	defstate {
		name { numInfoBits }
		type { int }
		default { 384 }
		desc { number of information bits per cell }
	}
	defstate {
		name { headerLength }
		type { int }
		default { 40 }
		desc { Number of bits per cell header. }
	}

	hinclude { "SeqATMCell.h" }

	protected { int count; }

	setup { count = 0; }

	go {
		if ( input.dataNew ) {
			Envelope inPkt;
			input.get().getMessage( inPkt );
			TYPE_CHECK( inPkt, "SeqATMCell" );
			count %= 8;
			const SeqATMCell* voiceCell = ( const SeqATMCell* )
					inPkt.myData();

			// if packet missing, fill in with all 0s
			while ( ( voiceCell->readSeq() ) != count ) {
				for ( int i = 0; i < numInfoBits; ++i )
					output.put( arrivalTime ) << 0;
				count++;
				count %= 8;
			}

			// output the arrived packet's bits
			for ( int j = headerLength;
					j < int( headerLength + numInfoBits ); ++j ) {
				if ( voiceCell->isON( j ) )
					output.put( arrivalTime ) << 1;
				else
					output.put( arrivalTime ) << 0;
			}

			count++;
		} // end if
	} // end go
} // end defstar
