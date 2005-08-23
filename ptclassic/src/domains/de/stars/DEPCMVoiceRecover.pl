defstar {
	name { PCMVoiceRecover }
	domain { DE }
	author { GSWalter }
	version { @(#)DEPCMVoiceRecover.pl	1.14	06 Oct 1996 }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { DE main library }

	desc {
This star inputs a stream of
.c SeqATMCell
objects.  All of the
information bits in the objects received with correct sequence
numbers are sent to 'output'.

If a missing
.c SeqATMCell
object is detected, then this star sends the most
recent 8 * 'tempSize' received bits to the 'temp' output, and the
most recent (8 * 'searchWindowSize' + 'numInfoBits') received bits
to the 'window' output.

The bits output on the 'window' and 'temp' outputs can be used by
the 'PatternMatch' galaxy to implement lost-speech recovery.
	}

	htmldoc {
<p>
This star performs a specific function related to a
voice-packet interpolation technique for PCM encoded
data.  On its <i>input</i> port, this star reads in
<tt>SeqATMCell</tt>
types which have a field containing a packet sequence
number.  The sequence number is read and the star determines
whether or not a packet has been dropped during network
transmission.  If the missing packet is one of the
first five
<tt>SeqATMCell</tt>
s to be sent over the network, then this star will substitute
all zero bits for the bits which were lost during
transmission.  These bits are sent through the <i>output</i>
port.  If the dropped packet was not one of the first
five, a pattern matching technique is used to determine
the proper bits to substitute for those lost.  This is
performed as follows:
<p>
Say the packet six was found to be missing by this
star after it sees a
<tt>SeqATMCell</tt>
numbered seven immediately following a
<tt>SeqATMCell</tt>
numbered five.  This star will then output a string
of bits on its <i>temp</i> port of size <i>tempSize</i>
* 8.  These bits describe the first <i>tempSize</i>
samples immediately preceding the missing packet.  Likewise,
<i>searchWindowSize</i> * 8 + <i>numInfoBits</i> bits
are sent through the <i>window</i> port.  These bits describe
the
<tt>SeqATMCell</tt>
immediately preceding the lost packet as well as the
previous <i>searchWindowSize</i> samples before the lost packet.
In this context of this example of a missing sixth packet, this means
that the <i>temp</i> bits will come from the end of
packet five and the <i>window</i> bits will include
all of packets 5, 4, 3, 2, and a portion of the
end of packet 1 (note the restriction on <i>searchWindowSize</i>).
These bits are converted to samples in an SDF domain
wormhole and a cross-correlation is performed between the
<i>temp</i> and the <i>window</i> to determine the best
match, i.e., the one with largest cross-correlation value.  This SDF
galaxy will take the <i>numInfoBits</i> / 8 samples
from the <i>window</i> samples following the <i>window</i>'s
best match with the <i>temp</i> samples. These samples
are then encoded, loaded into a
<tt>SeqATMCell</tt>
and sent back to the DE domain where they enter the
<i>subIn</i> port of this star.  This star unloads
the bits from this
<tt>SeqATMCell</tt>
and outputs them in their proper order on the <i>output</i>
port.  If more than one packet was missing, this star
will repeat the derived packet enough times to fill the
void.
<h3>References</h3>
J. Goodman, G. LockHart, O. Wasem, and W. Wong,
"Waveform Substitution Techniques for Recovering
Missing Speech Segments in Packet Voice Communications,"
<i>IEEE Trans. on Acoustics, Speech, and Signal Processing</i>,
vol ASSP-34, no. 6, pp. 1440-1448, December 1986.
	}

	defstate {
		name { numInfoBits }
		type { int }
		default { 384 }
		desc { number of information bits per cell <= 384 }
	}
	defstate {
		name { searchWindowSize }
		type { int }
		default { 176 }
		desc { number of samples <= 4*numInfoBits/8
			and >= 3*numInfoBits/8 with which to
			search and retrieve match in SDF domain }
	}
	defstate {
		name { tempSize }
		type { int }
		default { 32 }
		desc { number of samples <= 48 which act as a guide
			in determining missing packet samples }
	}

	input { name { input } type { message } }
	input { name { subIn } type { message } }
	output { name { temp } type { int } }
	output { name { window } type { int } }
	output { name { output } type { int } }

	hinclude { "SeqATMCell.h" }

	protected {
		int startTemp, stopTemp, startPkt5, stopPkt5;
		int count, missingPkt;
		//pkt1 more recently arrived than pkt5
		SeqATMCell *pkt1, *pkt2, *pkt3, *pkt4, *pkt5, *currentPkt,
				*created;
	}

	constructor {
		pkt1 = pkt2 = pkt3 = pkt4 = pkt5 = currentPkt = created = 0;
	}

	setup {
		if ( numInfoBits > 384 )
			Error::abortRun( *this, "illegal SeqATMCell size" );
		if ( searchWindowSize > 4*int(numInfoBits)/8 ||
				searchWindowSize < 3*int(numInfoBits)/8 )
			Error::abortRun( *this, "illegal searchWindowSize" );
		if ( tempSize > 48 )
			Error::abortRun( *this, "illegal tempSize" );
		count = missingPkt = 0;
		int samples = numInfoBits / 8;
		startTemp = ( samples - tempSize + 5 ) * 8;
		stopTemp = ( samples + 5 ) * 8 - 1;
		int pkt5Samples = int( searchWindowSize ) - 3 *
				int( numInfoBits ) / 8;
		startPkt5 = ( samples - pkt5Samples + 5 ) * 8;
		stopPkt5 = ( samples + 5 ) * 8 - 1;
		pkt1 = pkt2 = pkt3 = pkt4 = pkt5 = currentPkt = created = 0;
		subIn.before( input );
	} // end start

	go {
		if ( input.dataNew ) {
			int i, j;
			Envelope inPkt;
			input.get().getMessage( inPkt );
			TYPE_CHECK( inPkt, "SeqATMCell" );
			currentPkt = ( SeqATMCell* ) inPkt.writableCopy();

			// if proper order, output packet
			if ( ( currentPkt->readSeq() ) == count ) {
				for ( i = 40; i < int ( 40 + numInfoBits ); i++ ) {
					if ( currentPkt->isON( i ) )
						output.put( arrivalTime ) << 1;
					else
						output.put( arrivalTime ) << 0;
				}
				count++;
				count %= 8;
				if ( pkt5 != pkt4 ) {
					LOG_DEL; delete pkt5;
				}
				pkt5 = pkt4;
				pkt4 = pkt3;
				pkt3 = pkt2;
				pkt2 = pkt1;
				pkt1 = currentPkt;
			} // end proper order

			// else if a packet is missing
			else {
				while ( pkt5 == 0 && count !=
						( currentPkt->readSeq() ) ) {
					for ( i = 0; i < int( numInfoBits ); i++ )
						output.put( arrivalTime ) << 0;
					count++;
					count %= 8;
					if ( pkt5 != pkt4 ) {
						delete pkt5;
					}
					pkt5 = pkt4;
					pkt4 = pkt3;
					pkt3 = pkt2;
					pkt2 = pkt1;
					pkt1 = 0;
					LOG_NEW; pkt1 = new SeqATMCell;
				} // end while

				// at this point there may still be missing packets
				// 1) either we were never at the beginning of
				// transmission (pkt5 != 0) or 2) pkt5 became
				// nonzero in the above loop;
				// now calculate how many are still missing
				while ( ( currentPkt->readSeq() ) != count ) {
					missingPkt++;
					count++;
					count %= 8;
				}

				// if not missing any more, output currentPkt
				if ( !missingPkt ) {
					for ( i = 40; i < int ( 40 + numInfoBits ); i++ ) {
						if ( currentPkt->isON( i ) )
							output.put( arrivalTime ) << 1;
						else
							output.put( arrivalTime ) << 0;
					}
					count++;
					count %= 8;
					if ( pkt5 != pkt4 ) {
						LOG_DEL; delete pkt5;
					}
					pkt5 = pkt4;
					pkt4 = pkt3;
					pkt3 = pkt2;
					pkt2 = pkt1;
					pkt1 = currentPkt;
				} // end if not missing

				else if ( missingPkt ) {
					// output bits to do pattern matching
					// first output temp bits;
					for ( j = startTemp; j <= stopTemp; j++ ) {
						if ( pkt1->isON( j ) )
							temp.put( arrivalTime ) << 1;
						else
							temp.put( arrivalTime ) << 0;
					}
					// now output search window
					for ( j = startPkt5; j <= stopPkt5; j++ ) {
						if ( pkt5->isON( j ) )
							window.put( arrivalTime ) << 1;
						else
							window.put( arrivalTime ) << 0;
					}
					for ( j = 40; j < numInfoBits + 40; j++ ) {
						if ( pkt4->isON( j ) )
							window.put( arrivalTime ) << 1;
						else
							window.put( arrivalTime ) << 0;
					}
					for ( j = 40; j < numInfoBits + 40; j++ ) {
						if ( pkt3->isON( j ) )
							window.put( arrivalTime ) << 1;
						else
							window.put( arrivalTime ) << 0;
					}
					for ( j = 40; j < numInfoBits + 40; j++ ) {
						if ( pkt2->isON( j ) )
							window.put( arrivalTime ) << 1;
						else
							window.put( arrivalTime ) << 0;
					}
					for ( j = 40; j < numInfoBits + 40; j++ ) {
						if ( pkt1->isON( j ) )
							window.put( arrivalTime ) << 1;
						else
							window.put( arrivalTime ) << 0;
					}
				} // end else if missingPkt
			} // end else if missing packets
		} // end if input.dataNew

		if ( subIn.dataNew ) {
			int k, n;
			Envelope subPkt;
			subIn.get().getMessage( subPkt );
			TYPE_CHECK( subPkt, "SeqATMCell" );
			created = ( SeqATMCell* ) subPkt.writableCopy();
			// output packet missingPkt times
			for ( n = 1; n <= missingPkt; n++ ) {
				for ( k = 40; k < int ( 40 + numInfoBits ); k++ ) {
					if ( created->isON( k ) )
						output.put( arrivalTime ) << 1;
					else
						output.put( arrivalTime ) << 0;
				}
				if ( pkt5 != pkt4 ) {
					LOG_DEL; delete pkt5;
				}
				pkt5 = pkt4;
				pkt4 = pkt3;
				pkt3 = pkt2;
				pkt2 = pkt1;
				pkt1 = created;
			}

			// output current packet
			for ( k = 40; k < int ( 40 + numInfoBits ); k++ ) {
				if ( currentPkt->isON( k ) )
					output.put( arrivalTime ) << 1;
				else
					output.put( arrivalTime ) << 0;
			}

			count++;
			count %= 8;
			missingPkt = 0;
			if ( pkt5 != pkt4 ) {
				LOG_DEL; delete pkt5;
			}
			pkt5 = pkt4;
			pkt4 = pkt3;
			pkt3 = pkt2;
			pkt2 = pkt1;
			pkt1 = currentPkt;
		} // end if subIn.dataNew
	} // end go

	destructor {
		LOG_DEL; delete pkt5;
		LOG_DEL; delete pkt4;
		LOG_DEL; delete pkt3;
		LOG_DEL; delete pkt2;
		LOG_DEL; delete pkt1;
		LOG_DEL; delete currentPkt;
		LOG_DEL; delete created;
	}
} // end defstar
