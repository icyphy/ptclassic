defstar
{
  name { PulseGen }
  derivedFrom {RepeatStar}
  domain {DE}
  desc {
This star generates events with specified values at specified moments.
The events are specified in the "value" array, which consists of
time-value pairs, given in the syntax of complex numbers.
  }
  explanation { none }
  version { $Revision$ $Date$ }
  author { Stefan De Troch (of IMEC) }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { }
  output {
    name { output }
    type { float }
  }
  defstate {
    name { value }
    type { complexarray }
    default { "(0,1) (1,0)" }
  }
  protected {
    int pos;
  }
  setup {
    pos = 0;
  }
  go {
    // first event comes at time 0
    // check whether the first event is too early
    if (arrivalTime >= real(value[pos]))
    {
      output.put(real(value[pos])) << double(imag(value[pos]));
      pos++;
    }
    if (pos < value.size())
    {
      refireAtTime(real(value[pos]));
    }
  }
}
