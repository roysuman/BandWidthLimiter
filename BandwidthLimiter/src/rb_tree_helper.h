#ifndef RB_TREE_HELPER_H
#define RB_TREE_HELPER_H
#include <stdint.h>
#include<stdio.h>
#include<stdlib.h>
void int_dest(void* a);

int int_comp(const void* a,const void* b) ;
void int_print(const void* a) ;
void info_print(void* a);
void info_dest(void *a);
void uint32_dest(void* a) ;
int uint32_comp(const void* a,const void* b) ;
void uint32_print(const void* a);
#endif
