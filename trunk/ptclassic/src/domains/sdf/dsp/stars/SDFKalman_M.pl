defstar {
  name      { Kalman_M }
  domain    { SDF }
  desc      { Implements the Kalman filter. }
  version   { $Id$ }
  author    { Mike J. Chen }
  copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
  location  { SDF dsp library }
  explanation {
Initial values for the state transition matrix, state vector, correlation
matrix, and process noise correlation matrix are parameters to the star.
Also parameters are the number of elements in the observation vector and
the number of elements in the state vector.
Inputs are the current values of the state transition matrix, process
noise correlation matrix, measurement noise correlation matrix, 
measurement matrix, and the observation vector.
The single output is the state vector.
.UH REFERENCES
.ip [1]
R.E. Kalman, "A new approach to linear filtering and prediction problems",
\fITrans. ASME, J. Basic Eng., Ser 82D, pp. 35-45, March 1960.
.ip [2]
S. Haykin, \fIAdaptive Filter Theory\fR, Prentice-Hall, Inc., Englewood Cliffs,
N.J., 1986.
  }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
  }
  input {
    name { StateTransitionMatrixAtTimeN }
    type { FLOAT_MATRIX_ENV }
  }
  input { 
    name { MeasurementMatrixAtTimeN }
    type { FLOAT_MATRIX_ENV }
  }
  input {
    name { ProcessNoiseCorrMatrixAtTimeN }
    type { FLOAT_MATRIX_ENV }
  }
  input {
    name { MeasurementNoiseCorrMatrixAtTimeN }
    type { FLOAT_MATRIX_ENV }
  }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
  }
  defstate {
    name { stateDimension }
    type { int }
    default { 5 }
    desc { The number of elements in the state vector. }
  }
  defstate {
    name { inputDimension }
    type { int }
    default { 1 }
    desc { The number of elements in the observation vector. }
  }
  defstate {
    name { InitialState }
    type { floatarray }
    default { "0.0 [5]" }
    desc { The initial value of the state vector. }
  }
  defstate {
    name { InitialCorrMatrix }
    type { floatarray }
    default { ".1 0 [5] .1 0 [5] .1 0 [5] .1 0 [5] .1" }
    desc { The initial value of the correlation matrix of the error. }
  }
  defstate {
    name { InitialStateTransitionMatrix }
    type { floatarray }
    default { "1 0 [5] 1 0 [5] 1 0 [5] 1 0 [5] 1" }
    desc { The state transition matrix at time 0. PHI(1,0)}
  }
  defstate {
    name { InitialProcessNoiseCorrMatrix }
    type { floatarray }
    default { "0.0 [25]" }
    desc { Correlation matrix of process noise vector at time 0. Q(0)}
  }
  hinclude { <math.h>, "Matrix.h" }
  protected {
    int stateDim;
    int inputDim;

    // intermediate variables, part of the state
    FloatMatrix *stateVector;                        // x(n|y(n));
    FloatMatrix *gainMatrix;                         // G(n)
    FloatMatrix *innovationsVector;                  // alpha(n)
    FloatMatrix *innovationsCorrMatrix;              // sigma(n)
    FloatMatrix *predStateVector;                    // x(n+1|y(n));
    FloatMatrix *predStateErrCorrMatrix;             // K(n+1,n)
    FloatMatrix *presentErrCorrMatrix;               // K(n)
  }
  constructor {
    stateVector = 0;
    gainMatrix = 0;
    innovationsVector = 0;
    innovationsCorrMatrix = 0;
    predStateVector = 0;
    predStateErrCorrMatrix = 0;
    presentErrCorrMatrix = 0;
  }
  destructor {
    delete stateVector;
    delete gainMatrix;
    delete innovationsVector;
    delete innovationsCorrMatrix;
    delete predStateVector;
    delete predStateErrCorrMatrix;
    delete presentErrCorrMatrix;
  }
  setup {
    stateDim = int(stateDimension);
    inputDim = int(inputDimension);

    // delete previously allocation arrays
    delete stateVector;
    delete gainMatrix;
    delete innovationsVector;
    delete innovationsCorrMatrix;
    delete predStateVector;
    delete predStateErrCorrMatrix;
    delete presentErrCorrMatrix;

    // initialize storage for intermediate and state variables
    stateVector = new FloatMatrix(stateDim,1,InitialState);  // x(n|y(n))
    gainMatrix = new FloatMatrix(stateDim,inputDim);         // G(n)
    innovationsVector = new FloatMatrix(inputDim, 1);        // alpha(n)
    innovationsCorrMatrix = new FloatMatrix(inputDim,inputDim); // sigma(n)
    predStateVector = new FloatMatrix(stateDim,1);           // x(n+1|y(n))
    predStateErrCorrMatrix = new FloatMatrix(stateDim,stateDim);// K(n+1,n)
    presentErrCorrMatrix = new FloatMatrix(stateDim,stateDim,InitialCorrMatrix);  // K(n)

    // initialize working variables
    FloatMatrix initialStateTransitionMatrix(stateDim,stateDim, InitialStateTransitionMatrix);
    FloatMatrix initialProcessNoiseCorrMatrix(stateDim,stateDim, InitialProcessNoiseCorrMatrix);

    // calculate zero-th step variables

    // calculate K(1,0) = PHI(1,0)*K(0)*(PHI(1,0)->transpose)  +  Q(0)
    (*predStateErrCorrMatrix) = initialStateTransitionMatrix * 
                                (*presentErrCorrMatrix) * 
                                (~initialStateTransitionMatrix) + 
                                initialProcessNoiseCorrMatrix;
      
    // calculate x(1|y(0)), the one step prediction
    //  x(1|y(0)) = PHI(1,0)*x(0)
    multiply(initialStateTransitionMatrix,*stateVector,*predStateVector);
  }
  go {
    Envelope pkt1;
    Envelope pkt2;
    Envelope pkt3;
    Envelope pkt4;
    Envelope pkt5;
    // inputs to the system
    //    const FloatMatrix& inputVector;                        // y(n)
    //    const FloatMatrix& stateTransitionMatrix;              // PHI(n+1,n)
    //    const FloatMatrix& processNoiseCorrMatrix;             // Q(n)
    //    const FloatMatrix& measurementNoiseCorrMatrix;         // R(n)
    //    const FloatMatrix& measurementMatrix;                  // C(n)

    (input%0).getMessage(pkt1);
    const FloatMatrix& inputVector = *(const FloatMatrix *)pkt1.myData();

    (StateTransitionMatrixAtTimeN%0).getMessage(pkt2);
    const FloatMatrix& stateTransitionMatrix = *(const FloatMatrix *)pkt2.myData();

    (ProcessNoiseCorrMatrixAtTimeN%0).getMessage(pkt3);
    const FloatMatrix& processNoiseCorrMatrix = *(const FloatMatrix *)pkt3.myData();

    (MeasurementNoiseCorrMatrixAtTimeN%0).getMessage(pkt4);
    const FloatMatrix& measurementNoiseCorrMatrix = *(const FloatMatrix *)pkt4.myData();

    (MeasurementMatrixAtTimeN%0).getMessage(pkt5);
    const FloatMatrix& measurementMatrix = *(const FloatMatrix *)pkt5.myData();

    // calculate sigma(n) = C(n)*K(n,n-1)*(C(n)->transpose)  +  R(n)
    *innovationsCorrMatrix=(measurementMatrix * (*predStateErrCorrMatrix) *
                            (~measurementMatrix)) + 
                           measurementNoiseCorrMatrix;

    // calculate G(n)=PHI(n+1,n)*K(n,n-1)*(C(n)->transpose)*(sigma(n)->inverse)
    *gainMatrix = stateTransitionMatrix * (*predStateErrCorrMatrix) *
                  (~measurementMatrix) * (innovationsCorrMatrix->inverse());

    // calculate innovations vector: alpha(n) = y(n) - C(n)*x(n|y(n-1))
    *innovationsVector = inputVector - 
                         (measurementMatrix * (*predStateVector));

    // calculate one step prediction
    //  x(n+1|y(n)) = PHI(n+1,n)*x(n|y(n-1)) + G(n)*alpha(n)
    *predStateVector = (stateTransitionMatrix * (*predStateVector)) +
                       ((*gainMatrix) * (*innovationsVector));

    // calculate new state: x(n|y(n)) = PHI(n,n+1)*x(n+1|y(n))
    // makes the assumption that PHI(n,n+1) = PHI(n+1,n)
    multiply(stateTransitionMatrix,*predStateVector,*stateVector);

    // calculate correlation matrix of error in new state: K(n)
    // K(n) = K(n,n-1) - PHI(n,n+1)
    // makes the assumption that PHI(n,n+1) = PHI(n+1,n)
    *presentErrCorrMatrix = (*predStateErrCorrMatrix) -
                            (stateTransitionMatrix * (*gainMatrix) * 
                             measurementMatrix * (*predStateErrCorrMatrix));

    // calculate predStateErrCorrMatrix: K(n+1,n) 
    // K(n+1,n) = PHI(n+1,n)*K(n)*(PHI(n+1,n)->transpose)  + Q(n)
    *predStateErrCorrMatrix = (stateTransitionMatrix * 
                               (*presentErrCorrMatrix) *
                               (~stateTransitionMatrix)) +
                              processNoiseCorrMatrix;

    FloatMatrix* outMatrix = new FloatMatrix(*stateVector);
    output%0 << *outMatrix;

  }
}

