#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

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
#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 5000 /*port*/

int
main(int argc, char **argv)
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];

 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=2) {
  perror("Usage: TCPClient <IP address of the server");
//  exit(1);
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
 servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }

    std::string fileName = "test/testData.txt";
    std::fstream fileHandler;
    fileHandler.open ( fileName.c_str() , std::ios::in );
    if( !fileHandler.is_open() ){
	    std::cerr<<"File not opened";
	    exit(0);
    }
    std::string getLine;
    while( std::getline (fileHandler , getLine   ) ){
//	    std::cout<<getLine<<std::endl;
	    std::cout<<"Sending data\n";
	    ssize_t count = send(sockfd, getLine.c_str(), getLine.size(), 0);
	    std::cout<<"Send data: [ "<<count<<" ] \n";
/* /	     if (recv(sockfd, recvline, MAXLINE,0) == 0){
	    // error: server terminated prematurely
	      perror("The server terminated prematurely");
	      exit(4);
	     }*/
    }
  
//  printf("%s", "String received from the server: ");
 // fputs(recvline, stdout);
 

 exit(0);
}

