defstar {
	name { SeqATMSub }
	domain { DE }
	author { GSWalter }
	derivedFrom { SeqATMZero }
	version { $Id$ }
        copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { DE main palette }
	desc {
This star reads in a sequence of SeqATMCells. It will
check sequence numbers and if a SeqATMCell is found
missing, the information bits of the previously arrived
SeqATMCell will be output in its place.

The information bits from each correctly received
SeqATMCell are unloaded and sent to the output port.
	}

	protected { int* reserve; }

	setup {
		LOG_NEW; reserve = new int[ int( numInfoBits ) ];
		for ( int i = 0; i < numInfoBits; i++ )
			reserve[ i ] = 0;
		DESeqATMZero::setup();
	}

	go {
		if ( input.dataNew ) {
			Envelope inPkt;
			input.get().getMessage( inPkt );
			TYPE_CHECK( inPkt, "SeqATMCell" );
			count %= 8;
			const SeqATMCell* voiceCell = ( const SeqATMCell* )
					inPkt.myData();

			// if packet missing, output previous packet
			while( ( voiceCell->readSeq() ) != count ) {
				for ( int i = 0; i < numInfoBits; i++ )
					output.put( arrivalTime ) << reserve[ i ];
				count++;
				count %= 8;
			}

			// output current packet and store in reserver
			for ( int j = headerLength;
					j < int( headerLength + numInfoBits ); j++ ) {
				if ( voiceCell->isON( j ) ) {
					output.put( arrivalTime ) << 1;
					reserve[ j - headerLength ] = 1;
				}
				else {
					output.put( arrivalTime ) << 0;
					reserve[ j - headerLength ] = 0;
				} // end else
			} // end for

			// increment count to test next packet sequence number
			count++;
		} // end if
	} // end go

	destructor { LOG_DEL; delete [] reserve; }

} // end defstar
