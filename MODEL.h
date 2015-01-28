#ifndef MODEL_H
#define MODEL_H
 


/**********************these are all the model components*******/
struct Rootfilters{
	double size[2];			//8,9 
	double w[8][9][31];		//8x9x31
	double blocklabel;		//2
};

struct Offsets{
	double w; 			//-4.8487
	double blocklabel;		//1
};

struct Lowerbounds{
	double num1;
	double num2[1240];
	double num3[744];	
	double num4[4];
	double num5[1488];	
	double num6[4];
	double num7[744];	
	double num8[4];			
	double num9[1488];		
	double num10[4];	
};


struct Parts{
	int partindex;
	int defindex;
};	

struct Components{
	int rootindex; 			//1
	int offsetindex;		//1
	struct Parts parts[6];		//6x1
	int dim;			//5731
	int numblocks;			//10
	double x1[15];			//15x1
	double y1[15];			//15x1
	double x2[15];
	double y2[15]; 
};

struct Defs{
	double anchor[2];		//2x1
	double w[4];			//4x1	
	int blocklabel;			//an integer or doesnt exist in third cell and sixth				
};

struct Partfilters{
	double w[8][6][31];
	int fake;			//either 1/0
	int partner;			//an integer
	int blocklabel;			//an integer, doesnt exist in cell6, cell3
};
struct Feat{
	double w[8][6][31];
};	


//this is the model struct
//more complacated since it involves loading large 3d matrixs
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



#endif
