defstar {
  name { SGVQCodebk }
  domain { SDF }
  desc {  
Jointly optimized codebook design for shape-gain vector quantization.
Note that each input matrix will be viewed as a row vector in row by
row. Each row of first output matrix represent a codeword of the shape
codebook. Each element of the second output matrix represent a 
codeword of the gain codebook.
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF dsp library }
  defstate {
    name { dimension }
    type { int }
    default { 16 }
    desc { The dimension for the input training vector. }
  }
  defstate {
    name { sizeTrnSet }
    type { int }
    default { 1024 }
    desc { The number of input training vectors for yielding the codebooks. }
  }
  defstate {
    name { sizeShapeCodebook }
    type { int }
    default { 64 }
    desc { The number of desired codewords for the shape codebook. }
  }
  defstate {
    name { sizeGainCodebook }
    type { int }
    default { 64 }
    desc { The number of desired codewords for the gain codebook. }
  }
  defstate {
    name { thresholdRatio }
    type { float }
    default { 0 }
    desc { A number between 0 to 1. }
  }
  defstate {
    name { maxIteration }
    type { int }
    default { 100 }
    desc { The upper limited times of iterations. }
  }
  defstate {
    name { saveShapeCodebook }
    type { string }
    default { "" }
    desc { File to save final shape codebook values. }
  }
  defstate {
    name { saveGainCodebook }
    type { string }
    default { "" }
    desc { File to save final gain codebook values. }
  }
  input {
    name { input }
    type { FLOAT_MATRIX_ENV }
    desc{ 
Input matrix will be viewed as a row vector in row major ordering to
represent a training vector.
    }
  }
  output {
    name { output }
    type { FLOAT_MATRIX_ENV }
    desc{ Output shape codebook, ie, (sizeShapeCodebook)*(dimension) matrix. }
  }
  output {
    name { outGain }
    type { FLOAT_MATRIX_ENV }
    desc{ Output gain codebook, ie, (sizeGainCodebook)*1 matrix. }
  }
  ccinclude { "Matrix.h" }
  code {
// Code for partition function which is to find the optimum partition
// of the training vector for fixed gain and shape codebooks, and find
// the squared error (squared distance) of this training vector. 
     static void partition(int& indexShape, int& indexGain, 
			   double& distance, const double* trnVector, 
			   const double* shapeCodebook, int sizeShapeCodebook,
			   int dimension, const double* gainCodebook, 
			   int sizeGainCodebook) {

// If X=input vector, and Si=i_th shape codeword.
// First find the shape codeword Si to maximize X'*Si ( ' means transpose ),
	indexShape = 0;
	double shapeDistance = 0;
	for ( int j=0; j<dimension; j++ )
	    shapeDistance += trnVector[j]*shapeCodebook[j];

	double sum = 0;
	for ( int i = 1; i<sizeShapeCodebook; i++ ) {
	    for ( j=0; j<dimension; j++ )
	        sum += trnVector[j]*shapeCodebook[i*dimension+j];
	    if ( sum > shapeDistance ) {
	       shapeDistance = sum;
	       indexShape = i;
	    }
	    sum = 0;
	}
// now shapeDistance store the maximun X'*Si

// Then find the j_th gain codeword gj to minimize (gj-X'*Si)^2
	indexGain = 0;
	distance = gainCodebook[0]-shapeDistance;
	distance *= distance;

	for (i=1; i<sizeGainCodebook; i++) {
	  sum = gainCodebook[i]-shapeDistance;
	  sum *= sum;
          if ( sum < distance ) {
            distance = sum;
            indexGain = i;
          }
	}
// now distance store the minimun (gj-X'*Si)^2

// The squared error (squared distance) is ||X||^2+(gj-X'*Si)^2-(X'*Si)^2. 
	distance -= shapeDistance*shapeDistance;
	for (j=0; j<dimension;j++)
	  distance += trnVector[j]*trnVector[j];

     }  // end of partition function
  } 	 // end of code
  protected {
    double* shapeCodebook;
    double* gainCodebook;
    double* trnSet;
    double* shapeCentroid;
    double* gainCentroid;
    int*    numGainPart; 
  }
  constructor {
    shapeCodebook = 0;
    gainCodebook = 0;
    trnSet = 0;
    shapeCentroid = 0;
    gainCentroid = 0;
    numGainPart = 0;
  }
  destructor {
    LOG_DEL; delete [] shapeCodebook;
    LOG_DEL; delete [] gainCodebook;
    LOG_DEL; delete [] trnSet;
    LOG_DEL; delete [] shapeCentroid;
    LOG_DEL; delete [] gainCentroid;
    LOG_DEL; delete [] numGainPart;
  }
  setup {
    if ( int(sizeShapeCodebook) > int(sizeTrnSet) ||
	 int(sizeGainCodebook)  > int(sizeTrnSet) ) {
	Error::abortRun(*this,"The size of codebooks must be smaller than",
			      "the size of training set");
	return;
    }
    input.setSDFParams(int(sizeTrnSet), int(sizeTrnSet)-1);
  }
  go {
//  Define an array for storing input training vectors
    LOG_NEW; trnSet = new double[int(sizeTrnSet)*int(dimension)];

//  Get the input training vectors and store them in the 2-dimension array
    Envelope inpkt;
    FloatMatrix& vector = *(new FloatMatrix(1,int(dimension)));
    for (int i=0; i<int(sizeTrnSet); i++) {
      (input%(int(sizeTrnSet)-1-i)).getMessage(inpkt);
      vector = *(const FloatMatrix *)inpkt.myData();
      if ( vector.numRows()*vector.numCols() != int(dimension) ) {
	Error::message(*this,"The number of elements in input vector(matrix) ",
			     "must equal to the parameter 'dimension'.");
	return;
      }
      for (int j=0; j<int(dimension); j++) 
	trnSet[i*int(dimension)+j]=vector.entry(j);
    }
    
//  Allocate a array for storing the generated codebooks
    LOG_NEW; shapeCodebook = new double[int(sizeShapeCodebook)*int(dimension)];
    LOG_NEW; gainCodebook = new double[int(sizeGainCodebook)];

/*
 *  Initialize the initial codebooks using random code, ie, we take 
 *  every Kth training vector among the input traing vectors, where
 *  K = sizeTrnSet/sizeCodebook.
*/
    const int K1 = int(sizeTrnSet)/int(sizeShapeCodebook);
    double gain = 0;
    for (i=0; i<int(sizeShapeCodebook); i++) {
      for (int j=0; j<int(dimension); j++)
        gain += trnSet[i*K1*int(dimension)+j]*trnSet[i*K1*int(dimension)+j];
      gain = sqrt(gain);
      for (j=0; j<int(dimension); j++)
	shapeCodebook[i*int(dimension)+j] = trnSet[i*K1*int(dimension)+j]/gain;
      gain = 0;
    }
    const int K2 = int(sizeTrnSet)/int(sizeGainCodebook);
    for (i=0; i<int(sizeGainCodebook); i++) {
      gainCodebook[i] = 0;
      for (int j=0; j<int(dimension); j++)
        gainCodebook[i] += trnSet[i*K2*int(dimension)+j]*trnSet[i*K2*int(dimension)+j];
      gainCodebook[i] = sqrt(gainCodebook[i]);
    }

/*
 *  The array store the centroid of each partitioned cluster 
 *  of training set.
 */
    LOG_NEW; shapeCentroid = new double[int(sizeShapeCodebook)*int(dimension)];
    for (i=0; i<int(sizeShapeCodebook); i++)
      for (int j=0; j<int(dimension); j++)
        shapeCentroid[i*int(dimension)+j]  = 0;

    LOG_NEW; gainCentroid  = new double[int(sizeGainCodebook)];
    for (i=0; i<int(sizeGainCodebook); i++)
      gainCentroid[i] = 0;

/*
 *  The numGainPart[i] store the number of how many input training vectors
 *  are partitioned into the region of i_th gain codeword. 
 */
    LOG_NEW; numGainPart = new int[int(sizeGainCodebook)];
    for (i=0; i<int(sizeGainCodebook); i++)
      numGainPart[i] = 0;

    int count = 0;
    int indexShape = 0;
    int indexGain = 0;
    double distance = 0;
    double oldDistortion = 0;
    double distortion = 0;
//  Compute the optimum partition for the initial codebooks.
    for (i=0; i<int(sizeTrnSet); i++) {
      partition(indexShape,indexGain,distance,&(trnSet[i*int(dimension)]),
                shapeCodebook,int(sizeShapeCodebook),int(dimension),
		gainCodebook,int(sizeGainCodebook));
      distortion += distance;
      for (int j=0; j<int(dimension); j++)
        shapeCentroid[indexShape*int(dimension)+j] += gainCodebook[indexGain]*trnSet[i*int(dimension)+j];
    }
    count++;

    do {
//  compute the optimum shape codebook Cs(m+1) in partition R(Cs(m),Cg(m)).
      for (i=0; i<int(sizeShapeCodebook); i++) {
	gain = 0;
        for (int j=0; j<int(dimension); j++)
          gain += shapeCentroid[i*int(dimension)+j]*shapeCentroid[i*int(dimension)+j];
        gain = sqrt(gain);
	if (gain != 0) {
          for (j=0; j<int(dimension); j++)
            shapeCodebook[i*int(dimension)+j] = shapeCentroid[i*int(dimension)+j]/gain;
        } else {
        //  If gain=0, then just keep the i_th original codeword.
        }
      }

//  Reset the contents of array gainCentroid to 0.
      for (i=0; i<int(sizeGainCodebook); i++)
        gainCentroid[i] = 0;

//  Reset the contents of array numGainPart to 0.
      for (i=0; i<int(sizeGainCodebook); i++)
	numGainPart[i] = 0;

//  Compute the optimum partition R(Cs(m+1),Cg(m)).
      for (i=0; i<int(sizeTrnSet); i++) {
        partition(indexShape,indexGain,distance,&(trnSet[i*int(dimension)]),
                  shapeCodebook,int(sizeShapeCodebook),int(dimension),
                  gainCodebook,int(sizeGainCodebook));
        (numGainPart[indexGain])++;
        for (int j=0; j<int(dimension); j++)
          gainCentroid[indexGain] += shapeCodebook[indexShape*int(dimension)+j]*trnSet[i*int(dimension)+j];
      }

//  Compute the optimum gain codebook Cg(m+1) in partition R(Cs(m+1),Cg(m+1)).
      for (i=0; i<sizeGainCodebook; i++) {
	if (numGainPart[i] != 0 ) {
	  if (gainCentroid[i]<0) 
	    gainCodebook[i] = 0;
	  else
	    gainCodebook[i] = gainCentroid[i]/double(numGainPart[i]);       
	} else {
        //  If numGainPart[i]=0, then just keep the i_th original codeword.
	}
      }

//  Reset the contents of array shapeCentroid to 0.
      for (i=0; i<int(sizeShapeCodebook); i++)
        for (int j=0; j<int(dimension); j++)
          shapeCentroid[i*int(dimension)+j] = 0;

//  Reset the value of oldDistortion and Distortion.
      oldDistortion = distortion;
      distortion = 0;
	
//  Compute the optimum partition R(Cs(m+1),Cg(m+1)).
      for (i=0; i<int(sizeTrnSet); i++) {
        partition(indexShape,indexGain,distance,&(trnSet[i*int(dimension)]),
                  shapeCodebook,int(sizeShapeCodebook),int(dimension),
                  gainCodebook,int(sizeGainCodebook));
        distortion += distance;
        for (int j=0; j<int(dimension); j++)
          shapeCentroid[indexShape*int(dimension)+j] += gainCodebook[indexGain]*trnSet[i*int(dimension)+j];
      }
    } while ( (oldDistortion-distortion)/oldDistortion>double(thresholdRatio)
	      && (++count)<int(maxIteration) );

    FloatMatrix& result1 = *(new FloatMatrix(int(sizeShapeCodebook),int(dimension)));
    for (i=0; i<int(sizeShapeCodebook)*int(dimension); i++)
      result1.entry(i) = shapeCodebook[i];
    output%0 << result1;

    FloatMatrix& result2 = *(new FloatMatrix(int(sizeGainCodebook),1));
    for (i=0; i<int(sizeGainCodebook); i++)
      result2.entry(i) = gainCodebook[i];
    outGain%0 << result2;
  }	// end of go method
  wrapup {
    const char* sf1 = saveShapeCodebook;
    if (sf1 != NULL && *sf1 != 0) {
      char* saveFileName = savestring (expandPathName(sf1));
      // open the file for writing
      FILE* fp;
      if (!(fp = fopen(saveFileName,"w"))) {
      // File cannot be opened
         Error::warn(*this,"Cannot open saveTapsFile for writing: ",
                           saveFileName, ". Taps not saved.");
      } else
         for (int i=0; i<int(sizeShapeCodebook); i++) {
           for (int j=0; j<int(dimension); j++)
             fprintf(fp,"%e ",shapeCodebook[i*int(dimension)+j]);
           fprintf(fp,"\n");
         }
      fclose(fp);
      LOG_DEL; delete saveFileName;
    }
    const char* sf2 = saveGainCodebook;
    if (sf2 != NULL && *sf2 != 0) {
      char* saveFileName = savestring (expandPathName(sf2));
      // open the file for writing
      FILE* fp;
      if (!(fp = fopen(saveFileName,"w"))) {
      // File cannot be opened
         Error::warn(*this,"Cannot open saveTapsFile for writing: ",
                           saveFileName, ". Taps not saved.");
      } else {
         for (int i=0; i<int(sizeGainCodebook); i++) {
             fprintf(fp,"%e ",gainCodebook[i]);
           fprintf(fp,"\n");
         }
      }
      fclose(fp);
      LOG_DEL; delete saveFileName;
    }
  }	//end of wrapup method
}
