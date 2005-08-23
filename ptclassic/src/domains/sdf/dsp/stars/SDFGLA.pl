defstar {
	name { GLA }
	domain { SDF }
	desc {  
Use the Generalized Lloyd Algorithm (GLA) to yield a codebook from input  
training vectors.  Note that each input matrix will be viewed as a
row vector in row by row. Each row of output matrix represent a
codeword of the codebook. 
	}
	version { @(#)SDFGLA.pl	1.13	7/7/96 }
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
		desc {
Input matrix will be viewed as a row vector in row major ordering to
represent a training vector.
		}
	}
	output {
		name { output }
		type { FLOAT_MATRIX_ENV }
		desc {
Output codebook, i.e. (sizeCodebook)*(dimension) matrix.
		}
	}
	defstate {
		name { dimension }
		type { int }
		default { 16 }
		desc { The dimension for the input training vector }
	}
	defstate {
		name { sizeTrnSet }
		type { int }
		default { 1024 }
		desc {
The number of input training vectors for yielding the codebook
		}
	}
	defstate {
		name { sizeCodebook }
		type { int }
		default { 64 }
		desc { The number of desired codewords for the codebook }
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
		desc { Upper limit on the iterations in the Lloyd algorithm. }
	}
	defstate {
		name { saveFile }
		type { string }
		default { "" }
		desc { File to save the final codebook. }
	}
	ccinclude { "Matrix.h", "ptdspNearestNeighbor.h", "WriteASCIIFiles.h" }

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
		delete [] codebook;
		delete [] trnSet;
		delete [] numPart;
		delete [] centroid;
		delete [] halfCodewordPower;
	}

	setup {
		if ( int(sizeCodebook) > int(sizeTrnSet) ) {
		     Error::abortRun(*this,
			"The size of codebook must be smaller than ",
			"the size of training set");
		     return;
		}

		input.setSDFParams(int(sizeTrnSet), int(sizeTrnSet)-1);

		//  Allocate an array for storing the generated codebook
		delete [] codebook;
		codebook = new double[int(sizeCodebook)*int(dimension)];

		//  Allocate an array for storing the training vectors
		delete [] trnSet;
		trnSet = new double[int(sizeTrnSet)*int(dimension)];

		//  Allocate an array to counter how many input training
		// vectors are closest to the i_th codeword
		delete [] numPart;
		numPart = new int[int(sizeCodebook)];

		// Allocate an array to store the centroids of the codebook
		delete [] centroid;
		centroid = new double[int(sizeCodebook)*int(dimension)];

		// halfCodewordPower[i] store the value of Ai=||Yi||^2/2.
		delete [] halfCodewordPower;
		halfCodewordPower = new double[int(sizeCodebook)];
	}

	go {
		// Get input training vectors and store them in
		// the two-dimension array
		Envelope inpkt;
		FloatMatrix zerovector(1, int(dimension));
		zerovector = 0;
		int i;
		for (i = 0; i < int(sizeTrnSet); i++) {
		    const FloatMatrix* vectorp;
		    (input%(int(sizeTrnSet)-1-i)).getMessage(inpkt);
		    vectorp = (const FloatMatrix *)inpkt.myData();
		    // check for "null" matrix inputs, caused by delays:
		    // treat as zero matrix
		    if (inpkt.empty()) {
			vectorp = &zerovector;
		    }
		    if ( vectorp->numRows()*vectorp->numCols() !=
			int(dimension) ) {
			Error::abortRun(*this,
			    "The number of elements in input vector(matrix) ",
			    "must equal to the parameter 'dimension'.");
			return;
		    }
		    int rowloc = i * int(dimension);
		    for (int j = 0; j < int(dimension); j++) {
			trnSet[rowloc + j] = vectorp->entry(j);
		    }
		}
 
		// Initialize initial codebook using random code, i.e., we take 
		// every Kth training vector among the input traing vectors,
		// where K = sizeTrnSet/sizeCodebook.
		const int K = int(sizeTrnSet)/int(sizeCodebook);
		for (i = 0; i < int(sizeCodebook); i++) {
		     int rowloc = i * int(dimension);
		     for (int j = 0; j < int(dimension); j++) {
			codebook[rowloc + j] = trnSet[K * rowloc + j];
		     }
		}

		// numPart[i] stores the number of how many input training
		// vectors are closest to the i_th codeword in the codebook
		// after using the nearest neighbor condition to partition
		// the training set.
		for (i = 0; i < int(sizeCodebook); i++) {
		    numPart[i] = 0;
		}

		// The centroid[int(sizeCodebook)*int(dimension)] array stores
		// the centroid of each partitioned cluster of training set.
		for (i = 0; i < int(sizeCodebook); i++) {
		    int rowloc = i * int(dimension);
		    for (int j = 0; j < int(dimension); j++) {
			centroid[rowloc + j] = 0;
		    }
		}

		// Set halfCodewordPower[i] for the initial codebook.
		for (i = 0; i < int(sizeCodebook); i++ ) {
		    double sum = 0.0;
		    int rowloc = i * int(dimension);
		    for (int j = 0; j < int(dimension); j++ ) {
			double entry = codebook[rowloc + j];
			sum += entry * entry;
		    }
		    halfCodewordPower[i] = sum/2;
		}

		// Run Lloyd iteration for the initial codebook.

		int count = 0;
		int index = 0;
		double distance = 0;
		double oldDistortion = 0;
		double distortion = 0;

		for (i = 0; i < int(sizeTrnSet); i++) {
		    Ptdsp_NearestNeighbor(&index, &distance,
					  &trnSet[i*int(dimension)],
					  codebook, halfCodewordPower,
					  int(sizeCodebook), int(dimension));
		    (numPart[index])++;
		    distortion += distance;
		    for (int j = 0; j < int(dimension); j++) {
			centroid[index*int(dimension)+j] += trnSet[i*int(dimension)+j];
		    }
		}
	
		count++;
	
		do {
		    int i;
		    // Reset the improved codebook to the centroid of
		    // partitioned cluser set
		    for (i = 0; i < int(sizeCodebook); i++) {
			int rowloc = i * int(dimension);
			// Update ith code word
			// Avoid division-by-zero error when numPart[i] is zero
			if ( numPart[i] ) {
			    for (int j = 0; j < int(dimension); j++) {
				codebook[rowloc + j] =
				    centroid[rowloc + j] / numPart[i];
			    }
			}
		    }
	
		    // Reset the contents of array numPart to 0.
		    for (i = 0; i < int(sizeCodebook); i++) {
			numPart[i] = 0;
		    }
	
		    //  Reset the contents of array centroid to 0.
		    for (i = 0; i < int(sizeCodebook); i++) {
			int rowloc = i * int(dimension);
			for (int j = 0; j < int(dimension); j++) {
			    centroid[rowloc + j] = 0;
			}
		    }
	
		    // Precompute Ai=||Yi||^2/2 for the given codebook and
		    // store them in the array halfCodewordPower.
		    for (i = 0; i < int(sizeCodebook); i++ ) {
			double power = 0.0;
			int rowloc = i * int(dimension);
			for ( int j = 0; j < int(dimension); j++ ) {
			    double entry = codebook[rowloc + j];
			    power += entry * entry;
			}
			halfCodewordPower[i] = power/2;
		    }
	
		    // Reset the value of oldDistortion and Distortion.
		    oldDistortion = distortion;
		    distortion = 0;
		
		    // Lloyd iteration :
		    // For a given codebook, partition the training set into
		    // cluster sets using the nearest neighbor condition, and
		    // find the average distortion for the codebook. Then, find
		    // centroid of each partitioned cluster set.
		    for (i = 0; i < int(sizeTrnSet); i++) {
			Ptdsp_NearestNeighbor(&index, &distance,
					      &(trnSet[i*int(dimension)]),
					      codebook, halfCodewordPower,
					      int(sizeCodebook),
					      int(dimension));
			(numPart[index])++;
			distortion += distance;
			int t_rowloc = i * int(dimension);
			int c_rowloc = index * int(dimension);
			for (int j = 0; j < int(dimension); j++) {
			    centroid[c_rowloc + j] += trnSet[t_rowloc + j];
			}
		    }
		} while ( (oldDistortion-distortion)/oldDistortion >
			    double(thresholdRatio) &&
			  (++count) < int(maxIteration) );
	
		// allocate dynamic memory for output which will be
		// automatically deleted
		FloatMatrix& result = *(new FloatMatrix(int(sizeCodebook),
							int(dimension)));
		for (i = 0; i < int(sizeCodebook)*int(dimension); i++) {
		    result.entry(i) = codebook[i];
		}
		output%0 << result;
	}

	wrapup {
		int totalSize = int(sizeCodebook) * int(dimension);
		const char* sf = saveFile;
                if ( ! doubleArrayAsASCFile(sf, "%e ", FALSE, codebook,
					    totalSize, int(dimension)) ) {
		    Error::warn(*this,
				"Error writing the codebook to the file ",
				sf); 
		}
	}
}
