
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
 *         Author:  ROY (), email.suman.roy@gmail.com 
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include "ipc.hpp"
bool                       IpcServer::live;
__updateLimits__           IpcServer::limits;
regEnv::RegisterEnv        IpcServer::envIns;
int                        IpcServer::sockFd;
int                        IpcServer::connFd;
bool                       IpcServer::setSerEnv;
socklen_t                  IpcServer::addr_size;
struct sockaddr_in         IpcServer::serverAddr;
struct sockaddr_in         IpcServer::clientAddr;
struct sockaddr_storage    IpcServer::serverStorage;
int                        IpcServer::ipcServerPort = 8888;

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
	int sockFd;
#ifdef DEBUG
	std::cout<<PRINT<<"Create server for IPC"<<std::endl;
#endif
	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	/* Set all bits of the padding field to 0 */
	memset(&serverAddr, '0', sizeof (serverAddr));  
	/*---- Configure settings of the server address struct ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;
	/* Set IP address to localhost */
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_port = htons(ipcServerPort);
	/*---- Bind the address struct to the socket ----*/
#ifdef DEBUG
	std::cout<<PRINT<<"Bind IPC server"<<std::endl;
#endif
	if ( bind(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0 ){
		std::cerr<<PRINT<<"Bind failed"<<std::endl;
		returnValue = false;
	}

	/*---- Listen on the socket, with 5 max connection requests queued ----*/
#ifdef DEBUG
	std::cout<<PRINT<<"Listen for IPC Socket Fd   [ "<<sockFd<<" ] \n";
#endif
	listen(sockFd,3)==0 ? returnValue = true: returnValue = false;

	std::cout<<PRINT<<"Server created on Ip [ 127.0.0.1 ]  Port ["<<ipcServerPort<<"] \n";
	return returnValue;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  acceptClient
 *  Description:  accept the incoming client request
 *             @retyrn: true if communication with the client established lse false
 * =====================================================================================
 */
bool 
IpcServer::acceptClient( void ){
	bool    returnValue;
	/*---- Accept call creates a new socket for the incoming connection ----*/
	std::cout<<"Waiting for accepting client\n";
	addr_size = sizeof serverStorage;
	int c = sizeof ( struct sockaddr_in );
	connFd  = accept( sockFd, (struct sockaddr *) &clientAddr,(socklen_t*)&c);
	if ( connFd < 0 ){
		std::cerr<<PRINT<<"Connection failed for IPC server"<<std::endl;
		returnValue = false;
	}
	/*---- Send message to the socket of the incoming connection ----*/
//	returnValue = send(sockFd,"send Data\n",11,0);
	return returnValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  manager
 *  Description:  handler that manages ipc server
 * =====================================================================================
 */
void 
IpcServer::manager( void ){
	bool    returnValue;
	
	ssize_t    lengthByte;
	std::cout<<PRINT<<std::endl;
	returnValue = createServer();
	returnValue == true? returnValue = acceptClient( ):std::cout<<"Kill the thread\n";
	while ( returnValue  ){
			std::cout<<"Recv\n";
			/*---- Read the message from the server into the limitser ----*/
			lengthByte = recv( connFd, &limits, sizeof( __updateLimits__ ), 0);
			if ( lengthByte == -1 ) 
				std::cerr<<" Error on received msg from server\n";
			if ( lengthByte > 0 ) returnValue = envIns.modifyEntry( limits );
	}
}
void
IpcServer::getPort( int &port){
	port = ipcServerPort;
}

