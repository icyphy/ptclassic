defstar {
  name { GLA }
  domain { SDF }
  desc {  
Use the generalized Lloyd algorithm to yield a codebook from input  
training vectors.  Note that each input matrix will be viewed as a
row vector in row by row. Each row of output matrix represent a
codeword of the codebook. 
  }
  version { $Id$ }
  author { Bilung Lee }
  copyright { 1993 The Regents of the University of California }
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
    desc { The number of input training vectors for yielding the codebook. }
  }
  defstate {
    name { sizeCodebook }
    type { int }
    default { 64 }
    desc { The number of desired codewords for the codebook. }
  }
  defstate {
    name { thresholdRatio }
    type { float }
    default { 0 }
    desc {
A number between 0 to 1. When equal to 0, the resulting codebook will satisfy the necessary conditions for optimality.
    }
  }
  defstate {
    name { maxIteration }
    type { int }
    default { 100 }
    desc { The upper limited times of Lloyd iterations. }
  }
  defstate {
    name { saveFile }
    type { string }
    default { "" }
    desc { File to save final codebook values. }
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
    desc{ Output codebook, ie, (sizeCodebook)*(dimension) matrix. }
  }
  ccinclude { "Matrix.h" }
  code {
// Code for NN function which is to find the index of nearest neighbor
// in the codebook for the training vector, and find the squared error
// (squared distance) between the training vector and its nearest neighbor.
     static void NN(int& index, double& distance,
                    const double* trnVector, const double* codebook,
		    const double* halfCodewordPower,
		    int sizeCodebook, int dimension) {
/*
 * If X=input vector, and Yi=i_th codeword.
 * Find the nearest neighbor codeword Yi to maximize X'*Yi-Ai
 * ( ' means transpose ), where Ai=||Yi||^2/2 and should have
 * already been stored in the array halfCodewordPower[numCodewords].
 */
	index = 0;
	distance = 0;
	for ( int j=0; j<dimension; j++ )
	    distance += trnVector[j]*codebook[j];
	distance -= halfCodewordPower[0];

	double sum = 0;
	for ( int i = 1; i<sizeCodebook; i++ ) {
	    for ( j=0; j<dimension; j++ )
	        sum += trnVector[j]*codebook[i*dimension+j];
	    sum -= halfCodewordPower[i];
	    if ( sum > distance ) {
	       distance = sum;
	       index = i;
	    }
	    sum = 0;
	}
// The distance we compute here is the X'*Yi-||Yi||^2/2, but actually the 
// squared distance is ||X-Yi||^2 = ||X||^2+||Yi||^2-2*X'*Yi. Therefore we
// need to calculate the true squared distance before return. 
	distance *= -2;
	for (i=0; i<dimension;i++)
	  distance += trnVector[i]*trnVector[i];
     }  // end of NN function
  } 	 // end of code
  protected {
    double* codebook;
    double* trnSet;
    int* numPart;
    double* centroid;
    double* halfCodewordPower;
  }
  constructor {
    codebook = 0;
    trnSet = 0;
    numPart = 0;
    centroid = 0;
    halfCodewordPower = 0;
  }
  destructor {
    LOG_DEL; delete [] codebook;
    LOG_DEL; delete [] trnSet;
    LOG_DEL; delete [] numPart;
    LOG_DEL; delete [] centroid;
    LOG_DEL; delete [] halfCodewordPower;
  }
  setup {
    if ( int(sizeCodebook) > int(sizeTrnSet) ) {
	Error::abortRun(*this,"The size of codebook must be smaller than",
			      "the size of training set");
	return;
    }
    input.setSDFParams(int(sizeTrnSet), int(sizeTrnSet)-1);
  }
  go {
//  Define a 2-dimension array for storing input training vectors
    delete [] trnSet;
    LOG_NEW; trnSet = new double[int(sizeTrnSet)*int(dimension)];

//  Get the input training vectors and store them in the 2-dimension array
    Envelope inpkt;
    FloatMatrix& vector = *(new FloatMatrix(1,int(dimension))); // MEMORY LEAK
    for (int i=0; i<int(sizeTrnSet); i++) {
      (input%(int(sizeTrnSet)-1-i)).getMessage(inpkt);
      vector = *(const FloatMatrix *)inpkt.myData();
      // check for "null" matrix inputs, caused by delays
      if (inpkt.empty()) {
        // input empty, just think it as a zero matrix
        vector = *(new FloatMatrix(1,int(dimension)));
        vector = 0;
      }
      if ( vector.numRows()*vector.numCols() != int(dimension) ) {
	Error::message(*this,"The number of elements in input vector(matrix) ",
			     "must equal to the parameter 'dimension'.");
	return;
      }
      for (int j=0; j<int(dimension); j++) 
	trnSet[i*int(dimension)+j]=vector.entry(j);
    }
    
//  Allocate a array for storing the generated codebook
    delete [] codebook;
    LOG_NEW; codebook = new double[int(sizeCodebook)*int(dimension)];

/*
 *  Initialize the initial codebook using random code, ie, we take 
 *  every Kth training vector among the input traing vectors, where
 *  K = sizeTrnSet/sizeCodebook.
 */
    const int K = int(sizeTrnSet)/int(sizeCodebook);
    for (i=0; i<int(sizeCodebook); i++) {
      for (int j=0; j<int(dimension); j++)
	codebook[i*int(dimension)+j] = trnSet[i*K*int(dimension)+ j];
    }
 
/*
 *  The numPart[i] store the number of how many input training vectors
 *  are closest to the i_th codeword in the codebook after using the 
 *  nearest neighbor condition to partition the training set.
 */
    delete [] numPart;
    LOG_NEW; numPart = new int[int(sizeCodebook)];
    for (i=0; i<int(sizeCodebook); i++)
      numPart[i] = 0;

/*
 *  The 1-dimension array centroid[int(sizeCodebook)*int(dimension)]
 *  store the centroid of each partitioned cluster of training set.
 */
    delete [] centroid;
    LOG_NEW; centroid = new double[int(sizeCodebook)*int(dimension)];
    for (i=0; i<int(sizeCodebook); i++) {
      for (int j=0; j<int(dimension); j++)
        centroid[i*int(dimension)+j] = 0;
    }

//  halfCodewordPower[i] store the value of Ai=||Yi||^2/2.
    LOG_NEW; halfCodewordPower = new double[int(sizeCodebook)];
//  Set halfCodewordPower[i] for the initial codebook.
    for (i=0; i<int(sizeCodebook); i++ ) {
      halfCodewordPower[i] = 0;
      for ( int j=0; j<int(dimension); j++ )
        halfCodewordPower[i] += codebook[i*int(dimension)+j]*codebook[i*int(dimension)+j];
      halfCodewordPower[i]=halfCodewordPower[i]/2;
    }

    int count = 0;
    int index = 0;
    double distance = 0;
    double oldDistortion = 0;
    double distortion = 0;
//  Run Lloyd iteration for the initial codebook.
    for (i=0; i<int(sizeTrnSet); i++) {
      NN(index,distance,&(trnSet[i*int(dimension)]),codebook,halfCodewordPower,int(sizeCodebook),int(dimension));
      (numPart[index])++;
      distortion += distance;
      for (int j=0; j<int(dimension); j++)
        centroid[index*int(dimension)+j] += trnSet[i*int(dimension)+j];
    }
    count++;

    do {
//  Reset the improved codebook to the centroid of the partitioned cluser set.
      for (i=0; i<int(sizeCodebook); i++) {
        if (numPart[i] != 0) {
          for (int j=0; j<int(dimension); j++)
            codebook[i*int(dimension)+j] = centroid[i*int(dimension)+j]/numPart[i];
        }
        else {
//  If numPart[i]=0, ie, the i_th partitioned cluster set is empty,
//  then just keep the i_th original codeword.
        }
      }

//  Reset the contents of array numPart to 0.
      for (i=0; i<int(sizeCodebook); i++)
	numPart[i] = 0;

//  Reset the contents of array centroid to 0.
      for (i=0; i<int(sizeCodebook); i++)
        for (int j=0; j<int(dimension); j++)
          centroid[i*int(dimension)+j]  = 0;

//  Precompute Ai=||Yi||^2/2 for the given codebook and store them in 
//  the array halfCodewordPower.
      for (i=0; i<int(sizeCodebook); i++ ) {
	halfCodewordPower[i] = 0;
        for ( int j=0; j<int(dimension); j++ )
          halfCodewordPower[i] += codebook[i*int(dimension)+j]*codebook[i*int(dimension)+j];
        halfCodewordPower[i]=halfCodewordPower[i]/2;
      }

//  Reset the value of oldDistortion and Distortion.
      oldDistortion = distortion;
      distortion = 0;
	
/*
 *  Lloyd iteration :
 *  For a given codebook, partition the training set into cluster sets
 *  using the nearest neighbor condition, and find the average distortion
 *  for the codebook. Then find the centroid of each partitioned cluster set.
 */
      for (i=0; i<int(sizeTrnSet); i++) {
	NN(index,distance,&(trnSet[i*int(dimension)]),codebook,halfCodewordPower,int(sizeCodebook),int(dimension));
	(numPart[index])++;
	distortion += distance;
	for (int j=0; j<int(dimension); j++)
	  centroid[index*int(dimension)+j] += trnSet[i*int(dimension)+j];
      }
    } while ( (oldDistortion-distortion)/oldDistortion>double(thresholdRatio)
	      && (++count)<int(maxIteration) );

    // allocate dynamic memory for output which will be automatically deleted
    FloatMatrix& result = *(new FloatMatrix(int(sizeCodebook),int(dimension)));
    for (i=0; i<int(sizeCodebook)*int(dimension); i++)
        result.entry(i) = codebook[i];
    output%0 << result;

    // deallocate dynamic memory
    LOG_DEL; delete [] codebook;
    LOG_DEL; delete [] trnSet;
    LOG_DEL; delete [] numPart;
    LOG_DEL; delete [] centroid;
    LOG_DEL; delete [] halfCodewordPower;
  } 
  wrapup {
    const char* sf = saveFile;
    if (sf != NULL && *sf != 0) {
      char* saveFileName = expandPathName(sf);
      // open the file for writing
      FILE* fp;
      if (!(fp = fopen(saveFileName,"w"))) {
      // File cannot be opened
         Error::warn(*this,"Cannot open saveTapsFile for writing: ",
                           saveFileName, ". Taps not saved.");
      } else {
         for (int i=0; i<int(sizeCodebook); i++) {
           for (int j=0; j<int(dimension); j++)
             fprintf(fp,"%e ",codebook[i*int(dimension)+j]);
           fprintf(fp,"\n");
         }
      }
      fclose(fp);
      LOG_DEL; delete [] saveFileName;
    }
  }	//end of wrapup method
}
