/*
 *  * =====================================================================================
 * *
 **       Filename:  buffer.hpp
 **
 **    Description:  buffer management 
 **
 * *        Version:  1.0
 * *        Created:  Tuesday 12 May 2015 12:32:53  IST
 * *       Revision:  none
 * *       Compiler:  gcc
 * *
 * *         Author:  ROY (), email.suman.roy@gmail.com
 * *   Organization:  NA
 * *
 * * =====================================================================================
 * */
#ifndef BUFFER_HPP__
#define BUFFER_HPP__
#include <iostream>
#include <stdlib.h>
#include<string.h> 
#include<thread>
#include<mutex>
#define OUTPUT 1
#define MESSAGE_SIZE 256
#define BUFFER_POOL_INIT_SIZE 100
#define DEBUG 1
#define PRINT __DATE__<<"_"<<__TIME__<<" [ "<<__FILE__<<" ] [ "<<__FUNCTION__<<" ] [ "<<__LINE__<<" ] Thread Id [ "<<std::this_thread::get_id()<<"]"
typedef struct __message __message__;
struct __message{
	char          data[ MESSAGE_SIZE ];
	size_t        length;
	__message__   *next;
	__message__    *prev;
//	__message__   *nextFree;
};
class Message{
	private:
		static __message__       *freeHead;
		ssize_t                  bufferPoolSize;
		__message__              *getNode( void );
		__message__              *getFreeNode( void );
		void                     returnNode( __message__ *& );
		ssize_t                  incrementBufferSize( void );
		static pthread_mutex_t   bufferPoolUpdate;

	public:
		           Message():bufferPoolSize(0){}
		           ~Message(){}
		bool       init( );
		ssize_t    read(__message__ *&, void *buff, size_t length );
		ssize_t    write(__message__ *&, void *buff , size_t length );
};
#endif
