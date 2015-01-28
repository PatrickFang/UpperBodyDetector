#ifndef ME_HAARDETECTOPENCV_H
#define ME_HAARDETECTOPENCV_H

#include <iostream>
#include <string.h>
using namespace std;
//feat: the original array
//featdimensions: the original dimensions of the matrix which is stored as array in feat
//padx and pady are extending dimensions
double *me_HaarDetectOpenCV(char const*facemodel_path, double *grayimage, int min_size_x , int min_size_y, double scale_factor, int min_neighbors, int *dims, int *numofdet);
#endif
