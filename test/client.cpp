/*
 * =====================================================================================
 *
 *       Filename:  client.cpp
 *
 *    Description: socket client-- reduce bandwidth
 *     
 *
 *        Version:  1.0
 *        Created:  Wednesday 08 April 2015 11:15:06  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */


/*-----------------------------------------------------------------------------
 *  
 *-----------------------------------------------------------------------------*/
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
int main(int argc, char *argv[])
{
    int sockfd = 0, receiveresult = 0;
    char recvBuff[2560];
    struct sockaddr_in serv_addr; 
    struct timeval starttime,endtime,inststarttime,instendtime,actualinsttime;
    double downloadrate,timetodownload,instrate,expectedtime,delay;
    int limitrate;
    std::string getLine;


    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr<<"\n Error : Could not create socket \n";
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        std::cerr<<"\n inet_pton error occured\n";
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       std::cerr<<"\n Error : Connect Failed \n";
       return 1;
    } 

    std::string sendBuff="suman roy is a bad boy";
    for ( int i = 0 ; i < 10 ; ++ i){
	    sendBuff =sendBuff + sendBuff;
	    //std::cout<<"\nLength:["<<sendBuff.length()<<std::endl;
	    }
    std::string fileName = "./test/testData.txt";
    std::fstream fileHandler;
    fileHandler.open ( fileName.c_str() , std::ios::in );
    if( !fileHandler.is_open() ){
	    std::cerr<<"File not opened";
	    exit(0);
    }
  //  do{

//	    receiveresult = read(sockfd, recvBuff, sizeof(recvBuff)-1);
//	    recvBuff[receiveresult] = 0;
//	    std::cout<<recvBuff<<std::endl;
/*
 	    if ( receiveresult > 0 ){
		        gettimeofday(&instendtime, NULL);
			std::cout << "\nEnd time " << instendtime.tv_usec << " Start time " << inststarttime.tv_usec << std::endl;

			timetodownload = ((instendtime.tv_sec * 1000000 + instendtime.tv_usec) - (inststarttime.tv_sec * 1000000 + inststarttime.tv_usec));
			expectedtime = (double)receiveresult * 8/limitrate;
			expectedtime = expectedtime * 1000;
			delay =  expectedtime - timetodownload;
			if(delay > 0){
				usleep(delay);
			}

			//calculate the effectiveness

			gettimeofday(&actualinsttime, NULL);

			timetodownload = ((actualinsttime.tv_sec * 1000000 + actualinsttime.tv_usec) - (inststarttime.tv_sec * 1000000 + inststarttime.tv_usec));
			if(timetodownload > 0 && delay > 0){
				instrate = receiveresult * 8 * 1000 / (timetodownload);
				std::cout<<"\nCurrent rate is "<<instrate<<"Kbps"<<std::endl;
			}
	    }else { std::cerr<<"\n Read DONE\n";
		    break;
	    }


	    recvBuff[receiveresult] = 0;
	    std::cout<<recvBuff<<std::endl<<std::endl;
  	    //write( sockfd , recvBuff  , receiveresult );
	    write( sockfd , sendBuff.c_str()  , sendBuff.length() );

	    recvBuff[receiveresult] = 0;

  	    if(fputs(recvBuff, stdout) == EOF)
		    {
			    std::cerr<<"\n Error : Fputs error\n";
		    }

    }while ( receiveresult > 0);//receiveresult >0 ); 

*/

    int count;
    size_t loop =1;
    std::string line = "Suman roy";

	    while( std::getline (fileHandler , getLine   ) ){
//    while ( 1 ){
		    std::cout<<"Read data from file"<<loop++<<"\n";

		    count =write( sockfd  , getLine.c_str()  , getLine.size() );
		   // count =write( sockfd  , line.c_str()  , line.size() );
		    
		    std::cout<<"\n Send data of length [ "<<count<<" ] \n";
	    }
	    fileHandler.close();


    close ( sockfd );


    return 0;
}

