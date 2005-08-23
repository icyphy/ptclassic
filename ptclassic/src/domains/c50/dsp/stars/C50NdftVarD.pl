defstar{
  name { NdftVarD } 
  domain { C50 } 
  version { @(#)C50NdftVarD.pl	1.2	05/23/98}
  acknowledge {SDF version by M. Felder and J. Mason}
  author{ G. Arslan }
  location { C50 dsp library }
  desc {
This version of a Nonuniform DFT (NDFT) algorithm is like the
Goertzel algorithm in that it operates in n^2 time and constant
memory to detect one particular frequency from n input samples.
This particular star accepts a configurable number of samples.
The 2 cos(2*PI*(fi/fs)) coefficient is an input.  This star also
accepts a parameter which describes how many times it can fire
before resetting the state values.
  }
  input {
    name { sampleIn }
    type { fix }
    desc { input that represents a sample sent to the filter }
  }
  input {
    name { coeffIn }
    type { fix }
    numtokens { 1 }
    desc {
Value of the 2*cos(2*PI*fi/fs) coefficient,
where fi is the detection frequency and fs is the sample rate. 
    }
  }
  output {
    name { filterFreqOut }
    type { float }
    desc { amount of power detected at the given frequency }
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
  defstate {
    name { numExecs }
    type { int }
    default { 0 }
    attributes { A_NONCONSTANT|A_NONSETTABLE }
  }
  defstate {
    name { stateHistory }
    type { fixarray }
    default { "0 0 0" }
    attributes{A_NONSETTABLE | A_NONCONSTANT | A_BMEM}
  }
  defstate {
    name { dummy }
    type { fix }
    default { 0 }
    attributes{A_NONSETTABLE | A_NONCONSTANT | A_BMEM}
  }
  setup {
    /* set number of samples input per firing of star to be numSamples */
    if (int(numSamples) < 1) {
      Error::abortRun(*this, "numSamples must be greater than 0");
      return;
    }
    sampleIn.setSDFParams(int(numSamples), int(numSamples)-1);

    /* check number of sequential windows that are used */
    if (int(sequentialExecs) < 1) {
      Error::abortRun(*this, "sequentialExecs must be a positive number");
      return;
    }
    if (int(initialExec) > int(sequentialExecs)-1) {
      Error::abortRun(*this, "initialExec can't exceed sequentialExecs-1");
      return;
    }
    numExecs = int(initialExec);

    addCode(reset);   

  }
  codeblock( generate_freq_energy ){
	splk #@( numSamples - 1 ),BRCR   ;loop counter = number of samples
	lar AR2, #($addr(stateHistory)+1);AR2 -> stateHistory(1)
	lta $addr( coeffIn )             ;T = coeffIn
	mar AR2                          ;ARP -> AR2
	rptb block_end                   ;repeat block
	lacc $addr( sampleIn )           ;ACC = sampleIn(0)
	mpy *+                           ;P = coeffIn * stateHistory(1), AR2++
	apac                             ;ACC = ACC + P
	sub *                            ;ACC = ACC - stateHistory(2)
	lar AR2, #$addr( stateHistory )  ;AR2 -> stateHistory(0)
	rpt #2				
block_end: dmov *+                       ;stateHistory(1)=stateHistory(0)
                                         ;stateHistory(2)=stateHistory(1)
        lar AR2, #$addr( stateHistory )  ;AR2 -> stateHistory(0)
        sach *+                          ;stateHistory(0) = ACC , AR2++ 
	sph  $addr(dummy)                     ;dummy = coeffIn*stateHist(2)
	sqra *+                          ;P = stateHistory(1)^2
	lacc #0                          ;ACC = 0
	sqra *                           ;ACC = sateHistory(1)^2, P = stateHist(2)^2
	lt *                             ;T = stateHistory(1)
	mpy $addr(dummy)                 ;P = stateHistory(1)*coeffIn*stateHist(2)
	mpys *                           ;ACC = ACC - P (needless mult)
	sach $addr( ilterFreqOut )       ;Output = ACC		 		
  }
  codeblock( reset ){
      mar *,AR0
      zac
      rpt #2
      sach*+
  }
  go {
    if (int(numExecs) >= int(sequentialExecs)) {
      numExecs = 0;
    }
    if (int(numExecs) == 0) {
      addCode(reset); 
      stateHistory[0] = stateHistory[1] = stateHistory[2] = 0.0;
    }
    addCode(generate_freq_energy);
  }
  execTime {
    return (25+numSamples*14);
  }
}
