#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ebu.h"

int main(int argc, const char* argv[]) {
	char * input = NULL;
	char * output = NULL;
	int nshift = 0;
	int i = 0;
	for (i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i],"-i")){
			i++;
			if(i < argc){
				input = argv[i];
			}
		}
		else if(!strcmp(argv[i],"-t")){
			i++;
			if(i < argc){
				nshift = atoi(argv[i]);
			}
		}
		else{
			output = argv[i];
		}
	}

	if(input == NULL || output == NULL || nshift == 0){
    if(input == NULL)
      printf("no input set\n");
    if(output == NULL)
      printf("no output set\n");
    if(nshift == 0)
      printf("no shift set\n");
		printf("Usage: %s -i input.stl -t timeshift([-]HHMMSSFF) output.stl",argv[0]);
		return 0;
	}


  FILE* source = fopen(input,"r");
  if(source == NULL){
  		fclose(source);
  		return 1;
  }
  struct EBU ebu = parseEBU(source);
  fclose(source);

  struct EBU_TC shift;
  int positive = nshift>0?1:-1;
  if(positive < 0)
    nshift *= -1;
  shift.frames = nshift%100;
  nshift /= 100;
  shift.seconds = nshift%100;
  nshift /= 100;
  shift.minutes = nshift%100;
  nshift /= 100;
  shift.hours = nshift%100;
  
  

  shiftTCs(&ebu,shift,positive);
  FILE* dest = fopen(output,"w");
  if(dest == NULL){
  	fclose(dest);
  	return 2;
  }
  saveEBU(dest,&ebu);
  fclose(dest);


  unsigned char TNB[6];
  strncpy(TNB,ebu.gsi.TNB,5);
  TNB[5] = '\0';
  int nTNB = atoi(TNB);

  for(i = 0; i < nTNB; i++){
    printf("%02d:%02d:%02d:%02d\n",ebu.tti[i].TCI.hours,ebu.tti[i].TCI.minutes,ebu.tti[i].TCI.seconds,ebu.tti[i].TCI.frames);
    printf("%02d:%02d:%02d:%02d\n",ebu.tti[i].TCO.hours,ebu.tti[i].TCO.minutes,ebu.tti[i].TCO.seconds,ebu.tti[i].TCO.frames);
  }



  return 0;
}