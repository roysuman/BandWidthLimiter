/*
 * =====================================================================================
 *
 *       Filename:  threadClient.cpp
 *
 *    Description:  multiple thread
 *
 *        Version:  1.0
 *        Created:  Tuesday 09 June 2015 01:48:17  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include<iostream>
#include <sys/time.h>
#include <unistd.h>
#include<fstream>
#define MAX_SIZE 50
#define NUM_CLIENT 5
int por = 5001;
void *connection_handler(void *socket_desc);
int main()
{
    int socket_desc , new_socket , c , *new_sock, i;
    pthread_t sniffer_thread;
    for (i=1; i<=NUM_CLIENT; i++) {
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) i) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        //sleep(3);
    }
    pthread_exit(NULL);
    return 0;
}

void *connection_handler(void *threadid)
{
    int threadnum = (int)threadid;
    int sock_desc;
    struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];

    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5000);

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Failed to connect to server\n");
    }

    printf("Connected successfully client:%d\n", threadnum);
    std::string getLine;
    std::string fileName = "test/testData.txt";
    std::fstream fileHandler;
    fileHandler.open ( fileName.c_str() , std::ios::in );
    if( !fileHandler.is_open() ){
	    std::cerr<<"File not opened";
	    exit(0);
    }

    std::string data = "suman roy";

    while ( std::getline (fileHandler , getLine   )  ){
  //  while ( 1 ){

    
        printf("For thread : %d\n", threadnum);
//        fgets(sbuff, MAX_SIZE , stdin);
	std::cout<<"Sending data of size [ "<<getLine.size()<<" ] \n";
	write( sock_desc  , getLine.c_str()  , getLine.size()  );
//	write( sock_desc  , data.c_str()  , data.size()  );
    }
//        send(sock_desc,sbuff,strlen(sbuff),0);
/* 
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
            printf("Error");
        else
           fputs(rbuff,stdout);

        bzero(rbuff,MAX_SIZE);
        sleep(2);*/
    
   // close(sock_desc);
    return 0;
}
