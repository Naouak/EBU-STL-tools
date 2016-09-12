#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"
#include "string_utils.h"

void print_version(){
	printf("Compilation Date : %s\n",VERSION_NUMBER);
}

void print_help(int argc, const char** argv){
	printf("Usage:\n\t%s -i input.stl -n <name> output.stl\n",argv[0]);
	
	printf("Action:\n");
	printf("Change the TET field of the EBU GSI block with the one provided.\n");

}

int main(int argc, const char** argv) {
	char * input = NULL;
	char * output = NULL;
	char * name = NULL;
	for (int i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i],"-i")){
			i++;
			if(i < argc){
				input = (char *)argv[i];
			}
		}
		else if(!strcmp(argv[i],"-n")){
			i++;
			if(i < argc){
				name = (char *)argv[i];
			}
		}
		else{
			output = (char *)argv[i];
		}
	}

	if(input == NULL || output == NULL){
		if(input == NULL)
			printf("Error: no input set\n");
		if(output == NULL)
			printf("Error: no output set\n");
		print_version();
		print_help(argc,argv);
		return 0;
	}


	FILE* source = fopen(input,"r");
	if(source == NULL){
		fclose(source);
		return 1;
	}
	struct EBU* ebu = parseEBU(source);
	fclose(source);

	printf("Renaming...");

	int nameIndex = 0;
	for(; nameIndex < 32 && nameIndex < strlen(name); nameIndex++){
		ebu->gsi.TET[nameIndex] = name[nameIndex];
	}
	for(; nameIndex < 32; nameIndex++){
		ebu->gsi.TET[nameIndex] = '\0';
	}

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