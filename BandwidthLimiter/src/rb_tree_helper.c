#include "rb_tree_helper.h"

void 
int_dest(void* a) {
	free((int*)a);
}

int 
int_comp(const void* a,
	const void* b) {

	if( *(int*)a > *(int*)b) return(1);
	if( *(int*)a < *(int*)b) return(-1);
	return(0);
}

void 
int_print(const void* a) {
	printf("%i",*(int*)a);
}

void 
info_print(void* a) {
	 ;
}

void 
info_dest(void *a){
#ifdef DEBUG
	fprintf(stdout,"Info clean\n");
#endif
	free (a);
  ;
}
void 
uint32_dest(void* a) {
	;
//FIXME fix this free method.. which gwtting seg fault 
//	free(a);
//  free((uint32_t*)a);
}

int 
uint32_comp(const void* a,
	   const void* b) {

	if( *(uint32_t*)a > *(uint32_t*)b) return(1);
	if( *(uint32_t*)a < *(uint32_t*)b) return(-1);
	return(0);
}

void 
uint32_print(const void* a) {
	fprintf(stdout,"%u",*(uint32_t*)a);
}
