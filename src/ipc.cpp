
/*
 * =====================================================================================
 *
 *       Filename:  ipc.cpp
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
#include "ipc.hpp"
bool                       IpcServer::live;
__updateLimits__           IpcServer::limits;
regEnv::RegisterEnv        IpcServer::envIns;
int                        IpcServer::sockFd = -999;
int                        IpcServer::connFd;
bool                       IpcServer::setSerEnv;
socklen_t                  IpcServer::addr_size;
struct sockaddr_in         IpcServer::serverAddr;
struct sockaddr_in         IpcServer::clientAddr;
struct sockaddr_storage    IpcServer::serverStorage;
int                        IpcServer::ipcServerPort;// = 8888;
static std::string successString ="Updated";
static std::string errorString = "Error";
fd_set active_fd_set, read_fd_set;
globals::JsonData    IpcServer::jsonConfig;
std::string                IpcServer::sysIp;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  IpcServer
 *  Description:  constructor--- init the memory for __limits__
 * =====================================================================================
 */
IpcServer::IpcServer( void ){
	setSerEnv = false;
	/* 
	try{
		limits = new __limits__;
	}
	catch ( const std::bad_alloc &ba ){
		std::cerr<<PRINT<<"Exception"<<ba.what()<<std::endl;
		setSerEnv =false;
	}*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ~IpcServer
 *  Description:  Destructor -- free limitser memory ( __limits__ )
 * =====================================================================================
 */
IpcServer::~IpcServer( void ){/*
	try{
		delete limits;
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
	}*/
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  createServer
 *  Description:  create the server for remote ipc
 *               @return: true on success else false
 * =====================================================================================
 */
bool 
IpcServer::createServer( void ){

	bool    returnValue;
	ssize_t    lengthByte;
	std::string configFlag   = std::string ( getenv( "RESILIENCE_CONFIGFLAG" ) );
	if ( atoi(configFlag.c_str() ) ){
		ipcServerPort = jsonConfig.ipcServerConfig.port;
		sysIp = jsonConfig.ipcServerConfig.ipcIpAddress;
	}
	else{
		sysIp = std::string( getenv("RESILIENCE_SYS_IP") );
		ipcServerPort = atoi( std::string ( getenv("RESILIENCE_IPC_PORT")).c_str() );
#ifdef DEBUG
		std::cout<<PRINT<<"Ip:["<<sysIp<<" ] Port [ "<<ipcServerPort<<" ] "<<std::endl;
#endif

	}
#ifdef DEBUG
	std::cout<<PRINT<<"Create server for IPC"<<std::endl;
#endif
	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockFd == -1 ){ std::cerr<<"NOT ABLE TO CREATE IPC SERVER\n"; return false;}
	/* Set all bits of the padding field to 0 */
	memset(&serverAddr, '0', sizeof (serverAddr));  
	/*---- Configure settings of the server address struct ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;

	/* Set IP address to localhost */
	serverAddr.sin_addr.s_addr = inet_addr( sysIp.c_str());  // htonl(INADDR_ANY);
	/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_port = htons(ipcServerPort);
	/*---- Bind the address struct to the socket ----*/
#ifdef DEBUG
	std::cout<<PRINT<<"Bind IPC server"<<std::endl;
#endif
	if ( bind(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0 ){
		std::cerr<<PRINT<<"Bind failed"<<std::endl;
		returnValue = false;
		return returnValue;
	}

	/*---- Listen on the socket, with 5 max connection requests queued ----*/
#ifdef DEBUG
	std::cout<<PRINT<<"Listen for IPC Socket Fd   [ "<<sockFd<<" ] \n";
#endif
	  if (listen (sockFd, 1) < 0){
		  std::cerr<<"listen"<<std::endl;
		  exit (EXIT_FAILURE);
	  }
	  /*  Initialize the set of active sockets. */
	  FD_ZERO (&active_fd_set);
	  FD_SET (sockFd, &active_fd_set); 
	  while ( 1){
		  /*  Block until input arrives on one or more active sockets. */
		  read_fd_set = active_fd_set;
		  if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
			  std::cerr<<"select"<<std::endl;
			  exit (EXIT_FAILURE);
		  }
		  std::cout<<PRINT<<"Server created on Ip ["<<serverAddr.sin_addr.s_addr<<"]  Port ["<<ipcServerPort<<"] \n";
		  std::cout<<"Waiting for accepting client\n";
		  addr_size = sizeof serverStorage;
		  int c = sizeof ( struct sockaddr_in );
		  connFd  = accept( sockFd, (struct sockaddr *) &clientAddr,(socklen_t*)&c);
		  if ( connFd < 0 ){
			  std::cerr<<PRINT<<"Connection failed for IPC server"<<std::endl;
			  returnValue = false;
		  }
		  lengthByte = recv( connFd, &limits, sizeof( __updateLimits__ ), 0);
		  if ( lengthByte == -1 ) 
			  std::cerr<<" Error on received msg from server\n";
		  if ( lengthByte > 0 ){
			  returnValue = envIns.modifyEntry( limits );
			  int count;
			  returnValue == true ? count = write ( connFd , successString.c_str() , successString.length() ) : count = write ( connFd , errorString.c_str() , errorString.length() );
			  if ( count <0 ) std::cerr<<" Error on sending status to IPC CLIENT"<<std::endl;
			  else std::cout<<"STATUS send to IPC CLIENT"<<std::endl;
		  }
		  close(connFd);
		  FD_CLR (connFd, &active_fd_set);
	  }
	  return returnValue;
}
bool
IpcServer::closeServer( void ){
	bool    returnVal;
	if ( sockFd < 0 ) {
#ifdef DEBUG
		std::cout<<PRINT<<"IPC server already closed"<<std::endl;
#endif
		returnVal = true;
	}
	else{
		try{
			close( sockFd ) != -1 ? returnVal = true : returnVal = false;
			sockFd = -999;
		}catch ( const std::exception &e ){
			std::cerr<<PRINT<<"Exception :" <<e.what()<<std::endl;
			returnVal = false;
		}
	}
	
	return returnVal;
}
bool 
IpcServer::isServerStart( void ){
	bool    returnVal;
	sockFd >=0 ? returnVal= true: returnVal= false;
	return returnVal;
}
