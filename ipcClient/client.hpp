/*
 * =====================================================================================
 *
 *       Filename:  client.hpp
 *
 *    Description:  client for ipc communication
 *
 *        Version:  1.0
 *        Created:  Thursday 14 May 2015 12:34:28  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */

#ifndef CLIENT_HPP_
#define CLIENT_HPP_ 
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<iostream>
#include<getopt.h>
#include<stdlib.h>
#include <unistd.h>
#include<string.h>
#include<mutex>
#include<pthread.h>
#include<thread>
typedef struct _updateLimits__ __updateLimits__;
struct _updateLimits__{
	int                 upLimit;
	int                 downLimit;
	int                 bindPort;
	
};

class IpcClient{
	private:

		int    fd;
		struct sockaddr_in serverAddr;
	public:
		std::thread    threadClient;
		IpcClient(){}
		~IpcClient(){if ( threadClient.joinable() ) threadClient.join();}
		bool createClient( void );
		void initClient(void);
		bool sendData( __updateLimits__ );
		void usage( void );
};
#endif
