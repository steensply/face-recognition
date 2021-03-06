# Clemson FCT Facial Recognition

This repository contains the code for a face recognition system that combines three popular algorithms: PCA, LDA, and ICA.

## Getting Started

New team members should look at CONTRIBUTING.md to learn about our work-flow, especially those who are unfamiliar with Github.

## Testing

Usage for the face recognition system:

    Usage: ./face-rec [options]
    Options:
      --train DIRECTORY  create a database from a training set
      --rec DIRECTORY    test a set of images against a database
      --lda              run PCA, LDA
      --ica              run PCA, ICA2
      --all              run PCA, LDA, ICA2

To run an automated test (k-fold cross-validation) with the ORL face database:

    # test once with 1.pgm removed from each class
    ./cross-validate.sh 1 1

    # repeat with each index removed (takes much longer)
    ./cross-validate.sh 1 10

To test MATLAB code with ORL database:

    ./create-sets.sh 1 10
    ./convert-images.sh test_images orl_faces_ppm pgm ppm
    # run MATLAB/LDA/example.m
    # run MATLAB/PCA/example.m

## The Image Library

This software currently supports a subset of the [Netpbm](https://en.wikipedia.org/wiki/Netpbm_format) format, particularly with PGM and PPM images.

Images should __not__ be stored in this repository! Instead, images should be downloaded separately. Face databases are widely available on the Internet, such as [here](http://web.mit.edu/emeyers/www/face_databases.html) and [here](http://face-rec.org/databases/). I am currently using the [ORL database](http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html).

To convert JPEG images to PGM with ImageMagick:

    convert-images.sh [src-folder] [dst-folder] jpeg pgm

## The Matrix Library

Function Name              | PCA | LDA | ICA | Verification Status
---                        |:---:|:---:|:---:|---
_Constructors, Destructor_ |     |     |     |
m_initialize               |  x  |  x  |  x  | Verified
m_identity                 |     |     |  x  | Verified
m_zeros                    |     |     |  x  | Verified
m_copy                     |     |     |  x  | Verified
m_free                     |  x  |  x  |  x  | Verified
_Input/Output_             |     |     |     |
m_fprint                   |     |  x  |     | Verified
m_fwrite                   |  x  |     |     | Verified
m_fscan                    |     |     |     | Verified
m_fread                    |  x  |     |     | Verified
m_image_read               |  x  |     |     | Verified
m_image_write              |  x  |     |     | Verified
_Getters_                  |     |     |     |
m_covariance               |     |     |  x  | Verified w/ BLAS
m_eigen                    |  x  |  x  |     | Verified w/ BLAS
m_eigen2                   |     | (x) |     | Not Verified
m_inverse                  |     |  x  |  x  | Verified w/ BLAS
m_mean_column              |  x  |  x  |  x  | Verified
m_product                  |  x  |  x  |  x  | Verified w/ BLAS
m_sqrtm                    |     |     |  x  | Verified w/ BLAS
m_transpose                |  x  |  x  |  x  | Verified
_Mutators_                 |     |     |     |
m_add                      |     |  x  |  x  | Verified
m_elem_mult                |     |  x  |  x  | Verified
m_shuffle_columns          |     |     |  x  | Verified
m_subtract                 |     |  x  |  x  | Verified
m_subtract_columns         |  x  |     |     | Verified

#### BLAS and LAPACK

Much of the code in this project depends on BLAS and LAPACK. In order to run it properly, it is necessary to install the following libraries:

    libblas-dev (1.2.20110419-5)
    libblas3 (1.2.20110419-5)
    libgfortran3 (4.8.1-10ubuntu9)
    liblapack-dev (3.4.2+dfsg-2)
    liblapack3 (3.4.2+dfsg-2)
    liblapacke (3.4.2+dfsg-2)
    liblapacke-dev (3.4.2+dfsg-2)

Documentation for BLAS and LAPACK consists mostly of the documentation for each function. For any given BLAS/LAPACK function, you will want to reference two documents:

1. The Fortran source file http://www.netlib.org/lapack/double/
2. The cblas/lapacke header http://www.netlib.org/blas/cblas.h http://www.netlib.org/lapack/lapacke.h

The Fortran source provides documentation for function parameters, and the C headers show how to order those arguments with the C interface.

##### Ubuntu

    sudo apt-get install libblas-dev liblapacke-dev

##### Mac

Confirmed to run on Mac OS 10.11.1

Download LAPACK 3.5.0 http://www.netlib.org/lapack/

Download BLAS 3.5.0 http://www.netlib.org/blas/

(10.10 - 10.11) Download gfortran 5.2 http://coudert.name/software/gfortran-5.2-Yosemite.dmg

(10.7 - 10.9) Download gfortran 4.8.2 http://coudert.name/software/gfortran-4.8.2-MountainLion.dmg

    # in BLAS directory
    make
    sudo cp blas-LINUX.a /usr/local/lib/libblas.a

    # in LAPACK directory
    mv make.inc.example make.inc
    # set BLASLIB in make.inc line 68 equal to ‘/usr/local/lib/libblas.a’
    make
    sudo cp liblapack.a /usr/local/lib

## The Algorithms

Here is the working flow graph for the combined algorithm:

    m = number of dimensions per image
    n = number of images

    train: X -> (a, W', P)
        X = [X_1 ... X_n] (image matrix) (m-by-n)
        a = sum(X_i, 1:i:n) / n (mean face) (m-by-1)
        X = [(X_1 - a) ... (X_n - a)] (mean-subtracted image matrix) (m-by-n)
        W_pca' = PCA(X) (PCA projection matrix) (n-by-m)
        P_pca = W_pca' * X (PCA projected image matrix) (n-by-n)
        W_lda' = LDA(W_pca, P_pca) (LDA projection matrix) (n-by-m)
        P_lda = W_lda' * X (LDA projected image matrix) (n-by-n)
        W_ica' = ICA2(W_pca, P_pca) (ICA2 projection matrix) (n-by-m)
        P_ica = W_ica' * X (ICA2 projected image matrix) (n-by-n)

    recognize: X_test -> P_match
        a = mean face (m-by-1)
        (W_pca, W_lda, W_ica) = projection matrices (m-by-n)
        (P_pca, P_lda, P_ica) = projected image matrices (n-by-n)
        X_test = test image (m-by-1)
        P_test_pca = W_pca' * (X_test - a) (n-by-1)
        P_test_lda = W_lda' * (X_test - a) (n-by-1)
        P_test_ica = W_ica' * (X_test - a) (n-by-1)
        P_match_pca = nearest neighbor of P_test_pca (n-by-1)
        P_match_lda = nearest neighbor of P_test_lda (n-by-1)
        P_match_ica = nearest neighbor of P_test_ica (n-by-1)

    PCA: X -> W_pca'
        X = [X_1 ... X_n] (image matrix) (m-by-n)
        L = X' * X (surrogate matrix) (n-by-n)
        L_ev = eigenvectors of L (n-by-n)
        W_pca = X * L_ev (eigenfaces) (m-by-n)

    LDA: (W_pca, P_pca) -> W_lda'
        X = P_pca (n-by-n)
        c = number of classes
        n_i = size of class i
        U_i = sum(X_j, j in class i) / n_i (n-by-1)
        u = sum(U_i, 1:i:c) / c (n-by-1)
        S_b = sum((U_i - u) * (U_i - u)', 1:i:c) (n-by-n)
        S_w = sum(sum((X_j - U_i) * (X_j - U_i)', j in class i), 1:i:c) (n-by-n)
        W_fld = eigenvectors of (S_b, S_w) (n-by-n)
        W_lda' = W_fld' * W_pca' (n-by-m)

    ICA2: (W_pca, P_pca) -> W_ica'
        X = P_pca (n-by-n)
        W_z = 2 * Cov(X)^(-1/2) (n-by-n)
        X_sph = W_z * X (n-by-n)
        W = (train with sep96) (n-by-n)
        W_I = W * W_z (n-by-n)
        W_ica' = W_I * W_pca' (n-by-m)
