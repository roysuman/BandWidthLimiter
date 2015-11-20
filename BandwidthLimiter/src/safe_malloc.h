#ifndef SAFE_MEM_
#define SAFE_MEM_ 
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include<limits.h>
#include "bandwidth_globals.h"
char *safe_malloc(size_t length);
char *safe_calloc(int num, int length);
char *safe_realloc(char *ptr, int length);
char *safe_strdup(const char *s);
#endif
