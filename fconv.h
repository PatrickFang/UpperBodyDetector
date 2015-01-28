#ifndef FCONV_H
#define FCONV_H

#include <iostream>
#include <string.h>
using namespace std;
double **fconv(double *featrp,const int *featdims, double **filters, const int *filterdims, int start, int end, int *dims);
#endif
