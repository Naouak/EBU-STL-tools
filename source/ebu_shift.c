#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"

int main(int argc, const char** argv) {
	char * input = NULL;
	char * output = NULL;
	int nshift = 0;
	int i = 0;
	int onlyTCP = 0;
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
		else if(!strcmp(argv[i],"-TCP")){
			onlyTCP = 1;
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
		printf("Usage: %s -i input.stl [-t timeshift([-]HHMMSSFF)] [-TCP] output.stl\n",argv[0]);
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

	if(nshift == 0){
		if(isBelleNuit(ebu)){
			shift = malloc(sizeof(struct EBU_TC));
			shift->seconds = 0;
			shift->minutes = 0;
			shift->hours = 0;
		}
		else{
			shift = charToTC(ebu->gsi.TCP);
		}
	}
	else{
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
	}
	printf("Shifting: %02d:%02d:%02d:%02d\n",shift->hours,shift->minutes,shift->seconds,shift->frames);
	if(onlyTCP){
		struct EBU_TC* TCP = charToTC(ebu->gsi.TCP);
		struct EBU_TC tc = shiftTC(TCP,shift,positive);
		TCToChar(ebu->gsi.TCP,tc);
	}
	else{
		shiftTCs(ebu,shift,positive);
	}
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