/*
 * =====================================================================================
 *
 *       Filename:  interface.hpp
 *
 *    Description:  interface that handle everything
 *
 *        Version:  1.0
 *        Created:  Thursday 23 April 2015 06:34:33  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy (), 
 *   Organization:  Cognizant Technological solutions
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  for function description read the interface.cpp file
 *-----------------------------------------------------------------------------*/
#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include<iostream>
#include <time.h>
#include <sys/time.h>
#include "fd.hpp"
#include "ipc.hpp"
#include "lock.hpp"
//#include "readConfig.hpp" --- geting stdc++ lib error during read
#include "registerEnv.hpp"
#define NANO_MULT  1000000000L 
namespace __resilience__{
	class Interface: public fd::Fd , public regEnv::RegisterEnv {
		private:
			static Lock           lock;
			static IpcServer      ipcSerIns;
			static std::thread    ipcThread;
			static int            ipcServerFd;
			static int            ipcServerPort;

		public:
			Interface( void ){}
			~Interface(void ){if ( ipcThread.joinable() ) ipcThread.join();}
			
			bool init(void );
			bool init( int , uint32_t  );
			bool updateTime( int , bool );
			struct timespec returnCurrentTime( void );
			long int timeDifference( struct timespec , struct timespec );
			ssize_t getPortStatus( int  ,  bool);
			ssize_t updateReadBuffer( size_t , size_t  , int , void * );
			ssize_t updateSendBuffer( size_t , size_t , int , void * );
			void   readWriteLock( const int , const bool , const bool );
			bool initStartTimeForLatency( const int fd , const bool flag );
			void startIpcServer( void );



		};
}


#endif

