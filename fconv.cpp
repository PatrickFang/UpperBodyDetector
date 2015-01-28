using namespace std;
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "MODEL.h"


/*
 * This code is used for computing filter responses.  It computes the
 * response of a set of filters with a feature map.  
 *
 * Basic version, relatively slow but very compatible.
 */

struct thread_data {
  double *A;
  double *B;
  double *C;
  //mxArray *mxC;
 //const mwSize *A_dims;
 // const mwSize *B_dims;
 // mwSize C_dims[2];

//NOte: I changed the mwsize type to int
//seems no difference
  const int *A_dims;
  const int *B_dims;
  int C_dims[2];
};

// convolve A and B
void *process(void *thread_arg) {
  thread_data *args = (thread_data *)thread_arg;
  double *A = args->A;
  double *B = args->B;
  double *C = args->C;
  //const mwSize *A_dims = args->A_dims;
  //const mwSize *B_dims = args->B_dims;
  //const mwSize *C_dims = args->C_dims;
  //replace the type by int	
  const int *A_dims = args->A_dims;
  const int *B_dims = args->B_dims;
  const int *C_dims = args->C_dims;
  int num_features = args->A_dims[2];

  for (int f = 0; f < num_features; f++) {
    double *dst = C;
    double *A_src = A + f*A_dims[0]*A_dims[1];      
    double *B_src = B + f*B_dims[0]*B_dims[1];
    for (int x = 0; x < C_dims[1]; x++) {
      for (int y = 0; y < C_dims[0]; y++) {
	double val = 0;
	for (int xp = 0; xp < B_dims[1]; xp++) {
	  double *A_off = A_src + (x+xp)*A_dims[0] + y;
	  double *B_off = B_src + xp*B_dims[0];
	  switch(B_dims[0]) {
	  case 20: val += A_off[19] * B_off[19];
	  case 19: val += A_off[18] * B_off[18];
	  case 18: val += A_off[17] * B_off[17];
	  case 17: val += A_off[16] * B_off[16];
	  case 16: val += A_off[15] * B_off[15];
	  case 15: val += A_off[14] * B_off[14];
	  case 14: val += A_off[13] * B_off[13];
	  case 13: val += A_off[12] * B_off[12];
	  case 12: val += A_off[11] * B_off[11];
	  case 11: val += A_off[10] * B_off[10];
	  case 10: val += A_off[9] * B_off[9];
	  case 9: val += A_off[8] * B_off[8];
	  case 8: val += A_off[7] * B_off[7];
	  case 7: val += A_off[6] * B_off[6];
	  case 6: val += A_off[5] * B_off[5];
	  case 5: val += A_off[4] * B_off[4];
	  case 4: val += A_off[3] * B_off[3];
	  case 3: val += A_off[2] * B_off[2];
	  case 2: val += A_off[1] * B_off[1];
	  case 1: val += A_off[0] * B_off[0];
	    break;
	  default:	    	      
	    for (int yp = 0; yp < B_dims[0]; yp++) {
	      val += *(A_off++) * *(B_off++);
	    }
	  }
	}
	//original it is now only used to produce rootmatch
	//this fconv doesnt have threads but its ok because rootmatch is small and len=1
	*(dst++) += val;
	
      }
    }
  }
}




// matlab entry point
// C = fconv(A, cell of B, start, end);
//void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) { 	//this function takes 4 arguments
double **fconv(double *featrp,const int *featdims, double **filters, const int *filterdims, int start, int end, int *dims){  
//4 arguements originally
//1st: the padded feat m+10 and n+12, featr or m+20 and n+24, featp
//2nd is the filter, rootfilter or partfilter
//3rd and 4th are constants, start = 0; end = 0 for rootfilter, start=0 and end = 5 for partfilter 
//USE end to distinguish between featr and featp arugments


 									//make sure the feat is 3d and double
  // get B and start/end						//cellB is the filter pointer				
  //int num_bs = mxGetNumberOfElements(cellB);	
  int num_bs = 0;	
							//num_bs =#ofcells, which is 1cell for root and 6 cells for part
	  if (end==5)//partfilters and featp		
	  num_bs = 6; 
	  else if (end==0)//rootfilters and featr
	  num_bs = 1;	

	  if (start < 0 || end >= num_bs || start > end)				
	  cout<<"Invalid input: start/end"<<endl;

  int len = end-start+1;							//root: len = 1
										//part: len = 6			
  // output cell
  //plhs[0] = mxCreateCellMatrix(1, len);						//create the output matrix of 1xlen//root: 1x1, part:1x6
  double **match = (double**)calloc(len, sizeof(double*));

  // do convolutions
  thread_data td;
  //const int *A_dims = mxGetDimensions(mxA);					//an array describing the dimensions of feat
  //double *A = (double *)mxGetPr(mxA);						//an double pointer pointing to feat
  // get A
  double *A = featrp;
	  for (int i = 0; i < len; i++) {						//root: 0; part:0-5
	    //const mxArray *mxB = mxGetCell(cellB, i+start);							
	    //td.A_dims = A_dims;								//td.A_dims is fixed after this point
	    //td.A = A;
	    //td.B_dims = mxGetDimensions(mxB);  
	    //td.B = (double *)mxGetPr(mxB);
	    td.A_dims = featdims;
	    td.A = A;
	    td.B_dims = filterdims;
	    //get B
	    td.B = filters[i];
	    if (td.A_dims[2] != td.B_dims[2])
	    cout<<"Invalid input: B"<<endl;

	    // compute size of output
	    int height = td.A_dims[0] - td.B_dims[0] + 1;
	    int width = td.A_dims[1] - td.B_dims[1] + 1;
	    if (height < 1 || width < 1)
	    cout<<"Invalid input: B should be smaller than A"<<endl;
	    td.C_dims[0] = height;
	    td.C_dims[1] = width;
	    //in case the dimensions of the match are needed
	    dims[0] = height;
	    dims[1] = width;

	    //td.mxC = mxCreateNumericArray(2, td.C_dims, mxDOUBLE_CLASS, mxREAL);
	    //td.C = (double *)mxGetPr(td.mxC);
	    match[i] = (double*)calloc((td.C_dims[0])*(td.C_dims[1]), sizeof(double));	
	    td.C = match[i];	
	    //if(end==0)
	    process((void *)&td);	
	    //else if(end==5)	
	   // process_n((void *)&td);	
	  }
return match;
}

