#!/bin/bash
# Cross validation for PCA, LDA, ICA, based on the ORL face
# database, which should be located at ./orl_faces.
#
# EXAMPLES
#
# Perform k-fold on a single observation (2.pgm):
# ./cross-validate.sh 2 2
#
# Perform k-fold on a range of observations (4.pgm - 7.pgm):
# ./cross-validate.sh 4 7
#
# Perform k-fold for all observations:
# ./cross-validate.sh 1 10

# parse arguments
if [ "$#" = 4 ]; then
    START=$1
    END=$2
    ARGS="$3 $4"
elif [ "$#" = 3 ]; then
    START=$1
    END=$2
    ARGS="$3"
elif [ "$#" = 2 ]; then
    START=$1
    END=$2
    ARGS=""
else
    >&2 echo "usage: ./cross-validate.sh [begin-index] [end-index] [--lda --ica --all]"
    exit 1
fi

# build executables
make

echo "Performing k-fold cross-validation on the range [$START, $END]"
echo

for (( i = $START; i <= $END; i++ )); do
    echo "BEGIN: remove $i.pgm from each class"
    echo

    # create the training set and test set
    ./create-sets.sh $i

    # run the algorithms
    ./face-rec --train train_images --rec test_images $ARGS
done
