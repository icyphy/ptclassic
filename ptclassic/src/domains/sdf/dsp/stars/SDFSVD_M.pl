defstar {
    name 	{ SVD_M }
    domain 	{ SDF }
    version	{ $Id$ }
    author	{ Mike J. Chen }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location    { SDF dsp library }
    descriptor	{
Compute the singular-value decomposition (SVD) of a Toeplitz data matrix A
by decomposing A into A = UWV', where U and V are orthogonal matrices
and V' represents the transpose of V.
W is a diagonal matrix composed of the singular values of A, and
the columns of U and V are the left and right singular vectors of A.
    } 
    explanation {
.pp
The singular-value decomposition is performed on a data matrix, which
is usually provided by the
.c
Data_M
star.
The dimensions of the input data matrix is given by the
\fIrows\fR and \fIcols\fR parameters to the star.
The outputs are the three matrices: the vector of singular values,
the matrix of right singular vectors, and the matrix of left singular vectors.
The terminals for each output is labeled on the star's icon as
S, R, and L respectively.
.pp
The \fIthreshold\fR parameter gives the smallest floating point number
that the algorithm will represent.
Anything smaller is considered zero.
The \fImax_iterations\fR parameter allows the user to control the number of
iterations that the SVD algorithm will be allowed to run before stopping.
Normally, the SVD algorithm should converge before this value but this
parameter is provided to prevent non-convergent matrices from causing
the star from running too long.
.pp
The user can also speed up the execution of the star by optionally
specifying that the matrices of the left and/or right singular vectors 
should not be generated.
Not generating those matrices will speed up the execution.
The vector of singular values is always generated.
.Id "Haykin, S."
.UH "References"
.ip [1]
S. Haykin, \fIModern Filters\fR, pp. 333-335,
Macmillan Publishing Company, New York, 1989.
.SA
Data_M
    }
    defstate {
	name 	{ rows }
	type 	{ int }
	default	{ 2 }	
	desc	{ Number of rows in A. }
    }
    defstate {
	name 	{ cols }
	type	{ int }
	default { 2 }
	desc	{ Number of columns in A. }
    }
    defstate {
	name	{ threshold }
	type	{ float }
	default { 0.00000000000000001 }
	desc	{ Threshold for singularities.  Values less than threshold are
		  assumed equal to zero. }
    }
    defstate {
	name	{ max_iterations }
	type	{ int }
	default	{ 30 }
	desc    { Maximum number of iterations for the SVD routine to converge. }
    }
    defstate {
	name	{ generate_left }
	type	{ int }
	default	{ "YES" }
	desc    { Specify whether to generate U, the matrix of left singular vectors. }
    }
    defstate {
	name	{ generate_right }
	type	{ int }
	default	{ "YES" }
	desc    { Specify whether to generate V, the matrix of right singular vectors. }
    }
    input {
	name	{ input }
	type	{ FLOAT_MATRIX_ENV }
	desc	{ Input stream. }
    }
    output {
	name 	{ svals }
	type	{ FLOAT_MATRIX_ENV }
	desc	{ The singular values of X. }
    }
    output {
	name	{ rsvec }
	type	{ FLOAT_MATRIX_ENV }
	desc	{ Right singular vectors of X. }
    }
    output {
	name	{ lsvec }
	type	{ FLOAT_MATRIX_ENV }
	desc	{ Left singular vectors of X. }
    }
    ccinclude { <minmax.h> }
    protected	{
	int N,nrows,ncols;
        FloatMatrix *U, *W, *V;
    }
    setup {
	nrows = (int)rows;
	ncols = (int)cols;
    }
    hinclude 	{ <math.h>, "Matrix.h" }
    go {
	// These matricies will hold the results.
        // Don't delete these, let envelope class handle that

        // matrix of left singular vectors
        // replace this matrix at the end of svd
        LOG_NEW; U = new FloatMatrix(nrows,ncols);
        // vector of singular values 
        LOG_NEW; W = new FloatMatrix(ncols,1);    
        // matrix of right singular vectors
        LOG_NEW; V = new FloatMatrix(ncols,ncols);

	// Get the input matrix and call the svd function.
        Envelope inputPkt;
        (input%0).getMessage(inputPkt);
        // check for empty input, possibly caused by delays
        if(inputPkt.empty()) {
          FloatMatrix A(nrows,ncols);
          A = 0;
          svd(A,*U,*V,*W,(double)threshold,(int)generate_left,(int)generate_right);
        }
        else {
          const FloatMatrix& A = *(const FloatMatrix*)inputPkt.myData();
          svd(A,*U,*V,*W,(double)threshold,(int)generate_left,(int)generate_right);
        }

	// There are ncols singular values, a ncolsx1 matrix
        // Output singular values
        svals%0 << *W;

	// Output left singular vectors, a nrowsxncols matrix
        lsvec%0 << *U;

        // Output right singular vectors, a ncolsxncols matrix
        rsvec%0 << *V;
	
    }		
    method {
      name     { hypot }
      access   { protected }
      type     { double }
      arglist  { "(double a, double b)" }
      code     {
       /* computes the function sqrt(a*a + b*b) in a way which is less likely
        * to overflow.
      
        * version of the code by Bob Craig.
        */
        double r;
 
        if(a == 0)
          return b;
        else if(b == 0)
          return a;
        else {
          a = fabs(a);
          b = fabs(b);
          if(a < b) {
            r = a; a = b; b = r;
          }
          r = b/a;
          return(a*sqrt(1.0 + r*r));
        }
      } // end hypot code
    } // end hypot method
     
    method {
      name     { svd }
      access   { protected }
      type     { void }
      arglist  { "(const FloatMatrix& A,FloatMatrix& U,FloatMatrix& V,FloatMatrix& W,double threshhold, int withU, int withV)" }
      code     {

/*
*   This is an SVD algorithm by Wilkinson & Reinsch, translated from Algol 60
*   to C++.  The original code is from Handbook for Automatic Computation,
*   Vol II-Linear Algebra, pg 141-144.
*
*   The routine takes a single matrix (A) and computes two
*   additional matrices (U and V) and a vector W such that
*               A = UWV'
*   The calling sequence is
*               svd(A,U,V,W,threshold,withU,withV)
*   where
*               A is the original, m by n matrix
*               U is the matrix of left singular vectors
*               W is the vector of singular values
*               V is the matrix of right singular vectors
*               threshold is a convergence test constant
*               withU != 0 if the U matrix is to be computed
*               withV != 0 if the V matrix is to be computed
*
*   The A matrix is not changed.
*   The U matrix is computed only if withU != 0.
*   The V matrix is computed only if withV != 0.
*
*   Modifications to W&R's code by Mike J. Chen:
*       removed goto's
*       support of m<n
*       more bulletproof (prevents divide by zero and better protection
*                         against underflow and overflow)
*
*   The ideas for the last modifications come from Bob Craig, 
*   kat3@ihlpe.att.com.
*/

        int i, j, k, l = 0;             // Mostly loop variables
        double tol = 1.0e-22;           // tells about machine tolerance
        double c, f, g, h, s, x, y, z;  // Temporaries

        int numRows = A.numRows();
        int numCols = A.numCols();

        // Create a temporary working array
        double *temp = new double[numCols];

        // Copy A to U
        U = A;

        // Reduce the U matrix to bidiagonal form with Householder transforms.

        g = x = 0.0;
        for(i = 0; i < numCols; i++) {
          temp[i] = g;
          s = 0.0;
          l = i + 1;
          for (j = i; j < numRows; j++)
            s += U[j][i] * U[j][i];
          if (s < tol)
            g = 0.0;
          else {
            f = U[i][i];
            g = (f < 0.0) ? sqrt(s) : -sqrt(s);
            h = f * g - s;
            U[i][i] = f - g;
            for(j = l; j < numCols; j++) {
              s = 0.0;
              for(k = i; k < numRows; ++k)
                s += U[k][i] * U[k][j];
              f = s / h;
              for(k = i; k < numRows; ++k)
                U[k][j] += f * U[k][i];
            }
          }
          W.entry(i) = g;
          s = 0.0;
          for (j = l; j < numCols; j++)
            s += U[i][j] * U[i][j];
          if (s < tol)
            g = 0.0;
          else {
            f = U[i][i + 1];
            g = (f < 0.0) ? sqrt(s) : -sqrt(s);
            h = f * g - s;
            U[i][i + 1] = f - g;
            for (j = l; j < numCols; j++)
              temp[j] = U[i][j] / h;
            for (j = l; j < numRows; j++) {
              s = 0.0;
              for (k = l; k < numCols; ++k)
                s += U[j][k] * U[i][k];
              for (k = l; k < numCols; ++k)
                U[j][k] += s * temp[k];
            }
          }
          y = fabs(W.entry(i)) + fabs(temp[i]);
          if (y > x)
            x = y;
        }

        if(withV) {         // Now accumulate right-hand transforms
          for (i = numCols - 1; i >= 0; i--) {
            if (g != 0.0) {
              h = U[i][i + 1] * g;
              for (j = l; j < numCols; j++)
                V[j][i] = U[i][j] / h;
              for (j = l; j < numCols; j++) {
                s = 0.0;
                for (k = l; k < numCols; k++)
                  s += U[i][k] * V[k][j];
                for (k = l; k < numCols; k++)
                  V[k][j] += s * V[k][i];
              }
            }
            for (j = l; j < numCols; j++)
              V[i][j] = V[j][i] = 0.0;
  
            V[i][i] = 1.0;
            g = temp[i];
            l = i;
          }
        }

        if(withU) {         // Now accumulate the left-hand transforms.
          for (i = min(numRows,numCols) - 1; i >= 0; i--) {
            l = i + 1;
            g = W.entry(i);
            for (j = l; j < numCols; j++)
              U[i][j] = 0.0;
            if (g != 0.0) {
              h = U[i][i] * g;
              for (j = l; j < numCols; j++) {
                s = 0.0;
                for (k = l; k < numRows; k++)
                  s += U[k][i] * U[k][j];
                f = s / h;
                for (k = i; k < numRows; k++)
                  U[k][j] += f * U[k][i];
              }
              for (j = i; j < numRows; j++)
                U[j][i] /= g;
            }
            else {
              for (j = i; j < numRows; j++)
                U[j][i] = 0.0;
            }
            U[i][i] += 1.0;
          }
        }

        // Now diagonalise the bidiagonal form.

        threshhold = threshhold * x;
        for (k = numCols - 1; k >= 0; k--) {
          for( int its = 0; its < (int)max_iterations; its++) {
            // test splitting 
            for (l = k; l >= 0; l--) {
              if (fabs(temp[l]) <= threshhold) 
                break;          // goto testconvergence
              // there is no problem with l-1 < 0 for the index below
              // because temp[0] = 0 always so above will always break first
              if (fabs(W.entry(l-1)) <= threshhold) {
                // Cancellation of temp[l] if l > 0;
                c = 0.0;
                s = 1.0;
                for (i = l; i <= k; i++) {
                  f = s * temp[i];
                  temp[i] *= c;
                  if (fabs(f) <= threshhold) 
                    break;  // break out of for loop
                  g = W.entry(i);
                  h = W.entry(i) = hypot(f,g);
                  if(h != 0) {
                    c = g/h;
                    s = -f/h;  
                  }
                  if(withU) {
                    for (j = 0; j < numRows; j++) {
                      y = U[j][l];
                      z = U[j][i];
                      U[j][l] = y * c + z * s;
                      U[j][i] = -y * s + z * c;
                    }
                  }
                }
                break;  // goto testconvergence
              }
            }
            // testconvergence
  
            z = W.entry(k);
            if (l == k) {
              // convergence
              if (z < 0.0) {
                // W[k] is made non-negative.
                W.entry(k) = -z;
                if(withV) {
                  for (j = 0; j < numCols; j++)
                    V[j][k] = -V[j][k];
                }
              }
              break; // break out of test splitting 
            }
            if( its == (int)max_iterations - 1) {
              Error::abortRun(*this,"no convergence in SVD routine");
              return;
            }
    
            // Shift from bottom 2x2 minor.
            x = W.entry(l);
            y = W.entry(k - 1);
            g = temp[k - 1];
            h = temp[k];
            f = ((y - z)*(y + z) + (g - h)*(g + h)) / (2 * h) / y;
            g = hypot(f,1.0);
            f = ((x - z)*(x + z) + h*(y/((f < 0.0)?f-g:f+g) - h)) / x;
   
            // Next QR transformation.
  
            c = (s = 1);
            for (i = l+1; i <= k; i++) {
              g = temp[i];
              y = W.entry(i);
              h = s * g;
              g *= c;
              temp[i - 1] = z = hypot(f,h);
              if(z != 0) {
                c = f / z;
                s = h/z;
              }
              f = x * c + g * s;
              g = -x * s + g * c;
              h = y * s;
              y *= c;
              if(withV) {
                for (j = 0; j < numCols; j++) {
                  x = V[j][i - 1];
                  z = V[j][i];
                  V[j][i - 1] = x * c + z * s;
                  V[j][i] = -x * s + z * c;
                }
              }
              W.entry(i-1) = z = hypot(f,h);
              if(z != 0) {
                c = f / z;
                s = h / z;
              }
              f = c * g + s * y;
              x = -s * g + c * y;
              for (j = 0 ; j < numRows; j++) {
                y = U[j][i - 1];
                z = U[j][i];
                U[j][i - 1] = y * c + z * s;
                U[j][i] = -y * s + z * c;
              }
            }
            temp[l] = 0.0;
            temp[k] = f;
            W.entry(k) = x;
          } // end test splitting iteration loop
        }  // end k loop
	delete [] temp;
      } // close code
    } // end method
} // end defstar
