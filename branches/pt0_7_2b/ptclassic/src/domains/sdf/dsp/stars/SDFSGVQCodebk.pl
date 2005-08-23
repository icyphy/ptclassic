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
  version { @(#)SDFSGVQCodebk.pl	1.15 12/15/97 }
  author { Bilung Lee }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location  { SDF dsp library }
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
  ccinclude { "Matrix.h", "ptdspPartitionCodebook.h", "WriteASCIIFiles.h" }
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
    delete [] shapeCodebook;
    delete [] gainCodebook;
    delete [] trnSet;
    delete [] shapeCentroid;
    delete [] gainCentroid;
    delete [] numGainPart;
  }
  setup {
    if ( int(sizeShapeCodebook) > int(sizeTrnSet) ||
	 int(sizeGainCodebook)  > int(sizeTrnSet) ) {
	Error::abortRun(*this,"The size of codebooks must be smaller than",
			      "the size of training set");
	return;
    }

    input.setSDFParams(int(sizeTrnSet), int(sizeTrnSet)-1);

    //  Define an array for storing input training vectors
    delete [] trnSet;
    trnSet = new double[int(sizeTrnSet)*int(dimension)];

    // Allocate arrays for storing the generated codebooks
    delete [] shapeCodebook;
    shapeCodebook = new double[int(sizeShapeCodebook)*int(dimension)];
    delete [] gainCodebook;
    gainCodebook = new double[int(sizeGainCodebook)];

    // Allocate arrays to store the centroids for the codebooks
    delete [] shapeCentroid;
    shapeCentroid = new double[int(sizeShapeCodebook)*int(dimension)];
    delete [] gainCentroid;
    gainCentroid  = new double[int(sizeGainCodebook)];

    // Allocate an array to store number of training vectors for each codeword
    delete [] numGainPart;
    numGainPart = new int[int(sizeGainCodebook)];
  }

  go {
    // Get the input training vectors and store them in the 2-dimension array
    Envelope inpkt;
    int i,j;
    FloatMatrix zerovector(1, int(dimension));
    zerovector = 0;
    for (i=0; i<int(sizeTrnSet); i++) {
      const FloatMatrix* vectorp;
      (input%(int(sizeTrnSet)-1-i)).getMessage(inpkt);
      vectorp = (const FloatMatrix *)inpkt.myData();
      // check for "null" matrix inputs, caused by delays: treat as zero matrix
      if (inpkt.empty()) {
        vectorp = &zerovector;
      }
      if ( vectorp->numRows()*vectorp->numCols() != int(dimension) ) {
	Error::message(*this,
		       "The number of elements in input vector(matrix) ",
		       "must equal to the parameter 'dimension'.");
	return;
      }
      int rowloc = i * int(dimension);
      for (j = 0; j<int(dimension); j++) 
	trnSet[rowloc + j] = vectorp->entry(j);
    }

    // Initialize the initial codebooks using random code, ie, we take 
    // every Kth training vector among the input traing vectors, where
    // K = sizeTrnSet/sizeCodebook.
    const int K1 = int(sizeTrnSet)/int(sizeShapeCodebook);
    for (i = 0; i < int(sizeShapeCodebook); i++) {
      double gain = 0.0;
      int t_rowloc = i * K1 * int(dimension);
      for (j = 0; j < int(dimension); j++) {
        gain += trnSet[t_rowloc + j] * trnSet[t_rowloc + j];
      }
      gain = sqrt(gain);
      int s_rowloc = i * int(dimension);
      for (j = 0; j < int(dimension); j++) {
	shapeCodebook[s_rowloc + j] = trnSet[t_rowloc + j] / gain;
      }
    }
    const int K2 = int(sizeTrnSet)/int(sizeGainCodebook);
    for (i = 0; i < int(sizeGainCodebook); i++) {
      double gain = 0.0;
      int rowloc = i * K2 * int(dimension);
      for (j = 0; j < int(dimension); j++) {
        gain += trnSet[rowloc + j] * trnSet[rowloc + j];
      }
      gainCodebook[i] = sqrt(gain);
    }

    // Store the centroid of each partitioned cluster of training set
    for (i = 0; i < int(sizeShapeCodebook); i++) {
      int rowloc = i * int(dimension);
      for (j = 0; j < int(dimension); j++) {
        shapeCentroid[rowloc + j] = 0;
      }
    }

    for (i = 0; i < int(sizeGainCodebook); i++) {
      gainCentroid[i] = 0;
    }

    // numGainPart[i] store the number of how many input training vectors
    // are partitioned into the region of i_th gain codeword. 
    for (i = 0; i < int(sizeGainCodebook); i++) {
      numGainPart[i] = 0;
    }

    //  Compute the optimum partition for the initial codebooks.
    int count = 0;
    int indexShape = 0;
    int indexGain = 0;
    double distance = 0;
    double oldDistortion = 0;
    double distortion = 0;

    for (i = 0; i < int(sizeTrnSet); i++) {
      Ptdsp_PartitionCodebook(&indexShape, &indexGain, &distance,
			      &(trnSet[i*int(dimension)]), shapeCodebook,
			      int(sizeShapeCodebook), int(dimension),
			      gainCodebook, int(sizeGainCodebook));
      distortion += distance;
      for (j = 0; j < int(dimension); j++) {
        shapeCentroid[indexShape*int(dimension) + j] +=
		gainCodebook[indexGain] * trnSet[i*int(dimension) + j];
      }
    }
    count++;

    do {
      int i,j;
      //  compute optimum shape codebook Cs(m+1) in partition R(Cs(m),Cg(m)).
      for (i = 0; i < int(sizeShapeCodebook); i++) {
	double gain = 0.0;
	int rowloc = i * int(dimension);
        for (j = 0; j < int(dimension); j++) {
          gain += shapeCentroid[rowloc + j] * shapeCentroid[rowloc + j];
        }
        gain = sqrt(gain);
	//  update codeword if gain != 0; otherwise, we'll get division-by-zero
	if (gain != 0.0) {
          for (j = 0; j < int(dimension); j++) {
            shapeCodebook[rowloc + j] = shapeCentroid[rowloc + j] / gain;
	  }
        }
      }

      //  Reset the contents of array gainCentroid to 0.
      for (i = 0; i < int(sizeGainCodebook); i++) {
        gainCentroid[i] = 0;
      }

      //  Reset the contents of array numGainPart to 0.
      for (i = 0; i < int(sizeGainCodebook); i++) {
	numGainPart[i] = 0;
      }

      //  Compute the optimum partition R(Cs(m+1),Cg(m)).
      for (i=0; i<int(sizeTrnSet); i++) {
        Ptdsp_PartitionCodebook(&indexShape, &indexGain, &distance,
				&(trnSet[i*int(dimension)]),
				shapeCodebook, int(sizeShapeCodebook),
				int(dimension), gainCodebook,
				int(sizeGainCodebook));
        (numGainPart[indexGain])++;
	int s_rowloc = indexShape * int(dimension);
	int t_rowloc = i * int(dimension);
        for (j = 0; j < int(dimension); j++) {
          gainCentroid[indexGain] +=
		shapeCodebook[s_rowloc + j] * trnSet[t_rowloc + j];
	}
      }

      //  Compute optimum gain codebook Cg(m+1) in partition R(Cs(m+1),Cg(m+1))
      for (i = 0; i < sizeGainCodebook; i++) {
        //  Update if numGainPart[i] != 0; otherwise, get division-by-zero
	if (numGainPart[i] != 0 ) {
	  if (gainCentroid[i] < 0) 
	    gainCodebook[i] = 0;
	  else
	    gainCodebook[i] = gainCentroid[i] / double(numGainPart[i]);       
	}
      }

      //  Reset the contents of array shapeCentroid to 0.
      for (i = 0; i < int(sizeShapeCodebook); i++) {
	int rowloc = i * int(dimension);
        for (j = 0; j < int(dimension); j++) {
          shapeCentroid[rowloc + j] = 0;
	}
      }

      //  Reset the value of oldDistortion and Distortion.
      oldDistortion = distortion;
      distortion = 0;
	
      //  Compute the optimum partition R(Cs(m+1),Cg(m+1)).
      for (i = 0; i < int(sizeTrnSet); i++) {
        Ptdsp_PartitionCodebook(&indexShape, &indexGain, &distance,
			        &(trnSet[i*int(dimension)]),
			        shapeCodebook, int(sizeShapeCodebook),
			        int(dimension), gainCodebook,
			        int(sizeGainCodebook));
        distortion += distance;
	int s_rowloc = indexShape * int(dimension);
	int t_rowloc = i * int(dimension);
        for (j = 0; j < int(dimension); j++) {
          shapeCentroid[s_rowloc + j] +=
		gainCodebook[indexGain] * trnSet[t_rowloc + j];
	}
      }
    } while ( (oldDistortion-distortion)/oldDistortion>double(thresholdRatio)
	      && (++count)<int(maxIteration) );

    // allocate memory for first output which will be deleted automatically
    FloatMatrix& result1 =
		*(new FloatMatrix(int(sizeShapeCodebook),int(dimension)));
    for (i=0; i<int(sizeShapeCodebook)*int(dimension); i++)
      result1.entry(i) = shapeCodebook[i];
    output%0 << result1;

    // allocate memory for second output which will be deleted automatically
    FloatMatrix& result2 = *(new FloatMatrix(int(sizeGainCodebook),1));
    for (i=0; i<int(sizeGainCodebook); i++)
      result2.entry(i) = gainCodebook[i];
    outGain%0 << result2;
  }

  wrapup {
    int shapeTotalSize = int(sizeShapeCodebook) * int(dimension);
    const char* sf1 = saveShapeCodebook;
    if ( ! doubleArrayAsASCFile(sf1, "%e ", FALSE, shapeCodebook,
			        shapeTotalSize, int(dimension)) ) {
         Error::warn(*this, "Error writing the shape codebook to the file ",
		     sf1);
    }

    int gainTotalSize = int(sizeGainCodebook) * int(dimension);
    const char* sf2 = saveGainCodebook;
    if ( ! doubleArrayAsASCFile(sf2, "%e ", FALSE, gainCodebook, gainTotalSize,
			        int(dimension)) ) {
         Error::warn(*this, "Error writing the shape codebook to the file ",
		     sf1);
    }
  }
}
