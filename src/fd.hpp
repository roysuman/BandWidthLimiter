/*
 * =====================================================================================
 *
 *       Filename:  fd.hpp
 *
 *    Description:  database for all file descriptor
 *
 *        Version:  1.0
 *        Created:  Thursday 23 April 2015 10:36:34  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy (), 
 *   Organization:  Cognizant Technological solutions
 *
 * =====================================================================================
 */

#ifndef FD_HPP_
#define FD_HPP_
#define DEFAULT_UP 2
#define DEFAULT_DOWN 2
#include<unordered_map>
#include<map>
#ifdef UBUNTU
#include<uistd.h>
#endif
#include<stdint.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <mutex>
#include<thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "buffer.hpp"
//#define DEBUG 1
//#define PRINT __DATE__<<"_"<<__TIME__<<" [ "<<__FILE__<<" ] [ "<<__FUNCTION__<<" ] [ "<<__LINE__<<" ] "
/*-----------------------------------------------------------------------------
 *  bellow class for handling run time data..when a socket is geting created
 *  corresponding informaton need to store--( socket downlimit, uplimit, fd , port 
 *  and so on)
 *-----------------------------------------------------------------------------*/
namespace fd{

	//file descripto book mark
	typedef struct _fdLookUpStruct fdLookUpStruct;
	struct _fdLookUpStruct{
		pid_t          pid;
		uint32_t       port;
		ssize_t        refCount;
		std::string    ipAddress;
	};

	typedef std::map< int , fdLookUpStruct > fdLookupContainerType;
	typedef std::map< int , fdLookUpStruct >::iterator fdLookupItType;


	/*-----------------------------------------------------------------------------
	 *  struct for storing information of a file descriptor and it's corresponding 
	 *  parameters...... ( which read from registered environment config)
	 *-----------------------------------------------------------------------------*/
	typedef struct _fdFields__ fdFieldsStruct;
	struct _fdFields__{
		pid_t             pid;
		int               pFdNumber;
		__message__       *readBuff;
		__message__       *writeBuff;
		struct timespec   startTimeOfRequest_READ;
		struct timespec   startTimeOfRequest_WRITE;


	};

	/*-----------------------------------------------------------------------------
	 *  stl container for mapping file descriptor and it's parameters
	 *-----------------------------------------------------------------------------*/
	typedef std::map<  int , fdFieldsStruct >               fdContainerType;
//typedef of stl map iterator
	typedef  std::map< int , fdFieldsStruct >::iterator    fdFieldStructItType;

	/*-----------------------------------------------------------------------------
	 *  read the fd.cpp file for function description
	 *-----------------------------------------------------------------------------*/
	class Fd{
		private:
			static Message                 bufferCacheIns;
			static	fdContainerType        fdContainer;
			static fdLookupContainerType       fdLookupContainer;
		public:
			               Fd( void ){}
			               ~Fd( void ){}
			pid_t          getPid( const int );
			bool           isPresent(const  int );
			bool           getDestIp( const int , std::string &);
			bool           getDestPort(const int , int & );
			bool           init(const int, const int ,const std::string );
			bool           registerSock( const int ,const int );
			ssize_t        accessReadBufferData(const int , void* ,const ssize_t  , const bool );

			ssize_t        accessSendBufferData(const int , void* , const ssize_t  ,const bool );

			bool           unregisterFileDescriptor( const int );
			bool           insertStartTimeOfPacket( const int , const bool , struct timespec  );
			bool           getStartTimeOfPacket( const int , const bool , struct timespec &);


	};
}


#endif

