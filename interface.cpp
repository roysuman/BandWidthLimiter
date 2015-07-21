/*
 * =====================================================================================
 *
 *       Filename:  interface.cpp
 *
 *    Description:  interface that is the main controller
 *
 *        Version:  1.0
 *        Created:  Thursday 23 April 2015 06:30:50  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy (), 
 *   Organization:  Cognizant Technological solutions
 *
 * =====================================================================================
*/

#include"interface.hpp"

using namespace __resilience__;

//Lock           __resilience__::Interface::lock;
IpcServer      __resilience__::Interface::ipcSerIns;
std::thread    __resilience__::Interface::ipcThread;
int            __resilience__::Interface::ipcServerPort = 5021;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init
 *  Description:  initialise the environment... read env variable and register
 *               @return: true on success else false
 * =====================================================================================
 */
bool 
__resilience__::Interface::init( void ){
#ifdef DEBUG

	std::cout<<PRINT<<"Init from env_variable\n";
#endif

	bool    returnVal;
/*
	ipcSerIns.manager();
	exit(0);
*/

//	IpcServer ins;
	returnVal =doRegister();
	//create thread for ipc server
	//std::thread kipcThread( &IpcServer::manager &ipcSerIns );
	return returnVal;
	
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init
 *  Description:  register a new created socket 
 *              @sockFd: socket descriptor id,
 *              @port: port bind with the socketfd
 * =====================================================================================
 */

bool 
__resilience__::Interface::init ( int sockFd , uint32_t port ){
#ifdef DEBUG
	std::cout<<"\nregister a created socket\n";
#endif
	std::string    ipAddr;
	bool           returnVal;

	//check port registered or not
	 returnVal =  isPortRegistered( port ) ;

	 if ( returnVal ){
		 std::cout<<PRINT<<"Going to register port [ "<<port<<"] "<<std::endl;
		 returnVal = getIpAddress( port , ipAddr );
		 returnVal == true ? returnVal= fd::Fd::init( sockFd , port , ipAddr ) : returnVal = false;
	}
	 else{

		std::cerr<<PRINT<<" SocketFd [ "<<sockFd 
			<<" ] Not able to enter into the lookup table --Port [ "
			<<port<<" Not under filtering condition"<<std::endl;
	}

	return returnVal;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  returnCurrentTime
 *  Description:  return the current system time
 *                @parameter: void
 *                @returnVal CurrentTime.
 * =====================================================================================
 */
struct timespec 
__resilience__::Interface::returnCurrentTime( void ){
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timeDifference
 *  Description:  get the time difference in nanoseconds between two time.
 *                 @Parameter: two different time variable.
 * =====================================================================================
 */
long int
__resilience__::Interface::timeDifference( struct timespec oldTime , 
		struct timespec currentTime ){
	long int timeDiff;
	timeDiff = ( ((currentTime.tv_sec * NANO_MULT ) + currentTime.tv_nsec) -
			( ( oldTime.tv_sec  * NANO_MULT  ) + oldTime.tv_nsec ) );
	return timeDiff;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getPortStatus
 *  Description:  get the number of remaining byte avail for the port(current window)
 *                if the remaing buyte is zero, then go to sleep untill this time window
 *                and set the byte limit.
 *                to do--- one thread is going to sleep blocking other threads  
 *                @Parameter: port number and flag: true for read window and false for 
 *                write window.
 *                @Return: no of Byte can be send at current windo.
 * =====================================================================================
 */
ssize_t 
__resilience__::Interface::getPortStatus( int fd  , bool flag){
#ifdef DEBUG
	std::cout<<PRINT<<"Fd [ "<<fd<<" ] flag [ "<<flag<<" ] \n";
#endif
	long int            timeDiff;
	struct timespec     prevTime;
	struct timespec     sleepUntil;
	struct timespec     currentTime;
	ssize_t             byteCanHandleThisWindow;
	ssize_t             byteCanHandleThisWindow2;
	int                 port;

	getDestPort( fd , port );
	//check whether port is bzy for read/write?
//	acquireLock( port , flag );


	//check whether limit needed or not
	if ( !flag )
		byteCanHandleThisWindow = getInitUpLimit( port ) ;//* 1024;
	else byteCanHandleThisWindow = getInitDownLimit( port );// * 1024;
#ifdef DEBUG
	std::cout<<"Limit [ "<<byteCanHandleThisWindow<<" ] \n";
#endif
	if ( byteCanHandleThisWindow != -1 ) {
		
		currentTime = returnCurrentTime();
		prevTime = getTimeVal( port , flag );
		timeDiff = timeDifference( prevTime , currentTime );
#ifdef TEST
		if ( timeDiff == -1 ){
			std::cout<<"Changed took effect\n";
			exit( 1 );
		}
#endif
#ifdef DEBUG
		std::cout<<PRINT<<"Time Diff[ "<<timeDiff<<" ] \n";
#endif
		if ( timeDiff > NANO_MULT ){ // 1000000000 ){// time widow slot not present
#ifdef DEBUG
			std::cout<<PRINT<<"Last packet was 1sec before\n";
#endif
		//	lock.acquirePortStatLock( );
			setByteCountWindow( port , byteCanHandleThisWindow , flag );

		//	lock.releasePortStatLock( );
		}else{
#ifdef DEBUG
			std::cout<<PRINT<<"Get remaining byte counts for current window\n";
#endif
			byteCanHandleThisWindow2 =getRemainingByteCountForCurrentWindow( port , flag );
#ifdef DEBUG
			std::cout<<" Remaining Byte Count For Current Window[ "
				<<byteCanHandleThisWindow2<<"]  \n";
#endif
			//if bytecan read=0..then sleep untill this window gets over
			if ( byteCanHandleThisWindow2 <= 0 ){
				long int delaySleep = NANO_MULT - timeDiff ;   
				sleepUntil = { delaySleep / NANO_MULT , delaySleep % NANO_MULT };
#ifdef DEBUG
				std::cout<<"Going to sleep for Sec  ["<<sleepUntil.tv_sec 
					<<" ]  Nano Second [ "<<sleepUntil.tv_nsec <<" ] \n";
#endif
				struct timespec t1= { 0 , 0};
			/* 	if ( !flag)
					lock.releseWriteLock();
				else lock.releaseReadLock();
			*/	
				nanosleep(&sleepUntil, &t1);
			/* 	if ( !flag)
					lock.acquireWriteLock();
				else lock.acquireReadLock();
				if ( !flag )
					byteCanHandleThisWindow = getInitUpLimit( port ) ;
				else byteCanHandleThisWindow = getInitDownLimit( port );// * 1024;
				*/
				setByteCountWindow( port , byteCanHandleThisWindow , flag );
		//		updateTimeVal ( port , flag);
			}else byteCanHandleThisWindow = byteCanHandleThisWindow2;
		}
	}else setByteCountWindow( port , 0 , flag ); // set bytecount window to 0-- when port limit set to -1 
	//lock.releasePortStatLock(  );
	return byteCanHandleThisWindow;
}

bool __resilience__::Interface::updateTime( int fd , bool flag ){
	int        port;
	uint32_t   latency;
	bool       returnVal;
	long int   timeDiff;
	struct timespec    oldTime;
	struct timespec    sleepUntil;

	returnVal = false;


	//calculation for latency
//	struct timespec currentTime;
//	currentTime = returnCurrentTime();


	returnVal =getDestPort( fd , port );
	//get latency of porti
	getLatencyOfPort( port , latency );
	if ( returnVal && latency != 0 && getStartTimeOfPacket( fd , flag , oldTime  ) ){
		timeDiff = timeDifference( oldTime , returnCurrentTime() ); 
		latency *= 1000;
		if ( latency > timeDiff ){
			long int delaySleep = latency - timeDiff;
			sleepUntil = { delaySleep / NANO_MULT , delaySleep % NANO_MULT };
			struct timespec t1 ={0,0};
			nanosleep(&sleepUntil, &t1);

		}
	}
	if ( returnVal  ) returnVal = updateTimeVal( port , flag );
	//release read/write lock
//	releaseLock( port , flag );
//	std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
	if ( !returnVal )std::cerr<<PRINT<<"Not able to update time value\n";

	return returnVal;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  updateReadBuffer
 *  Description:  after reading data from the kernel socket buffer. If the read byte is
 *                > than the allowed window bytes then need to store into a buffer.
 *                and prediocally need to send back.
 *                @parameter: 1. howManyByteCanRead: how many byte allowd for the window
 *                2.readCount : no Byte read from the kerbel socket buffer
 *                3. fd : file ( socket ) descriptor
 *                4. buff: contains the data read from the kernel buffer.
 *                ( if the read byte is <= the allowed byte then no need to store data else 
 *                store the extra byte) 
 * =====================================================================================
 */
ssize_t 
__resilience__::Interface::updateReadBuffer( size_t howManyByteCanRead , 
		size_t readCount , int fd, void * buff   ){
#ifdef DEBUG
	std::cout<<PRINT<<" Read bytes possible [ "<<howManyByteCanRead
		<<" ] Read Coount [ "<<readCount<<" ] FD [ "
		<<fd<<" ] "<<std::endl;
#endif

	ssize_t    cutSize;
	ssize_t    readData;
	int        port;
/*  	size_t             byteCanHandleThisWindow;

	byteCanHandleThisWindow = getInitDownLimit( port );
	if ( byteCanHandleThisWindow < readCount ){
		howManyByteCanRead = byteCanHandleThisWindow ;

	}
*/
	if ( readCount <= howManyByteCanRead )
		readData = readCount;
		//update remaing byte for this window

	else{
		//cut the overflow data and update to read buffer
		//else data send back
		cutSize = readCount - howManyByteCanRead ;
		char data[ cutSize ];
		memcpy ( data ,(char*) buff + howManyByteCanRead , cutSize  );
		memset( (char *)buff + howManyByteCanRead , 0 ,  cutSize );
		cutSize = accessReadBufferData ( fd , data , cutSize ,  false );

		//update the time also...
//		updateTimeVal ( getDestPort( fd  ) , true);
		readData = howManyByteCanRead ;
	}
	
	if ( readData >0 && getDestPort( fd , port ) )updateByteCountCurrentWindow(  port , readData  , true );
	return  readData;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  updateSendBuffer
 *  Description:  same logic as updateReadBuffer.
 *                Data going to kernel buffer, depending on the window size 
 * =====================================================================================
 */
ssize_t 
__resilience__::Interface::updateSendBuffer(size_t  noBytes , 
		size_t howManyByteCanRead ,  int fd, void * buff   ){
#ifdef DEBUG
	std::cout<<PRINT<<"  NOBytes [ "<<noBytes <<" ] Max Read Byte [ "
		<<howManyByteCanRead <<" ] Fd [ "<<fd <<" ] "<<std::endl;
#endif
	ssize_t    cutSize;
	ssize_t    writeByte;
	int        port;
/*  	size_t    byteCanHandleThisWindow;
 	byteCanHandleThisWindow = getInitUpLimit( port ) ;

	if ( byteCanHandleThisWindow < noBytes ){
		writeByte = ( ssize_t ) noBytes ;
		howManyByteCanRead = byteCanHandleThisWindow;
	}
*/
	if ((ssize_t ) noBytes <= (ssize_t )howManyByteCanRead ) 
		writeByte = ( ssize_t ) noBytes ;
	else{
		cutSize = noBytes - howManyByteCanRead ;
#ifdef DEBUG
		std::cout<<PRINT<<"Cut size [ "<<cutSize<<" ] \n";
#endif
		char data [ cutSize ] ;
		memcpy ( data , ( char *) buff +howManyByteCanRead , cutSize );
		memset ( (char*)buff + howManyByteCanRead , 0 , cutSize );
		accessSendBufferData( fd , data ,cutSize , false );
		writeByte = howManyByteCanRead;
		//update the time also...
	//	updateTimeVal ( getDestPort( fd  ) , false);
	}
	if ( getDestPort( fd , port ) )
		updateByteCountCurrentWindow( port , writeByte   , false );

	return writeByte;
}
void
__resilience__::Interface::readWriteLock( const int fd , const bool readWrite ,const bool acqu ){
	int                 port;

	getDestPort( fd , port );

	acqu == true?acquireLock( port , readWrite ):
		releaseLock( port , readWrite );
}
bool
__resilience__::Interface::initStartTimeForLatency( const int fd , const bool flag ){
	return ( insertStartTimeOfPacket ( fd , flag ,returnCurrentTime() ) ); 
}

void
__resilience__
::Interface::startIpcServer( void ){
#ifdef DEBUG
	std::cout<<PRINT<<"Start ipc server"<<std::endl;
#endif
	ipcThread = std::thread ( &IpcServer::manager ,ipcSerIns );

//	return ipcThread.joinable();
}

