defstar {
	name {PSServer}
	derivedFrom { RepeatStar }
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This star emulates a deterministic, processor-sharing server.
If input events arrive when it is not busy, it delays them by the nominal service time.
If they arrive when it is busy, the server is shared. Hence prior arrivals that are
still in service will be delayed by more than the nominal service time.
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
		   double serviceNeeded;
		   double lastUpdate;
		    token() : pp(0) {}
		   ~token() { if (pp) pp->die();}
		};
	}
	protected {
		// structure to store tokens in service
		SequentialList tokensInService;
		int numberInService;
	}
	constructor {
		// input does not trigger any zero-delay output events;
		// but feedback port does, so don't set delayType.
		input.triggers();
		numberInService = 0;
	}
	setup {
		// Remove any remaining tokens
		while (numberInService > 0) {
			token* t = (token*)tokensInService.getAndRemove();
			LOG_DEL; delete t;
			numberInService--;
		}
		tokensInService.initialize();
	}
	destructor {
		// Remove any remaining tokens
		while (numberInService > 0) {
			token* t = (token*)tokensInService.getAndRemove();
			LOG_DEL; delete t;
			numberInService--;
		}
	}
	go {
	   completionTime = arrivalTime;

	   // Check to see whether the serviceNeeded of the first token
	   // in the list goes to zero, and if so, output the particle.
	   // Keep trying as long as the first particle in the list is ready
	   // to be output.
	   int outputP = FALSE;
	   // Number of tokens in service since last update : used to
	   // determine service time required for a token. Must remain
	   // constant until the end of the 'while' loop.
	   int lastUpdatedNumberInService = numberInService;

	   while(numberInService > 0) {
	      token *t = (token*)(tokensInService.head());
	      if (t->serviceNeeded
	              <= (arrivalTime - t->lastUpdate)/lastUpdatedNumberInService) {
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
	   ListIter nextTok(tokensInService);
	   for(int i = numberInService; i > 0; i--) {
		token *t = (token*) nextTok++;
		t->serviceNeeded = t->serviceNeeded -
			(arrivalTime - t->lastUpdate)/lastUpdatedNumberInService;
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
		LOG_NEW; token *t = new token;
		t->pp = newp;
		t->serviceNeeded = double(nomServiceTime);
		t->lastUpdate = arrivalTime;

		// Append new token to the list of tokensInService
		tokensInService.append(t);
		numberInService++;

		// Schedule a firing at the current estimate of
		// the completion time of the service.
		refireAtTime(arrivalTime + (t->serviceNeeded)*numberInService);
	   }

	}
}
