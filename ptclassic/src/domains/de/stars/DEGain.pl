defstar {
  name { Gain }
  domain { DE }
  desc {
This is an amplifier; the output is the input multiplied by the "gain" (default 1.0). The output is absent when the input is absent.
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
  output {
    name { output }
    type { float }
  }
  defstate {
    name { gain }
    type { float }
    default { "1.0" }
    desc { Gain of the star. }
  }
  go {
    completionTime = arrivalTime;
    if (input.dataNew) {
      output.put(completionTime) << double(gain) * double(input.get());
    }
  }
}

