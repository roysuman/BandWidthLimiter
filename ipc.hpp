/*
 * =====================================================================================
 *
 *       Filename:  ipc.hpp
 *
 *    Description:  interprocess communication over socket--to a remote machine                                                                       
 *
 *        Version:  1.0
 *        Created:  Sunday 10 May 2015 10:02:32  IST 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com 
 *   Organization:  NA
 *
 * =====================================================================================
 */
#ifndef IPC__HPP__
#define IPC__HPP__
  
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>
#include "registerEnv.hpp" 
extern void check(void);
class IpcServer{
        private:
		static bool                       live;
		static int                        sockFd;
		static int                        connFd;
		static regEnv::RegisterEnv        envIns;
		static __updateLimits__           limits;
		static socklen_t                  addr_size;
		static bool                       setSerEnv;
                static struct sockaddr_in         serverAddr;   
                static struct sockaddr_in         clientAddr;   
		static struct sockaddr_storage    serverStorage;
		//method
		bool createServer( void );
		bool acceptClient( void );
		static int ipcServerPort;


        public:
                IpcServer( void );
                ~IpcServer( void );
		void manager(void );
		void getPort(int &);
		


};
#endif

