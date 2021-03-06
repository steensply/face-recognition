% Recognizing step....
%
% Description: This function compares two faces by projecting the images into facespace and
% measuring the Euclidean distance between them.
%
% Argument:      TestImage              - Path of the input test image
%
%                m                      - (M*Nx1) Mean of the training
%                                         database, which is output of 'EigenfaceCore' function.
%
%                Eigenfaces             - (M*Nx(P-1)) Eigen vectors of the
%                                         covariance matrix of the training
%                                         database, which is output of 'EigenfaceCore' function.
%
%                A                      - (M*NxP) Matrix of centered image
%                                         vectors, which is output of 'EigenfaceCore' function.
%
% Returns:       OutputName             - Name of the recognized image in the training database.
%
% See also: RESHAPE, STRCAT
%
% Original version by Amir Hossein Omidvarnia, October 2007
%                     Email: aomidvar@ece.ut.ac.ir
%
function OutputName = Recognition(TestImage, m, A, Eigenfaces)

%%%%%%%%%%%%%%%%%%%%%%%% Projecting centered image vectors into facespace
% All centered images are projected into facespace by multiplying in
% Eigenface basis's. Projected vector of each face will be its corresponding
% feature vector.

ProjectedImages = [];
Train_Number = size(Eigenfaces,2);
Test_Number = size(A,2);
% - 1
for i = 1 : Test_Number
    temp = Eigenfaces'*A(:,i); % Projection of centered images into facespace
    ProjectedImages = [ProjectedImages temp];
end
%%%%%%%%%%%%%%%%%%%%%%%% Extracting the PCA features from test image
InputImage = imread(TestImage);
temp = InputImage(:,:,1);
[irow, icol] = size(temp);
InImage = reshape(temp',irow*icol,1);
Difference = double(InImage)-m; % Centered test image

%%%%%%%%%%%%%%%%%%%%%%%% Calculating Euclidean distances
% Euclidean distances between the projected test image and the projection
% of all centered training images are calculated. Test image is
% supposed to have minimum distance with its corresponding image in the
% training database.

Euc_dist = [];

%%% FPGA

ProjectedTestImage = Eigenfaces'*Difference; % Test image feature vector
save projectedtest.txt ProjectedTestImage -ascii
verbose = 0;

truncate = 0;	% perform truncation if 1
truncateverbose = 0;	% output truncation information if 1

if(truncate)
    truncated = 0; % count of truncated eigenvalues
    E3B = 0;	% count of eigenvalues with magnitude of 9.999e3 and below
    E4 = 0;	% magnitude of 1e4 - 9.999e4
    E5 = 0;	% magnitude of 1e5 - 9.999e5
    E6 = 0;	% magnitude of 1e6 - 9.999e6
    E7 = 0;	% magnitude of 1e7 - 9.999e7
    E8 = 0;	% magnitude of 1e8 - 9.999e8
    E9B = 0;	% magnitude of 1e9 or greater

    Absolute = abs(ProjectedImages); % use magnitude of eigenvalues

    for x = 1 : Train_Number
        for y = 1 : Test_Number
            entry = abs(ProjectedImages(x,y));
            if(entry < 1e4) E3B = E3B + 1;
                else if(entry < 1e5) E4 = E4 + 1;
                    else if(entry < 1e6) E5 = E5 + 1;
                        else if(entry < 1e7) E6 = E6 + 1;
                            else if(entry < 1e8) E7 = E7 + 1;
                                else if(entry < 1e9) E8 = E8 + 1;
                                    else E9B = E9B + 1;
                                end
                            end
                        end
                     end
                end
            end

            truncatepoint = 1e6; % cutoff point

            if(entry > truncatepoint) % > or < determines whether you truncate above or below the cutoff point
                truncated = truncated + 1;
                if(ProjectedImages(x,y) < 0) ProjectedImages(x,y) = -truncatepoint; % keep negative sign if original eigenvalue is negative
                else ProjectedImages(x,y) = truncatepoint;
                end
            end
        end
    end
end

if(truncate && truncateverbose) % output min and max eigenvalues as well as truncation statistics from above
    max(max(Absolute))
    min(min(Absolute))

    truncated
    Train_Number^2
    percent = truncated/Train_Number^2 % percent of eigenvalues truncated

    E3B
    E4
    E5
    E6
    E7
    E8
    E9B
end

if(verbose)
    size(ProjectedImages)
    size(ProjectedImages(:,i))
    ProjectedImages
    size(ProjectedTestImage)
    ProjectedTestImage
    Absolute = abs(ProjectedImages)
    max(max(Absolute))
    min(min(Absolute))
    size(Eigenfaces')
end

ProjectedTestImage

for i = 1 : Test_Number
    q = ProjectedImages(:,i);
    temp = ( norm (ProjectedTestImage - q)  )^2;
    Euc_dist = [Euc_dist; temp];
end

%%% FPGA

Euc_dist

[Euc_dist_min , OutputName] = min(Euc_dist);
Euc_dist_min
OutputName