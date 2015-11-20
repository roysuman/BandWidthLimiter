#include "safe_malloc.h"

static void alloc_failed(const char *c, int length)
{
	fprintf(stderr, "%s of %d bytes failed. Exiting\n", c, length);
	exit(1);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  safe_malloc
 *  Description:  safe_malloc - mallocs specified space or exits if there's                  a problem
 * =====================================================================================
 */
char *safe_malloc(size_t length)
{
	char *ptr;

	if(length <= 0)
		length = 1;

	ptr = malloc(length);
	if(ptr == (char *)0)
	{
		/* doesn't return */
		alloc_failed("malloc", length);
	}
	memset( ptr , 0  , length);

	return ptr;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  safe_calloc
 *  Description:   callocs specified space or exits if there's a problem
 * =====================================================================================
 */
char *safe_calloc(int num, int length)
{
	char *ptr;

	ptr = calloc(num, length);
	if(ptr == (char *)0)
	{
		/* doesn't return */
		alloc_failed("calloc", length);
	}
	return ptr;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  safe_realloc
 *  Description:  - reallocs specified space or exits if there's a problem
 * =====================================================================================
 */

char *safe_realloc(char *src, int length)
{
	char *ptr;

	if (src)
		ptr = realloc((void *)src, length);
	else
		ptr = malloc(length);
	if(ptr == (char *)0)
	{
		/* doesn't return */
		alloc_failed("realloc", length);
	}
	return ptr;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  safe_strdup
 *  Description:   duplicates a string or exits if there's a problem
 * =====================================================================================
 */
char *safe_strdup(const char *src)
{
	char *cpy = safe_malloc(strlen ( src ) + 1 );


	if (cpy == (char *)0)
	{
		/* doesn't return */
		alloc_failed("strdup", strlen(src) + (size_t)1);
	}else{
		strcpy( cpy , src );
	}
	return cpy;
}
