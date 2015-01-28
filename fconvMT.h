#ifndef FCONVMT_H
#define FCONVMT_H

#include <iostream>
#include <string.h>
using namespace std;
double **fconvMT(double *featrp,const int *featdims, double **filters, const int *filterdims, int start, int end, int *dims);
#endif
