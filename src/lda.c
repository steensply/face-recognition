/**
 * @file lda.c
 *
 * Implementation of LDA (Belhumeur et al., 1996; Zhao et al., 1998).
 */
#include "database.h"
#include "matrix.h"
#include <stdlib.h>

/**
 * Compute the scatter matrices S_w and S_b for a set of images.
 *
 * @param X        pointer to input matrix
 * @param c        number of classes
 * @param entries  list of entries for each column of X
 * @param S_b      pointer to store between-scatter matrix
 * @param S_w      pointer to store within-scatter matrix
 */
void m_scatter(matrix_t *X, int c, database_entry_t *entries, matrix_t *S_b, matrix_t *S_w)
{
    matrix_t **X_classes = (matrix_t **)malloc(c * sizeof(matrix_t *));
    matrix_t **U = (matrix_t **)malloc(c * sizeof(matrix_t *));

    // compute the mean of each class
    int i, j;
    for ( i = 0, j = 0; i < c; i++ ) {
        // extract the columns of X in class i
        int k;
        for ( k = j; k < X->cols; k++ ) {
            if ( entries[k].class != entries[j].class ) {
                break;
            }
        }

        X_classes[i] = m_copy_columns(X, j, k);
        j = k;

        // compute the mean of the class
        U[i] = m_mean_column(X_classes[i]);
    }

    // compute the mean of all classes
    matrix_t *u = m_initialize(X->rows, 1);  // m_mean_column(U);

    for ( i = 0; i < c; i++ ) {
        for ( j = 0; j < X->rows; j++ ) {
            elem(u, j, 0) += elem(U[i], j, 0);
        }
    }
    for ( i = 0; i < X->rows; i++ ) {
        elem(u, i, 0) /= c;
    }

    // compute the between-scatter S_b = sum(S_b_i, i=1:c)
    // compute the within-scatter S_w = sum(S_w_i, i=1:c)
    for ( i = 0; i < c; i++ ) {
        matrix_t *X_class = X_classes[i];

        // compute S_b_i = n_i * (u_i - u) * (u_i - u)'
        matrix_t *u_i = m_copy(U[i]);
        m_subtract(u_i, u);

        matrix_t *u_i_tr = m_transpose(u_i);
        matrix_t *S_b_i = m_product(u_i, u_i_tr);
        m_elem_mult(S_b_i, X_class->cols);

        m_add(S_b, S_b_i);

        // compute S_w_i = X_class * X_class', X_class is mean-subtracted
        m_subtract_columns(X_class, U[i]);

        matrix_t *X_class_tr = m_transpose(X_class);
        matrix_t *S_w_i = m_product(X_class, X_class_tr);

        m_add(S_w, S_w_i);

        // cleanup
        m_free(u_i);
        m_free(u_i_tr);
        m_free(S_b_i);
        m_free(X_class_tr);
        m_free(S_w_i);
    }

    // cleanup
    for ( i = 0; i < c; i++ ) {
        m_free(X_classes[i]);
        m_free(U[i]);
    }
    free(X_classes);
    free(U);
}

/**
 * Compute the projection matrix of a training set with LDA.
 *
 * @param W_pca_tr  PCA projection matrix
 * @param P_pca     PCA projected images
 * @param c         number of classes
 * @param entries   list of entries for each image
 * @return projection matrix W_lda'
 */
matrix_t * LDA(matrix_t *W_pca_tr, matrix_t *P_pca, int c, database_entry_t *entries)
{
    // TODO: take only the first n - c columns of P_pca

    // compute scatter matrices S_b and S_w
    matrix_t *S_b = m_zeros(P_pca->rows, P_pca->rows);
    matrix_t *S_w = m_zeros(P_pca->rows, P_pca->rows);

    m_scatter(P_pca, c, entries, S_b, S_w);

    // compute W_fld = eigenvectors of S_w^-1 * S_b
    // TODO: implement with LAPACKE_dggev
    matrix_t *S_w_inv = m_inverse(S_w);
    matrix_t *J = m_product(S_w_inv, S_b);
    matrix_t *J_eval = m_initialize(J->rows, 1);
    matrix_t *J_evec = m_initialize(J->rows, J->cols);

    m_eigen(J, J_eval, J_evec);

    // TODO: take only the first c - 1 columns of J_evec
    matrix_t *W_fld = J_evec;

    matrix_t *W_fld_tr = m_transpose(W_fld);

    // compute W_lda' = W_fld' * W_pca'
    matrix_t *W_lda_tr = m_product(W_fld_tr, W_pca_tr);

    m_free(S_b);
    m_free(S_w);
    m_free(S_w_inv);
    m_free(J);
    m_free(J_eval);
    m_free(J_evec); // W_fld
    m_free(W_fld_tr);

    return W_lda_tr;
}
