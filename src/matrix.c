/**
 * @file matrix.c
 *
 * Implementation of the matrix library.
 */
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <cblas.h>
#include <lapacke.h>
#include "matrix.h"

/**
 * Construct a matrix.
 *
 * @param rows
 * @param cols
 * @return pointer to a new matrix
 */
matrix_t * m_initialize (int rows, int cols)
{
	matrix_t *M = (matrix_t *)malloc(sizeof(matrix_t));
	M->rows = rows;
	M->cols = cols;
	M->data = (precision_t *) malloc(rows * cols * sizeof(precision_t));

	return M;
}

/**
 * Construct an identity matrix.
 *
 * @param rows
 * @return pointer to a new identity matrix
 */
matrix_t * m_identity (int rows)
{
	matrix_t *M = (matrix_t *)malloc(sizeof(matrix_t));
	M->rows = rows;
	M->cols = rows;
	M->data = (precision_t *) calloc(rows * rows, sizeof(precision_t));

	int i;
	for ( i = 0; i < rows; i++ ) {
		elem(M, i, i) = 1;
	}

	return M;
}

/**
 * Construct a zero matrix.
 *
 * @param rows
 * @param cols
 * @return pointer to a new zero matrix
 */
matrix_t * m_zeros (int rows, int cols)
{
	matrix_t *M = (matrix_t *)malloc(sizeof(matrix_t));
	M->rows = rows;
	M->cols = cols;
	M->data = (precision_t *) calloc(rows * cols, sizeof(precision_t));

	return M;
}

/**
 * Copy a matrix.
 *
 * @param M  pointer to matrix
 * @return pointer to copy of M
 */
matrix_t * m_copy (matrix_t *M)
{
	matrix_t *C = m_initialize(M->rows, M->cols);

	memcpy(C->data, M->data, C->rows * C->cols * sizeof(precision_t));

	return C;
}

/**
 * Deconstruct a matrix.
 *
 * @param M  pointer to matrix
 */
void m_free (matrix_t *M)
{
	free(M->data);
	free(M);
}

/**
 * Write a matrix in text format to a stream.
 *
 * @param stream  pointer to file stream
 * @param M       pointer to matrix
 */
void m_fprint (FILE *stream, matrix_t *M)
{
	fprintf(stream, "%d %d\n", M->rows, M->cols);

	int i, j;
	for ( i = 0; i < M->rows; i++ ) {
		for ( j = 0; j < M->cols; j++ ) {
			fprintf(stream, "%lg ", elem(M, i, j));
		}
		fprintf(stream, "\n");
	}
}

/**
 * Write a matrix in binary format to a stream.
 *
 * @param stream  pointer to file stream
 * @param M       pointer to matrix
 */
void m_fwrite (FILE *stream, matrix_t *M)
{
	fwrite(&M->rows, sizeof(int), 1, stream);
	fwrite(&M->cols, sizeof(int), 1, stream);
	fwrite(M->data, sizeof(precision_t), M->rows * M->cols, stream);
}

/**
 * Read a matrix in text format from a stream.
 *
 * @param stream  pointer to file stream
 * @return pointer to new matrix
 */
matrix_t * m_fscan (FILE *stream)
{
	int rows, cols;
	fscanf(stream, "%d %d", &rows, &cols);

	matrix_t *M = m_initialize(rows, cols);
	int i, j;
	for ( i = 0; i < rows; i++ ) {
		for ( j = 0; j < cols; j++ ) {
			fscanf(stream, "%lf", &(elem(M, i, j)));
		}
	}

	return M;
}

/**
 * Read a matrix in binary format from a stream.
 *
 * @param stream  pointer to file stream
 * @return pointer to new matrix
 */
matrix_t * m_fread (FILE *stream)
{
	int rows, cols;
	fread(&rows, sizeof(int), 1, stream);
	fread(&cols, sizeof(int), 1, stream);

	matrix_t *M = m_initialize(rows, cols);
	fread(M->data, sizeof(precision_t), M->rows * M->cols, stream);

	return M;
}

/**
 * Read a column vector from a PPM image.
 *
 * Color images are converted to grayscale.
 *
 * @param M      pointer to matrix
 * @param col    column index
 * @param image  pointer to PPM image
 */
void m_ppm_read (matrix_t *M, int col, ppm_t *image)
{
	assert(M->rows == image->height * image->width);

	int i;
	for ( i = 0; i < M->rows; i++ ) {
		elem(M, i, col) =
			0.299 * (precision_t) image->pixels[3 * i] +
			0.587 * (precision_t) image->pixels[3 * i + 1] +
			0.114 * (precision_t) image->pixels[3 * i + 2];
	}
}

/**
 * Write a column of a matrix to a PPM image.
 *
 * @param M      pointer to matrix
 * @param col    column index
 * @param image  pointer to PPM image
 */
void m_ppm_write (matrix_t *M, int col, ppm_t *image)
{
	assert(M->rows == image->height * image->width);

	int i;
	for ( i = 0; i < M->rows; i++ ) {
		memset(&image->pixels[3 * i], (char) elem(M, i, col), 3);
	}
}

/**
 * Compute the real eigenvalues and right eigenvectors of a matrix.
 *
 * The eigenvalues are returned as a column vector, and the
 * eigenvectors are returned as column vectors. The i-th
 * eigenvalue corresponds to the i-th column vector.
 *
 * @param M	      pointer to matrix, m-by-n
 * @param M_eval  pointer to eigenvalues matrix, m-by-1
 * @param M_evec  pointer to eigenvectors matrix, m-by-n
 */
void m_eigenvalues_eigenvectors (matrix_t *M, matrix_t *M_eval, matrix_t *M_evec)
{
	matrix_t *M_work = m_copy(M);
	precision_t *wi = (precision_t *)malloc(M->rows * sizeof(precision_t));

	LAPACKE_dgeev(LAPACK_COL_MAJOR, 'N', 'V',
		M->cols, M_work->data, M->rows,  // input matrix
		M_eval->data, wi,					  // real, imag eigenvalues
		NULL, M->rows,					  // left eigenvectors
		M_evec->data, M->rows);			 // right eigenvectors

	m_free(M_work);
	free(wi);
}

/**
 * Get the product of two matrices.
 *
 * @param A  pointer to left matrix
 * @param B  pointer to right matrix
 * @return pointer to new matrix equal to A * B
 */
matrix_t * m_matrix_multiply (matrix_t *A, matrix_t *B)
{
	assert(A->cols == B->rows);

	matrix_t *C = m_zeros(A->rows, B->cols);

	// C := alpha * op(A) * op(B) + beta * C, alpha = 1, beta = 0
	cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
		A->rows, B->cols, A->cols,
		1, A->data, A->rows, B->data, B->rows,
		0, C->data, C->rows);

	return C;
}

/**
 * Get the mean column of a matrix.
 *
 * @param M  pointer to matrix
 * @return pointer to mean column vector
 */
matrix_t * m_mean_column (matrix_t *M)
{
	matrix_t *a = m_zeros(M->rows, 1);

	int i, j;
	for ( i = 0; i < M->cols; i++ ) {
		for ( j = 0; j < M->rows; j++ ) {
			elem(a, j, 0) += elem(M, j, i);
		}
	}

	for ( i = 0; i < M->rows; i++ ) {
		elem(a, i, 0) /= M->cols;
	}

	return a;
}

/**
 * Get the transpose of a matrix.
 *
 * @param M  pointer to matrix
 * @return pointer to new transposed matrix
 */
matrix_t * m_transpose (matrix_t *M)
{
	matrix_t *T = m_initialize(M->cols, M->rows);

	int i, j;
	for ( i = 0; i < T->rows; i++ ) {
		for ( j = 0; j < T->cols; j++ ) {
			elem(T, i, j) = elem(M, j, i);
		}
	}

	return T;
}

/**
 * Subtract a "mean" column vector from each column in a matrix.
 *
 * @param M  pointer to matrix
 * @param a  pointer to column vector
 */
void m_normalize_columns (matrix_t *M, matrix_t *a)
{
	int i, j;
	for ( i = 0; i < M->cols; i++ ) {
		for ( j = 0; j < M->rows; j++ ) {
			elem(M, j, i) -= elem(a, j, 0);
		}
	}
}

/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GROUP 2 FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~  */
// These operate on input matrix M and will change the data stored in M
//2.0
//2.0.0
/*******************************************************************************
 * m_flipCols
 *
 * Swaps columns in M from left to right
 *
 * ICA:
 * 		void fliplr(data_t *outmatrix, data_t *matrix, int rows, int cols)
*******************************************************************************/
void m_flipCols (matrix_t *M) {
	int i, j;
	precision_t temp;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols / 2; j++) {
			temp = elem(M, i, j);
			elem(M, i, j) = elem(M, i, M->cols - j - 1);
			elem(M, i, M->cols - j - 1) = temp;
		}
	}
}

/*******************************************************************************
 * void normalize(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * normalizes the matrix
*******************************************************************************/
void m_normalize (matrix_t *M) {
	int i, j;
	precision_t max, min, val;
	min = elem(M, 0, 0);
	max = min;

	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			val = elem(M, i, j);
			if (min > val) {
				min = val;
			}
			if (max < val) {
				max = val;
			}
		}
	}
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = (elem (M, i, j) - min) / (max - min);
		}
	}
}

//2.0.1
/*******************************************************************************
 * m_truncateAll
 *
 * Truncates the entries in matrix M
 *
 * ICA:
 * 		void fix(data_t *outmatrix, data_t *matrix, int rows, int cols);
*******************************************************************************/
void m_elem_truncate (matrix_t *M) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = ((precision_t) ((int)elem(M, i, j)));
		}
	}
}

/*******************************************************************************
 * m_acosAll
 *
 * applies acos to all matrix elements
 *
 * ICA:
 * 		 void matrix_acos(data_t *outmatrix, data_t *matrix, int rows, int cols);
*******************************************************************************/
void m_elem_acos (matrix_t *M) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = acos (elem(M, i, j));
		}
	}
}

/*******************************************************************************
 * void matrix_sqrt(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * applies sqrt to all matrix elements
*******************************************************************************/

void m_elem_sqrt (matrix_t *M) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = sqrt(elem(M, i, j));
		}
	}
}

/*******************************************************************************
 * void matrix_negate(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * negates all matrix elements
*******************************************************************************/
void m_elem_negate (matrix_t *M) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = - elem(M, i, j);
		}
	}
}

/*******************************************************************************
 * void matrix_exp(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * raises e to all matrix elements
*******************************************************************************/
void m_elem_exp (matrix_t *M) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = exp ( elem(M, i, j) );
		}
	}
}

//2.0.2
/*******************************************************************************
 * void raise_matrix_to_power(data_t *outmatrix, data_t *matrix, int rows, int cols, int scalar);
 *
 * raises all matrix elements to a power
*******************************************************************************/
void m_elem_pow (matrix_t *M, precision_t num) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) = pow ( elem(M, i, j) , num);
		}
	}
}

/*******************************************************************************
 * void scale_matrix(data_t *outmatrix, data_t *matrix, int rows, int cols, int scalar);
 *
 * scales matrix by contant
*******************************************************************************/
void m_elem_mult (matrix_t *M, precision_t x) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) =  elem(M, i, j) * x;
		}
	}
}

/*******************************************************************************
 * void divide_by_constant(data_t *outmatrix, data_t *matrix, int rows, int cols, data_t scalar);
 *
 * divides matrix by contant
*******************************************************************************/
void m_elem_divideByConst (matrix_t *M, precision_t x) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) =  elem(M, i, j) / x;
		}
	}
}

/*******************************************************************************
 * void divide_scaler_by_matrix(data_t *outmatrix, data_t *matrix, int rows, int cols, data_t scalar) ;
 *
 * divides constant by matrix element-wise
*******************************************************************************/
void m_elem_divideByMatrix (matrix_t *M, precision_t num) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) =  num / elem(M, i, j);
		}
	}
}

/*******************************************************************************
 * void sum_scalar_matrix(data_t *outmatrix, data_t *matrix, int rows, int cols, data_t scalar);
 *
 * adds element-wise matrix to contant
*******************************************************************************/
void m_elem_add (matrix_t *M, precision_t x) {
	int i, j;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			elem(M, i, j) =  elem(M, i, j) + x;
		}
	}
}

//2.1
//2.1.0
/*******************************************************************************
 * void sum_columns(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * sums the columns of a matrix, returns a row vector
*******************************************************************************/
matrix_t * m_sumCols (matrix_t *M) {
	matrix_t *R = m_initialize(1, M->cols);
	int i, j;
	precision_t val;
	for (j = 0; j < M->cols; j++) {
		val = 0;
		for (i = 0; i < M->rows; i++) {
			val += elem(M, i, j);
		}
		elem(R, 0, j) = val;
	}

	return R;
}

/*******************************************************************************
 * void mean_of_matrix(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * takes the mean value of each column
*******************************************************************************/
matrix_t *m_meanCols (matrix_t *M) {
	matrix_t *R = m_sumCols (M);
	int i;
	for (i = 0; i < M->cols; i++) {
		elem(R, 0, i) = elem(R, 0, i) / M->rows;
	}
	return R;
}

//2.1.1
/*******************************************************************************
 * void sum_rows(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * sums the rows of a matrix, returns a col vect
*******************************************************************************/
matrix_t * m_sumRows (matrix_t *M) {
	matrix_t *R = m_initialize(M->rows, 1);
	int i, j;
	precision_t val;
	for (i = 0; i < M->rows; i++) {
		val = 0;
		for (j = 0; j < M->cols; j++) {
			val += elem(M, i, j);
		}
		elem(R, i, 0) = val;
	}

	return R;
}

/*******************************************************************************
 * void find(data_t *outvect, data_t **matrix, int rows, int cols);
 * NOTE: this also assumes that outvect is a column vector)
 * places the row indeces of non-zero elements in a vector
 * This vector has additional, non-used space, not sure what to do about this -miller
*******************************************************************************/
matrix_t * m_findNonZeros (matrix_t *M) {
	matrix_t *R = m_zeros(M->rows * M->cols, 1);
	precision_t val;
	int i, j, count = 0;
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			val = elem(M, i, j);
			if (val != 0) {
				elem(R, count, 0) = (precision_t) (i + 1);
				count++;
			}
		}
	}
	return R;
}

//2.1.2
/*******************************************************************************
 * void reshape(data_t **outmatrix, int outRows, int outCols, data_t **matrix, int rows, int cols)
 *
 * reshapes matrix by changing dimensions, keeping data
*******************************************************************************/
matrix_t *m_reshape (matrix_t *M, int newNumRows, int newNumCols) {
	assert (M->rows * M->cols == newNumRows * newNumCols);
	int i;
	int r1, c1, r2, c2;
	matrix_t *R = m_initialize(newNumRows, newNumCols);
	for (i = 0; i < newNumRows * newNumCols; i++) {
		r1 = i / newNumCols;
		c1 = i % newNumCols;
		r2 = i / M->cols;
		c2 = i % M->cols;
		elem(R, r1, c1) = elem(M, r2, c2);
	}

	return R;
}

/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GROUP 3 FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~  */
// Temp moved this here for dependancy issues. Group 3 code person should work on this
// for right now.
// TODO - Documentation & Free's - Miller 10/30
// UPDATE 03/02/16: make sure to compile time include -llapacke
// NEEDS VERIFICATION - Greg
void m_inverseMatrix (matrix_t *M) {
	//assert(M->rows == M->cols);
	int info;
	//int lwork = M->rows * M->cols;
	int *ipiv = malloc((M->rows + 1) * sizeof(int));
	//precision_t *work = malloc(lwork * sizeof(precision_t));
	//		  (rows   , columns, matrix , lda	, ipiv, info );
	info=LAPACKE_dgetrf(LAPACK_ROW_MAJOR,M->cols, M->rows, M->data, M->rows, ipiv);
	if(info!=0){
		//printf("\nINFO != 0\n");
		exit(1);
	}
	//printf("\ndgertrf passed\n");
	//		  (order  , matrix, Leading Dim, IPIV,
	info=LAPACKE_dgetri(LAPACK_ROW_MAJOR,M->cols,M->data,M->rows, ipiv);
	if(info!=0){
		//printf("\nINFO2 != 0\n");
		exit(1);
	}
}

/*******************************************************************************
 * data_t norm(data_t *matrix, int rows, int cols);
 *
 * NOTE: I think the norm def I looked up was different. I kept this one for
 * right now but we need to look at that again
*******************************************************************************/
precision_t m_norm (matrix_t *M, int specRow) {
	int i, j;
	precision_t val, sum = 0;

	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			val = elem(M, i, j);
			sum += val * val;
		}
	}

	return sqrt (sum);
}

/*******************************************************************************
 * void matrix_sqrtm(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * matrix square root
 *  element-wise square rooting of eigenvalues matrix
 *  divide eigenvectors matrix by the product of the e-vectors and e-values
*******************************************************************************/
matrix_t * m_sqrtm (matrix_t *M) {
	/*
	matrix_t *eigenvectors;
	matrix_t *eigenvalues;
	// TODO: EIGENVALUES NOT CURRENTLY WORKING
	m_eigenvalues_eigenvectors (M, &eigenvalues, &eigenvectors);

	m_elem_sqrt (eigenvalues);

	matrix_t * temp = m_matrix_multiply (eigenvectors, eigenvalues, 0);
	m_free (eigenvalues);
	matrix_t * R = m_matrix_division (temp, eigenvectors);
	m_free (temp);
	m_free(eigenvectors);
	return R;*/
	return M;
}

/*******************************************************************************
 * void determinant(data_t *matrix, int rows, double *determ);
 *
 * find the determinant of the matrix
*******************************************************************************/
precision_t m_determinant (matrix_t *M) {
	//int i, j, j1, j2;
	int i, j, r, c, k, sign;
	precision_t det = 0, val;
	matrix_t *A = NULL;
	assert (M->cols == M->rows);

	if (M->rows < 1)   printf("error finding determinant\n");
	else if (M->rows == 1) det = elem(M, 0, 0); // Shouldn't get used
	else if (M->rows == 2) det = elem(M, 0, 0) * elem(M, 1, 1) - elem(M, 1, 0) * elem(M, 0, 1);
	else {

		det = 0;
		A = m_initialize(M->rows - 1, M->cols - 1);
		for (j = 0; j < M->cols; j++) {
			// Fill matrix
			c = 0;
			for (k = 0; k < M->cols; k++) {
				if (k == j) continue; // skip over columns that are the same
				for (i = 1; i < M->rows; i++) {
					r = i - 1;
					elem(A, r, c) = elem(M, i, k);
				}
				c++;
			}
			val = m_determinant (A);
			sign = 1 - 2 * ((i % 2) ^ (j % 2));;
			det += sign * elem(M, 0, j) * val;
		}
		m_free (A);

	}
	return det;
}

/*******************************************************************************
 * void cofactor(data_t *outmatrix,data_t *matrix, int rows);
 *
 * cofactor a matrix
*******************************************************************************/
matrix_t * m_cofactor (matrix_t *M) {
	//int i, j, ii, jj, i1, j1;
	int i, j, r, c, row, col, sign;
	assert (M->rows == M->cols);
	matrix_t *A = m_initialize(M->rows - 1, M->cols - 1);
	matrix_t *R = m_initialize(M->rows, M->cols);
	precision_t val;

	// For every element in M
	for (i = 0; i < M->rows; i++) {
		for (j = 0; j < M->cols; j++) {
			// Make matrix of values not sharing this column/row
			for (r = 0, row = 0; r < M->rows; r++) {
				if (i == r) continue;
				for (c = 0, col = 0; c < M->cols; c++) {
					if (j == c) continue;
					elem(A, row, col) = elem(M, r, c);
					col++;
				}
				row++;
			}
			val = m_determinant (A);
			sign = 1 - 2 * ((i % 2) ^ (j % 2)); // I think this is illegal
			val *= sign;
			elem(R, j, i) = val;
		}
	}
	m_free (A);
	return R;
}

/*******************************************************************************
 * void covariance(data_t *outmatrix, data_t *matrix, int rows, int cols);
 *
 * return the covariance matrix
*******************************************************************************/
matrix_t * m_covariance(matrix_t *M) {
	int i, j, k;
	precision_t val;
	matrix_t *colAvgs = m_meanCols(M);
	matrix_t *norm = m_initialize(M->rows, M->cols);
	matrix_t *R = m_initialize(M->rows, M->cols);

	for (j = 0; j < M->cols; j++) {
		for (i = 0; i < M->rows; i++) {
			val = elem(M, i, j) - elem(colAvgs, 0, j);
		}
	}

	for (j = 0; j < M->cols; j++) {
		for (k = 0; k < M->cols; k++) {
			val = 0;
			for (i = 0; i < M->rows; i++) {
				val += elem(norm, i, j) * elem(norm, i, j);
			}
			val /= M->cols - 1;
			elem(R, j, k) = val;
		}
	}

	m_free (colAvgs);
	m_free (norm);

	return R;
}

/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GROUP 4 FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~  */
/*  These functions manipulate multiple matrices and return a matrix of
 *   *  equivalent dimensions.
 *	*/
/*******************************************************************************
 *  * void subtract_matrices(data_t *outmatrix, data_t *matrix1, data_t *matrix2, int rows, int cols);
 *   *
 *	* return the difference of two matrices element-wise
 *	*******************************************************************************/
matrix_t * m_dot_subtract (matrix_t *A, matrix_t *B) {
	assert (A->rows == B->rows && A->cols == B->cols);
	matrix_t *R = m_initialize(A->rows, A->cols);
	int i, j;
	for (i = 0; i < A->rows; i++) {
		for (j = 0; j < A->cols; j++) {
			elem(R, i, j) = elem(A, i, j) - elem(B, i, j);
		}
	}
	return R;
}

/*******************************************************************************
 *  * void add_matrices(data_t *outmatrix, data_t *matrix1, data_t *matrix2, int rows, int cols);
 *   *
 *	* element wise sum of two matrices
 *	*******************************************************************************/
matrix_t * m_dot_add (matrix_t *A, matrix_t *B) {
	assert (A->rows == B->rows && A->cols == B->cols);
	matrix_t *R = m_initialize(A->rows, A->cols);
	int i, j;
	for (i = 0; i < A->rows; i++) {
		for (j = 0; j < A->cols; j++) {
			elem(R, i, j) = elem(A, i, j) + elem(B, i, j);
		}
	}
	return R;
}

/*******************************************************************************
 *  * void matrix_dot_division(data_t *outmatrix, data_t *matrix1, data_t *matrix2, int rows, int cols);
 *   *
 *	* element wise division of two matrices
 *	*******************************************************************************/
matrix_t * m_dot_division (matrix_t *A, matrix_t *B) {
	assert (A->rows == B->rows && A->cols == B->cols);
	matrix_t *R = m_initialize(A->rows, A->cols);
	int i, j;
	for (i = 0; i < A->rows; i++) {
		for (j = 0; j < A->cols; j++) {
			elem(R, i, j) = elem(A, i, j) / elem(B, i, j);
		}
	}
	return R;
}

/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GROUP 5 FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~  */
/*  These functions manipulate a multiple matrices but return a matrix of
 *  inequivalent dimensions.*/
/*******************************************************************************
 * void matrix_division(data_t *outmatrix, data_t *matrix1, data_t *matrix2, int rows1, int cols1, int rows2, int cols2);
 *
 * multiply one matrix by the inverse of another
*******************************************************************************/
matrix_t * m_matrix_division (matrix_t *A, matrix_t *B) {
	matrix_t *C = m_copy (B);
	m_inverseMatrix (C);
	matrix_t *R = m_matrix_multiply (A, C);
	m_free (C);
	return R;
}

/*******************************************************************************
 * m_reorderCols
 *
 * This reorders the columns of input matrix M to the order specified by V into output matrix R
 *
 * Note:
 * 		V must have 1 row and the number of columns as M
 *
 * ICA:
 * 		void reorder_matrix(data_t *outmatrix, data_t *matrix, int rows, int cols, data_t *rowVect);
*******************************************************************************/
matrix_t * m_reorder_columns (matrix_t *M, matrix_t *V) {
	assert (M->cols == V->cols && V->rows == 1);

	int i, j, row;
	matrix_t *R = m_initialize(M->rows, M->cols);
	for (j = 0; j < R->cols; j++) {
		row = elem(V, 1, j);
		for (i = 0; i < R->rows; i++) {
			elem(R, i, j) = elem(M, row, j);
		}
	}
	return R;
}