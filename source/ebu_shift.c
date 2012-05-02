#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"
#include "string_utils.h"

void CRLFtoTeletext(char * str,int position){
	int j = 0;
	for(j = 107;j >= position;j--){
			str[j+4] = str[j];
	}
	str[position] = 0x0D;
	str[position+1] = 0x01;
	str[position+2] = 0x0B;
	str[position+3] = 0x0B;
}

void applyTeletextfix(struct EBU* ebu){
	char tnb[6];
	tnb[5] = '\0';
	strncpy(tnb,ebu->gsi.TNB,5);

	int TNB = atoi(tnb);
	int i;
	for(i = 0; i < TNB; i++){
		int j = 0;
		for(j = 107;j >= 0;j--){
			ebu->tti[i].TF[j+4] = ebu->tti[i].TF[j];
		}
		ebu->tti[i].TF[0] = 0x0D;
		ebu->tti[i].TF[1] = 0x07;
		ebu->tti[i].TF[2] = 0x0B;
		ebu->tti[i].TF[3] = 0x0B;	

		for(j = 107;j >= 0;j--){
			if(ebu->tti[i].TF[j] == 0x8A){
				CRLFtoTeletext(ebu->tti[i].TF,j+1);
			}
		}	
	}
}

int main(int argc, const char** argv) {
	char * input = NULL;
	char * output = NULL;
	int nshift = 0;
	int start = -1;
	int i = 0;
	int onlyTCP = 0;
	char DSC[2] = " ";
	char LC[3] = "  ";
	char CPN[4] = "   ";
	int Teletextfix = 0;
	int rmSPE = 0;
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
		else if(!strcmp(argv[i],"-s")){
			i++;
			if(i < argc){
				start = atoi(argv[i]);
			}
		}
		else if(!strcmp(argv[i],"-TCP")){
			onlyTCP = 1;
		}
		else if(!strcmp(argv[i],"-DSC")){
			i++;
			if(i < argc)
				strncpy(DSC,argv[i],1);
		}
		else if(!strcmp(argv[i],"-LC")){
			i++;
			if(i < argc)
				strncpy(LC,argv[i],2);
		}
		else if(!strcmp(argv[i],"-CPN")){
			i++;
			if(i < argc){
				strncpy(CPN,argv[i],3);
			}
		}
		else if(!strcmp(argv[i],"-Teletextfix")){
			Teletextfix = 1;
		}
		else if(!strcmp(argv[i],"-rmSPE")){
			rmSPE = 1;
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

	if(start >= 0){
		struct EBU_TC* startTC = malloc(sizeof(struct EBU_TC));

		startTC->frames = start%100;
		start /= 100;
		startTC->seconds = start%100;
		start /= 100;
		startTC->minutes = start%100;
		start /= 100;
		startTC->hours = start%100;

		struct EBU_TC* TCP = charToTC(ebu->gsi.TCP);
		struct EBU_TC shift2 = shiftTC(TCP,startTC,1);
		free(TCP);

		shift = malloc(sizeof(struct EBU_TC));
		shift->frames = shift2.frames;
		shift->seconds = shift2.seconds;
		shift->minutes = shift2.minutes;
		shift->hours = shift2.hours;

		free(startTC);
	}
	else if(nshift == 0){
		if(isBelleNuit(ebu)){
			shift = malloc(sizeof(struct EBU_TC));
			shift->frames = 0;
			shift->seconds = 0;
			shift->minutes = 0;
			shift->hours = 0;

			BelleNuitFix(ebu);
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

	if(strcmp(LC,"  ")){
		strncpy(ebu->gsi.LC,LC,2);
	}
	if(strcmp(DSC," ")){
		ebu->gsi.DSC = DSC[0];
	}
	if(strcmp(CPN,"   ")){
		strncpy(ebu->gsi.CPN,CPN,3);
	}

	if(Teletextfix == 1){
		applyTeletextfix(ebu);
	}
	if(rmSPE == 1){
		EBURemoveSpecialChars(ebu);
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