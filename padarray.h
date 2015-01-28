#ifndef PADARRAY_H
#define PADARRAY_H

#include <iostream>
#include <string.h>
using namespace std;
//feat: the original array
//featdimensions: the original dimensions of the matrix which is stored as array in feat
//padx and pady are extending dimensions
double *padarray(const double *feat, const int *featdims, const int padx, const int pady, int *paddeddims);
#endif
