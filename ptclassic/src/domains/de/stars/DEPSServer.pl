ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/21/90

 This star emulates a deterministic, processor-sharing server.
 If input events arrive when it is not busy,
 it delays them by the nominal service time.
 If they arrive when it is busy, the server is shared.
 Hence prior arrivals that are still in service
 will be delayed by more than the nominal service time.

**************************************************************************/
}
defstar {
	name {PSServer}
	derivedFrom { RepeatStar }
	domain {DE}
	desc {
	   "This star emulates a processor-sharing server"
	}
	input {
		name {input}
		type {anytype}
	}
	output {
		name {output}
		type {anytype}
	}
	defstate {
		name {nomServiceTime}
		type {float}
		default {"1.0"}
		desc { "Nominal service time" }
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	code {
		struct token {
		   Particle* pp;
		   float serviceNeeded;
		   float lastUpdate;
		};
	}
	protected {
		// structure to store tokens in service
		SingleLinkList tokensInService;
		int numberInService;
	}
	start {
		numberInService = 0;
		tokensInService.initialize();
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
		delete t;
		numberInService--;
		outputP = TRUE;
	      } else {
		break;
	      }
	   }

	   // Check for false alarm, and return if nothing has changed
	   if((!input.dataNew) & (!outputP)) return;

	   // Modify the serviceNeeded for all tokensInService.
	   ListIter nextTok((SequentialList&)tokensInService);
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
		*newp = pp;
		t = new token;
		t->pp = newp;
		t->serviceNeeded = double(nomServiceTime);
		t->lastUpdate = arrivalTime;

		// Append new token to the list of tokensInService
		tokensInService.append(t);
		numberInService += 1;

		// Schedule a firing at the current estimate of
		// the completion time of the service.
		refireAtTime(arrivalTime + (t->serviceNeeded)*numberInService);

		input.dataNew = FALSE;
	   }

	}
}
