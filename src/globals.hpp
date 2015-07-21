/*
 * =====================================================================================
 *
 *       Filename:  globals.hpp
 *
 *    Description:  read from config file
 *
 *        Version:  1.0
 *        Created:  Thursday 25 June 2015 10:39:29  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
//-ljson_linux-gcc-4.8_libmt
//READ FROM JSON FILE
#ifndef _GLOBALS_HPP
#define _GLOBALS_HPP
#include<iostream>
#include<fstream>
#include "jsoncpp/json.h"
#include "jsoncpp/writer.h"
#include <unistd.h>
#include <sys/types.h>
#include<thread>
//#define DEBUG 1
//#define TEST 1
//#define OUTPUT 1
#define PRINT __DATE__<<"_"<<__TIME__<<" [ "<<__FILE__<<" ] [ "<<__FUNCTION__<<" ] [ "<<__LINE__<<" ] Thread Id [ "<<std::this_thread::get_id()<<"] PID [ "<<getpid()<<"] "
#define FILE_NAME config/systemConfig.json
#define SIZE 100
namespace globals{
	typedef struct _jsonPortData jsonPortData;
	struct _jsonPortData{
		unsigned int port;
		std::string ip;
		unsigned int downLimit;
		unsigned int upLimit;
		unsigned int latency;
	};
	typedef struct _jsonIpcServerConfig jsonIpcServerConfig;
	struct _jsonIpcServerConfig{
		std::string     ipcIpAddress;
		unsigned int    port;
	};
	class JsonData{
		public:
			int           portCount;
			static jsonPortData           portBindData[SIZE];
			static jsonIpcServerConfig    ipcServerConfig;
			static std::string            glibcPath;
			static std::fstream    fileHandler;              
			bool notParsed( std::string);	
			bool readFromConfigFile();
			
	};
}

#endif 
