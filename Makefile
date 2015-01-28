UB: main.o resize.o features.o padarray.o fconvMT.o fconv.o dt.o me_HaarDetectOpenCV.o
	g++ -g main.o resize.o features.o padarray.o fconvMT.o fconv.o dt.o me_HaarDetectOpenCV.o -o UB -O2 -L/usr/X11R6/lib -lm -lpthread -lX11  `pkg-config --cflags --libs opencv`
main.o: main.cpp MODEL.h resize.h
	g++ -c main.cpp -O2 -L/usr/X11R6/lib -lm -lpthread -lX11  
resize.o: resize.cpp resize.h
	g++ -c resize.cpp resize.h -O2 -L/usr/X11R6/lib -lm -lpthread -lX11
features.o: features.cpp features.h
	g++ -c features.cpp features.h 
padarray.o: padarray.cpp padarray.h
	g++ -c padarray.cpp padarray.h 
dt.o: dt.cpp dt.h
	g++ -c dt.cpp dt.h
fconvMT.o: fconvMT.cpp fconvMT.h
	g++ -c fconvMT.cpp fconvMT.h

fconv.o: fconv.cpp fconv.h
	g++ -c fconv.cpp fconv.h

me_HaarDetectOpenCV.o: me_HaarDetectOpenCV.cpp me_HaarDetectOpenCV.h
	g++ -c me_HaarDetectOpenCV.cpp me_HaarDetectOpenCV.h `pkg-config --cflags --libs opencv`
