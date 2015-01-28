using namespace std;
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "MODEL.h"
#include "CImg.h"
using namespace cimg_library;
/*
 * Fast image subsampling.
 * This is used to construct the feature pyramid.
 */

// struct used for caching interpolation values
struct alphainfo {
  int si, di;
  double alpha;
};
 
// copy src into dst using pre-computed interpolation values
void alphacopy(double *src, double *dst, struct alphainfo *ofs, int n) {
  struct alphainfo *end = ofs + n;
  while (ofs != end) {
    dst[ofs->di] += ofs->alpha * src[ofs->si];
    ofs++;
  }
}

// resize along each column
// result is transposed, so we can apply it twice for a complete resize
void resize1dtran(double *src, int sheight, double *dst, int dheight, 
		  int width, int chan) {
  double scale = (double)dheight/(double)sheight;
  double invscale = (double)sheight/(double)dheight;
  
  // we cache the interpolation values since they can be 
  // shared among different columns
  int len = (int)ceil(dheight*invscale) + 2*dheight;
  alphainfo ofs[len];
  int k = 0;
  for (int dy = 0; dy < dheight; dy++) {
    double fsy1 = dy * invscale;
    double fsy2 = fsy1 + invscale;
    int sy1 = (int)ceil(fsy1);
    int sy2 = (int)floor(fsy2);       

    if (sy1 - fsy1 > 1e-3) {
      //assert(k < len);
      //assert(sy-1 >= 0);
      ofs[k].di = dy*width;
      ofs[k].si = sy1-1;
      ofs[k++].alpha = (sy1 - fsy1) * scale;
    }

    for (int sy = sy1; sy < sy2; sy++) {
     // assert(k < len);
      //assert(sy < sheight);
      ofs[k].di = dy*width;
      ofs[k].si = sy;
      ofs[k++].alpha = scale;
    }

    if (fsy2 - sy2 > 1e-3) {
     // assert(k < len);
     // assert(sy2 < sheight);
      ofs[k].di = dy*width;
      ofs[k].si = sy2;
      ofs[k++].alpha = (fsy2 - sy2) * scale;
    }
  }

  // resize each column of each color channel
  bzero(dst, chan*width*dheight*sizeof(double));
  for (int c = 0; c < chan; c++) {
    for (int x = 0; x < width; x++) {
      double *s = src + c*width*sheight + x*sheight;
      double *d = dst + c*width*dheight + x;
      alphacopy(s, d, ofs, k);
    }
  }
}

// main function
// takes a double color image and a scaling factor 
// returns resized image
double *resize(double* src, const double scale, double* sdims) {//maxscale is a number, ==>1x1 not a vector!
  								//mxGetPr(pm) pm is array of double. returns the address of the first element in the array
								//here src points to image matrix which has the dimensions of mxnx3
								//sdims is the pointer the dimentsions of image which is like [m n 3]\	
  //if (mxGetNumberOfDimensions(mxsrc) != 3 || 			//should be 3d matrix
  //    mxGetClassID(mxsrc) != mxDOUBLE_CLASS)			//make sure the matrix of image is double 
  // cout<<"Invalid input"<<endl;  

 			//get the value of scale
  if (scale > 1)						//make sure is less than 1
    cout<<"Invalid scaling factor"<<endl;   

 /* int ddims[3];							//adjust the dimensions of image.. original is [m n 3]
  ddims[0] = (int)round(sdims[0]*scale);			//adjust the first and second dimensions ==> the m and n
  ddims[1] = (int)round(sdims[1]*scale);			//NOTE: call the adjusted m and n, am and an
  ddims[2] = sdims[2];						//constant 3 doesnt change
*/
  //make a copy of the passed in dimensions, and update the new dimensions
	int sdimscopy[3] = {sdims[0], sdims[1], sdims[2]};
	sdims[0] = (int)round(sdims[0]*scale);			//adjust the first and second dimensions ==> the m and n
  	sdims[1] = (int)round(sdims[1]*scale);			//NOTE: call the adjusted m and n, am and an
  	sdims[2] = sdims[2];						

  //mxArray *mxdst = mxCreateNumericArray(3, ddims, mxDOUBLE_CLASS, mxREAL);	//declaration for the output 
										//mxdst the output is 3d double, real, matrix of am by an by 3
  //double *dst = (double *)calloc(ddims[0]*ddims[1]*ddims[2], sizeof(double));	//dst points to mxdst, the 3d matrix	
 	double *dst = (double *)calloc(sdims[0]*sdims[1]*sdims[2], sizeof(double));	

  //double *tmp = (double *)calloc(ddims[0]*sdims[1]*sdims[2], sizeof(double));	//dynamically allocate temp as double of size, refer this function as calloc
 	double *tmp = (double *)calloc(sdims[0]*sdimscopy[1]*sdimscopy[2], sizeof(double));	
  //resize1dtran(src, sdims[0], tmp, ddims[0], sdims[1], sdims[2]);		//up to here matlab part is done, the rest is coded in C
  //resize1dtran(tmp, sdims[1], dst, ddims[1], ddims[0], sdims[2]);
	resize1dtran(src, sdimscopy[0], tmp, sdims[0], sdimscopy[1], sdimscopy[2]);		//up to here matlab part is done, the rest is coded in C
	resize1dtran(tmp, sdimscopy[1], dst, sdims[1], sdims[0], sdimscopy[2]);

  free(tmp);
  return dst;
}

// matlab entry point
// dst = resize(src, scale)
// image should be color with double values



