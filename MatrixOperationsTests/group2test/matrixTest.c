// matrix test
#define XDIM 6
#define YDIM 6

#include <stdio.h>
#include <stdlib.h>
#include "matrixOperations1.h"
#include "matrixOperations2.h"
#include "matrixOperations3.h"
#include "matrixOperations4.h"
#include "matrixOperations5.h"
//#include "matrixOperations6.h"

int main (void) {
	
	FILE *output = fopen ("testResults.txt", "w");

	matrix_t *M = m_initialize (FILL, XDIM, YDIM);

	fprintf (output, "M = \n");
	m_fprint (output, M);

	// Test Group 2.0.0
	fprintf (output, "\n-------------Test Group 2.0.0 -------------\n");
	m_flipCols (M);
	fprintf (output, "m_flipCols(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_normalize (M);
	fprintf (output, "m_normalize(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_normalize (M);
    m_elem_mult(M, 35);
    fprintf (output, "m_normalize(M)\n");
	fprintf (output, "m_elem_mult(M, 35) =\n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_inverseMatrix (M);
	fprintf (output, "m_inverseMatrix(M) = \n");
	m_fprint (output, M);
	m_free (M);

    M = m_initialize (IDENTITY, XDIM, YDIM);
    m_inverseMatrix (M);
    fprintf (output, "This test will take the inverse of the identity\n");
    fprintf (output, "m_inverseMatrix(M) = \n");
    m_fprint (output, M);
    m_free (M);

	// Test Group 2.0.1
	fprintf (output, "\n-------------Test Group 2.0.1 -------------\n");
	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_truncate (M);
	fprintf (output, "m_elem_truncate(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
    m_elem_divideByConst(M, 6);
	m_elem_truncate (M);
    fprintf (output, "m_divide_by_constant(M, 6)\n");
	fprintf (output, "m_elem_truncate(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_acos (M);
	fprintf (output, "m_elem_acos(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_sqrt (M);
	fprintf (output, "m_elem_sqrt(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_negate (M);
	fprintf (output, "m_elem_negate(M) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_exp (M);
	fprintf (output, "m_elem_exp(M) = \n");
	m_fprint (output, M);
	m_free (M);

	// Test Group 2.0.2
	fprintf (output, "\n-------------Test Group 2.0.2 -------------\n");
	precision x = 2.0;
	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_pow (M, x);
	fprintf (output, "m_elem_pow(M, x) = \n");
	m_fprint (output, M);
	m_free (M);


	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_mult (M, x);
	fprintf (output, "m_elem_mult(M, x) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_divideByConst (M, x);
	fprintf (output, "m_elem_divideByConst(M, x) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_divideByMatrix (M, x);
	fprintf (output, "m_elem_divideByMatrix(M, x) = \n");
	m_fprint (output, M);
	m_free (M);

	M = m_initialize (FILL, XDIM, YDIM);
	m_elem_add (M, x);
	fprintf (output, "m_elem_add(M, x) = \n");
	m_fprint (output, M);
	m_free (M);

	// Test Group 2.1.0
	fprintf (output, "\n-------------Test Group 2.1.0 -------------\n");
	matrix_t *R;
	M = m_initialize (FILL, XDIM, YDIM);
	
	R = m_sumCols (M);
	fprintf (output, "m_sumCols(M) = \n");
	m_fprint (output, R);
	m_free (R);

	R = m_meanCols (M);
	fprintf (output, "m_meanCols(M) = \n");
	m_fprint (output, R);
	m_free (R);

	// Test Group 2.1.1
	fprintf (output, "\n-------------Test Group 2.1.1 -------------\n");
	R = m_sumRows (M);
	fprintf (output, "m_sumRows(M) = \n");
	m_fprint (output, R);
	m_free (R);

	R = m_meanRows (M);
	fprintf (output, "m_meanRows(M) = \n");
	m_fprint (output, R);
	m_free (R);

	elem (M, 1, 2) = 0.0;  elem (M, 5, 5) = 0.0;
	elem (M, 4, 5) = 0.0;  elem (M, 5, 4) = 0.0;
	R = m_findNonZeros (M);
	fprintf (output, "m_findNonZeros(M) = \n");
	m_fprint (output, R);
	m_free (R);
	m_free (M);

	// Test Group 2.1.2
	fprintf (output, "\n-------------Test Group 2.1.2 -------------\n");
	matrix_t *A = m_initialize (FILL, XDIM, YDIM);
	fprintf (output, "A = \n");
	m_fprint (output, A);
	
	R = m_transpose (A);
	fprintf (output, "m_transpose (A) = \n");
	m_fprint (output, R);
	m_free (R);

	R = m_reshape (A, XDIM / 2, YDIM * 2);
	fprintf (output, "m_reshape (A, XDIM / 2, YDIM * 2) = \n");
	m_fprint (output, R);
	m_free (R);

	// Test Group 3
	fprintf (output, "\n-------------Test Group 3 -------------\n");
	M = m_initialize (FILL, XDIM, YDIM);
	
	fprintf (output, "m_norm (M, specRow) is SKIPPED IN THIS TEST\n");
	
	R = m_sqrtm (M);
	fprintf (output, "m_sqrtm(M) = \n");
	m_fprint (output, R);
	m_free (R);

	
	precision val = m_determinant (M);
	fprintf (output, "m_determinant(M) = %lf\n", val);

	R = m_cofactor (M);
	fprintf (output, "m_cofactor(M) = \n");
	m_fprint (output, R);
	m_free (R);

    matrix_t *N = m_initialize (FILL, 3, 3);
	R = m_cofactor (N);
    fprintf (output, "Three-by-Three matrix N\n");
	fprintf (output, "m_cofactor(N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    N = m_initialize (FILL, 2, 2);
	R = m_cofactor (N);
    fprintf (output, "Two-by-Two matrix N\n");
	fprintf (output, "m_cofactor(N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    N = m_initialize (IDENTITY, XDIM, YDIM);
	R = m_cofactor (N);
    fprintf (output, "Identity matrix 6x6 N\n");
    fprintf (output, "m_determinant(N)\n");
    fprintf (output, "%lf\n", m_determinant (N));
	fprintf (output, "m_cofactor(N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    N = m_initialize (IDENTITY, 5, 5);
	R = m_cofactor (N);
    fprintf (output, "Identity matrix 5x5 N\n");
    fprintf (output, "m_determinant(N)\n");
    fprintf (output, "%lf\n", m_determinant (N));
    fprintf (output, "m_cofactor(N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    N = m_initialize (IDENTITY, 4, 4);
	R = m_cofactor (N);
    fprintf (output, "Identity matrix 4x4 N\n");
    fprintf (output, "m_determinant (N)\n");
    fprintf (output, "%lf\n", m_determinant (N));
    fprintf (output, "m_cofactor (N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    N = m_initialize (IDENTITY, 3, 3);
	R = m_cofactor (N);
    fprintf (output, "Identity matrix 3x3 N\n");
    fprintf (output, "m_determinant (N)\n");
    fprintf (output, "%lf\n", m_determinant (N));
    fprintf (output, "m_cofactor (N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    N = m_initialize (IDENTITY, 2, 2);
	R = m_cofactor (N);
    fprintf (output, "Identity matrix 2x2 N\n");
    fprintf (output, "m_determinant (N)\n");
    fprintf (output, "%lf\n", m_determinant (N));
    fprintf (output, "m_cofactor (N) = \n");
	m_fprint (output, R);
	m_free (R);
    m_free (N);

    R = m_covariance (M);
	fprintf (output, "m_covariance(M) = \n");
	m_fprint (output, R);
	m_free (R);

	//m_free (eigenvectors);
	//m_free (eigenvalues);
	m_free (M);
	m_free (B);
	m_free (A);
	m_free (R);

	return 0;
}
