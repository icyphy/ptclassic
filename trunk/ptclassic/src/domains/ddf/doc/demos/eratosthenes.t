.\" $Date$	$Id$
.NA eratosthenes
.SD
The sieve of Eratosthenes is a recursive algorithm for computing prime numbers.
.DE
.LO "$PTOLEMY/src/domains/ddf/demo"
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
The top level system just feeds a sequence of integers beginning with 2
into the sieve.  Anything emerging from the sieve is prime, and is displayed.
The sieve is implemented by a galaxy called "sift".  This sift galaxy passes
its first input directly to the output and also to a galaxy called "filter".
All subsequent inputs are passed only to the "filter" galaxy, which filters
out all multiples of previously discovered primes.  Thus, any output emerging
from the filter star must be prime.  It is sent a recursive invocation
of the "sift" galaxy, which thus creates a new filter for the new prime.
The net effect is that a chain of filters, one for each prime discovered
so far, is dynamically constructed.  Any input value that gets through all
the filters is prime, and results in the creation of a new filter.
It is also displayed on the screen.
.pp
The filter galaxy uses a small feedback loop to store its first input.
Each subsequent input is compared against the first input to see whether
it is a multiple of it.  Any input that is a multiple of the first input
is discarded.  Other inputs are sent to the output.
.SA
Case,
EndCase,
Self.
.ES
