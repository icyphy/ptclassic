defstar {
  name { ResetableChannel }
  domain { DE }
  desc {
    Under normal circumstances, the channel just passes input to output. However, if the reset is present, the channel passes reset to output.
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { DE main library }
  input {
    name { input }
    type { float }
  }
  input {
    name { reset }
    type { float }
  }
  output {
    name { output }
    type { float }
  }
  go {
    completionTime = arrivalTime;
    if (input.dataNew && !reset.dataNew) {
	output.put(completionTime) << double(input.get());
    } else {
        output.put(completionTime) << double(reset.get());
        input.dataNew = FALSE;
    } 
  }
}

