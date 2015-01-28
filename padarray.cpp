
using namespace std;
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "MODEL.h"
//this functions extends the dimensions of the feat matrix
//however the feat matrix is stored in matlab array form thus this function
//will also return the extended matrix in an array format
/*
double *padarray(const double *feat, const int *featdims, const int padx, const int pady, int *paddeddims){//padx =6, pady=5
	paddeddims[0] = featdims[0]+2*pady;		//new number of rows
	paddeddims[1] = featdims[1]+2*padx;		//new # of cols
	paddeddims[2] = featdims[2];
	const int numofelements  = paddeddims[0]*paddeddims[1]*paddeddims[2];
	const int oldnumofelements = featdims[0]*featdims[1]*featdims[2];
	double *padded = (double*)calloc(numofelements, sizeof(double));
	int oldindex = 0;
	//remap the feat into padded feat with the new dimensions paddeddims
	//NOTE: there are 31 channels and each is extended
	for (int index = 0; index < numofelements; index ++){
	int dy = index/(paddeddims[0]);			//paddeddims[0] is the width
	int dx = index%(paddeddims[0]);
		//first set the "frame" to zeros	
		//if (dx<padx||dy<(pady-1)||dx>(padx+featdims[1]-1)||dy>(pady+featdims[0]+1))
		if (dx<pady||dx>(featdims[0]+pady-1)||dy<padx
		||(dy>(padx+featdims[1]-1)&&dy<(3*padx+featdims[1]))
		||(dy>(3*padx+2*featdims[1]-1)&&dy<(5*padx+2*featdims[1]))
		||(dy>(5*padx+3*featdims[1]-1)&&dy<(7*padx+3*featdims[1]))
		||(dy>(7*padx+4*featdims[1]-1)&&dy<(9*padx+4*featdims[1]))
		||(dy>(9*padx+5*featdims[1]-1)&&dy<(11*padx+5*featdims[1]))
		||(dy>(11*padx+6*featdims[1]-1)&&dy<(13*padx+6*featdims[1]))
		||(dy>(13*padx+7*featdims[1]-1)&&dy<(15*padx+7*featdims[1]))
		||(dy>(15*padx+8*featdims[1]-1)&&dy<(17*padx+8*featdims[1]))
		||(dy>(17*padx+9*featdims[1]-1)&&dy<(19*padx+9*featdims[1]))
		||(dy>(19*padx+10*featdims[1]-1)&&dy<(21*padx+10*featdims[1]))
		||(dy>(21*padx+11*featdims[1]-1)&&dy<(23*padx+11*featdims[1]))
		||(dy>(23*padx+12*featdims[1]-1)&&dy<(25*padx+12*featdims[1]))
		||(dy>(25*padx+13*featdims[1]-1)&&dy<(27*padx+13*featdims[1]))
		||(dy>(27*padx+14*featdims[1]-1)&&dy<(29*padx+14*featdims[1]))
		||(dy>(29*padx+15*featdims[1]-1)&&dy<(31*padx+15*featdims[1]))
		||(dy>(31*padx+16*featdims[1]-1)&&dy<(33*padx+16*featdims[1]))
		||(dy>(33*padx+17*featdims[1]-1)&&dy<(35*padx+17*featdims[1]))
		||(dy>(35*padx+18*featdims[1]-1)&&dy<(37*padx+18*featdims[1]))
		||(dy>(37*padx+19*featdims[1]-1)&&dy<(39*padx+19*featdims[1]))
		||(dy>(39*padx+20*featdims[1]-1)&&dy<(41*padx+20*featdims[1]))
		||(dy>(41*padx+21*featdims[1]-1)&&dy<(43*padx+21*featdims[1]))
		||(dy>(43*padx+22*featdims[1]-1)&&dy<(45*padx+22*featdims[1]))
		||(dy>(45*padx+23*featdims[1]-1)&&dy<(47*padx+23*featdims[1]))
		||(dy>(47*padx+24*featdims[1]-1)&&dy<(49*padx+24*featdims[1]))
		||(dy>(49*padx+25*featdims[1]-1)&&dy<(51*padx+25*featdims[1]))
		||(dy>(51*padx+26*featdims[1]-1)&&dy<(53*padx+26*featdims[1]))
		||(dy>(53*padx+27*featdims[1]-1)&&dy<(55*padx+27*featdims[1]))
		||(dy>(55*padx+28*featdims[1]-1)&&dy<(57*padx+28*featdims[1]))
		||(dy>(57*padx+29*featdims[1]-1)&&dy<(59*padx+29*featdims[1]))
		||(dy>(59*padx+30*featdims[1]-1)&&dy<(61*padx+30*featdims[1]))
		||(dy>(61*padx+31*featdims[1]-1)&&dy<(63*padx+31*featdims[1]))
		)
		padded[index] = 0;
		//the rest which is not newly padded, only have to copy one by one assuming 
		//this part lies where we expect
		else {

			for(int i=0; i<featdims[0]; i++){
			padded[index] = feat[oldindex];
			oldindex++;
			index++;
			}
		}
	}
return padded;
}*/

using namespace std;
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "MODEL.h"
//this functions extends the dimensions of the feat matrix
//however the feat matrix is stored in matlab array form thus this function
//will also return the extended matrix in an array format
double *padarray(const double *feat, const int *featdims, const int padx, const int pady, int *paddeddims){//padx =6, pady=5
	paddeddims[0] = featdims[0]+2*pady;		//new number of rows
	paddeddims[1] = featdims[1]+2*padx;		//new # of cols
	paddeddims[2] = featdims[2];
	const int numofelements  = paddeddims[0]*paddeddims[1]*paddeddims[2];
	const int onepage = paddeddims[0]*paddeddims[1];
	const int oldonepage = featdims[0]*featdims[1];
	double *padded = (double*)calloc(numofelements, sizeof(double));
	int oldindex = 0;
	//remap the feat into padded feat with the new dimensions paddeddims
	//NOTE: there are 31 channels and each is extended
	for (int index = 0; index < onepage; index ++){
	int dy = index/(paddeddims[0]);			//paddeddims[0] is the width
	int dx = index%(paddeddims[0]);
		//first set the "frame" to zeros	
		//if (dx<padx||dy<(pady-1)||dx>(padx+featdims[1]-1)||dy>(pady+featdims[0]+1))
		if (dx<pady||dx>(featdims[0]+pady-1)||dy<padx
		||(dy>(padx+featdims[1]-1))){
		padded[index] = 0;
		padded[index+onepage] = 0;
		padded[index+2*onepage] = 0;
		padded[index+3*onepage] = 0; 
		padded[index+4*onepage] = 0;
		padded[index+5*onepage] = 0;
		padded[index+6*onepage] = 0;
		padded[index+7*onepage] = 0;
		padded[index+8*onepage] = 0;
		padded[index+9*onepage] = 0;		
		padded[index+10*onepage] = 0;
		padded[index+11*onepage] = 0;
		padded[index+12*onepage] = 0;		
		padded[index+13*onepage] = 0;
		padded[index+14*onepage] = 0;
		padded[index+15*onepage] = 0;		
		padded[index+16*onepage] = 0;
		padded[index+17*onepage] = 0;
		padded[index+18*onepage] = 0;		
		padded[index+19*onepage] = 0;
		padded[index+20*onepage] = 0;
		padded[index+21*onepage] = 0;		
		padded[index+22*onepage] = 0;
		padded[index+23*onepage] = 0;
		padded[index+24*onepage] = 0;		
		padded[index+25*onepage] = 0;
		padded[index+26*onepage] = 0;
		padded[index+27*onepage] = 0;
		padded[index+28*onepage] = 0;
		padded[index+29*onepage] = 0;
		padded[index+30*onepage] = 0;
		}
		//the rest which is not newly padded, only have to copy one by one assuming 
		//this part lies where we expect
		else {
			for(int i=0; i<featdims[0]; i++){
			padded[index] = feat[oldindex];
			padded[index+onepage] = feat[oldindex+oldonepage];
			padded[index+2*onepage] = feat[oldindex+2*oldonepage];
			padded[index+3*onepage] = feat[oldindex+3*oldonepage];
			padded[index+4*onepage] = feat[oldindex+4*oldonepage];
			padded[index+5*onepage] = feat[oldindex+5*oldonepage];
			padded[index+6*onepage] = feat[oldindex+6*oldonepage];
			padded[index+7*onepage] = feat[oldindex+7*oldonepage];
			padded[index+8*onepage] = feat[oldindex+8*oldonepage];
			padded[index+9*onepage] = feat[oldindex+9*oldonepage];
			padded[index+10*onepage] = feat[oldindex+10*oldonepage];
			padded[index+11*onepage] = feat[oldindex+11*oldonepage];
			padded[index+12*onepage] = feat[oldindex+12*oldonepage];
			padded[index+13*onepage] = feat[oldindex+13*oldonepage];
			padded[index+14*onepage] = feat[oldindex+14*oldonepage];
			padded[index+15*onepage] = feat[oldindex+15*oldonepage];
			padded[index+16*onepage] = feat[oldindex+16*oldonepage];
			padded[index+17*onepage] = feat[oldindex+17*oldonepage];
			padded[index+18*onepage] = feat[oldindex+18*oldonepage];
			padded[index+19*onepage] = feat[oldindex+19*oldonepage];
			padded[index+20*onepage] = feat[oldindex+20*oldonepage];
			padded[index+21*onepage] = feat[oldindex+21*oldonepage];
			padded[index+22*onepage] = feat[oldindex+22*oldonepage];
			padded[index+23*onepage] = feat[oldindex+23*oldonepage];
			padded[index+24*onepage] = feat[oldindex+24*oldonepage];
			padded[index+25*onepage] = feat[oldindex+25*oldonepage];
			padded[index+26*onepage] = feat[oldindex+26*oldonepage];
			padded[index+27*onepage] = feat[oldindex+27*oldonepage];
			padded[index+28*onepage] = feat[oldindex+28*oldonepage];
			padded[index+29*onepage] = feat[oldindex+29*oldonepage];
			padded[index+30*onepage] = feat[oldindex+30*oldonepage];
			oldindex++;
			index++;
			}
		}
	}
return padded;
}




