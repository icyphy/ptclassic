// $Id$
// Joseph T. Buck
// Created 6/6/90
//
// This is a mechanism to allow the same interpreter code to support
// multiple schedulers; only the methods for this class need to be
// changed.

// Eventually, this class will allow the scheduler to support multiple
// domains at once, which is why these are functions instead of data
// members.

class Scheduler;

class Domain {
public:
	static int set(const char*);
	static Scheduler* newSched();
	static const char* universeType();
	static const char* domainName();
};


