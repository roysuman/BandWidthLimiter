/*
 * =====================================================================================
 *
 *       Filename:  overload.cpp
 *
 *    Description:  overload all glibc socket methodes
 *
 *        Version:  1.0
 *        Created:  Friday 17 April 2015 11:51:41  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy (), 
 *   Organization:  Cognizant Technological solutions
 *
 * =====================================================================================
 */
#include "overload.hpp"

ObjectPool                 *__pool__ = NULL;
Lock                       lock;
char                       *__error__;
void                       *__handle__;
bool                       __isDelay__ = false;
std::map < int , bool >    __registeredSock__;
static bool                __pass__ = false;
static bool                ipcServerInit= false;
static pthread_mutex_t     initThread =PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t     releseResource =PTHREAD_MUTEX_INITIALIZER;
extern "C" void signalHandler(int);
extern "C" void initSignals(void);
extern "C" void panic(const char *, ...);
struct sigaction sigact;
//TODO - remove the following method
bool 
isPresent ( int sockFd ){
	std::map <int , bool >::iterator    it;
	bool                                returnVal;

	it = __registeredSock__.find ( sockFd );
	if ( it != __registeredSock__.end() ) returnVal = true;
	else returnVal = false;

	return returnVal;
}
//TODO remove the following method
bool 
registerSock( int sockFd ){
#ifdef DEBUG
	std::cout<<"Register [ "<<sockFd<<" ] \n";
#endif
	std::map <int , bool >::iterator    it;
	bool                                returnVal; 

	it = __registeredSock__.find ( sockFd );
	if ( it == __registeredSock__.end () ){
		__registeredSock__.insert ( std::pair < int , bool >(sockFd , true ) );
		returnVal = true;
	}else returnVal = false;

	return returnVal;
}
//TODO remove the following method
bool
removeSock( int sockFd){
	std::map<int , bool >::iterator    it;
	bool                               returnVal;
	it = __registeredSock__.find ( sockFd );
	if ( it != __registeredSock__.end() ){
		__registeredSock__.erase( it );
//		if ( __registeredSock__.size() == 0 ) __pass__ = false; 
		returnVal = true;

	}else returnVal = false;
	return returnVal;
}
void
relese( void ){
#ifdef DEBUG
	std::cout<<PRINT<<" Relese all kerner resources "<<std::endl;
#endif
	if ( __pool__ != NULL ){
		__resilience__::Interface *init;
		init = __pool__->getInterfaceObj();
		init->stopIpcServer();
		__pool__->returnInterfaceObj( init );
		__pool__->freeObjectPool();
		delete __pool__;
		__pool__ = NULL;
		std::cout<<"\nBYE\n";
	}

}
extern "C"  void 
deallocFunc( void){
#ifdef DEBUG
	std::cout<<"Perform all deallocation"<<std::endl;
#endif
	pthread_mutex_lock( &releseResource);

	//release all allocated resources
	relese( );
	sigemptyset(&sigact.sa_mask);
	pthread_mutex_unlock( &releseResource);
	return;
}

extern "C" void 
initSignals(void){
    sigact.sa_handler = signalHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
    sigaction(SIGKILL, &sigact, (struct sigaction *)NULL);
    sigaction(SIGCONT, &sigact, (struct sigaction *)NULL);
//    sigaction(SIG_IGN, &sigact, (struct sigaction *)NULL);
    sigaction( 129, &sigact, (struct sigaction *)NULL);
}
extern "C" void 
signalHandler(int sig){
	fprintf(stdout,"Signal [ %d ] received\n", sig);
	if (sig == SIGINT) panic("Caught signal for Ctrl+C\n");
	if (sig == SIGKILL || sig == SIGCONT || sig == 129) panic("Caught signal for EXIT\n");
	
//	deallocFunc();
	exit( -1 );
}
void panic(const char *fmt, ...){
    char buf[50];
    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buf, fmt, argptr);
    va_end(argptr);
    fprintf(stderr,"%s ", buf);
    exit(-1);
}
/* all socket layer method 
 */

__FUNCTION_DECLARE_(dup, int, (int));
__FUNCTION_DECLARE_(close, int, (int));
__FUNCTION_DECLARE_(dup2, int, (int, int));
__FUNCTION_DECLARE_(connect, int ,( int , const struct sockaddr* , socklen_t) );
__FUNCTION_DECLARE_(socket, int, (int, int, int));
__FUNCTION_DECLARE_(dup3, int, (int, int , int ));
__FUNCTION_DECLARE_(read, ssize_t, (int, void *, size_t));
__FUNCTION_DECLARE_(poll, int, (struct pollfd *, int, int));
__FUNCTION_DECLARE_(recv, ssize_t, (int, void *, size_t, int));
__FUNCTION_DECLARE_(write, ssize_t, (int,const void *, size_t));
__FUNCTION_DECLARE_(send, ssize_t, (int, const void *, size_t, int));
__FUNCTION_DECLARE_(readv, ssize_t, (int, const struct iovec *, int));
__FUNCTION_DECLARE_(writev, ssize_t, (int, const struct iovec *, int));
__FUNCTION_DECLARE_(accept, int, (int, struct sockaddr *, socklen_t *));
__FUNCTION_DECLARE_ ( bind , int ,(int , const struct sockaddr* , socklen_t ) );
__FUNCTION_DECLARE_(select, int, (int, fd_set *, fd_set *, fd_set *, struct timeval *));
__FUNCTION_DECLARE_(sendto, ssize_t, (int, const void *, size_t, int,
	    const struct sockaddr *, socklen_t));
__FUNCTION_DECLARE_(recvfrom, ssize_t, (int, void *, size_t, int, struct sockaddr *,
	    socklen_t *));
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initOverLoad
 *  Description:  make the environemnt ready........
 * =====================================================================================
 */
//extern "C" 
void 
initOverLoad(){
#ifdef DEBUG
	std::cout<<PRINT<<"INIT"<<std::endl;
#endif
	__resilience__::Interface    *init;
	std::string                  glibcPath;
	
	pthread_mutex_lock( &initThread);
	std::cout<<PRINT<<"__isDelay__ [ "<<__isDelay__<<" ] "<<std::endl;
	if ( !__isDelay__ ){
		std::cout<<PRINT<<"__isDelay__ [ "<<__isDelay__<<" ] "<<std::endl;
		__isDelay__ = true;
			__pass__ = true;
		try{
			/*  if ( ( __handle__ = dlopen("/lib/x86_64-linux-gnu/libc.so.6",RTLD_LAZY ) ) == NULL ){
				std::cerr<<"Failed to open libc\n";
				exit( 1 );
			} */
	
			glibcPath = std::string( getenv("RESILIENCE_GLIBC_PATH") );
			if ( ( __handle__ = dlopen(glibcPath.c_str() ,RTLD_LAZY ) ) == NULL ){
				std::cerr<<"Failed to open libc\n";
				exit ( EXIT_FAILURE );
			}
			__LOAD__ADDR__ ( socket );
			__LOAD__ADDR__( close );
			__LOAD__ADDR__ ( read );
			__LOAD__ADDR__ ( recv );
			__LOAD__ADDR__ ( readv);
			__LOAD__ADDR__ ( recvfrom );
			__LOAD__ADDR__ ( write );
			__LOAD__ADDR__ ( send );
			__LOAD__ADDR__ ( writev );
			__LOAD__ADDR__ ( sendto );
			__LOAD__ADDR__ ( select );
			__LOAD__ADDR__ ( poll );
			__LOAD__ADDR__ ( accept );
			__LOAD__ADDR__ ( dup );
			__LOAD__ADDR__( dup2 );
			__LOAD__ADDR__( dup3 );
			__LOAD__ADDR__ ( bind );
			__LOAD__ADDR__ ( connect );
			__pool__ = ObjectPool::getInstance();
			init = __pool__->getInterfaceObj();
			if ( !init->init() ){
				std::cerr<<PRINT<<"Resilience init terminated\n";
				exit ( EXIT_FAILURE );
			}
			__pool__->returnInterfaceObj( init );
			//if ipc thread not created
			if ( !ipcServerInit ){ 
				ipcServerInit = true; 
#ifdef DEBUG
				std::cout<<PRINT<<"Start IPC server"<<std::endl;
#endif
			//	init->startIpcServer(); 

			}
#ifdef DEBUG
			std::cout<<PRINT<<"Register environment var"<<std::endl;
#endif
		}
		catch ( const std::bad_alloc &ba ){
			std::cerr<<"\nException on creating instance\
				of Interface Handler\nNo delay can maintain";
			exit ( EXIT_FAILURE );
		}
	//	initSignals();
		atexit(deallocFunc);
	}
#ifdef DEBUG
	std::cout<<PRINT<<"Init DONE"<<std::endl;
#endif
	pthread_mutex_unlock( &initThread);
}


/*-----------------------------------------------------------------------------
 *  
 *   read/recv/ recvfrom :read requested  data from kernel socket buff
 *                 Resilience  Logic: Read the available data from kernel
 *                 buffer. Then if the read Byte( kernel buff) is <= allowed
 *                 byte limit then frwrd is back else send it to the buffer 
 *                 mapped with the FD. and slowly forward to the calling method.
 *                 Buffer cache read returns -1? there is an __error__ on reading
 *                 from the buffer.
 *             
 *-----------------------------------------------------------------------------*/
ssize_t    
preOpRead( int fd , void *buff  , ssize_t & howManyByteCanRead , 
		__resilience__::Interface *&intObj){

	ssize_t    readCount;
	size_t     containerSize;
#ifdef DEBUG
	std::cout<<PRINT<<"Socket fd:["<<fd <<" ] "<<std::endl;
#endif

	containerSize = howManyByteCanRead;
	intObj = __pool__->getInterfaceObj();
	//acquire lock
	intObj->readWriteLock( fd , true , true );
	//update packet start time for latency calc
	intObj->initStartTimeForLatency( fd , true);

	howManyByteCanRead = intObj->getPortStatus( fd , true );
#ifdef DEBUG
		std::cout<<PRINT<<"Bytes can read["<<howManyByteCanRead<<"]\n";
#endif
	if ( howManyByteCanRead == -1 ){ // no limit
		readCount = intObj->accessReadBufferData
			( fd , buff , containerSize ,  true ); // read data from buffer storago
	} else 	if ( howManyByteCanRead > 0 )
		readCount = intObj->accessReadBufferData
			( fd , buff , howManyByteCanRead ,  true ); // read data from buffer storago
#ifdef DEBUG
	std::cout<<PRINT<<" Bytes read from Read\
		Buff [ "<<readCount<<" ] \n";
#endif
	return readCount;
}

ssize_t 
preOpSend ( int fd , void *buff , size_t len , bool &fstTime ) {
#ifdef DEBUG
	std::cout<<PRINT<<" Fd [ "<<fd<< " ] Size [ "
		<< len<< "] fstTime [ "<<fstTime <<" ]"<<std::endl;
#endif
	ssize_t    sendCount;
	ssize_t    bytesCanHandleWindow;
	int        port;

	__resilience__::Interface    *intObj;
	intObj = __pool__->getInterfaceObj();
	//update packet start time for latency calc
	intObj->initStartTimeForLatency( fd , false);
	bytesCanHandleWindow = 
		intObj->getPortStatus( fd  , false );
#ifdef DEBUG
	std::cout<<PRINT<<"Bytes can write this windo time [ "
		<<bytesCanHandleWindow<<" ] \n";
#endif
	if ( bytesCanHandleWindow == -1 ){
		sendCount =intObj->accessSendBufferData( fd , buff , 
				len , true );
		std::cout<<"Got -1 ";
		exit(1);
	}
	//extra bytes need to update in the send buffer
	else if ( !fstTime && bytesCanHandleWindow > 0){
		sendCount =intObj->accessSendBufferData( fd , buff , 
				bytesCanHandleWindow , true );
		if ( intObj->getDestPort( fd , port ) ){
			intObj->updateByteCountCurrentWindow(
					port , sendCount  , false );
		}
	}
	else if( bytesCanHandleWindow > 0 ){
		sendCount =  intObj->updateSendBuffer( 
				len , bytesCanHandleWindow , fd , buff );
		fstTime = false;
	} else sendCount =0;// bytesCanHandleWindow;
	__pool__->returnInterfaceObj( intObj );

	return sendCount;

}
/*-----------------------------------------------------------------------------
 *  all the below methodes are a wrapper for libc sock methodes..
 *  to understand the method description plese do "man methodName"
 *-----------------------------------------------------------------------------*/
int 
socket( int sockDomain , int sockType , int sockProtocol ){
#ifdef DEBUG
	std::cout<<PRINT<<std::endl;
#endif	
	int    sock;
	
	if ( !__pass__  ) initOverLoad();
	lock.acquireCreateCloseSock(  );

	sock = ( *libc_socket ) ( sockDomain, sockType , sockProtocol );
	lock.releaseCreateCloseSock (  );
#ifdef DEBUG
	std::cout<<"\n Socket created:"<<sock<<std::endl;
#endif
	return ( sock );
}

int 
close ( int fd ){ 

	int                          cls;
	__resilience__::Interface    *intObj;


	if ( !__pass__  ) initOverLoad();
	cls = (*libc_close)(fd);
	
	if ( __pass__ && isPresent( fd ) ){
	
	intObj = __pool__->getInterfaceObj();
	if ( cls == 0 ){
		lock.acquireCreateCloseSock(  );
		intObj->unregisterFileDescriptor( fd ) == true?
			cls = 0:cls=-1;
		removeSock( fd ) == true? cls = 0:cls =-1;
		if ( cls!=0 )std::cerr<<PRINT<<"Error on closing socket\n";
		lock.releaseCreateCloseSock(  );
	}
	__pool__->returnInterfaceObj( intObj );
	}
	

	return cls;
}
 
 
ssize_t 
read( int fd , void *buff , size_t noBytes ){
	
	__resilience__::Interface    *intObj;
	ssize_t                      readCount;
	ssize_t                      howManyByteCanRead;

	if ( !__pass__ ) initOverLoad();

	if ( (ssize_t )noBytes == 0 || !isPresent( fd ) ) 
		return ( (*libc_read)(fd , buff , noBytes ) );

	
#ifdef DEBUG
	std::cout<<PRINT<<"Socket fd:["<<fd<<std::endl;
#endif
		if ( __isDelay__ ){
			howManyByteCanRead = noBytes;
			readCount = preOpRead( fd , buff, 
					howManyByteCanRead , intObj   );
			if ( readCount == -1 ){/* buffer empty*/ 
				readCount = (*libc_read)(fd , buff , noBytes );
			}
			if ( readCount>0 ){
				readCount = intObj->updateReadBuffer(
						howManyByteCanRead , readCount , fd , buff );
			}
			intObj->updateTime( fd , true );
			intObj->readWriteLock( fd , true , false );
			__pool__->returnInterfaceObj( intObj );
		
	}else readCount = (*libc_read)(fd , buff , noBytes );

	return readCount;
}


ssize_t 
recv( int fd , void* buff , size_t len, int flags ){
	__resilience__::Interface    *intObj;
	ssize_t                      readCount;
	ssize_t                      howManyByteCanRead;

	if ( !__pass__ ) initOverLoad();
	if ( (ssize_t)len ==0 || !isPresent( fd ) ) 
		return ( (*libc_recv)(fd , buff , len , flags ));
	

#ifdef DEBUG
	std::cout<<PRINT<<"Socket fd:["<<fd<<"]"<<std::endl;
#endif
	if ( __isDelay__ ){
		howManyByteCanRead = len;
		readCount = preOpRead( fd , buff, howManyByteCanRead  , intObj );
		if ( readCount == -1 ){/* buffer empty*/
			readCount  = (*libc_recv)(fd , buff , len , flags );
		}
		if ( readCount > 0 ){
			readCount = intObj->updateReadBuffer(  
					howManyByteCanRead , readCount ,fd  , buff );
		}
		intObj->updateTime( fd , true );
		intObj->readWriteLock( fd , true , false );
		__pool__ ->returnInterfaceObj( intObj );
		
	}else readCount  = (*libc_recv)(fd , buff , len , flags );

	return readCount;

}

//no need any delay logic.. becayse it's calling read method only.. see kernel source
ssize_t 
readv( int __fd__ , const struct iovec* __iovec__ , int __iovCount__){
#ifdef DEBUG
	std::cout<<PRINT<<std::endl;
#endif
	if ( !__pass__ ) initOverLoad();
	ssize_t    readCount;

	readCount = (*libc_readv )(__fd__ , __iovec__ , __iovCount__ );
	return readCount;
}

ssize_t 
recvfrom ( int fd, void* buff , size_t len , int flags , struct sockaddr * from , 
		socklen_t *fromlen ){
#ifdef DEBUG
	std::cout<<PRINT<<"Socket fd:["<<fd<<"]"<<std::endl;
#endif

	__resilience__::Interface    *intObj;
	ssize_t                      readCount;
	ssize_t                      howManyByteCanRead;
	if ( !__pass__ ) initOverLoad();
	//for ipcServer
	std::cout<<PRINT<<"#########################\n";
	  	if ( !isPresent( fd ) ){
		intObj = __pool__->getInterfaceObj();
		if ( intObj->registerSock ( fd , fd ) ) // update the accepted fd
			registerSock( fd );
		__pool__->returnInterfaceObj( intObj );
	}

	if ( (ssize_t)len == 0 || !isPresent( fd )) 
		return ( (*libc_recvfrom )( fd , buff , len , flags , from , fromlen ) );

	if ( __isDelay__ ){
		howManyByteCanRead = len;
		readCount = preOpRead( fd , buff, howManyByteCanRead , intObj );
		if ( readCount == -1 ){/* buffer empty */
			std::cout<<PRINT<<"Call glibc recv from\n";
			intObj->readWriteLock( fd , true , false );
			readCount = (*libc_recvfrom )( fd , buff , len , 
					flags , from , fromlen );
			intObj->readWriteLock( fd , true , true );
		}
		if ( readCount > 0 ){
			readCount = intObj->updateReadBuffer(  howManyByteCanRead , 
					readCount ,fd  , buff );
		}
		intObj->updateTime( fd , true );
		intObj->readWriteLock( fd , true , false );
		__pool__->returnInterfaceObj( intObj );
	}else readCount  = (*libc_recvfrom )( fd , buff , len , flags , 
			from , fromlen );

	return readCount;
}

ssize_t 
write(int fd ,const void * buff, size_t len ){

	bool       fstTime;
	ssize_t    byteSend;
	ssize_t    sendCount;
	ssize_t    noOfBytes; 
	
	if ( !__pass__ ) initOverLoad();

	if ( len == 0 || !isPresent( fd ) ) 
		return ((*libc_write)(fd  , buff , len ) );
#ifdef DEBUG
	std::cout<<PRINT<<" FD [ "<<fd<<" ] Pass [ "<<__pass__ <<" ] IsDelay [ "
		<<__isDelay__  <<" ] Length [ "<<len<<" ] "<<std::endl;
#endif

	noOfBytes = 1;

	if ( __isDelay__ ){
		byteSend = 0;
		fstTime = true;
		char tempBuff [ len +1 ];
		memcpy ( tempBuff , buff , len );
		__resilience__::Interface    *intObj = __pool__->getInterfaceObj();
		intObj->readWriteLock( fd , false , true );
		while ( noOfBytes  > 0 ){

			sendCount = preOpSend ( fd , tempBuff, len , fstTime );
			if ( sendCount > 0 ){
				noOfBytes = (*libc_write)(fd  , tempBuff , sendCount );
				byteSend +=noOfBytes;
				//update time
				intObj->updateTime( fd , false);
#ifdef TEST
				std::cout<<"Byte SENDED to KERNEL\
				       	BUFFER : [ "<<noOfBytes<<" ] \n";
#endif
			}else    	noOfBytes = 0;
			if ( byteSend > ( ssize_t ) len ) { std::cout<<"THIS\
			       	SITUATION SHOULD NOT  ";noOfBytes = 0;}
		}
		intObj->readWriteLock( fd , false , false );
		__pool__->returnInterfaceObj( intObj );
		//lock.releseWriteLock(  );
		
	} else byteSend  = (*libc_write)(fd  , buff , len  );
#ifdef TEST
	std::cout<<"TOTAL BYTE FORWARDED TO KERNEL BUFFER [ "<<byteSend<<" ] \n";
#endif

	return byteSend ;
	
}


ssize_t 
send(int fd , const void * buff , size_t len , int  flags ){
	if ( !__pass__ ) initOverLoad();
	if ( len == 0|| !isPresent( fd ) ) 
		return (  (*libc_send)( fd  , buff , len , flags ) );
	
#ifdef DEBUG
	std::cout<<PRINT<<" FD [ "<<fd<<" ] Pass [ "<<__pass__ <<" ] IsDelay [ "
		<<__isDelay__  <<" ] Length [ "<<len<<" ] "<<std::endl;
#endif


	bool       fstTime;
	ssize_t    byteSend;
	ssize_t    sendCount;
	ssize_t    noOfBytes;

	noOfBytes = 1;

	byteSend = 0;
	fstTime = true;
	if ( __isDelay__ ){
			char tempBuff [ len ];
			memcpy ( tempBuff , buff , len );
			__resilience__::Interface    *intObj = __pool__->getInterfaceObj();
		while ( noOfBytes  > 0 ){
			intObj->readWriteLock( fd , false , true );
			sendCount = preOpSend ( fd , tempBuff, len , fstTime );
			 if ( sendCount > 0 ){
				 noOfBytes = (*libc_send)( fd  , 
						 tempBuff , sendCount , flags );
				 byteSend +=noOfBytes;
				 //update time
				intObj->updateTime( fd , false);

			 } else noOfBytes = 0;
			 if ( byteSend >= (ssize_t )len ) noOfBytes = 0;
			 intObj->readWriteLock( fd , false , false );
		}
		__pool__->returnInterfaceObj( intObj );
	} else byteSend =  (*libc_send)( fd  , buff , len , flags );

	return byteSend ;
}
//no need , because it's calling write method only... see kernel source
ssize_t 
writev(int fd , const struct iovec * iovec , int iovCount ){
	if ( !__pass__ ) initOverLoad();
	ssize_t    noOfBytes;
	noOfBytes = (*libc_writev )( fd , iovec , iovCount );
	return noOfBytes;
}

ssize_t 
sendto(int fd ,  const  void *buff, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addrlen ){
	if ( !__pass__ ) initOverLoad();
	if ( len == 0|| !isPresent( fd ) ) 
		return (  (*libc_sendto)(fd , buff , len  , flags, dest_addr , addrlen ) );

#ifdef DEBUG
	std::cout<<PRINT<<" FD [ "<<fd<<" ] Pass [ "<<__pass__ <<" ] IsDelay [ "
		<<__isDelay__  <<" ] Length [ "<<len<<" ] "<<std::endl;
#endif
	bool       fstTime;
	ssize_t    byteSend;
	ssize_t    sendCount;
	ssize_t    noOfBytes;

	noOfBytes = 1;
	byteSend = 0;
	fstTime = true;

	if ( __isDelay__ ){
		lock.acquireWriteLock(  );
			char tempBuff [ len ];
			memcpy ( tempBuff , buff , len );
			__resilience__::Interface    *intObj = __pool__->getInterfaceObj();
		while ( noOfBytes  > 0 ){
			intObj->readWriteLock( fd , false , true );
			sendCount = preOpSend ( fd , tempBuff, len , fstTime );
			if ( sendCount > 0 ){
				noOfBytes = (*libc_sendto)(fd , tempBuff , 
						sendCount  , flags, dest_addr , addrlen );
				byteSend +=noOfBytes;
				 //update time
				intObj->updateTime( fd , false);
			}else noOfBytes = 0;
			intObj->readWriteLock( fd , false , false );
		}
		__pool__->returnInterfaceObj( intObj );

	} else byteSend =  (*libc_sendto)(fd , buff , len  , flags, dest_addr , addrlen );

	return byteSend ;
}

int 
select(int nfds , fd_set * read_fds, fd_set * write_fds, fd_set *exceptfds, 
		struct timeval * timeout){

	if ( !__pass__ ) initOverLoad();

	int   noFd;
	noFd = (*libc_select)( nfds , read_fds , write_fds , exceptfds , timeout );

	return noFd;
}

int 
poll(struct pollfd *fds, nfds_t nfds, int timeout){

	if ( !__pass__ ) initOverLoad();

	int    returnVal;

	returnVal = (*libc_poll )( fds , nfds , timeout );
	
	return ( returnVal );
}
int 
accept(int fd, struct sockaddr *addr, socklen_t *addrlen){
	if ( !__pass__ ) initOverLoad();

	__resilience__::Interface    *intObj;
	int                          returnVal;

	returnVal = (*libc_accept )( fd , addr , addrlen );
#ifdef DEBUG
	std::cout<<PRINT<<"Update Old FD [ "<<fd <<" ] to New\
	       	Fd [ "<<returnVal<<" ] "<<std::endl;
#endif
	if ( __isDelay__ ){
		intObj = __pool__->getInterfaceObj();
		lock.acquireCreateCloseSock(  );
		//for ipcServer
		if ( intObj->registerSock ( fd , returnVal ) ) // update the accepted fd
		registerSock( returnVal );

		lock.releaseCreateCloseSock( );
		__pool__->returnInterfaceObj( intObj );
	}
	return returnVal;

}
/* 
int 
dup(int oldfd){
	if ( !__pass__ ) initOverLoad();

	__resilience__::Interface    *intObj;
	int                          returnVal;

	returnVal = (*libc_dup )( oldfd );
	if ( returnVal != -1 && __isDelay__  ){

		intObj = __pool__->getInterfaceObj();
		intObj->registerSock( oldfd  , returnVal ); // update the accepted fd
		__pool__->returnInterfaceObj( intObj );
	}

	return returnVal;
 }

int 
dup2(int oldfd, int newfd){
	if ( !__pass__ ) initOverLoad();

	__resilience__::Interface    *intObj;
	int                          returnVal;

	returnVal = (*libc_dup2 )( oldfd , newfd );
	if ( returnVal != -1 && __isDelay__ ){
		intObj = __pool__->getInterfaceObj();
		intObj->registerSock( oldfd  , returnVal ); // update the accepted fd
		__pool__->returnInterfaceObj( intObj );
	}


	return returnVal;
}

int 
dup3(int oldfd, int newfd, int flags){
	if ( !__pass__ ) initOverLoad();

	__resilience__::Interface    *intObj;
	int                          returnVal;

	returnVal = (*libc_dup3 )( oldfd , newfd , flags);
	if ( returnVal != -1 && __isDelay__ ){
		
		intObj = __pool__->getInterfaceObj();
		intObj->registerSock( oldfd  , returnVal ); // update the accepted fd
		__pool__->returnInterfaceObj( intObj );
	}

	return returnVal;
}
*/
int 
bind(int fd, const struct sockaddr *addr, socklen_t addrlen){
#ifdef DEBUG
	std::cout<<PRINT<<"File Descriptor [ "<<fd<<" ] \n";
#endif
	int                          port;
	int                          ipAddr;
	__resilience__::Interface    *intObj;
	int                          returnVal;
	char                         str[INET_ADDRSTRLEN];

	if ( !__pass__ ) initOverLoad();
 
	if ( __pass__ &&  __isDelay__ ){ // environment is ready for delay
		if (addr->sa_family == AF_INET){
		       port = ntohs (((struct sockaddr_in*)addr)->sin_port);
		       ipAddr = ((struct sockaddr_in*)&addr)->sin_addr.s_addr;
		       inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
		       std::string ip = std::string(str);
		}
		else
			port = ntohs(((struct sockaddr_in6*)addr)->sin6_port);
#ifdef DEBUG
		std::cout<<PRINT<<"FD ["<<fd<<"] port ["<<port<<"]\n";
#endif
		intObj = __pool__->getInterfaceObj();

		if ( !intObj->init( fd , port ) ){
		//	std::cerr<<PRINT<<" sock FD ["<<fd
		//		<<"] -- not able to register\n";
		//	returnVal = -1;
		}
		__pool__->returnInterfaceObj( intObj );
	}//returnVal == -1 ? returnVal = -1: 
	returnVal = ( *libc_bind ) ( fd , addr , addrlen );

	return returnVal;

}
int 
connect(int fd, const struct sockaddr *addr, socklen_t addrlen){
#ifdef DEBUG
	std::cout<<PRINT<<"File Descriptor [ "<<fd<<" ] \n";
#endif
	int                          port;
	int                          ipAddr;
	__resilience__::Interface    *intObj;
	int                          returnVal;
	char                         str[INET_ADDRSTRLEN];

	if ( !__pass__ ) initOverLoad();
  
	if ( __pass__ &&  __isDelay__ ){ // environment is ready for delay
		if (addr->sa_family == AF_INET){
		       port = ntohs (((struct sockaddr_in*)addr)->sin_port);
		       ipAddr = ((struct sockaddr_in*)&addr)->sin_addr.s_addr;
		       inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
		       std::string ip = std::string(str);
		}
		else
			port = ntohs(((struct sockaddr_in6*)addr)->sin6_port);
#ifdef DEBUG
		std::cout<<PRINT<<"FD ["<<fd<<"] port ["<<port<<"]\n";
#endif
		intObj = __pool__->getInterfaceObj();

		if ( !intObj->init( fd  , port ) ){
			std::cerr<<PRINT<<" sock FD ["<<fd
				<<"] -- not able to register\n";
			//returnVal = -1;
		}else if ( intObj->registerSock ( fd  , fd ) ) // update the accepted fd
			registerSock( fd );

		__pool__->returnInterfaceObj( intObj );
	}//returnVal == -1 ? returnVal = -1: 
	returnVal = ( *libc_connect ) ( fd , addr , addrlen );

	return returnVal;

}
/*    
int main(){
	initOverLoad();


	return 0;

}
*/
