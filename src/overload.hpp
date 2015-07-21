/*
 * =====================================================================================
 *
 *       Filename:  overload.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Wednesday 17 June 2015 01:09:20  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */

#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/time.h>
#include <mutex>
#include <thread>
#include <netinet/in.h>
#include <err.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdarg.h>
#include <string.h>
#include<stdbool.h>
#include <signal.h>
#include <map>
#include "objectpool.hpp"
#include"interface.hpp"
#include "lock.hpp"
#include <stdarg.h>
//#define DEBUG 1
#define HOME 1
#ifndef OVERLOAD_HPP
#define OVERLOAD_HPP
extern  bool                       __isDelay__ ;
//TO DO
//if send(all send) returns error.. then fkush the buffer
#define __FUNCTION_DECLARE_(funcName, returnType, arguments)\
       	static returnType (*libc_##funcName) arguments


#define __LOAD__ADDR__( var ) do {\
	*(void **) (&libc_##var) = dlsym ( __handle__ , #var); \
	if ( ( __error__ = dlerror ( ) ) != NULL ){\
		std::cerr<<__error__<<std::endl;\
		exit ( -1 );	}	\
} while (0)
#endif
