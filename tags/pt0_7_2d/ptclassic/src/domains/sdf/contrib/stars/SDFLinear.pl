defstar {
    name { Linear }
    domain { SDF }
    desc {
This is the output function described in [1]. f(x) = 0.5*(|x+k| - |x-k|) when
x is between -k and k. When x is greater than k, the output is k. When x is 
less than -k, the output is -k. k is a non-negative number.
  }
    author { Biao Lu }
    acknowledge { Brian L. Evans }
    location  { SDF contrib library }
    version { @(#)SDFLinear.pl	1.1	03/24/98 }
    copyright {
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    htmldoc {
This work is based on the cellular neural network [1].
<p>
REFERENCES
<ol>
<li>L. O. Chua and L. Yang, <em>"Cellular neural networks: theory"</em>,
<em>IEEE Transactions on Circuits and Systems</em>, vol. 35, no. 10, 
pp. 1257-1272, Oct. 1988
</ol>
    }
    input {
	name { input }
	type { FLOAT_MATRIX_ENV }
    }
  
    output {
	name { output }
	type { FLOAT_MATRIX_ENV }
	desc { 
Output matrix is calculated by the output function given above.
        }
    }
 
    defstate {
      name { k_value }
      type { float }
      default { 1.0 }
      desc {
Users can set the value of k to meet their requirements.
  }
    }
    ccinclude { "Matrix.h" }
    go {
      // get inputs
      Envelope Apkt;
      (input%0).getMessage(Apkt);
      const FloatMatrix& Amatrix = *(const FloatMatrix *)Apkt.myData();
            
      // check empty
      if (Apkt.empty()) {
	output %0 << double(0);
      }
      else {
	//valid input matrix;
	int height = Amatrix.numRows();
	int width  = Amatrix.numCols();
	    
	FloatMatrix& result = *(new FloatMatrix(height,width));
	for (int n2 = 0; n2 < height; n2++) {
	  for (int n1 = 0; n1 < width; n1++) {
	    if (Amatrix[n2][n1] >= double(k_value)) {
	      result[n2][n1] = k_value;
	    }
	    else {
	      if (Amatrix[n2][n1] <= double(-k_value)) {
		result[n2][n1] = -k_value;
	      }
	      else {
		result[n2][n1] = Amatrix[n2][n1];
	      }
	    }
	  }
	}
	output%0 << result;
      }
    }
}

    
