#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ebu.h"

short unsigned int isBelleNuit(const struct EBU* ebu){
	unsigned char* TC;
	TC = (unsigned char *) ebu->gsi.TCP;

	if(
		(TC[0] < 0x30 || TC[0] > 0x39) || 
		(TC[1] < 0x30 || TC[1] > 0x39) || 
		(TC[2] < 0x30 || TC[2] > 0x39) || 
		(TC[3] < 0x30 || TC[3] > 0x39) 
	){
		return 1;
	}
	else{
		return 0;
	}
}

void BelleNuitFix(const struct EBU* ebu){
	if(isBelleNuit(ebu)){
		strncpy(ebu->gsi.TCF,ebu->gsi.TCP,8);
		strncpy(ebu->gsi.TCP,"00000000",8);
	}
}

int TCcmp(const struct EBU_TC* tc1, const struct EBU_TC* tc2){
	if(tc1->hours != tc2->hours)
		return tc1->hours - tc2->hours;
	if(tc1->minutes != tc2->minutes)
		return tc1->minutes - tc2->minutes;
	if(tc1->seconds != tc2->seconds)
		return tc1->seconds - tc2->seconds;
	return tc1->frames - tc2->frames;
}

void TrimEBU(struct EBU* ebu,const struct EBU_TC* tc){
	char tnb[6];
	tnb[5] = '\0';
	strncpy(tnb,ebu->gsi.TNB,5);

	int TNB = atoi(tnb);
	int i;
	for(i = 0; i < TNB; i++){
		if(TCcmp(tc,&(ebu->tti[i].TCO)) < 0){
			i--;
			break;
		}
	}

	strncpy(ebu->gsi.TNB,tnb,5);
	strncpy(ebu->gsi.TNS,tnb,5);

	ebu->tti = realloc(ebu->tti,i*sizeof(struct EBU_TTI));
}

struct EBU* parseEBU(FILE* f){
	struct EBU* ebu = malloc(sizeof(struct EBU));

	fread(&(ebu->gsi), 1024, 1, f);

	unsigned char TNB[6];
	strncpy(TNB,ebu->gsi.TNB,5);
	TNB[5] = '\0';
	int nTNB = atoi(TNB);

	//printf("TNB : : %d \n",nTNB);

	ebu->tti = (struct EBU_TTI*) malloc(sizeof(struct EBU_TTI) * nTNB);
	fread(ebu->tti, sizeof(struct EBU_TTI), nTNB, f);

	return ebu;
}

void saveEBU(FILE* f,const struct EBU * ebu){
	fwrite(&(ebu->gsi),1024,1,f);

	unsigned char TNB[6];
	strncpy(TNB,ebu->gsi.TNB,5);
	TNB[5] = '\0';
	int nTNB = atoi(TNB);

	fwrite(ebu->tti,128,nTNB,f);
}

struct EBU_TC* charToTC(const unsigned char TC[8]){
	struct EBU_TC* tc = malloc(sizeof(struct EBU_TC));

	if(
		(TC[0] < 0x30 || TC[0] > 0x39) || 
		(TC[1] < 0x30 || TC[1] > 0x39) || 
		(TC[2] < 0x30 || TC[2] > 0x39) || 
		(TC[3] < 0x30 || TC[3] > 0x39) 
	){
		tc->hours = TC[0];
		tc->minutes = TC[1];
		tc->seconds = TC[2];
		tc->frames = TC[3];
	}
	else{
		unsigned char part[3];
		part[2] = '\0';

		strncpy(part,TC,2);
		tc->hours = atoi(part);
		strncpy(part,TC+2,2);
		tc->minutes = atoi(part);
		strncpy(part,TC+4,2);
		tc->seconds = atoi(part);
		strncpy(part,TC+6,2);
		tc->frames = atoi(part);
	}
	return tc;
}

void TCToChar(unsigned char tc[8],const struct EBU_TC TC){
	unsigned char * ctc = malloc(9*sizeof(unsigned char));

	int ntc = (((int)TC.hours*100+(int)TC.minutes)*100+(int)TC.seconds)*100+(int)TC.frames;
	sprintf(ctc,"%08d",ntc);

	strncpy(tc,ctc,8);
	free(ctc);
	return;
}

struct EBU_TC shiftTC(const struct EBU_TC* tc, const struct EBU_TC* shift, const short int positive){
	struct EBU_TC newTC;
	int carry = 0;
	int frames = (int) (tc->frames) - positive * (int) (shift->frames);
	if(frames < 0){
		carry = 1;
		frames+=25;
	}
	else if(frames > 24){
		carry = -1;
		frames -= 25;
	}
	newTC.frames = (unsigned char) frames;

	int seconds = (int) tc->seconds - positive * (int) shift->seconds - carry;
	carry = 0;
	if(seconds < 0){
		carry = 1;
		seconds += 60;
	}
	else if(seconds > 59){
		carry = -1;
		seconds -= 60;
	}
	newTC.seconds = (unsigned char) seconds;

	int minutes = (int) tc->minutes - positive * (int) shift->minutes - carry;
	carry = 0;
	if(minutes < 0){
		carry = 1;
		minutes += 60;
	}
	else if(minutes > 59){
		carry = -1;
		minutes -= 60;
	}
	newTC.minutes = (unsigned char) minutes;

	int hours = (int) tc->hours - positive * (int) shift->hours - carry;
	if(hours < 0){
		hours = 0;
	}
	else if(hours > 99){
		hours = 99;
	}
	newTC.hours = (unsigned char) hours;

	return newTC;
}

void shiftTCs(struct EBU* ebu, const struct EBU_TC* shift, const int positive){
	struct EBU_TC* tc = charToTC(ebu->gsi.TCF);
	struct EBU_TC newtc = shiftTC(tc,shift,positive);
	free(tc);

	TCToChar(ebu->gsi.TCF,newtc);

	tc = charToTC(ebu->gsi.TCP);
	newtc = shiftTC(tc,shift,positive);
	printf("%02d:%02d:%02d:%02d\n",newtc.hours,newtc.minutes,newtc.seconds,newtc.frames);


	
	free(tc);
	TCToChar(ebu->gsi.TCP,newtc);

	unsigned char TNB[6];
	strncpy(TNB,ebu->gsi.TNB,5);
	TNB[5] = '\0';
	int nTNB = atoi(TNB);

	int i = 0;
	for(i = 0; i < nTNB; i++){
		ebu->tti[i].TCI = shiftTC(&(ebu->tti[i].TCI),shift,positive);
	  	ebu->tti[i].TCO = shiftTC(&(ebu->tti[i].TCO),shift,positive);
	}

}

void EBUTC30to25(struct EBU_TC* tc){
	float newframes = (float) tc->frames;
	newframes *= 25;
	newframes /= 30;
	tc->frames = (int) roundf(newframes);
}

void EBU30to25(struct EBU* ebu){
	if(ebu->gsi.DFC[3] == '2' && ebu->gsi.DFC[4] == '5'){
		printf("No conversion needed\n");
		return;
	}

	ebu->gsi.DFC[3] = '2';
	ebu->gsi.DFC[4] = '5';

	struct EBU_TC* newtc;
	newtc = charToTC(ebu->gsi.TCP);
	EBUTC30to25(newtc);
	TCToChar(ebu->gsi.TCP,*newtc);

	newtc = charToTC(ebu->gsi.TCF);
	EBUTC30to25(newtc);
	TCToChar(ebu->gsi.TCF,*newtc);

	char cTNB[6];
	cTNB[5] = '\0';

	strncpy(cTNB,ebu->gsi.TNB,5);

	int TNB = atoi(cTNB);
	int i;
	for(i = 0; i < TNB; i++){
		EBUTC30to25(&(ebu->tti[i].TCI));
		EBUTC30to25(&(ebu->tti[i].TCO));
	}
}