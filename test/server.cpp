/*
 * =====================================================================================
 *
 *       Filename:  server.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Wednesday 08 April 2015 10:36:58  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include<iostream>
#include<string>
#include<fstream>
int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    std::string    sendBuff;


   // char sendBuff[1025];
    time_t ticks; 
    char recvBuff[2560];
#ifdef HOME
    std::string fileName= "/home/roy/Downloads/27_fileSystem.mp4";
#else
    std::string fileName = "test/testData.txt";
#endif
//    std::getline( std::cin , fileName );
    std::fstream fileHandler;
    fileHandler.open ( fileName.c_str() , std::ios::in );
    if( !fileHandler.is_open() ){
	    std::cerr<<"File not opened";
	    exit(0);
    }
/* 
    std::string sendBuff="suman roy is a bad boy";
    for ( int i = 0 ; i < 10 ; ++ i){
	    sendBuff =sendBuff + sendBuff;
	    std::cout<<"\nLength:["<<sendBuff.length()<<std::endl;
    }
    
  */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
//    const char * c = sendBuff.c_str();

    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
    std::cout<<"\n#####["<<connfd<<"]-----["<<listenfd<<"]\n";
    int count;
/* 	    while( std::getline (fileHandler , sendBuff )  ){
		    std::cout<<"Length:[ "<<sendBuff.length()<<" ] \n";
		    count =write(connfd, sendBuff.c_str(), sendBuff.length());
	    }*/
do{	    count = read ( connfd , recvBuff, sizeof( recvBuff )- 1 );
		std::cout<<recvBuff<<std::endl;	
		recvBuff[count] = 0;
}while ( count > 0 );
    close ( listenfd );

     //   sleep(1);

}
