#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"
#include "string_utils.h"

int main(int argc, const char** argv) {
	char * output = NULL;
	int i = 0;
	for (i = 1; i < argc; ++i)
	{
		output = (char *) argv[i];
	}

	if(output == NULL){
		if(output == NULL)
			printf("no output set\n");
		printf("Usage: %s file.stl\n",argv[0]);
		return 0;
	}

	FILE* source = fopen(output,"r");
	if(source == NULL){
		printf("Error: Source not loaded\n");
		fclose(source);
		return 1;
	}

	struct EBU* ebu = parseEBU(source);
	fclose(source);

	isBelleNuit(ebu);

	ebu->gsi.DSC = 0x30;
	char TNB[6];
	strncpy(TNB,ebu->gsi.TNB,5);
	TNB[5] = '\0';
	int nTNB = atoi(TNB);
	
	char search[2] = " ";
	short j = 0;
	for(i = 0; i < nTNB; i++){
		for(j = 1; j <= 0x1F; j++){
			search[0] = j;
			search[1] = '\0';
			char *line = malloc(112*sizeof(char));

			strncpy(line,ebu->tti[i].TF,112);
			line[111] = '\0';
			str_replace(line,search,"");
			free(line);
			strncpy(ebu->tti[i].TF,line,112);	
		}
	}


	FILE* dest = fopen("test.stl","w");
	if(dest == NULL){
		printf("Error: Output not loaded\n");
		fclose(dest);
		return 2;
	}
	saveEBU(dest,ebu);
	fclose(dest);
	free(ebu);

	return 0;
}