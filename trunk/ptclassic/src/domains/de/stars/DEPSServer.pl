defstar {
	name {PSServer}
	derivedFrom { RepeatStar }
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc { Emulates a processor-sharing server.  }
	explanation {
This star emulates a deterministic, processor-sharing server.
If input events arrive when it is not busy,
it delays them by the nominal service time.
If they arrive when it is busy, the server is shared.
Hence prior arrivals that are still in service
will be delayed by more than the nominal service time.
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {=input}
	}
	defstate {
		name {nomServiceTime}
		type {float}
		default {"1.0"}
		desc { "Nominal service time" }
	}
	code {
		struct token {
		   Particle* pp;
		   float serviceNeeded;
		   float lastUpdate;
		    token() : pp(0) {}
		   ~token() { if (pp) pp->die();}
		};
	}
	protected {
		// structure to store tokens in service
		SingleLinkList tokensInService;
		int numberInService;
	}
	constructor {
		delayType = TRUE;
	}
	start {
		numberInService = 0;
		tokensInService.initialize();
	}
	wrapup {
		// remove any remaining tokens.
		while (numberInService > 0) {
			token* t = (token*)tokensInService.getAndRemove();
			LOG_DEL; delete t;
			numberInService--;
		}
	}
	destructor {
		wrapup();
	}
	go {
	   token* t;

	   completionTime = arrivalTime;

	   // Check to see whether the serviceNeeded of the first token
	   // in the list goes to zero, and if so, output the particle.
	   // Keep trying as long as the first particle in the list is ready
	   // to be output.
	   int outputP = FALSE;
	   while(numberInService > 0) {
	      t = (token*)(tokensInService.getNotRemove());
	      if (t->serviceNeeded
	              <= (arrivalTime - t->lastUpdate)/numberInService) {
		// This token is ready to be output
		output.put(completionTime) = *(t->pp);
		tokensInService.getAndRemove();
		LOG_DEL; delete t;
		numberInService--;
		outputP = TRUE;
	      } else {
		break;
	      }
	   }

	   // Check for false alarm, and return if nothing has changed
	   if((!input.dataNew) & (!outputP)) return;

	   // Modify the serviceNeeded for all tokensInService.
	   // I sincerely apologize for this cast!
	   ListIter nextTok(*(SequentialList*)&tokensInService);
	   for(int i = numberInService; i > 0; i--) {
		t = (token*) nextTok++;
		t->serviceNeeded = t->serviceNeeded -
			(arrivalTime - t->lastUpdate)/numberInService;
		t->lastUpdate = arrivalTime;
		// Schedule a refiring.
		// Need to account for number that will be in
		// service in the next interval.
		int nISNext = numberInService;
		if ( input.dataNew ) nISNext++;
		refireAtTime(arrivalTime
				+ (t->serviceNeeded)*nISNext);

	   }

	   // Now check for new input data
	   if ( input.dataNew ) {
		// Create a token.
		Particle& pp = input.get();
		Particle* newp = pp.clone();
		LOG_NEW; t = new token;
		t->pp = newp;
		t->serviceNeeded = double(nomServiceTime);
		t->lastUpdate = arrivalTime;

		// Append new token to the list of tokensInService
		tokensInService.append(t);
		numberInService += 1;

		// Schedule a firing at the current estimate of
		// the completion time of the service.
		refireAtTime(arrivalTime + (t->serviceNeeded)*numberInService);

	   }

	}
}
