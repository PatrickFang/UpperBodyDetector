using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h> 
#include <string.h>
#include <stdlib.h>
#include "MODEL.h"   	
#include "CImg.h"
#include "resize.h"
#include "features.h"
#include "padarray.h"
#include "fconv.h"
#include "fconvMT.h"
#include "dt.h" 
#include "me_HaarDetectOpenCV.h"

using namespace cimg_library;

static inline double min(double x, double y) { return (x <= y ? x : y); }	//function overloading 
static inline double max(double x, double y) { return (x <= y ? y : x); }

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }
#define MULTITHREADED 1
#define FACE_DETECTION 1
int main(int argc, char* argv[]){
	if(argc!=2){
	cout<<"error: input the image name"<<endl;
	return 0;
	}
	
	char *imagename = argv[1];
	char imagepath[50];
	strcpy(imagepath, "./image/");
	strcat(imagepath, imagename);
	cout<<imagepath<<endl;
	//read in the image	
	//CImg<double>image("./image/000003.jpeg");
	CImg<double>image(imagepath);
	
	//image.save("./data/cppimage.txt", -1);
	//some specfications of the image
	int width = image.width();		//cols
	int height = image.height();		//rows
	int depth = image.depth(); 		//depth
	int spec = image.spectrum();
	int totalnumpixels = image.size();
	cout<<height<<" "<<width<<" "<<depth<<" "<<spec<<" "<<totalnumpixels<<endl;

	//copy all contents of det_pars and model into the variables declared
	//first copy det_pars
	double det_pars_iou_thresh = 0.300000000000000;
	double det_pars_opencv_face_regparams[4]= {1.13000000000000, 0.340000000000000, 3.22000000000000, 0.900000000000000};
	double det_pars_ubfpff_thresh = -0.525000000000000;
	int det_pars_opencvface_minneighbours = 3; 
	int det_pars_ubfpff_scale = 1;
	

	//now model data
	//the challenges in this part is to fill values in to rootfilters and partfilters
	//also handle the differences between a col array and the array defined here	
	double model_sbin = 8;
	
	//rootfilters	
	double model_rootfilters_size[2]  = {8, 9};
	double model_rootfilters_blocklabel = 2;
	double model_rootfilters_w[8][9][31];
	FILE* fptr_rootfilters = fopen("./data/rootfilters_w.txt", "r");
	if (fptr_rootfilters == NULL){
	cout<<"can't open file"<<endl;
	return 0;
	}
	
	int k = 0;
	while (!feof(fptr_rootfilters)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_rootfilters);
	  // cout<< line <<endl;
	   char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 9; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_rootfilters_w[i][j][k] = w;
		//cout<<model_rootfilters_w[i][j][k]<<endl; 
		//cout<<temp<<endl;	
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}
	fclose(fptr_rootfilters);

	
	double model_offsets_w = -4.848717814588008; 	 
	int model_offsets_blocklabel = 1;
	int model_blocksizes[10] = {1, 1240, 744, 4, 1488, 4, 744, 4, 1488, 4};
	int model_regmult[10] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	double model_learnmult[10] = {20, 1, 1, 0.100000000000000, 1, 0.100000000000000, 1, 0.100000000000000, 1, 0.100000000000000};
	
	//lowerbounds
	//loop through to initialize all elements to -100
	double model_lowerbounds_num1 = -100;
	double model_lowerbounds_num2[1240];
	double model_lowerbounds_num3[744];
	double model_lowerbounds_num4[4] = {0.01, -100, 0.01, -100};
	double model_lowerbounds_num5[1488];
	double model_lowerbounds_num6[4] = {0.01, -100, 0.01, -100};
	double model_lowerbounds_num7[744];
	double model_lowerbounds_num8[4] = {0.01, -100, 0.01, -100};
	double model_lowerbounds_num9[1488];
	double model_lowerbounds_num10[4] = {0.01, -100, 0.01, -100};
	int i;
	for (i=0; i<1240; i++)
		model_lowerbounds_num2[i] = -100;
	for (i=0; i<744; i++){
		model_lowerbounds_num3[i] = -100; 
		model_lowerbounds_num7[i] = -100; 
	}
	for (i=0; i<1488; i++){
		model_lowerbounds_num5[i] = -100;
		model_lowerbounds_num9[i] = -100;
	}

	//components
	int model_components_rootindex = 1;
	int model_components_offsetindex = 1;
	struct Parts model_components_parts[6];
	for(i = 0; i<6; i++){
		model_components_parts[i].partindex = i+1;
		model_components_parts[i].defindex = i+1;
	}
	int model_components_dim = 5731;
	int model_components_numblocks = 10;

	double model_components_x1[15] = {
				-0.0753031443409232,
				-0.0689584393609972,
				-0.0267535279744776,
				0.0489902446068492,
				0.404718700744640,
				0.609963739365928,
				-0.133256152713204,
				0.340515887722052,
				-0.387021901154795,
				0.212066748478071,
				0.0983925201033671,
				-0.0523912914148885,
				-0.0291906100855903,
				-0.0944905828960073,
				0.0702827102326274,
				};
	
	double model_components_y1[15] = {
				0.0215950426860591,
				0.0365966811775287,
				0.0401618363993591,
				0.0893744742867296,
				0.818415422154475,
				0.118028243987939,
				0.00722148577465082,
				-0.0897398802563600,
				0.0147500984509829,
				-0.0565388876542801,
				0.0394667160944422,
				-0.0459150902647140,
				0.0532814463083353,
				-0.0493595921484444,
				0.0300685790070408,
				};
	double model_components_x2[15] = {
				0.367233432996008,
				-0.0705375706644149,
				-0.00743029261097799,
				-0.249218065878024,
				-0.496311742462224,
				0.268957275383827,
				0.392293709987871,
				0.452821664676554,
				0.147362332674355,
				0.360471049547888,
				0.0652544854820066,
				0.202669149187098,
				-0.0722528342882415,
				0.0385382579527437,
				-0.0228596063855035,
				};
	double model_components_y2[15] = {
				0.425944913307472,
				0.0339901827358986,
				0.0600242393869750,
				-0.179251300248170,
				-0.0120515528739334,
				-0.189530209633435,
				0.479982490440465,
				0.0131262907224966,
				0.496618161276418,
				0.0771101868062198,
				0.000366090934804124,
				0.183042605938201,
				0.0226314563976425,
				0.0701271856427759,
				-0.0430053437081291,
				};	
	double model_interval =  10;
	int model_numcomponents = 1;
	int model_numblocks = 10;
	
	//partfilters
	struct Partfilters model_partfilters[6];
	FILE* fptr_part1 = fopen("./data/partfilters_wc1.txt", "r");
	FILE* fptr_part2 = fopen("./data/partfilters_wc2.txt", "r");
	FILE* fptr_part3 = fopen("./data/partfilters_wc3.txt", "r");
	FILE* fptr_part4 = fopen("./data/partfilters_wc4.txt", "r");
	FILE* fptr_part5 = fopen("./data/partfilters_wc5.txt", "r");
	FILE* fptr_part6 = fopen("./data/partfilters_wc6.txt", "r");
	if (fptr_part1 == NULL){
	cout<<"can't open file"<<endl;
	return 0;
	}
	
	//first cell
	k = 0;
	while (!feof(fptr_part1)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_part1);
	  // cout<< line <<endl;
	 char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 6; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_partfilters[0].w[i][j][k] = w;
		//cout<<model_rootfilters_w[i][j][k]<<endl; 
		//cout<<temp<<endl;	
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}

	//second cell
	k = 0;
	while (!feof(fptr_part2)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_part2);
	  // cout<< line <<endl;
	   char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 6; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_partfilters[1].w[i][j][k] = w;
		//cout<<model_rootfilters_w[i][j][k]<<endl; 
		//cout<<temp<<endl;	
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}


	//third cell
	k = 0;
	while (!feof(fptr_part3)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_part3);
	  // cout<< line <<endl;
	 char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 6; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_partfilters[2].w[i][j][k] = w;
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}

	
	k = 0;
	while (!feof(fptr_part4)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_part4);
	  // cout<< line <<endl;
	 char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 6; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_partfilters[3].w[i][j][k] = w;
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}


	k = 0;
	while (!feof(fptr_part5)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_part5);
	  // cout<< line <<endl;
	 char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 6; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_partfilters[4].w[i][j][k] = w;
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}

	
	k = 0;
	while (!feof(fptr_part6)&&k<31){
	 //read row by row, know that the file has data size of 8x9x31
	   for(int i = 0; i < 8; i++){
	   char line[200];
	   fgets(line, sizeof line, fptr_part6);
	  // cout<< line <<endl;
	 char *temp =  NULL;	
	   temp = strtok(line, ",");	
	    for(int j = 0; j < 6; j++){ 
		char *stopstring = NULL;
		//convert the char* to double and store in rootfilters_w
		double w = strtod(temp, &stopstring);
		model_partfilters[5].w[i][j][k] = w;
		temp = strtok(NULL, ",");	
	    }	
	   }
	k++;
	}

	fclose(fptr_part1);
	fclose(fptr_part2);
	fclose(fptr_part3);
	fclose(fptr_part4);
	fclose(fptr_part5);
	fclose(fptr_part6);
	
	model_partfilters[0].fake = 0;
	model_partfilters[0].partner = 0;
	model_partfilters[0].blocklabel = 3;
	model_partfilters[1].fake = 0;
	model_partfilters[1].partner = 3;
	model_partfilters[1].blocklabel = 5;
	model_partfilters[2].fake = 1;
	model_partfilters[2].partner = 2;
	model_partfilters[2].blocklabel = 0;//not applicable
	model_partfilters[3].fake = 0;
	model_partfilters[3].partner = 0;
	model_partfilters[3].blocklabel = 7;
	model_partfilters[4].fake = 0;
	model_partfilters[4].partner  = 6;
	model_partfilters[4].blocklabel  = 9;
	model_partfilters[5].fake = 1;
	model_partfilters[5].partner = 5;
	model_partfilters[5].blocklabel = 0; //not applicable
	
	
	//defs
	struct Defs model_defs[6]; 
	double temp0[4] = {0.0164557704376990, 0.00591515063137820, 0.0248102631442582, -0.00331222978888120};
	double temp1[4] = {0.0312092627507148, -0.00908578419495217, 0.0231415601757135,0.00183075547788028};
	double temp2[4] = {0.0312092627507148, 0.00908578419495217, 0.0231415601757135,	0.00183075547788028}; 
	double temp3[4] = {0.0192965347387716, -0.00135346521901133, 0.0218327211612357, -0.00750267470745942};
	double temp4[4] = {0.0160454177160079, 0.0102309751567277, 0.0246551221966866, -0.000172005729354739};
	double temp5[4] = {0.0160454177160079, -0.0102309751567277, 0.0246551221966866, -0.000172005729354739}; 
		for(int i= 0; i<4; i++){	
		model_defs[0].w[i] = temp0[i];
		model_defs[1].w[i] = temp1[i];
		model_defs[2].w[i] = temp2[i];
		model_defs[3].w[i] = temp3[i];
		model_defs[4].w[i] = temp4[i];
		model_defs[5].w[i] = temp5[i];
		}
	//model_defs[0].anchor = {7, 1};
	model_defs[0].anchor[0] = 7;
	model_defs[0].anchor[1] = 1;	
	model_defs[0].blocklabel = 1;
	//model_defs[1].anchor = {1, 9};
	model_defs[1].anchor[0] = 1;
	model_defs[1].anchor[1] = 9;
	model_defs[1].blocklabel = 6;
	//model_defs[2].anchor = {13, 9};
	model_defs[2].anchor[0] = 13;
	model_defs[2].anchor[1] = 9; 
	model_defs[2].blocklabel = 0;//not applicable
	//model_defs[3].anchor = {7, 9};	
	model_defs[3].anchor[0] = 7;
	model_defs[3].anchor[1] = 9; 
	model_defs[3].blocklabel = 8;
	//model_defs[4].anchor = {1, 1};
	model_defs[4].anchor[0] = 1;
	model_defs[4].anchor[1] = 1;  	
	model_defs[4].blocklabel = 10;
	//model_defs[5].anchor = {13, 1};	
	model_defs[5].anchor[0] = 13; 
	model_defs[5].anchor[1] = 1; 
	model_defs[5].blocklabel = 0;//not applicable


	double model_maxsize[2] = {8, 9};
	double model_minsize[2] = {8, 9};
	double model_thresh = -0.0790300000000000;
	double model_selthresh = -2;
	/**up to here, done copying the det_pars and model from .mat files**/


	/**the second step is to perform what exactlythe detect function does to produce boxes**/
	//INITIALIZATIONS
	//bool latent  = false; //always 3 arguments
	//bool write = false; //<6 arguments

	//more initialization: a lot of lines down here can actually be simply replaced by assigning  
	//constants to the variables. but to preserve the integrety of the original function
	//use the same assigning method described in the matlab functions
	int ridx = model_components_rootindex;		//=1
	int oidx = model_components_offsetindex;	//=1

	int rsize[2] = {8, 9};				//={8,9}, first and second dimentsions values of root
	int numparts = 6;				//length(model_components_parts);
	int pidx[6];
	int didx[6];
	int psize[6][2];
	int rpidx[6][2];
	for (i=0; i<numparts; i++){			//6 iterations
	pidx[i] = i+1;					//pidx = didx =  [1 2 3 4 5 6]
	didx[i] = i+1;
	//part == model_partfilters
		for (int j=0; j<2; j++){			//psize has the first and second dimentsions of partfilters
			if(j==0){				//all cell contain [8 9]
			psize[i][j] = 8;
			rpidx[i][j] = 1;
			}
			else {
			psize[i][j] = 9;
			rpidx[i][j] = i+1;
			}
		}	
	}
	int padx = ceil(model_maxsize[1]/2+1);		//6
	int pady = ceil(model_maxsize[0]/2+1);		//5
	double interval = model_interval;
	//up to here basically done with detect init, now call featpyramid
	//which takes 3 arguments (im, sbin, interval) and returns 2
	double sbin = model_sbin;
	double sc = pow(2.0,1.0/interval);			//sc = 2^0.1 is constant
	double imsize[2] = {height, width};		//this is [num num] -->variable depending on the image
	int max_scale = 1 + floor(log(min(imsize[1], imsize[0])/(5*sbin))/log(sc)); //a integer calculated based on imsize and sbin and sc
	
  	// Allocate memory
	//this points to the 2d, double array of feat in matlab form
	int **featdimensions = (int**)calloc(max_scale + interval, sizeof(int*));//keeps track of the real dimensions of featmatrix	
  	double **featmatrix = (double**)calloc(max_scale + interval, sizeof(double*));
	double *scales = (double*)calloc(max_scale + interval, sizeof(double));	//here is an horizontal array, but in matlab vertical
	cout<<"max_scale+interval: " <<max_scale+interval<<endl;
	
	const int h = image.height();					//# of rows
	const int w =  image.width();					//#of cols
	const int chan = image.spectrum();				//# of channels
 
	double *temp =  image;
	//convert src array to the same form as matlab src array
	double *src = (double*)calloc(h*w*chan, sizeof(double));	
		//walk through the temp array, reallocate the values in src, remap
		for(int index = 0; index<totalnumpixels; index++){
		int dx = index%w;
		int dy = index/w;
			if(dy<h)			//R channel
			src[dx*h+dy] = temp[index];
			else if(h<=dy&&dy<2*h)		//B channel
			src[dx*h+dy+h*(w-1)] = temp[index];
			else 				//G channael
			src[dx*h+dy+2*h*(w-1)] = temp[index];
		}

	for (i=0; i<interval; i++){
	//NOTE: both resize and feature function needs the dimensions of the scaled image
	//pass them in as parameters
	double sdims[3] = {h,w,chan};	//sdims is the pointer the dimentsions of image which is like [m n 3]	
	double* scaledsrc = resize(src, 1/pow(sc, i), sdims);		
	
	// "first" 2x interval
	featdimensions[i] = (int*)calloc(3, sizeof(int));
	featmatrix[i] = features(scaledsrc, sbin/2, sdims, featdimensions[i]);
	scales[i] = 2/pow(sc,i);

	 //"second" 2x interval
	featdimensions[int(i+interval)] = (int*)calloc(3, sizeof(int));
	featmatrix[int(i+interval)] = features(scaledsrc, sbin, sdims, featdimensions[int(i+interval)]);
	scales[int(i+interval)] = 1/pow(sc, i);

		// remaining interals
		for(int j= i+interval;j<max_scale; j=j+10){
		    double *srctemp = scaledsrc;
		    scaledsrc = resize(scaledsrc, 0.5, sdims);		
		    free(srctemp);
		    //cout<<"sdims:"<<sdims[0]<<" "<<sdims[1]<<" "<<sdims[2]<<endl;
		    featdimensions[int(j+interval)] = (int*)calloc(3, sizeof(int));
		    featmatrix[(int)(j+interval)] = features(scaledsrc, sbin, sdims, featdimensions[(int)(j+interval)]);
		    scales[int(j+interval)] = 0.5 * scales[j];
		}
	free(scaledsrc);
	}
	//free(src);	

	//the section of DETECT.m after getting feat and scale
	//first remap the rootfilters and partfilters in matlab array form..
	double **rootfilters = (double**)calloc(1, sizeof(double*));			//1 cell in total
	for (int i = 0; i<1; i++)
	rootfilters[i] = (double*)calloc(8*9*31, sizeof(double));
	int index = 0;
	for (int k=0; k<31; k++){
		for(int j=0; j<9; j++){
			for(int i=0;i<8;i++){
			rootfilters[0][index] = model_rootfilters_w[i][j][k];
			index ++;
			}
		}	
	}

	
	double **partfilters = (double**)calloc(6, sizeof(double*));			//6cells in total
	for (int i = 0; i<6; i++)
	partfilters[i] = (double*)calloc(8*6*31, sizeof(double));
	
	int cellindex = 0;
	while (cellindex<6){
	index=0;
	for (int k=0; k<31; k++){
		for(int j=0; j<6; j++){
			for(int i=0;i<8;i++){
			//cout<< model_partfilters[cellindex].w[i][j][k]<<endl;
			partfilters[cellindex][index] = model_partfilters[cellindex].w[i][j][k];
			index ++;
			}
		}	
	}
	cellindex++;
	}


	int level;
	//create boxes matrix as the final container, but dont know the exact size.=>put 300 just in case
	double **boxes = (double **)calloc(300, sizeof(double *));
	int boxescount = 0;//globally keeps track of number of boxes found

	for (level = interval; level<max_scale + interval; level++){	//start from level 11
  	double scale = model_sbin/scales[level];
	//convolve feature maps with filters: first padd the feat array
	int filtersdims[3] = {8, 9, 31};
	int rootmatchdims[2] = {0, 0};	
	int featrdims[3] = {0, 0, 0};			
	double *featr = padarray(featmatrix[level], featdimensions[level],  padx, pady, featrdims);

	//cout<<"paddeddims: "<<featrdims[0]<<" "<<featrdims[1]<<" "<<featrdims[2]<<endl;
	//call fconv on featr and rootfilters
	double **rootmatch = fconv(featr, featrdims, rootfilters, filtersdims, 0, 0, rootmatchdims);

	int featpdims[3] = {0, 0, 0};
	filtersdims[0] = 8;	
	filtersdims[1] = 6;
	filtersdims[2] = 31;
	int partmatchdims[2] = {0, 0};
	double *featp;
	double **partmatch;
	  	if(1){//length(partfilters)=6 is CONSTANT!, ==>always run
		featp = padarray(featmatrix[(int)(level-interval)], featdimensions[(int)(level-interval)],
                2*padx,2*pady, featpdims);									
	   	//partmatch = fconv(featp, partfilters, 1, length(partfilters));
		//cout<<"paddeddims: "<<featpdims[0]<<" "<<featpdims[1]<<" "<<featpdims[2]<<endl;
		//call fconv on featp and partfilters
		
		#if MULTITHREADED
		//cout<<"is mult"<<endl;
		partmatch = fconvMT(featp, featpdims, partfilters, filtersdims, 0, 5, partmatchdims);
		#else 
		//cout<<"not multithreaded"<<endl;
		partmatch = fconv(featp, featpdims, partfilters, filtersdims, 0, 5, partmatchdims);
		#endif
		}

	  //with rootmatch and partmatch created. now start a new loop
	  for (int c = 0; c<model_numcomponents; c++){			//doesnt even loop c=0 to c<1
   	  //root score + offset
	  //this line adds the value of this w to all rootmatch elements
	  double *score = (double*)calloc(rootmatchdims[0]*rootmatchdims[1], sizeof(double));
		  for (int i = 0; i<rootmatchdims[0]*rootmatchdims[1]; i++)
		  score[i] =  rootmatch[0][i] +  model_offsets_w;
	  //free the rootmatch which is not useful anymore	  
	  free(rootmatch[0]);
	  free(rootmatch);

	  //declare variables to contain the indices found, assume the max number of indeces found
	 
	  int *x = (int *)calloc(rootmatchdims[0]*rootmatchdims[1], sizeof(int));
	  int *y = (int *)calloc(rootmatchdims[0]*rootmatchdims[1], sizeof(int));
	  //also the counter that keep track of the #of indices found
	  int countindex = 0;


   	  //add in parts
	  int32_t **Ix = (int32_t **)calloc(6, sizeof(int32_t *));
  	  int32_t **Iy = (int32_t **)calloc(6, sizeof(int32_t *));
 	  double *M;
  	  //cout<<partmatchdims[0]<<" "<<partmatchdims[1]<<" "<<partmatchdims[0]*partmatchdims[1]<<endl;
		  for (int j = 0; j< numparts; j++){		//from j=0 to j=6
		  double def[4] = {model_defs[j].w[0],
				   model_defs[j].w[1],
				   model_defs[j].w[2],
				   model_defs[j].w[3]
				  };
		  double anchor[2] = {model_defs[j].anchor[0],
				      model_defs[j].anchor[1]
				     };

		  //cout<<def[0]<<" "<<def[1]<<" "<<def[2]<<def[3]<<endl;
	
		  //the anchor position is shifted to account for misalignment
		  //between features at different resolutions
		  int ax = anchor[0] + 1;
		  int ay = anchor[1] + 1;
		  #if MULTITHREADED==0
		  //cout<<"not multithreaded match here"<<endl;
		  double *match = (double *)calloc(partmatchdims[0]*partmatchdims[1], sizeof(double));
		  //cout<<partmatchdims[0]<<" "<<partmatchdims[1]<<" "<<partmatchdims[0]*partmatchdims[1]<<endl;
		  	  //add negative sign to every value in current partmatch cell and copy into match
			  for(int i= 0; i< partmatchdims[0]*partmatchdims[1]; i++)
			  match[i] = (-1)*partmatch[j][i];
		  #endif 
		  Ix[j] = (int32_t *)calloc(partmatchdims[0]*partmatchdims[1], sizeof(int32_t));
	  	  Iy[j] = (int32_t *)calloc(partmatchdims[0]*partmatchdims[1], sizeof(int32_t));
		 // M = dt(match, def[0], def[1], def[2], def[3], partmatchdims, Ix[j], Iy[j]);
		  #if MULTITHREADED
		  M = dt(partmatch[j], def[0], def[1], def[2], def[3], partmatchdims, Ix[j], Iy[j]);
		  #else 
		  M = dt(match, def[0], def[1], def[2], def[3], partmatchdims, Ix[j], Iy[j]);
		  #endif 
		  //change score by subtracting M, also get all good matches
		  int tempMindex = 0;
		
			  for (int t = ax-1; t<=ax-1+2*(rootmatchdims[1]-1); t =t+2){		 //cols
			  	for(int i = ay -1; i<=ay-1+2*(rootmatchdims[0]-1); i= i+2){	 //rows	
				int index = t*partmatchdims[0] + i;
				score[tempMindex] = score[tempMindex]-M[index];
				if(j==5&& score[tempMindex]>det_pars_ubfpff_thresh){//meaning in the last iteration already
					//check the score>thresh condition and save the scores and the correspoding indexes
					//that satisfy the condition
					//cout<<"I: "<<tempMindex<<endl;
					x[countindex] = ceil((double)tempMindex/(double)rootmatchdims[0]);//both x and y are calculated in the matlab indexing method 
					y[countindex] =	tempMindex%rootmatchdims[0]+1;//==>starting from 1, without ceil function gives c++ index
					countindex ++;		
					}
				tempMindex ++;			
				}

			  }		
		
		  free(M);
		  #if MULTITHREADED==0
		  //cout<<"not multithreaded end"<<endl;
		  free(match);
		  #endif 
		  }
		  

		  //create tmp matrix with size of countindex by 30
		  for (int i = 0; i<countindex; i++){
		  //create b of size 1 by 30
		  double *b = (double *)calloc(30, sizeof(double));		//28+2, the last two cols are c and score(I(i))
		  b[0] = ((double)x[i] - (double)padx)*scale + 1.0;		//x1
	 	  b[1] = ((double)y[i] - (double)pady)*scale + 1.0;		//y1
		  b[2] = b[0] + ((double)rsize[1])*scale -1.0;			//x2
		  b[3] = b[1] + ((double)rsize[0])*scale - 1.0;			//y2
		  //cout<<"b: "<<b[0]<<" "<<b[1]<<" "<<b[2]<<" "<<b[3]<<endl;
		  	for(int j = 0; j<numparts; j++){
			double ax = model_defs[j].anchor[0] +1;					//ax
			double ay = model_defs[j].anchor[1] +1;					//ay
			int probex = (x[i]-1)*2 + ax;
			int probey = (y[i]-1)*2 + ay; 
			double px = (double)(Ix[j][(probex-1)*partmatchdims[0] + (probey-1)]);
			double py = (double)(Iy[j][(probex-1)*partmatchdims[0] + (probey-1)]);
			//cout<<"probex: "<<probex<<"probey: "<<probey<<endl;
			//cout<<"px: "<<px<<" py: "<<py<<endl;
			b[(j+1)*4] = ((double)(px-2)/2.0+1.0-(double)padx)*scale+1.0;	//px1
			b[(j+1)*4+1] = ((double)(py-2)/2.0+1.0-(double)pady)*scale+1.0;	//py1
			b[(j+1)*4+2] = b[(j+1)*4] + 6.0*scale/2.0 - 1.0;	        //px2
			b[(j+1)*4+3] = b[(j+1)*4+1] + 8.0*scale/2.0 - 1.0;		//py2
			}
		  b[28] = 1.00;
		  b[29] = score[(x[i]-1)*rootmatchdims[0]+y[i]-1];
		  //tmp[i] = b;
	  	  //put the b into boxes  
		  boxes[boxescount+i] = b;	
		  }
	  boxescount = boxescount + countindex;
	  

	  //free x and y
	  free(x);
	  free(y);

	  	  //free Ix and Iy
		  for (int i = 0; i< 6; i++){
		  free(Ix[i]);
		  free(Iy[i]);
		  }
	  free(Ix);
	  free(Iy);
	  }
 

 	//free tmp and b(the boxes)
        	
	free(featp);
	free(featr);
		//free rootmatch and partmatch
		for(int i= 0; i<6; i++)
		free(partmatch[i]);
	free(partmatch);
	} 
	//cout<<height<<" "<<width<<" "<<depth<<" "<<spec<<" "<<totalnumpixels<<endl;

	
	//down here DETECT.m is done and I have generated boxes 
	//CALL getboxes : simulate what getboxes.m does.  and then make bbox more accurate by 
	//I is the adscending list of last col values
	//performing the calculations on the _center _height...
	double det_hwratio = 0.9;
	double **bbox= 	(double **)calloc(boxescount, sizeof(double *));
	for (int i = 0; i<boxescount; i++){
	bbox[i] = (double*)calloc(5, sizeof(double));
	//the first 4 elements of bbox[i] is the matrix product of x1*A, y1*A, x2*A and y2*A
	//A consists of 3rd-1st and 1,2...terms of boxes[i] 1,2, 5,6, 9,10, 13,14, 17,18, 21,22, 25,26 
	bbox[i][0] = (boxes[i][2] - boxes[i][0])*model_components_x1[0]
			+boxes[i][0]*model_components_x1[1]+boxes[i][1]*model_components_x1[2]
			+boxes[i][4]*model_components_x1[3]+boxes[i][5]*model_components_x1[4]
			+boxes[i][8]*model_components_x1[5]+boxes[i][9]*model_components_x1[6]
			+boxes[i][12]*model_components_x1[7]+boxes[i][13]*model_components_x1[8]
			+boxes[i][16]*model_components_x1[9]+boxes[i][17]*model_components_x1[10]
			+boxes[i][20]*model_components_x1[11]+boxes[i][21]*model_components_x1[12]
			+boxes[i][24]*model_components_x1[13]+boxes[i][25]*model_components_x1[14];
	bbox[i][1] = (boxes[i][2] - boxes[i][0])*model_components_y1[0]
			+boxes[i][0]*model_components_y1[1]+boxes[i][1]*model_components_y1[2]
			+boxes[i][4]*model_components_y1[3]+boxes[i][5]*model_components_y1[4]
			+boxes[i][8]*model_components_y1[5]+boxes[i][9]*model_components_y1[6]
			+boxes[i][12]*model_components_y1[7]+boxes[i][13]*model_components_y1[8]
			+boxes[i][16]*model_components_y1[9]+boxes[i][17]*model_components_y1[10]
			+boxes[i][20]*model_components_y1[11]+boxes[i][21]*model_components_y1[12]
			+boxes[i][24]*model_components_y1[13]+boxes[i][25]*model_components_y1[14];
  	bbox[i][2] = (boxes[i][2] - boxes[i][0])*model_components_x2[0]
			+boxes[i][0]*model_components_x2[1]+boxes[i][1]*model_components_x2[2]
			+boxes[i][4]*model_components_x2[3]+boxes[i][5]*model_components_x2[4]
			+boxes[i][8]*model_components_x2[5]+boxes[i][9]*model_components_x2[6]
			+boxes[i][12]*model_components_x2[7]+boxes[i][13]*model_components_x2[8]
			+boxes[i][16]*model_components_x2[9]+boxes[i][17]*model_components_x2[10]
			+boxes[i][20]*model_components_x2[11]+boxes[i][21]*model_components_x2[12]
			+boxes[i][24]*model_components_x2[13]+boxes[i][25]*model_components_x2[14];
	bbox[i][3] = (boxes[i][2] - boxes[i][0])*model_components_y2[0]
			+boxes[i][0]*model_components_y2[1]+boxes[i][1]*model_components_y2[2]
			+boxes[i][4]*model_components_y2[3]+boxes[i][5]*model_components_y2[4]
			+boxes[i][8]*model_components_y2[5]+boxes[i][9]*model_components_y2[6]
			+boxes[i][12]*model_components_y2[7]+boxes[i][13]*model_components_y2[8]
			+boxes[i][16]*model_components_y2[9]+boxes[i][17]*model_components_y2[10]
			+boxes[i][20]*model_components_y2[11]+boxes[i][21]*model_components_y2[12]
			+boxes[i][24]*model_components_y2[13]+boxes[i][25]*model_components_y2[14];
	//the last term of each bbox[i] is just the last value of boxes[i]
	bbox[i][4] = boxes[i][29];

	double bbox_center[2] = {(bbox[i][0]+bbox[i][2])/2.0, (bbox[i][1]+bbox[i][3])/2.0}; 
	double bbox_height = bbox[i][3]-bbox[i][1];
	double dist_from_center[2] = {1.0/det_hwratio*bbox_height/2.0, bbox_height/2.0};
	//cout<<"center: "<<bbox_center[0]<<" "<<bbox_center[1]<<endl;
	//cout<<"height: "<<bbox_height<<endl;
	//cout<<"dis: "<<dist_from_center[0]<<" "<<dist_from_center[1]<<endl;
	//reassign values for bbox
	bbox[i][0] = bbox_center[0] - dist_from_center[0];
	bbox[i][1] = bbox_center[1] - dist_from_center[1];
	bbox[i][2] = bbox_center[0] + dist_from_center[0];
	bbox[i][3] = bbox_center[1] + dist_from_center[1];
	}
	//bbox(:,1:end-1) = bbox(:,1:end-1)/det_pars.ubfpff_scale;
	// =>skiped because scale =1


	double *area = (double *)calloc(boxescount, sizeof(double));
		//generate area
		for (int i= 0; i<boxescount; i++){ 	
		area[i] = (bbox[i][2] - bbox[i][0] + 1)*(bbox[i][3] - bbox[i][1]+1);
		}
	
	//put the indexes in adcending order
	//first make a copy of the last col
	double lastcolcopy[boxescount];
	for (int i = 0; i < boxescount; i++)
		lastcolcopy[i] = bbox[i][4];	

	int lengthofI = boxescount;
	int *I = (int *)calloc(boxescount, sizeof(int));	
	while(lengthofI >0){
	int largestindex = 0;
		for (int i = 0; i < boxescount; i++)
			if (lastcolcopy[i] > lastcolcopy[largestindex])
			largestindex = i;
		I[lengthofI-1] = largestindex;
		//"delete" the one found already
		lastcolcopy[largestindex] = -10000;
		lengthofI--;
	}
	

	for(int i=0; i<boxescount; i++){  
	cout<<"I["<<i<<"]: ";
		cout<<I[i]<<" ";
	cout<<endl;
	}  

	//CALL  me_iou_nms(false, bbox, det_pars.iou_thresh);
	int *pick = (int *)calloc(boxescount, sizeof(int));
	int pickcount = 0;
	lengthofI = boxescount;	
	int last = 0;	
	while (lengthofI > 0){	
	//last term
	int lengthdeduction= 0;	
	int last = lengthofI -1;
	int i  = I[last];
	pick[pickcount] = i;
	pickcount++;
	//cout<<"last: "<<last<<endl;
	//cout<<"i: "<<i<<endl;
	I[last] = -10000;
	//cout<<"I[last]: "<<I[last]<<endl;
	lengthdeduction++;
		for(int pos = 0; pos<lengthofI-1; pos++){
		int j = I[pos];
		double xx1 = max(bbox[i][0], bbox[j][0]);
		double yy1 = max(bbox[i][1], bbox[j][1]);
		double xx2 = min(bbox[i][2], bbox[j][2]);
		double yy2 = min(bbox[i][3], bbox[j][3]);	    
		double w = xx2 - xx1 +1;
		double h = yy2 - yy1 +1;
			if(w>0&&h>0){
			//compute overlap 
			double o = w * h / (area[j] + area[i] - w * h);
				if (o > det_pars_iou_thresh){
				I[pos] = -10000;
				lengthdeduction++;
				}
			}
		}

	lengthofI = lengthofI - lengthdeduction;
	//update I by copying the new I into I  
	int t = 0;
		for (int x= 0; x<boxescount; x++){
			if(I[x]!=-10000){
			I[t] = I[x];
			t++;
			}
		}
	
	}

	//free area
	free(area);
	free(I);
	//free boxes
	for(int i = 0; i < 300; i++){
	free(boxes[i]);
	}
	free(boxes);

	//free rootfilters and partfilters
	for(int i = 0; i < 6; i++){
	free(partfilters[i]);
	}
	free(partfilters);
	free(rootfilters[0]);
	free(rootfilters);
	
	//free feat //free featdimensions
	for(i = 0; i < max_scale + interval; i++){
	free(featdimensions[i]);
	free(featmatrix[i]);
	}
	free(featdimensions);	
	free(featmatrix);
	free(scales);


	//check final picks
	for(int i=0; i<pickcount; i++){  
	cout<<"pickcount["<<i<<"]: ";
		//for (int x= 0; x<5; x++){
		cout<<bbox[pick[i]][0]<<" "<<bbox[pick[i]][1]<<" "<<bbox[pick[i]][2]<<" "<<bbox[pick[i]][3]<<" "<<bbox[pick[i]][4]<<" "<<endl;
		//}
	cout<<endl;
	}  
	
	int numofdets = 0;
	//this part only runs if the user has opencv properly installed
	#if FACE_DETECTION
	//rgb2gray(gray2rgb(img))  0.2989 * R + 0.5870 * G + 0.1140 * B 
	double *grayimg = (double *)calloc(width*height, sizeof(double));
		for(int i=0; i<width*height; i++){
		grayimg[i] = round(src[i]*0.2989 + src[i+height*width]*0.5870 + 0.1140 *src[i+2*height*width]);
		}

	int Dimms[2] = {height, width};
	/*cout<<"oringinal"<<endl;
  	cout<<(double)src[0]<<" "<<(double)src[1]<<" "<<(double)src[2]<<" "<<(double)src[3]<<" "<<endl;
    	cout<<(double)src[Dimms[0]*Dimms[1]-1]<<" "<<(double)src[Dimms[0]*Dimms[1]-2]<<" "<<(double)src[Dimms[0]*Dimms[1]-3]<<" "<<(double)src[Dimms[0]*Dimms[1]-4]<<" "<<endl;
	 cout<<(double)src[Dimms[0]*Dimms[1]*3-1]<<" "<<(double)src[Dimms[0]*Dimms[1]*3-2]<<" "<<(double)src[Dimms[0]*Dimms[1]*3-3]<<" "<<(double)src[Dimms[0]*Dimms[1]*3-4]<<" "<<(double)src[Dimms[0]*Dimms[1]*3-5]<<" "<<endl;
	*/
	
	double *fbox = me_HaarDetectOpenCV("haarcascade_frontalface_alt2.xml", grayimg ,30 ,30 ,1.1, 2, Dimms, &numofdets);
	free(grayimg);
	
	if(fbox!=NULL){
	for(int i =0; i<4*numofdets; i++){
	cout<<fbox[i]<<" ";
		if(i%4 == 3)
		cout<<endl; 
	}

	double newx[numofdets];
	double newy[numofdets];
	double neww[numofdets];
	double newh[numofdets];
		/*
		for(int i= 0; i<numofdets; i++){
		newx[i] = fbox[i*4] - round(fbox[i*4+2]*det_pars_opencv_face_regparams[0]);
		newy[i] = fbox[i*4+1] - round(fbox[i*4+3]*det_pars_opencv_face_regparams[1]);
		neww[i] = fbox[i*4+2] * det_pars_opencv_face_regparams[2];
		newh[i] = neww[i]  * det_pars_opencv_face_regparams[3];
		}
  		*/
		for(int i= 0; i<numofdets; i++){
		newx[i] = fbox[i] - round(fbox[numofdets*2+i]*det_pars_opencv_face_regparams[0]);
		newy[i] = fbox[numofdets*1+i] - round(fbox[numofdets*3+i]*det_pars_opencv_face_regparams[1]);
		neww[i] = fbox[numofdets*2+i] * det_pars_opencv_face_regparams[2];
		newh[i] = neww[i]  * det_pars_opencv_face_regparams[3];
		}
	
	

	//put selected bbox and fbox into newbbox
	//NOTE: remember to freee newbbox starting from pickcount!
	double **newbbox = (double **)calloc(pickcount+numofdets, sizeof(double *));
		for (int i = 0; i<pickcount; i++)
		newbbox[i] = bbox[pick[i]];
	
		for (int i = 0; i<numofdets; i++){
		newbbox[pickcount+i] = (double *)calloc(5, sizeof(double));
		newbbox[pickcount+i][0] = newx[i];
		newbbox[pickcount+i][1] = newy[i];
		newbbox[pickcount+i][2] = newx[i]+neww[i]-1;
		newbbox[pickcount+i][3] = newy[i]+newh[i]-1;	
		//score is 10% less than the body detections
		newbbox[pickcount+i][4] = det_pars_ubfpff_thresh-abs(det_pars_ubfpff_thresh/10);
		}

		//put the indexes in adcending order
		//first make a copy of the last col
		double newlastcolcopy[pickcount+numofdets];
		for (int i = 0; i < pickcount+numofdets; i++)
			newlastcolcopy[i] = newbbox[i][4];	

		for (int i = 0; i < pickcount+numofdets; i++)
			cout<<newlastcolcopy[i]<<" ";
		cout<<endl;	

		int newlengthofI = pickcount+numofdets;
		int *newI = (int *)calloc(pickcount+numofdets, sizeof(int));	
		while(newlengthofI >0){
		int largestindex = 0;
			for (int i = 0; i < pickcount+numofdets; i++)
				if (newlastcolcopy[i] > newlastcolcopy[largestindex])
				largestindex = i;
			newI[newlengthofI-1] = largestindex;
			//"delete" the one found already
			newlastcolcopy[largestindex] = -10000;
			newlengthofI--;
		}



		for(int i=0; i<pickcount+numofdets; i++){  
		cout<<"newI["<<i<<"]: ";
			cout<<newI[i]<<" ";
		cout<<endl;
		}  
	
		cout<<"numofdefs: "<<numofdets<<endl;
		for(int i =0; i<pickcount+numofdets; i++){
			for(int j= 0; j<5; j++)
			cout<<newbbox[i][j]<<" ";
		cout<<endl;
		}

	//generate area
	double *newarea = (double *)calloc(pickcount+numofdets, sizeof(double));	
		for (int i= 0; i<pickcount+numofdets; i++){ 	
		newarea[i] = (newbbox[i][2] - newbbox[i][0] + 1)*(newbbox[i][3] - newbbox[i][1]+1);
		}


	//CALL  me_iou_nms(false, bbox, det_pars.iou_thresh);
	int *newpick = (int *)calloc(pickcount+numofdets, sizeof(int));
	int newpickcount = 0;
	newlengthofI = pickcount+numofdets;	
	int newlast = 0;	
	while (newlengthofI > 0){	
	//last term
	int newlengthdeduction= 0;	
	int newlast = newlengthofI -1;
	int i  = newI[newlast];
	newpick[newpickcount] = i;
	newpickcount++;
	//cout<<"last: "<<last<<endl;
	//cout<<"i: "<<i<<endl;
	newI[newlast] = -10000;
	//cout<<"I[last]: "<<I[last]<<endl;
	newlengthdeduction++;
		for(int pos = 0; pos<newlengthofI-1; pos++){
		int j = newI[pos];
		double xx1 = max(newbbox[i][0], newbbox[j][0]);
		double yy1 = max(newbbox[i][1], newbbox[j][1]);
		double xx2 = min(newbbox[i][2], newbbox[j][2]);
		double yy2 = min(newbbox[i][3], newbbox[j][3]);	    
		double w = xx2 - xx1 +1;
		double h = yy2 - yy1 +1;
			if(w>0&&h>0){
			//compute overlap 
			double o = w * h / (newarea[j] + newarea[i] - w * h);
				if (o > det_pars_iou_thresh){
				newI[pos] = -10000;
				newlengthdeduction++;
				}
			}
		}

	newlengthofI = newlengthofI - newlengthdeduction;
	//update I by copying the new I into I  
	int t = 0;
		for (int x= 0; x<pickcount+numofdets; x++){
			if(newI[x]!=-10000){
			newI[t] = newI[x];
			t++;
			}
		}
	
	}

	for(int i=0; i<newpickcount; i++){  
	cout<<"newpickcount["<<i<<"]: ";
		//for (int x= 0; x<5; x++){
		cout<<newbbox[newpick[i]][0]<<" "<<newbbox[newpick[i]][1]<<" "<<newbbox[newpick[i]][2]<<" "<<newbbox[newpick[i]][3]<<" "<<newbbox[newpick[i]][4]<<" "<<endl;
		//}
	cout<<endl;
	}  
	
	const unsigned char color[] = { 255,128,64 };
	for(int i=0; i<newpickcount; i++){
	image.draw_line(newbbox[newpick[i]][0], newbbox[newpick[i]][1],newbbox[newpick[i]][0],newbbox[newpick[i]][3], color);
	image.draw_line(newbbox[newpick[i]][0], newbbox[newpick[i]][3],newbbox[newpick[i]][2],newbbox[newpick[i]][3], color);
	image.draw_line(newbbox[newpick[i]][2], newbbox[newpick[i]][3],newbbox[newpick[i]][2],newbbox[newpick[i]][1], color);
	image.draw_line(newbbox[newpick[i]][2], newbbox[newpick[i]][1],newbbox[newpick[i]][0],newbbox[newpick[i]][1], color);
	} 


	//done with fbox
	free(fbox);
	free(newarea);
	free(newI);
	for (int i = 0; i<numofdets; i++)
	free(newbbox[pickcount+i]);
	free(newbbox);
	free(newpick);
	}
	#endif 
	free(src);

	

	
	//draw all the boxes
	const unsigned char color[] = { 255,128,64 };
	for(int i=0; i<pickcount; i++){
	image.draw_line(bbox[pick[i]][0], bbox[pick[i]][1],bbox[pick[i]][0],bbox[pick[i]][3], color);
	image.draw_line(bbox[pick[i]][0], bbox[pick[i]][3],bbox[pick[i]][2],bbox[pick[i]][3], color);
	image.draw_line(bbox[pick[i]][2], bbox[pick[i]][3],bbox[pick[i]][2],bbox[pick[i]][1], color);
	image.draw_line(bbox[pick[i]][2], bbox[pick[i]][1],bbox[pick[i]][0],bbox[pick[i]][1], color);
	} 
	
	//free bbox
	for(int i = 0; i < boxescount; i++){
	free(bbox[i]);
	}
	free(bbox);
	//free pick 
	free(pick);

	

	//display the original image
	CImgDisplay main_disp(image,"original");
	while(1){
	main_disp.wait();
	}

	return 0;

}


