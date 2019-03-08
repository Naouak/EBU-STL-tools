#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ebu.h"

int log(char* opt, char* name, char* format, ...) {
	if (opt != NULL && strcmp(opt, name) != 0) {
		return 0;
	}

	va_list args;
	va_start(args, format);

	if (opt != NULL && strcmp(opt, name) == 0) {
		vprintf(format, args);
	}
	else {
		char output[80];
		strcpy(output, name);
		strcat(output, ": ");
		strcat(output, format);
		vprintf(output, args);
	}
	va_end(args);
	return 1;
}

void dumpTTI(struct EBU_TTI* tti){
	printf("SGN: %hX\n",tti->SGN);
	printf("SN : %02hX%02hX\n",tti->SN[1],tti->SN[0]);
	printf("EBN: %hX\n",tti->EBN);
	printf("CS : %hX\n",tti->CS);
	printf("TCI: %02d:%02d:%02d:%02d\n",tti->TCI.hours,tti->TCI.minutes,tti->TCI.seconds,tti->TCI.frames);
	printf("TCO: %02d:%02d:%02d:%02d\n",tti->TCO.hours,tti->TCO.minutes,tti->TCO.seconds,tti->TCO.frames);
	printf("VP : %hX\n",tti->VP);
	printf("JC : %hX\n",tti->JC);
	printf("CF : %hX\n",tti->CF);

	int i = 0;
	int j = 0;
	for(i = 0; i < 112; i++){
		if(tti->TF[i] == 0X1E){
			tti->TF[i] = 0x8A;
		}
		if(tti->TF[i] >= 0x20 && tti->TF[i] < 0x80){
			j++;
		}
		if(tti->TF[i] == 0x8A || tti->TF[i] == 0x8F){
			printf("Char Count : %d\t%s\n",j,tti->TF);
			j = 0;
			if(tti->TF[i] == 0x8F){
				break;
			}
		}

	}


	printf("TF : %.112s\n",tti->TF);
}

int main(int argc, const char** argv) {
	char * output = NULL;
	char * option = NULL;
	int full = 0;
	int i = 0;
	for (i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i],"-f")){
			full = 1;
		}
		else if (strstr(argv[i], "--option=") != NULL){
			option = strtok(argv[i], "--option=");
		}
		else{
			output = (char *) argv[i];
		}
	}

	if(output == NULL){
		if(output == NULL)
			printf("no output set\n");
		printf("Usage: %s [-f] input.stl\n",argv[0]);
		return 0;
	}

	FILE* source = fopen(output,"r");
	if(source == NULL){
		printf("Error: Source not loaded\n");
		fclose(source);
		return 1;
	}

	if (full == 1) {
		option = NULL;
	}

	struct EBU* ebu = parseEBU(source);
	fclose(source);

	isBelleNuit(ebu);

	log(option, "CPN", "%.3s\n",ebu->gsi.CPN);
	log(option, "DFC", "%.8s\n",ebu->gsi.DFC);
	log(option, "DSC", "%c\n",ebu->gsi.DSC);
	log(option, "CCT", "%.2s\n",ebu->gsi.CCT);
	log(option, "LC",  "%.2s\n",ebu->gsi.LC);
	log(option, "OPT", "%.32s\n",ebu->gsi.OPT);
	log(option, "OET", "%.32s\n",ebu->gsi.OET);
	log(option, "TPT", "%.32s\n",ebu->gsi.TPT);
	log(option, "TET", "%.32s\n",ebu->gsi.TET);
	log(option, "TN",  "%.32s\n",ebu->gsi.TN);
	log(option, "TCD", "%.32s\n",ebu->gsi.TCD);
	log(option, "SLR", "%.16s\n",ebu->gsi.SLR);
	log(option, "CD",  "%.6s\n",ebu->gsi.CD);
	log(option, "RD",  "%.6s\n",ebu->gsi.RD);
	log(option, "RN",  "%.2s\n",ebu->gsi.RN);
	log(option, "TNB", "%.5s\n",ebu->gsi.TNB);
	log(option, "TNS", "%.5s\n",ebu->gsi.TNS);
	log(option, "TNG", "%.3s\n",ebu->gsi.TNG);
	log(option, "MNC", "%.2s\n",ebu->gsi.MNC);
	log(option, "MNR", "%.2s\n",ebu->gsi.MNR);
	log(option, "TCS", "%hX\n",ebu->gsi.TCS);

	struct EBU_TC *tc = charToTC(ebu->gsi.TCP);
	log(option, "TCP", "%02hd:%02hd:%02hd:%02hd\n", tc->hours,tc->minutes,tc->seconds,tc->frames);
	free(tc);
	tc = charToTC(ebu->gsi.TCF);
	log(option, "TCF", "%02hd:%02hd:%02hd:%02hd\n", tc->hours,tc->minutes,tc->seconds,tc->frames);
	free(tc);

	log(option, "TND", "%hX\n",ebu->gsi.TND);
	log(option, "DSN", "%hX\n",ebu->gsi.DSN);
	log(option, "CO",  "%.3s\n",ebu->gsi.CO);
	log(option, "PUB", "%.32s\n",ebu->gsi.PUB);
	log(option, "EN",  "%.32s\n",ebu->gsi.EN);
	log(option, "ECD", "%.32s\n",ebu->gsi.ECD);

	if(full == 1){
		char TNB[6];
		strncpy(TNB,ebu->gsi.TNB,5);
		TNB[5] = '\0';
		int nTNB = atoi(TNB);
		printf("%d\n",nTNB);
		for(i = 0; i < nTNB; i++){
			dumpTTI(&(ebu->tti[i]));
		}
	}
	
	free(ebu);

	return 0;
}