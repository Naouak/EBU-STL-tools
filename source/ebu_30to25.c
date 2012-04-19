#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"

int main(int argc, const char** argv) {
	char * input = NULL;
	char * output = NULL;
	int i = 0;
	for (i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i],"-i")){
			i++;
			if(i < argc){
				input = (char *)argv[i];
			}
		}
		else{
			output = (char *)argv[i];
		}
	}

	if(input == NULL || output == NULL){
		if(input == NULL)
			printf("no input set\n");
		if(output == NULL)
			printf("no output set\n");
		printf("Usage: %s -i input.stl output.stl\n",argv[0]);
		return 0;
	}


	FILE* source = fopen(input,"r");
	if(source == NULL){
		fclose(source);
		return 1;
	}
	struct EBU* ebu = parseEBU(source);
	fclose(source);


	EBU30to25(ebu);

	FILE* dest = fopen(output,"w");
	if(dest == NULL){
		fclose(dest);
		return 2;
	}
	saveEBU(dest,ebu);
	fclose(dest);

	free(ebu);

	return 0;
}