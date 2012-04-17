#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"

int main(int argc, const char** argv) {
	char * input = NULL;
	char * output = NULL;
	int nshift = 0;
	int i = 0;
	for (i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i],"-i")){
			i++;
			if(i < argc){
				input = (char *)argv[i];
			}
		}
		else if(!strcmp(argv[i],"-t")){
			i++;
			if(i < argc){
				nshift = atoi(argv[i]);
			}
		}
		else{
			output = (char *)argv[i];
		}
	}

	if(input == NULL || output == NULL || nshift == 0){
		if(input == NULL)
			printf("no input set\n");
		if(output == NULL)
			printf("no output set\n");
		printf("Usage: %s -i input.stl [-t trim([-]HHMMSSFF)] output.stl\n",argv[0]);
		return 0;
	}


	FILE* source = fopen(input,"r");
	if(source == NULL){
		fclose(source);
		return 1;
	}
	struct EBU* ebu = parseEBU(source);
	fclose(source);

	struct EBU_TC* shift;

	int positive = 1;

	positive = nshift>0?1:-1;
	if(positive < 0)
		nshift *= -1;
	shift = malloc(sizeof(struct EBU_TC));
	shift->frames = nshift%100;
	nshift /= 100;
	shift->seconds = nshift%100;
	nshift /= 100;
	shift->minutes = nshift%100;
	nshift /= 100;
	shift->hours = nshift%100;

	printf("Trimming at: %02d:%02d:%02d:%02d\n",shift->hours,shift->minutes,shift->seconds,shift->frames);

	TrimEBU(ebu,shift);

	free(shift);

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