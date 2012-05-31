#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"
#include "string_utils.h"

typedef struct {
	int hours;
	int minutes;
	int seconds;
	int milliseconds;
} SRT_timecode;

typedef struct {
	int number;
	SRT_timecode* TCI;
	SRT_timecode* TCO;
	char* text;
} SRT_item;

typedef struct {
	int count;
	SRT_item* srt;
} SRT;

void SRT_itemToEBU_TC(const SRT_timecode* srt, EBU_TC* tc){
	tc->hours = (char) srt->hours;
	tc->minutes = (char) srt->minutes;
	tc->seconds = (char) srt->seconds;
	tc->frames = (char) (srt->milliseconds/40);
}


SRT* loadSRT(FILE* src){
	SRT* srt = malloc(sizeof(SRT));
	srt->count = 0;

	char buffer[256];

	while(fgets(buffer, 256, src) != NULL){
		int count = 0;
		if(sscanf(buffer, "%d",&count) == 1){
			if(count == 0)
				continue;
			srt->count++;
			srt->srt = realloc(srt->srt,count*sizeof(SRT_item));
			SRT_item* item = &(srt->srt[srt->count-1]);

			srt->srt[srt->count-1].TCI = malloc(sizeof(SRT_timecode));
			srt->srt[srt->count-1].TCO = malloc(sizeof(SRT_timecode));
			srt->srt[srt->count-1].text = malloc(sizeof(char));
			srt->srt[srt->count-1].text[0] = '\0';

			//printf("%d\n",count);
			
			SRT_timecode *tcin = srt->srt[srt->count-1].TCI; 
			SRT_timecode *tcout = srt->srt[srt->count-1].TCO;

			if(fgets(buffer, 256, src)==NULL){
				break;
			}
			if(sscanf(
				buffer, "%d:%d:%d,%d --> %d:%d:%d,%d",
				&(tcin->hours),&(tcin->minutes),&(tcin->seconds),&(tcin->milliseconds),
				&(tcout->hours),&(tcout->minutes),&(tcout->seconds),&(tcout->milliseconds)
			) != 8){
				printf("Error format SRT");
				break;
			}
			else{		
				//printf("%d:%d:%d,%d --> %d:%d:%d,%d\n",tcin->hours,tcin->minutes,tcin->seconds,tcin->milliseconds,tcout->hours,tcout->minutes,tcout->seconds,tcout->milliseconds);
			}

			int first = 1;

			printf("*****************************\n");
			while(1){
				if(fgets(buffer, 256, src)==NULL){
					break;
				}
				printf("BUFFER:\t%d\t%s\n",strlen(buffer),buffer);
				printf("TEXT:\t%d\t%s\n",strlen(item->text),item->text);
				if(strlen(buffer) > 2){
					int length = strlen(item->text);
					if(first == 1){
						item->text = malloc(strlen(buffer) * sizeof(char));
						length = 0;
						first--;
					}
					else{
						item->text = realloc(item->text, (length + strlen(buffer)) * sizeof(char) );
						item->text[length-1] = '\n';
					}
					
					strncpy(&(item->text[length]), buffer, strlen(buffer));
					item->text[length+strlen(buffer)-1] = '\0';
				}
				else {
					break;
				}
			}
			printf("%s\n",item->text);
			item->text = str_replace(item->text,"\r\n","\n");


		}

	}

	return srt;
}

EBU* srtToEBU(SRT* srt){
	EBU* ebu = malloc(sizeof(EBU));

	strncpy(ebu->gsi.CPN,"850",3);
	strncpy(ebu->gsi.DFC,"STL25.01",8);
	ebu->gsi.DSC = '1';
	strncpy(ebu->gsi.CCT,"00",2);
	strncpy(ebu->gsi.LC,"0F",2);
	strcpy(ebu->gsi.OPT,"Original Program Title");
	strcpy(ebu->gsi.OET,"Original Episode Title");
	strcpy(ebu->gsi.TPT,"Translated Program Title");
	strcpy(ebu->gsi.TET,"Translated Episode Title");
	strcpy(ebu->gsi.TN,"Auto-Generated");
	strcpy(ebu->gsi.TCD,"github.com/naouak/ebutools");
	strcpy(ebu->gsi.SLR,"");
	strncpy(ebu->gsi.CD,"000000",6);
	strncpy(ebu->gsi.RD,"000000",6);
	strncpy(ebu->gsi.RN,"01",2);

	char buf[6] = "";
	sprintf(buf,"%05d",srt->count);
	strncpy(ebu->gsi.TNB,buf,5);
	strncpy(ebu->gsi.TNS,buf,5);

	strncpy(ebu->gsi.TNG,"000",3);
	strncpy(ebu->gsi.MNC,"40",2);
	strncpy(ebu->gsi.MNR,"23",2);
	ebu->gsi.TCS = '1';
	strncpy(ebu->gsi.TCP,"00000000",8);

	char buf2[9] = "00000000";
	if(srt->count > 0)
		sprintf(buf2,"%02d%02d%02d%02d",srt->srt[0].TCI->hours,srt->srt[0].TCI->minutes,srt->srt[0].TCI->seconds,srt->srt[0].TCI->milliseconds/40);

	strncpy(ebu->gsi.TCF,"00000000",8);
	ebu->gsi.TND = '1';
	ebu->gsi.DSN = '1';
	strncpy(ebu->gsi.CO,"FRA",3);
	strcpy(ebu->gsi.PUB,"");
	strcpy(ebu->gsi.EN,"");
	strcpy(ebu->gsi.ECD,"");

	ebu->tti = malloc(srt->count*sizeof(EBU_TTI));

	int i = 0;
	for(i = 0; i < srt->count; i++){
		//printf("%d\n",i);
		ebu->tti[i].SGN = 1;
		ebu->tti[i].SN[1] = i/256;
		ebu->tti[i].SN[0] = i%256;
		ebu->tti[i].EBN = 0xFF;
		ebu->tti[i].CS = 0;
	    SRT_itemToEBU_TC(srt->srt[i].TCI,&(ebu->tti[i].TCI));
	    SRT_itemToEBU_TC(srt->srt[i].TCO,&(ebu->tti[i].TCO));

	    ebu->tti[i].VP = 20 - str_count(srt->srt[i].text,'\n') * 2;
	    ebu->tti[i].JC = 2;
	    ebu->tti[i].CF = 0;

	    int j;

	    
	    if(strlen(srt->srt[i].text) > 112){
	    	srt->srt[i].text[111] = '\0';
	    }



	    int len = strlen(srt->srt[i].text);
	    char * text = malloc(112 * sizeof(char));
	    for(j=0;j<112;j++){
	    	text[j] = 0x8F;
	    }
	    strncpy(text,srt->srt[i].text,len>112?112:len);

	    //printf("%.112s\n",text);

	    char single[2] = " ";
	    single[0] = 0x80;
	    str_replace(text,"<I>",single);
	    str_replace(text,"<i>",single);
	    single[0] = 0x81;
	    str_replace(text,"</I>",single);
	    str_replace(text,"</i>",single);
	    single[0] = 0x82;
	    str_replace(text,"<U>",single);
	    str_replace(text,"<u>",single);
	    single[0] = 0x83;
	    str_replace(text,"</U>",single);
	    str_replace(text,"</u>",single);
	    single[0] = 0x84;
	    str_replace(text,"<B>",single);
	    str_replace(text,"<b>",single);
	    single[0] = 0x85;
	    str_replace(text,"</B>",single);
	    str_replace(text,"</b>",single);

	    single[0] = 0XFA;
		str_replace(text,"œ",single);
		single[0] = 0XEA;
		str_replace(text,"Œ",single);	    


	    char accent[3] = " e";
	    accent[0] = 0xC2;
	    str_replace(text,"é",accent);
	    accent[0] = 0xC1;
	    str_replace(text,"è",accent);
	    accent[0] = 0xC3;
	    str_replace(text,"ê",accent);
	    accent[1] = 'a';
	    str_replace(text,"â",accent);
	    accent[0] = 0xC1;
	    str_replace(text,"à",accent);
	    accent[1] = 'u';
	    str_replace(text,"ù",accent);
	    accent[0] = 0xC3;
	    str_replace(text,"û",accent);
	    accent[1] = 'i';
	    str_replace(text,"î",accent);
	    accent[1] = 'o';
	    str_replace(text,"ô",accent);

	    accent[1] = 'E';
	    accent[0] = 0xC2;
	    str_replace(text,"É",accent);
	    accent[0] = 0xC1;
	    str_replace(text,"È",accent);
	    accent[0] = 0xC3;
	    str_replace(text,"Ê",accent);
	    accent[1] = 'A';
	    str_replace(text,"Â",accent);
	    accent[0] = 0xC1;
	    str_replace(text,"À",accent);
	    accent[1] = 'U';
	    str_replace(text,"Ù",accent);
	    accent[0] = 0xC3;
	    str_replace(text,"Û",accent);
	    accent[1] = 'I';
	    str_replace(text,"Î",accent);
	    accent[1] = 'O';
	    str_replace(text,"Ô",accent);

	    accent[0] = 0xCB;
	    accent[1] = 'c';
	    str_replace(text,"ç",accent);
	    accent[1] = 'C';
	    str_replace(text,"Ç",accent);

	    accent[0] = 0xC2;
	    accent[1] = 0xA0;
	    single[0] = 0xA0;
	    str_replace(text,accent,single);

	    /*accent[0] = 0x00;
	    accent[1] = 0xA0;
	    single[0] = 0xA0;
	    str_replace(text,accent,single);
		*/

	    j = (len>112?112:len)-1;
	    for(;j < 112; j++){
	    	text[j] = 0x8F;
	    }
	    for(j=0;j<112;j++){
	    	if(text[j] == '\n'){
	    		text[j] = 0x8A;
	    	}
	    	if(text[j] == 0x00){
	    		text[j] = 0x8F;
	    	}
	    }
	    //printf("%.112s\n",text);
	    strncpy(ebu->tti[i].TF,text,112); 
	    
	    //printf("%.112s\n",ebu->tti[i].TF);
	}

	return ebu;
}

void main(int argc, char** argv){
	FILE* f = fopen(argv[1],"r");
	if(f == NULL){
		printf("error opening file");
		return;
	}
	SRT* srt = loadSRT(f);
	fclose(f);
	
	EBU* ebu = srtToEBU(srt);
	
	f = fopen(argv[2],"w");
	saveEBU(f,ebu);
	fclose(f);
	
}