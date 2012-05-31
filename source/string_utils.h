#ifndef STRING_UTILS_H
#define STRING_UTILS_H value


char* str_shift(char* source, int position, int shift);
char* str_replace(char* source,char* needle,char* replace);
int str_count(const char* source,const char needle);

#endif