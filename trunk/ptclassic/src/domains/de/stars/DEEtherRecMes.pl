defstar {
	name { EtherRecMes }
	domain { DE }
	derivedfrom { EtherRec }
	version { $Id$ }
	author { Edward A. Lee and Tom M. Parks }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This star receives message particles transmitted to it by
an EtherSend star.  The particle is produced at the output after
some duration of transmission specified at the transmitter.
	}
	explanation {
See the explanation for the DEEtherRec stars.
The only difference is that this stars forces the output to be a message.
	}
	ccinclude { "Message.h" }
	constructor {
recData.setPort("recData", this, MESSAGE);
	}
}
