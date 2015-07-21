/*
 * =====================================================================================
 *
 *       Filename:  registerEnv.hpp
 *
 *    Description:  register the env into some ds
 *
 *        Version:  1.0
 *        Created:  Saturday 25 April 2015 12:17:13  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), suman.roy@cognizant.com
 *   Organization:  NA
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  read registerEnv.cpp for function definitions
 *-----------------------------------------------------------------------------*/

#ifndef REGISTERENV_HPP__
#define REGISTERENV_HPP__  
#include<iostream>
#include <string>
#include<stdlib.h>
#include<getopt.h>
#include<stdlib.h>
#include <unistd.h>
#include<time.h>
#include<map>
#include<stdint.h>
#include <mutex>
#include <thread>
#include <pthread.h>
#include "buffer.hpp"
#include "globals.hpp"
//#define LOCK_ERROR 1
typedef struct _limits__ __limits__;

/*-----------------------------------------------------------------------------
 *  structure for maintaing the upload limit and downloadlimit of a particular IP along with timer part
 *  -----------------------------------------------------------------------------*/

struct _limits__{
	ssize_t                  upLimit;
	uint32_t                 bindPort;
	pthread_mutex_t   readLock;
	pthread_mutex_t   writeLock;
	std::string              ipAddress;
	ssize_t                  downLimit;
	struct timespec          startTimeRead;
	struct timespec          startTimeWrite;
	ssize_t                  remainingReadByteForCurrentWindow;
	ssize_t                  remainingWriteByteForCurrentWindow;
	unsigned int             latency;

	
};

typedef struct _updateLimits__ __updateLimits__;
struct _updateLimits__{
	int                 upLimit;
	int                 downLimit;
	int                 bindPort;
	
};
/*-----------------------------------------------------------------------------
 *  slt map container for mapping port sting with the upload and download limit
 *  using slt map for better lookup performance log(n)..it's use red balck tree
 *  
 *-----------------------------------------------------------------------------*/
typedef std::map< uint32_t , __limits__  >    __limitsContainer__;
//iterator typedef
typedef __limitsContainer__::iterator          __limitsContainerIterator__;

 extern __limitsContainer__    limitStore;


namespace regEnv{
/*-----------------------------------------------------------------------------
 *  till now all the defination were for setting up the environment....
 *  reading from the environment variable and upload into datastructure
 *-----------------------------------------------------------------------------*/

	class RegisterEnv{
		private:
			static Message                buffIns;
			static globals::JsonData                  jsonConfig;
	//		 __limitsContainer__    limitStore;
		public:
			bool               doRegister( );
			                   RegisterEnv( void ){}
			                   ~RegisterEnv(void ){}
			ssize_t            getInitUpLimit( const uint32_t ); // todo--ip
			bool               modifyEntry( __updateLimits__  buff );
			ssize_t            getInitDownLimit( const uint32_t  ); // todo--ip
			struct timespec    getTimeVal( const uint32_t , bool );
			bool               updateTimeVal( const uint32_t ,  bool);
			bool               setByteCountWindow( int , ssize_t , bool );
			ssize_t            getRemainingByteCountForCurrentWindow( int , bool);
			bool               updateByteCountCurrentWindow(  int , ssize_t  , bool);
			bool               isPortRegistered( const uint32_t ); 
			bool               getIpAddress( int , std::string & );	
			void               acquireLock(const uint32_t ,  const bool );
			void               releaseLock(const uint32_t , const bool );
			bool               getLatencyOfPort( const uint32_t , uint32_t &);


	};

}
#endif
