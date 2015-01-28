using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h> 
#include <string.h>
#include <stdlib.h>
#include "cv.h" 
#include "highgui.h"
 
// compiled with opencv2.0 gives different results then opencv2.1
// e.g. buffy_s5e2/buffy_s5e2_frames/001668.jpg

// matlab interface to OpenCV cvHaarDetectObjects function  
// author: Marci`pkg-config --cflags --libs opencv`n Eichner


double *me_HaarDetectOpenCV(char const *facemodel_path, double *grayimage, int min_size_x , int min_size_y, double scale_factor, int min_neighbors, int *dims, int *numofdet){
  	
    int classifiername_length,ncols, nrows,c,r,d;
    uchar *image_pointer = (uchar *)calloc(dims[0]*dims[1],sizeof(uchar)); // equivalent to uint8
    uchar temp;
    const int flags = 0; // no CV_HAAR_DO_CANNY_PRUNING
    // init default values:

    
    //classifiername_length = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;
    //classifiername = mxArrayToString(prhs[0]);
    
    classifiername_length = strlen(facemodel_path); //length of haarcascade_frontalface_alt2.xml
    const char *classifiername = facemodel_path;// haarcascade_frontalface_alt2.xml
    
    // HANDLE INPUT
    //cout<<"here1"<<endl; 
    // read in the classifier
    CvHaarClassifierCascade * cascade = (CvHaarClassifierCascade*) cvLoad(classifiername, 0, 0, 0);
cout<<facemodel_path<<endl;
 //cout<<"here2"<<endl;
    if(!cascade){
	cout<<"ERROR: Could not load the classifier"<<endl;
	return NULL;
    }  
    /* Check if the prhs image is in double format*/ 
    //if (!(mxIsUint8(prhs[1]))) 
    //  mexErrMsgTxt("ERROR: arg2 (image) must be of type uint8");

    //int image_ndim = mxGetNumberOfDimensions(prhs[1]);
    //if (mxGetNumberOfDimensions(prhs[1]) > 2) make sure is gray
    //  mexErrMsgTxt("ERROR: arg2 (image) must be a imgscale image");
    //cout<<"here3"<<endl;
    //image_pointer =  (uchar*)mxGetPr(prhs[1]);
     //uchar *image_pointer =  (uchar*)grayimage;
	    for (int i=0; i<dims[0]*dims[1]; i++)
	    image_pointer[i] =  (uchar)grayimage[i];
	/*
	cout<<"new"<<endl;
    cout<<(double)image_pointer[0]<<" "<<(double)image_pointer[1]<<" "<<(double)image_pointer[2]<<" "<<(double)image_pointer[3]<<" "<<endl;
    cout<<(double)image_pointer[dims[0]*dims[1]-1]<<" "<<(double)image_pointer[dims[0]*dims[1]-2]<<" "<<(double)image_pointer[dims[0]*dims[1]-3]<<" "<<(double)image_pointer[dims[0]*dims[1]-4]<<" "<<endl;
	*/
    //ncols = mxGetN(prhs[1]); 
    //nrows = mxGetM(prhs[1]); 
    ncols = dims[1];
    nrows = dims[0];  
    IplImage* img = cvCreateImage( cvSize(ncols, nrows), IPL_DEPTH_8U, 1 );
     //cout<<"here5"<<endl;
    // Load the column wise vector into the IplImage
    // IplImage data is read in a rowwise manner
    // Appropriate conversion is carried out here
    for(c=0;c<ncols;c++)
       for(r=0;r<nrows;r++)
        {
            temp = (uchar)image_pointer[r+(nrows*c)];
            ((uchar *)(img->imageData + r*img->widthStep))[c]=temp;
        }
 //cout<<"here6"<<endl;
  /*  if (nrhs > 2)
      min_size_x = (int)round(mxGetScalar(prhs[2]));
    if (nrhs > 3)
      min_size_y = (int)round(mxGetScalar(prhs[3]));
    if (nrhs > 4)
      scale_factor = mxGetScalar(prhs[4]);
    if (nrhs > 5)
      min_neighbors = (int)round(mxGetScalar(prhs[5]));
   */

    CvMemStorage* storage = cvCreateMemStorage(0);
   //cout<<"here7"<<endl;  
    // PROCESS IMAGE
      
    // equalize hist image before running face detection;
    cvEqualizeHist(img, img);
 //cout<<"here8"<<endl;
    //mexPrintf("%f %d %d %d %d",scale_factor,min_neighbors,flags,min_size_x,min_size_y);
    // run the classifier
    CvSeq* detections = cvHaarDetectObjects(img, cascade, storage, scale_factor, min_neighbors, flags,
                                            cvSize(min_size_x, min_size_y));

 //cout<<"here9"<<endl;
    //create output 
    double *output=(double *)calloc(detections->total*4, sizeof(double));
    *numofdet = detections->total;
    if (detections->total == 0)
    {
      //plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
      cvReleaseImage(&img);
      cvReleaseHaarClassifierCascade(&cascade);
      cvReleaseMemStorage(&storage);
      return NULL;
    }
    else
    {
        //plhs[0] = mxCreateDoubleMatrix(detections->total, 4, mxREAL);
        //double* output = mxGetPr(plhs[0]);
        CvRect* rect;
        for(d = 0; d < detections->total; d++)
        {
           rect = (CvRect*)cvGetSeqElem(detections, d);
           output[d] = rect->x+1;
           output[d+detections->total*1] = rect->y+1;
           output[d+detections->total*2] = rect->width;
           output[d+detections->total*3] = rect->height;
        }
    }
    cvReleaseImage(&img);
    cvReleaseHaarClassifierCascade(&cascade);
    cvReleaseMemStorage(&storage);
    free(image_pointer);
    return output;
//     cvDestroyAllWindows();
}

