defstar{
  name { NdftVarD } 
  domain { SDF } 
  desc {
This version of a Nonuniform Discrete Fourier Transform (NDFT) algorithm
is like the Goertzel algorithm in that it operates in order n time and
constant memory to detect one particular frequency from n input samples.
This particular star accepts a configurable number of samples.  The
2 cos(2*PI*(fi/fs)) coefficient is an input.  This star also accepts a
parameter which describes how many times it can fire before resetting the
state values.
  }
  version { @(#)SDFNdftVarD.pl	1.1	03/18/98 }
  author{ Matt Felder and Jimmy Mason }
  copyright {
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF dsp library }
  input {
    name { sampleIn }
    type { float }
    desc { input sample }
  }
  input {
    name { coeffIn }
    type { float }
    numtokens { 1 }
    desc {
Value of the 2*cos(2*PI*fi/fs) coefficient,
where fi is the detection frequency and fs is the sample rate. 
    }
  }
  defstate {
    name { numSamples }
    type { int }
    default { 106 }
    desc { number of samples to read per execution }
  }
  defstate {
    name { sequentialExecs }
    type { int }
    default { 1 }
    desc { number of times this star fires before resetting the states }
  }
  defstate {
    name { initialExec }
    type { int }
    default { 0 }
    desc {
how many executions we pretend to have performed at startup
    }
  }
  state {
    name { num_execs }
    type { int }
    default { 0 }
    attributes { A_NONCONSTANT|A_NONSETTABLE }
  }
  output {
    name { filterFreqOut }
    type { float }
    desc { amount of power detected at the given frequency }
  }

  private {
    double stateHistory[3];        /* state history (3 states deep) */
  }

  method {
    name     { generate_freq_energy }
    access   { protected }
    type     { void }
    arglist  { "()" }
    code     {
      /* The first sample received will be at the location with
       * the largest index, i.e. at sampleIn%(int(numSamples)-1),
       * and the first sample out also has the greater index.
       */

      /* reset the states if required */
      if (int(num_execs) == 0) {
        stateHistory[0] = stateHistory[1] = stateHistory[2] = 0.0;
      }

      for (int i=int(numSamples)-1; i>=0; i--){
        stateHistory[0] = double(sampleIn%i);
        stateHistory[0] += double(coeffIn%0) * stateHistory[1];
	stateHistory[0] -= stateHistory[2];
        /* calculate txfer fn, mult current sample */

        /* shift current state memory */
        stateHistory[2] = stateHistory[1];
        stateHistory[1] = stateHistory[0];
      }

      /* calc power of non-recursive part of the second-order filter */
      double outputPower =
        stateHistory[1]*stateHistory[1] -
	(stateHistory[1]*double(coeffIn%0)*stateHistory[2]) +
	(stateHistory[2]*stateHistory[2]);

      filterFreqOut%0 << outputPower;

      num_execs = int(num_execs) + 1;
    }
  }

  setup {
    /* set number of samples input per firing of star to be numSamples */
    if (int(numSamples) < 1) {
      Error::abortRun(*this, "numSamples must be greater than 0");
      return;
    }
    sampleIn.setSDFParams(int(numSamples),int(numSamples)-1);

    /* check number of sequential windows that are used */
    if (int(sequentialExecs) < 1) {
      Error::abortRun(*this, "sequentialExecs must be a positive number");
      return;
    }
    if (int(initialExec) > int(sequentialExecs)-1) {
      Error::abortRun(*this, "initialExec can't exceed sequentialExecs-1");
      return;
    }
    num_execs = int(initialExec);
    stateHistory[0] = stateHistory[1] = stateHistory[2] = 0.0;
  }

  go {
    if (int(num_execs) >= int(sequentialExecs)) {
      num_execs = 0;
    }

    // read inputs and produce output
    generate_freq_energy();
  }
}
