#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"

char* str_shift(char* source, int position, int shift){
	int i = 10;
	if(shift < 0){
		source = realloc(source,(strlen(source)-shift)*sizeof(char));
		int len = strlen(source);
		for(i=len; i < len-shift;i++){
			source[i] = '\n';
		}
		source[len-shift] = '\0';
	}
	
	if(shift > 0){
		for(i = position; i < strlen(source)-shift; i++){
			source[i] = source[i+shift];
		}
		source[strlen(source)-shift-1] = '\0';

		source = realloc(source,strlen(source)*sizeof(char));
	}
	else{
		for(i = strlen(source); i >= position; i--){
			source[i] = source[i+shift];
		}	
	}
	return source;
}

char* str_replace(char* source,char* needle,char* replace){
	//return source;
	int i = 0;

	for(i=0; i < strlen(source); i++){

		if(source[i] == needle[0]){
			int j = 0;
			int willreplace = 1;

			for(j=1; j < strlen(needle); j++){

				if(i+j >= strlen(source)){
					willreplace = 0;
					break;
				}
				if(i+j < strlen(source) && j < strlen(needle)){
					if((char)source[i+j] == (char)needle[j]){
					}
					else{
						willreplace = 0;
						break;
					}
				}

			}
			if(willreplace == 1){
				int diff = (int) ((int)strlen(needle) - (int)strlen(replace));
				source = str_shift(source,i,diff);
				int k = 0;
				for(k = 0; k < strlen(replace); k++){
					source[i+k] = replace[k];
				}
			}
		}
	}
	return source;
}

int str_count(const char* source,const char needle){
	int i = 0;
	int count = 0;
	for(i = 0; i < strlen(source);i++){
		if(source[i] == needle){
			count++;
		}
	}

	return count;
}
