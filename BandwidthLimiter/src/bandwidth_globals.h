#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include<error.h>
#include <errno.h>
#include <limits.h>

#define PTHREAD true
#ifdef PTHREAD
#include<pthread.h>
#endif

/*-----------------------------------------------------------------------------
 *  TO ENABLE DEBUG LOG__ UNCOMMENT THE BELOW LINE ELSE COMMENT
 *-----------------------------------------------------------------------------*/
//#define DEBUG true
/*-----------------------------------------------------------------------------
 *  TO ENABLE  INFO LOG__ UNCOMMENT THE BELOW LINE ELSE COMMENT
 *-----------------------------------------------------------------------------*/
//#define INFO true
#define MEM_SOCK_STRUCT         0x01
#define MEM_BUFF_STR            0x02
#define MEM_ENV_STR             0x03
#define ACTIVE_DELAY true
#define READ_REQUSET true
#define WRITE_REQUEST false
#define ZERO 0
#define NEG_ONE -1
long int global_flag;

#endif
