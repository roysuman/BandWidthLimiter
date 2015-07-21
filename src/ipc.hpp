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
 *         Author:  ROY (), suman.roy@cognizant.com 
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
#include "globals.hpp"
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
		static std::string                sysIp;

		//method
		static int ipcServerPort;
		static globals::JsonData                  jsonConfig;


        public:
                IpcServer( void );
                ~IpcServer( void );
		bool createServer( void );
		void getPort(int &);
		bool closeServer( void );
		bool isServerStart(void);
		


};
#endif

