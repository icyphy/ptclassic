defstar {
	name { PrevPacketSub }
	domain { DE }
	author { GSWalter }
	derivedFrom { ZeroSubstitution }
	version { $Id$ }
	copyright { 1992 (c) U. C. Regents }
	location { DE main palette }
	desc {
This star reads in a sequence of BitArrays. It will
check sequence numbers and if a BitArray is found
missing, the bits of the previously arrived BitArray
will be used in its place. The bits from each
BitArray are unloaded before being sent through
the output port.
	}

	protected { int* reserve; }

	start {
		LOG_NEW; reserve = new int[ int( numInfoBits ) ];
		for ( int i = 0; i < numInfoBits; i++ )
			reserve[ i ] = 0;
		DEZeroSubstitution :: setup();
	}

	go {
		if ( input.dataNew ) {
			Envelope inPkt;
			input.get().getMessage( inPkt );
			TYPE_CHECK( inPkt, "BitArray" );
			count %= 8;
			const BitArray* voiceCell = ( const BitArray* )
					inPkt.myData();

			// if packet missing, output previous packet
			while( ( voiceCell->readSeq() ) != count ) {
				for ( int i = 0; i < numInfoBits; i++ )
					output.put( arrivalTime ) << reserve[ i ];
				count++;
				count %= 8;
			}

			// output current packet and store in reserver
			for ( int j = 40; j < int( 40 + numInfoBits ); j++ ) {
				if ( voiceCell->isON( j ) ) {
					output.put( arrivalTime ) << 1;
					reserve[ j - 40 ] = 1;
				}
				else {
					output.put( arrivalTime ) << 0;
					reserve[ j - 40 ] = 0;
				} // end else
			} // end for

			// increment count to test next packet sequence number
			count++;
		} // end if
	} // end go

	destructor { LOG_DEL; delete [] reserve; }

} // end defstar
