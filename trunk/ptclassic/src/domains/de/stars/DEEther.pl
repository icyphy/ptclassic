defstar {
	name {Ether}
	domain {DE}
	derivedfrom { RepeatStar }
	version { $Id$ }
	author { E. A. Lee  and T. Parks }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This is the base class for transmitter and receiver
stars that communicate over a shared medium.
Each transmitter can communicate with any or all receivers
that have the same value for the "medium" parameter.
The communication is accomplished without graphical connections,
and the communication topology can be continually changing.
This base class implements the data structures that are shared
between the transmitters and receivers.
	}
	explanation {
This star, and its derivatives
.c EtherRec
and
.c EtherSend,
demonstrate the use of shared data structures
.Ie "shared data structures"
to simulate one or more shared physical media.
Each instance of the star specifies the medium it uses by name;
all stars that use that name will share that medium.
Any ASCII string can be used as a name.
.pp
The
.c EtherSend
star is given a Particle, an address, and a duration.
It checks to see whether the medium is currently occupied by another
transmitter using the same medium.  If not, it occupies
the medium for the specified duration, and delivers a copy of
the particle to a receiver that matches the specified address.
The
.c EtherRec
star, after the "duration" time has elapsed, outputs the particle.
In this implementation, receiver addresses are arbitrary non-negative integers.
They need not be contiguous, but they must be unique.
The implementation could easily be extended to permit more
than one receiver to have the same address.
If a negative integer is specified as the address at the transmitter,
then the transmitter broadcasts a copy of the data particle to
all receivers.
.pp
When the transmitter receives a new data particle, the medium
could be occupied (by the same transmitter or a different one using
the same medium).
If this is the case, then the data particle is not transmitted to
any receiver, but instead is sent out through the "collision"
output port.
This is a pretty simple-minded implementation, and may not be rich
enough to implement practical collision detection protocols.
So it should be viewed as a concept demonstration only.
.pp
The way these stars work is to define, in the base class
.c DEEther,
a static hash table that contains an entry for each medium.
By making this hash table static, it is made available to
all derived stars.
Each entry in this table contains another hash table
with an entry for each receiver that uses the medium.
The index into the hash table is the receiver address.
In this implementation, only one receiver can occupy
a given address, but it would be easy to change the hash table
entry to a
.c SequentialList
of receivers.
This would allow any number of receivers to share the same address.
.pp
The receiver addresses are interpreted as strings.  Thus,
anything can be used.  The particular implementation of
the derived star
.c EtherSend
assumes the receivers will have non-negative integers for addresses.
But another implementation might use arbitrary ASCII strings.
.pp
The medium is reset in the
.c wrapup() method of the star.
.Ie wrapup
Note that this does not get invoked if an error occurs during the run.
Thus, if an error occurs, the star should be destroyed
and recreated.  Pigi takes care of this automatically,
but if the star is run under ptcl, then it is up to the
user to do this.
	}
	defstate {
		name {medium}
		type {string}
		default {"radio900MHz"}
		desc { The name of the medium to share. }
	}
	// The following goes into the header file
	header {

	    class DEEtherRec;

	    // Define a class that represents a particular medium
	    // and keeps track of whether it is occupied.
	    class Medium {
	    public:
		Medium() : starCount(0), receiversInitialized(0) {}

		// Count the number of stars using this medium
		int starCount;

		// Flag indicating whether the receiver list has been init'd
		int receiversInitialized;

		int occupiedAt(double curTime) { return (curTime < freeTime); }
		void occupyUntil(double time) { freeTime = time; }
		void initialize() { receivers.clear(); }

		// Register a new receiver for the medium.  If a receiver
		// with the specified name was already registered, then
		// this new receiver will be ignored.
		int registerReceiver(const char* recName, DEEtherRec* rec);

		// Return a pointer to a receiver with a specified name.
		// If no such receiver has been registered, return 0.
		DEEtherRec* findReceiver(const char* recName);

		HashTable receivers;
	    private:
		double freeTime;
	    };
	}
	code {
	    int Medium::registerReceiver(const char* recName, DEEtherRec* rec)
	    {
		// For now, allow only one receiver with each unique name.
		// May want to allow multiple receivers with the same name.
		if (receivers.hasKey(recName)) {
		    return FALSE;
		}
		receivers.insert(recName, rec);
		return TRUE;
	    }
	    DEEtherRec* Medium::findReceiver(const char* recName) {
		return (DEEtherRec*)(receivers.lookup(recName));
	    }

	    // Instantiate the static members of the DEEther star
	    HashTable DEEther::medList;
	}
	hinclude { "HashTable.h" }
	protected {
	    // List of media, indexed by name
	    static HashTable medList;
	    Medium* med;
	}
	method {
	    name { registerReceiver }
	    access { protected }
	    arglist { "(const char* name, DEEtherRec* receiver)" }
	    type { int }
	    code {
		if (med) {
		    return med->registerReceiver(name, receiver);
		} else {
		    Error::abortRun(*this,"No medium!");
		    return 0;
		}
	    }
	}
	begin {
	    if (medList.hasKey(medium)) {
		// Medium already exists
		med = (Medium*)medList.lookup(medium);
	    } else {
		// Need to create a new medium
		LOG_NEW; med = new Medium;
		medList.insert(medium, med);
	    }
	    (med->starCount)++;

	    if(!(med->receiversInitialized)) {
		med->initialize();
		med->occupyUntil(0.0);
		med->receiversInitialized = 1;
	    }
	}
	setup {
	    DERepeatStar :: setup ();

	    // This initialization is required by the base class RepeatStar
	    completionTime = 0;
	}
	wrapup {
	    // Indicate that receivers should be reinitialized on the
	    // next run.
	    // Note that this does not get invoked if an error occurs.
	    // Thus, if an error occurs, the star should be destroyed
	    // and recreated.  Pigi takes care of this automatically,
	    // but if the star is run under ptcl, then it is up to the
	    // user to do this.
	    med->receiversInitialized = 0;
	}
	destructor {
	    if (med) {
		med->starCount--;
		if (med->starCount == 0) {
		   medList.remove(medium);
		   LOG_DEL; delete med;
		}
	    }
	}
}
