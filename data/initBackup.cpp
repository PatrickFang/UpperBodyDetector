using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include "MODEL.h"
#include "CImg.h"
using namespace cimg_library;

int main(){
	//read in the image
	CImg<unsigned char> image("./image/000000.jpg");
	//some specfications of the image
	int width = image.width();		//cols
	int height = image.height();		//rows
	int depth = image.depth(); 		//depth
	int spec = image.spectrum();
	cout<<width<<" "<<height<<" "<<depth<<" "<<spec<<endl;
	/*int totalnumpixels = image.size();
	SOME RBG
	cout<<totalnumpixels<<endl;
	cout<<(int)image(49, 49, 0, 0)<<endl;
	cout<<(int)image(49, 49, 0, 1)<<endl;
	cout<<(int)image(49, 49, 0, 2)<<endl;
	*/

struct Model{
	double sbin;
	//most complicated
	struct Rootfilters rootfilters;
	struct Offsets offsets;
	double blocksizes[10];		//1, 1240, 744, 4, 1488, 4, 744, 4, 1488, 4	
	double regmult[10];		//0 1 1 1 1 1 1 1 1 1 1
	double learnmult[10];		//20 1 1 0.1 1 0.1 1 0.1 1 0.1	
	struct Lowerbounds lowerbounds;
	struct Components components;
	int interval;			//10
	int numcomponents;		//1
	//most complicated
	struct Partfilters partfilters[6];	
	struct Defs defs[6];		//6x1
	double maxsize[2];		//[8 9]			
	double minsize[2];		//[8 9]
	double thresh;			//-0.0790
	int me_selthresh;		//-2
};	



	//copy all contents of det_pars and model into the variables declared
	//first copy det_pars
	/*
	struct Det_pars det_pars;
	det_pars.iou_thresh = 0.300000000000000;
	det_pars.opencv_face_regparams= {1.13000000000000, 0.340000000000000, 3.22000000000000, 0.900000000000000};
	det_pars.ubfpff_thresh = -0.525000000000000;
	det_pars.opencvface_minneighbours = 3; 
	det_pars.ubfpff_scale = 1;
	*/

	double det_pars_iou_thresh = 0.300000000000000;
	double det_pars_opencv_face_regparams[4]= {1.13000000000000, 0.340000000000000, 3.22000000000000, 0.900000000000000};
	double det_pars_ubfpff_thresh = -0.525000000000000;
	int det_pars_opencvface_minneighbours = 3; 
	int det_pars_ubfpff_scale = 1;
	
/*
	//now model data
	//the challenges in this part is to fill values in to rootfilters and partfilters
	//also handle the differences between a col array and the array defined here
	struct Model model;
	model.sbin = 8;
	
	//rootfilters	
	//model.rootfilters.size  = {8, 9};
	model.rootfilters.blocklabel = 2;

	FILE* fptr_rootfilters = fopen("./data/rootfilters_w.txt", "r");
	if (fptr_rootfilters == NULL){
	cout<<"can't open file"<<endl;
	return 0;
	}
	//cout<<model.rootfilters.size[0]<<endl;
	//cout<<model.rootfilters.size[1]<<endl;

	while (!feof(fptr_rootfilters)){
	char line[200];
	char *l = fgets(line, sizeof line, fptr_rootfilters);
	 if(l == line){
	 //read row by row, know that the file has data size of 8x9x31
	  for(int k = 0; k < 31; k++){
	   for(int i = 0; i < 8; i++){
	    for(int j = 0; j < 9; j++){
	  	model.rootfilters.w[i][j][k] = line[j]; 	
	    }	
	   }
	  }
	  }
	}
	fclose(fptr_rootfilters);



	//model.rootfilters

	model.offsets.w = -4.84871781458801; 
	model.offsets.blocklabel = 1;
	model.blocksizes = {1, 1240, 744, 4, 1488, 4, 744, 4, 1488, 4};
	model.regmult = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	model.learnmult = {20, 1, 1, 0.100000000000000, 1, 0.100000000000000, 1, 0.100000000000000, 1, 0.100000000000000};
	
	//lowerbounds
	//loop through to initialize all elements to -100
	model.lowerbounds.num1 = -100;
	for (i=0; i<1240; i++)
		model.lowerbounds.num2[i] = -100;
	for (i=0; i<744; i++){
		model.lowerbounds.num3[i] = -100; 
		model.lowerbounds.num7[i] = -100; 
	}
	for (i=0; i<1488; i++){
		model.lowerbounds.num5[i] = -100;
		model.lowerbounds.num9[i] = -100;
	}
	model.lowerbounds.num4 = {0.01, -100, 0.01, -100};
	model.lowerbounds.num6 = {0.01, -100, 0.01, -100};
	model.lowerbounds.num8 = {0.01, -100, 0.01, -100};
	model.lowerbounds.num10 = {0.01, -100, 0.01, -100};

	//components
	model.components.rootindex = 1;
	model.components.offsetindex = 1;
	for(i = 0; i<6; i++){
		model.components.parts[i].partindex = i+1;
		model.components.parts[i].defindex = i+1;
	}
	model.components.dim = 5731;
	model.components.numblocks = 10;
	model.components.x1 = {
				-0.0753031443409232
				-0.0689584393609972
				-0.0267535279744776
				0.0489902446068492
				0.404718700744640
				0.609963739365928
				-0.133256152713204
				0.340515887722052
				-0.387021901154795
				0.212066748478071
				0.0983925201033671
				-0.0523912914148885
				-0.0291906100855903
				-0.0944905828960073
				0.0702827102326274
				};
	model.components.y1 = {
				0.0215950426860591
				0.0365966811775287
				0.0401618363993591
				0.0893744742867296
				0.818415422154475
				0.118028243987939
				0.00722148577465082
				-0.0897398802563600
				0.0147500984509829
				-0.0565388876542801
				0.0394667160944422
				-0.0459150902647140
				0.0532814463083353
				-0.0493595921484444
				0.0300685790070408
				};
	model.components.x2 = {
				0.367233432996008
				-0.0705375706644149
				-0.00743029261097799
				-0.249218065878024
				-0.496311742462224
				0.268957275383827
				0.392293709987871
				0.452821664676554
				0.147362332674355
				0.360471049547888
				0.0652544854820066
				0.202669149187098
				-0.0722528342882415
				0.0385382579527437
				-0.0228596063855035
				};
	model.components.y2 = {
				0.425944913307472
				0.0339901827358986
				0.0600242393869750
				-0.179251300248170
				-0.0120515528739334
				-0.189530209633435
				0.479982490440465
				0.0131262907224966
				0.496618161276418
				0.0771101868062198
				0.000366090934804124
				0.183042605938201
				0.0226314563976425
				0.0701271856427759
				-0.0430053437081291
				};	
	model.interval =  10;
	model.numcomponents = 1;
	model.numblocks = 10;
	
	//partfilters
	//model.partfilters
	

	//defs
	model.defs[0].anchor = {7, 1};	
	model.defs[0].w = {0.0164557704376990, 0.00591515063137820, 0.0248102631442582, -0.00331222978888120};
	model.defs[0].blocklabel = 1;
	model.defs[1].anchor = {1, 9};	
	model.defs[1].w = {0.0312092627507148, -0.00908578419495217, 0.0231415601757135,0.00183075547788028};
	model.defs[1].blocklabel = 6;
	model.defs[2].anchor = {9, 13};	
	model.defs[2].w = {0.0312092627507148, 0.00908578419495217, 0.0231415601757135,	0.00183075547788028};
	model.defs[2].blocklabel = 0;//not applicable
	model.defs[3].anchor = {7, 9};	
	model.defs[3].w = {0.0192965347387716, -0.00135346521901133, 0.0218327211612357, -0.00750267470745942};
	model.defs[3].blocklabel = 8;
	model.defs[4].anchor = {1, 1};	
	model.defs[4].w = {0.0160454177160079, 0.0102309751567277, 0.0246551221966866, -0.000172005729354739};
	model.defs[4].blocklabel = 10;
	model.defs[5].anchor = {13, 1};	
	model.defs[5].w = {0.0160454177160079, -0.0102309751567277, 0.0246551221966866, -0.000172005729354739};
	model.defs[5].blocklabel = 0;//not applicable

	model.maxsize = {8, 9};
	model.minsize = {8, 9};
	model.thresh = -0.0790300000000000;
	model.selthresh = -2;


*/


	//display the original image
	CImgDisplay main_disp(image,"original");
	while(1){
	main_disp.wait();
	}



	return 0;

}
