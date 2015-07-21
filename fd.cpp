/*
 * =====================================================================================
 *
 *       Filename:  fd.cpp
 *
 *    Description:  all socket info
 *
 *        Version:  1.0
 *        Created:  Thursday 23 April 2015 11:23:24  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy (), 
 *   Organization:  Cognizant Technological solutions
 *
 * =====================================================================================
 */
#include "fd.hpp"
using namespace fd;

Message                  fd::Fd::bufferCacheIns;
fdContainerType          fd::Fd::fdContainer;
fdLookupContainerType    fd::Fd::fdLookupContainer;

bool
fd::Fd::init ( const int sockFd , const int port , const std::string ipAddr ){
	fdLookupItType    lookUpItr;
	bool              returnVal;
	fdLookUpStruct    fdStructVar;      
	std::pair< fdLookupItType , bool >    ret;
#ifdef DEBUG
	std::cout<<PRINT<<" Look up init for Socket Fd [ "<<sockFd 
		<<" ] Port [ "<<port <<"] ipAddr [ "<<ipAddr<<" ] "
		<<std::endl;
#endif

	try{
		
		lookUpItr = fdLookupContainer.find( sockFd );
		if ( lookUpItr == fdLookupContainer.end() ){
			fdStructVar.refCount = 0;
			fdStructVar.port = port;
			fdStructVar.ipAddress = ipAddr;
			fdStructVar.pid = 0;//getPid();
			ret = fdLookupContainer.insert( std::pair<int , fdLookUpStruct>( sockFd , fdStructVar ) );
			ret.second == false ? returnVal = false : returnVal = true;
			if ( !returnVal ) 
				std::cerr<<PRINT<<"Not able to isert socketFd [ "<<sockFd 
					<<" ] Port [ "<<port <<" ] IpAddress ["
					<<ipAddr<<" ] into Lookup container "<<std::endl;
			else std::cout<<"SocketFd [ "<<sockFd<<" ] Registered"<<std::endl;
		}else{
			std::cerr<<PRINT<<" Sock descriptor [ "<<sockFd <<" ] already registerd\n";
			returnVal = false;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}
bool
fd::Fd::registerSock( const int listenFd ,const int sockFd ){
	bool    returnVal;
	fdFieldStructItType    fieldStrIt;
	fdLookupItType         fdLookUpItr;
	std::pair< fdFieldStructItType , bool > ret;
	fdFieldsStruct    fieldStrVar;
#ifdef DEBUG
	std::cout<<PRINT<<"Do Mape listen socket [ "<<listenFd <<" ] to Accepted sock fd [ "<<sockFd<<" ] "<<std::endl;
#endif

	try{
		fieldStrIt = fdContainer.find ( sockFd );
		fdLookUpItr = fdLookupContainer.find ( listenFd );
		if ( fieldStrIt == fdContainer.end() && fdLookUpItr != fdLookupContainer.end() ){
			fieldStrVar.pFdNumber = listenFd;
			fieldStrVar.readBuff = NULL;
			fieldStrVar.writeBuff = NULL;
			ret = fdContainer.insert( std::pair<int , fdFieldsStruct > ( sockFd , fieldStrVar ) );
			ret.second == false? returnVal = false : returnVal = true;
			if ( returnVal ) fdLookUpItr->second.refCount = fdLookUpItr->second.refCount + 1;
		}
		else{
			std::cerr<<PRINT<<"Not able to register ListenFd [ "
				<<listenFd<<" ] Accepted Fd [  "<<sockFd <<" ] "<<std::endl;
			returnVal = false;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}
	
/* 
 * ===  FUNCTION  ======================================================================
 *         Name: isPresent
 *  Description:  check the file descriptor is already registered or not
 *                @fd: socket descriptor/file descriptor
 *                @return: true if present else false
 * =====================================================================================
 */

bool 
fd::Fd::isPresent( const int fd ){
#ifdef DEBUG
	std::cout<<PRINT<<" FD [ "<<fd<<" ] "<<std::endl;
#endif

	fdContainerType::iterator    it;
	bool                             returnVal;

	returnVal = true;
	it = fdContainer.find( fd );
	if ( it == fdContainer.end() ){
		returnVal = false;
		std::cerr<<PRINT<<"File descriptor [ "<<fd <<" ] not registered\n";
	}
	return returnVal;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getPid
 *  Description:  get the process id who is the prent of the socket fd
 *              @sockFd: socket descriptor 
 *              @return: if socket fd registered then return pid rlse ret 0.( whois the
 *                      oparent of all the process)
 * =====================================================================================
 */
pid_t 
fd::Fd::getPid( const int sockFd ){
#ifdef DEBUG
	std::cout<<PRINT<<"Socket FD [ "<<sockFd<<"] "<<std::endl;
#endif

	fdFieldStructItType      fieldStrIt;
	pid_t                             pid;


	pid = 0; //which can't be.. only root has pid0
	fieldStrIt  = fdContainer.find ( sockFd);
	if ( fieldStrIt  != fdContainer.end() )
		pid = fieldStrIt ->second.pid;
	else std::cerr<<PRINT<<"File descriptor [ "<<sockFd<<" ] not registered\n";
	
	return pid;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getDestIp
 *  Description:  get the destination ip bind with the socket
 *             @sockFd: socket fd 
 *             @retrn: return the destination ip if the sockFd registered.
 * =====================================================================================
 */
bool 
fd::Fd::getDestIp ( const int sockFd  , std::string &ip ){
#ifdef DEBUG
	std::cout<<PRINT<<"SocketFd[ "<<sockFd<<"] "<<std::endl;
#endif


	fdFieldStructItType      fieldStrIt;
	fdLookupItType           fdLookIt;
	bool                     returnVal;
	try{
		fieldStrIt  = fdContainer.find ( sockFd);
		if ( fieldStrIt  != fdContainer.end() ){
			fdLookIt = fdLookupContainer.find ( fieldStrIt->second.pFdNumber );
			if ( fdLookIt != fdLookupContainer.end() ){
				ip = fdLookIt->second.ipAddress;
				returnVal = true;
			}else {
				std::cerr<<PRINT<<"Lookup File Descriptor [ "
					<<fieldStrIt->second.pFdNumber<<" ] not registered "<<std::endl;
				returnVal = false;
			}
			}else{
				std::cerr<<PRINT<<"File descriptor  [ "
					<<sockFd <<" ] not registered "<<std::endl;
				returnVal = false;
			}
	}catch( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;

}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getDestPort
 *  Description:  get the port bound with the fd
 *              @sockFd: socket descriptor 
 *              @return: destination port if sockFd registeres else return 0
 * =====================================================================================
 */
bool
fd::Fd::getDestPort ( const int sockFd , int &port ){
#ifdef DEBUG

	std::cout<<PRINT<<"SocketFd[ "<<sockFd<<" ] \n";
#endif
	fdFieldStructItType      fieldStrIt;
	fdLookupItType           fdLookIt;
	bool                     returnVal;
	try{
		fieldStrIt  = fdContainer.find ( sockFd);
		if ( fieldStrIt  != fdContainer.end() ){
			fdLookIt = fdLookupContainer.find ( fieldStrIt->second.pFdNumber );
			if ( fdLookIt != fdLookupContainer.end() ){
				port = fdLookIt->second.port;
				returnVal = true;
			}else {
				std::cerr<<PRINT<<"Lookup File Descriptor [ "
					<<fieldStrIt->second.pFdNumber<<" ] not registered "<<std::endl;
				returnVal = false;
			}
		}else{
			std::cerr<<PRINT<<"File descriptor  [ "
				<<sockFd <<" ] not registered "<<std::endl;
			returnVal = false;
		}
		}catch( const std::exception &e ){
			std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
			returnVal = false;
		}

	if ( !returnVal ) std::cout<<PRINT<<"Port  [ "<<port<<" ] not registered"<<std::endl;
	return returnVal;
}



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  accessReadBufferData
 *  Description:  access the read buffer( opackes coming from kernel buff)
 *               @fd: file descriptor 
 *               @buff: containes data during writing( flag == false) 
 *                     empty container to read the data( flag = true )
 *               @noByte:data read / write bytes
 *               @flag: true: read data , flase: write data
 *               @return: No of bytes read/write 
 *                        -1 on error 0 if no data processed
 * =====================================================================================
 */
ssize_t  
fd::Fd::accessReadBufferData( const int fd , void * buff, 
		const ssize_t noByte ,const bool flag ){
#ifdef DEBUG
	std::cout<<PRINT<<"Fd [ "<<fd<<" ] No Of Bytes[ "<<noByte 
		<<" ] Flag [ "<<flag<<" ] "<<std::endl;
#endif
	fdFieldStructItType     fieldStrIt;

	ssize_t               byteAccess;
	
	fieldStrIt  =fdContainer.find ( fd );

	if ( fieldStrIt != fdContainer.end() ){
		if ( flag){ //read data
			byteAccess = bufferCacheIns.read
				(fieldStrIt->second.readBuff,  buff ,  noByte );
		}
		else{
			byteAccess = bufferCacheIns.write
				( fieldStrIt ->second.readBuff , buff ,  noByte );
		}
	}else byteAccess = 0;
	return byteAccess;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  accessSendBufferData
 *  Description:  access the send buffer( packes going to  kernel buff)
 *               @fd: file descriptor 
 *               @buff: containes data during writing( flag == false) 
 *                     empty container to read the data( flag = true )
 *               @noByte:data read / write bytes
 *               @flag: true: read data , flase: write data
 *               @return: No of bytes read/write 
 *                        -1 on error 0 if no data processed
 * =====================================================================================
 */

ssize_t 
Fd::accessSendBufferData ( const int fd ,  void * buff ,const ssize_t noByte ,
	       	const bool flag ){
#ifdef DEBUG
	std::cout<<PRINT<<"Fd [ "<<fd<<" ] No Of Bytes[ "<<noByte 
		<<" ] Flag [ "<<flag<<" ] "<<std::endl;
#endif
	ssize_t               byteAccess;
	fdFieldStructItType     fieldStrIt;

	try{
		fieldStrIt  =fdContainer.find( fd );
		if ( fieldStrIt  != fdContainer.end() ){
			if ( flag ){
				byteAccess = bufferCacheIns.read
					( fieldStrIt ->second.writeBuff,buff  ,  noByte );
			}else {
				byteAccess = bufferCacheIns.write
					( fieldStrIt->second.writeBuff , buff  , noByte) ;
#ifdef TEST
				std::cout<<PRINT<<"No oF byte wrote on buffer [ "<<byteAccess<<"] \n";
				__message__ *head2 = it->second.writeBuff;
				int cc= 0;
				while ( head2->next != fieldStrIt ->second.writeBuff ){
					++cc;
					std::cout<<head2->length<<std::endl;
					head2= head2->next;
				} std::cout<<head2->length<<std::endl;
				std::cout<<PRINT<<"Total no : [ "<<cc<<" ] \n";
				exit( EXIT_FAILURE);
			
#endif
			}
#ifdef DEBUG
			std::cout<<PRINT<<" Bytee accessed from Buffer Cache [ "
				<<byteAccess<<"]  \n";
#endif
		} else{
			byteAccess = 0;
#ifdef DEBUG
			std::cerr<<PRINT<<"RETURNING 0";
			exit( EXIT_FAILURE);
#endif
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<"Exception : "<<e.what()<<std::endl;
	}
	return byteAccess;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  unregisterFileDescriptor
 *  Description:  unregister a registered FD when fd is geting closed
 *              @fd: File descriptor for the socket.
 *              @return: true on success else false( if the fd not
 *                registered prev or exception during delet memory)
 * =====================================================================================
 */
bool
fd::Fd::unregisterFileDescriptor(const int sockFd ){
	fdFieldStructItType      fieldStrIt;
	fdLookupItType           fdLookIt;
	bool                     returnVal;
#ifdef DEBUG
	std::cout<<PRINT<<"Unregister fd [ "<<sockFd<<" ]"<<std::endl;
#endif
	try{
		fieldStrIt  = fdContainer.find ( sockFd);
		if ( fieldStrIt  != fdContainer.end() ){
			fdLookIt = fdLookupContainer.find ( fieldStrIt->second.pFdNumber );
			if ( fdLookIt != fdLookupContainer.end() ){
				fdLookIt->second.refCount --;
				if ( fdLookIt->second.refCount == 0 ){
					fdLookupContainer.erase( fdLookIt );
				}
				fdContainer.erase ( fieldStrIt);
				returnVal = true;
			}else {
				std::cerr<<PRINT<<"Lookup File Descriptor [ "
					<<fieldStrIt->second.pFdNumber<<" ] not registered "<<std::endl;
				returnVal = false;
			}
			}else{
				std::cerr<<PRINT<<"File descriptor  [ "
					<<sockFd <<" ] not registered "<<std::endl;
				returnVal = false;
			}
	}catch( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}

bool
fd::Fd::insertStartTimeOfPacket( const int sockFd , const bool flag , struct timespec startTime ){
#ifdef DEBUG
	std::cout<<PRINT<<" Insert start time of request for Socket Fd [ "<<sockFd<<" ] "<<std::endl;
#endif
	bool                    returnVal;
	fdFieldStructItType     fieldStrIt;
	try{
		fieldStrIt  =fdContainer.find ( sockFd );
		if ( fieldStrIt != fdContainer.end() ){
			if ( flag){ //read data
				fieldStrIt->second.startTimeOfRequest_READ = startTime;
			}
			else{
				fieldStrIt->second.startTimeOfRequest_WRITE = startTime;
			}
			returnVal = true;
		}else{
			std::cerr<<PRINT<<"Fd [ "<<sockFd<<" Not registered"<<std::endl;
			returnVal = false;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}
bool
fd::Fd::getStartTimeOfPacket( const int sockFd , const bool flag , struct timespec &startTime){
	bool                    returnVal;
	fdFieldStructItType     fieldStrIt;
	try{
		fieldStrIt  =fdContainer.find ( sockFd );
		if ( fieldStrIt != fdContainer.end() ){
			if ( flag){ //read data
				startTime= fieldStrIt->second.startTimeOfRequest_READ ;
			}
			else{
				startTime =fieldStrIt->second. startTimeOfRequest_WRITE;
			}
			returnVal = true;
		}else{
			std::cerr<<PRINT<<"Fd [ "<<sockFd<<" Not registered"<<std::endl;
			returnVal = false;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}
