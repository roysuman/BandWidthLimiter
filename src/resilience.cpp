/*
 * =====================================================================================
 *
 *       Filename:  resilience.cpp
 *
 *    Description:  the main programm
 *
 *        Version:  1.0
 *        Created:  Thursday 23 April 2015 03:09:50  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy (), 
 *   Organization:  Cognizant Technological solutions
 *
 * =====================================================================================
 */
#define _GNU_SOURCE 1
#include<iostream>
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define VERSION "1.0"
#define MAX_PATH_LENGTH 256
#define LIB_NAME "libc.so.6"
//#define DEBUG 1
void usage(void  ){

	std::cout<<"\n\t -f/-F Enabled config file\n"
		<<"\n\t-u: Uplimits\n"
		<<"\t-d: Downlimits\n"
		<<"\t-h: Help\n"
		<<"\t-l: Licence\n"
		<<"\t-i: Ipaddress\n"
		<<"\t-p: Ports\n"
		<<"\t-P: Ipc Server Port"
		<<"\t-v: Version\n"
		<<"\t-I:system IP\n"
		<<"\t-g: Kernel GlibC path at local machine\n"
		<<"\t At the end programm to execute\n";
	std::cout<<"Option -g and executable prog name is mandatory\n"
		<<"Resilience Version "<<VERSION<<" Author: Suman roy ( suman.roy@cognizant.com)\n";
	exit(EXIT_FAILURE);
}

void 
parse(char *line, char **argv){
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv = NULL;//'\0';                 /* mark the end of argument list  */
}
void  
execute(char **argv){
#ifdef DEBUG
	std::cout<<"\nargv[0]"<<argv[0]<<std::endl
		<<"argv[1]"<<argv[1]<<std::endl
		<<"argv[2]"<<argv[2]<<std::endl
		<<"argv[3]"<<argv[3]<<std::endl
		<<"argv[4]"<<argv[4]<<std::endl;
#endif
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
}
int main( int argc , char** argv){
	
	int      opt;
	bool     flagConfig = false;
	
	char*    port="5000;";
	char*    ipAddr="127.0.0.1;";
	char*    sysIp="127.0.0.1";
	char*    upLimit="-1;";
	char*    downLimit="-1;";
	char*    glibc="";
	char*    latency="0";
	char     buff [ MAX_PATH_LENGTH];
	bool     ifGlib;
	bool     cmdFlag;
	char     *command;//[1024];             /* the input line                 */
	char     *argv2[64];              /* the command line argument      */
	char     *ipcPort = "8888";

	ifGlib = false;
	cmdFlag = false;
	while (  ( opt = getopt ( argc , argv ,"fFhHvVi:I:u:U:d:D:l:L:p:P:g:G:c:C:") ) != -1 && !ifGlib )
		switch ( opt ){
			case 'f':
			case 'F':
				flagConfig = true;
				break;

			case 'u':
			case 'U':
				upLimit =  optarg;
				break;
			case 'd':
			case 'D':
				downLimit = optarg ;
				break;
			case 'l':
			case 'L':
				latency = optarg;
				break;
			case 'i':
				ipAddr = optarg;
				break;
			case 'I':
				sysIp = optarg;
			case 'p':
				port = optarg;
				break;
			case 'P':
				ipcPort = optarg;
			case 'v':
			case 'V':
				std::cout<<"Version "<<VERSION<<"-- under test\n";
				break;
			case 'g':
			case 'G':
				glibc = optarg;
				ifGlib = true;

				break;
			case 'C':
			case 'c':
				command = optarg;
				cmdFlag = true;
			case '?':
				if (optopt == 'c')
					std::cerr<<"Option -"<<optopt<<" requires an argument.\n";
				else if (isprint (optopt))
					std::cerr<<"Unknown option "<< optopt<<std::endl;
				break;
			case 'h':
			case 'H':
			default:
				usage();
		}
		argc -= optind  ;
		argv +=optind  ;
#ifdef DEBUG
	std::cout<<"RESILIENCE--\n iPaDDRESS [ "<<ipAddr<<" ] pORT [ "<<port
		<<" ] uPLIMIT [ "<<upLimit<<" ] dOWNLIMIT [ "<<downLimit
		<<" ] Latency [ "<<latency<<" ] Command [ "<<command<<" ]\n";
#endif
	if ( flagConfig  && cmdFlag && ifGlib ){
		setenv("RESILIENCE_CONFIGFLAG","1", 1 );
		goto CALL;
	}


	if(  !ifGlib || !cmdFlag  ) usage();
	

	setenv("RESILIENCE_UP_LIMITS",upLimit, 1 );
	setenv("RESILIENCE_IPS", ipAddr, 1);
	setenv("RESILIENCE_PORTS", port, 1 );
	setenv("RESILIENCE_DOWN_LIMITS",downLimit, 1 );
	setenv("RESILIENCE_LATENCY",latency ,1); 
	setenv("RESILIENCE_SYS_IP",sysIp,1);
	setenv("RESILIENCE_IPC_PORT" , ipcPort , 1 );
	setenv("RESILIENCE_CONFIGFLAG","0", 1 );
CALL:
	setenv("RESILIENCE_GLIBC_PATH",glibc,1);
	parse(command,argv2);
	if ( getcwd( buff , sizeof(buff)) == NULL ) {
		std::cerr<<"\ngetced-error\n";
		return(0);
	}

	std::string pwd = std::string(buff);
	std::string lib = pwd +"/" + LIB_NAME ;
	setenv( "LD_PRELOAD",lib.c_str(),1);
	execute(argv2);
	//control will never return
	return 0;
}

