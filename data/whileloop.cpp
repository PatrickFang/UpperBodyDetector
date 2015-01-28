
	//check # of ,
	FILE* ptr = fopen("./data/im.txt", "r");
	if (ptr== NULL){
	cout<<"can't open file"<<endl;
	return 0;
	}
	

	
	while (!feof(ptr)){
	//read row by row, know that the file has data size of 8x9x31
	int count = 0;
	char line[10000];
	fgets(line, sizeof line, ptr);
	int xx =0;
	while (line[xx]!='\n'){

	xx++;
	if(line[xx] == ',')    
	count++;  
	}
	cout<<count<<endl;
	}
	fclose(ptr);
	


	//image.save ("./data/im2.txt",-1);

	ptr = fopen("./data/im2.txt", "r");
	if (ptr== NULL){
	cout<<"can't open file"<<endl;
	return 0;
	}
	
	//int k = 0;
	
	while (!feof(ptr)){
	//read row by row, know that the file has data size of 8x9x31
	int count = 0;
	char line[10000];
	fgets(line, sizeof line, ptr);
	int xx =0;
	while (line[xx]!='\n'){

	xx++;
	if(line[xx] == ',')    
	count++;  
	}
	cout<<count<<endl;
	}
	fclose(ptr);
	
