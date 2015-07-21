/*
 * =====================================================================================
 *
 *       Filename:  registerEnv.cpp
 *
 *    Description:  do refister
 *
 *        Version:  1.0
 *        Created:  Saturday 25 April 2015 12:19:57  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include"registerEnv.hpp"
using namespace regEnv;
Message                RegisterEnv::buffIns; 
 __limitsContainer__    limitStore;

#ifdef LOCK_ERROR
 static pthread_mutex_t   readLock1;
 static pthread_mutex_t   writeLock1;
#endif


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  doRegister
 *  Description:  process the ip, port , Upload Limit, DownloadLimit from the environment
 *                 variable and update into a stl map container
 *                 @return: true on success else false
 * =====================================================================================
 */
bool 
regEnv::RegisterEnv::doRegister( ){
#ifdef DEBUG
	std::cout<<PRINT<<"Register filter ports"<<std::endl;
#endif

	std::string    ips;
	int            count;
	std::string    ports;
	std::string    upLimits;
	std::string    downLimits;
	bool           returnVal;
	std::string    latency;


	returnVal = true;

	try{
		//read data from envuironment veriable
		upLimits   = std::string ( getenv( "RESILIENCE_UP_LIMITS" ) );
		downLimits = std::string ( getenv( "RESILIENCE_DOWN_LIMITS") );
		ips        = std::string (getenv( "RESILIENCE_IPS") );
		ports      = std::string ( getenv ("RESILIENCE_PORTS" ) );
		latency    = std::string(getenv ("RESILIENCE_LATENCY" ) );

	}
	catch ( const std::exception &e ){
		std::cerr<<"\n Exception on reading environmeny var"
			<<e.what()<<std::endl;
	}
	count = 1; // to do have to count the no of ips
#ifdef DEBUG
	std::cout<<"\nNo of IPS-["<<count<<"]\n";
#endif
	
	for ( int t = 0 ; t < count && returnVal ; ++t ){
		//read ip
		size_t   pos = ips.find_first_of (';');
		std::string tempIp = ips.substr( 0, pos  );
		ips.erase( 0 , pos );

		//read port
		pos = ports.find_first_of(';');
		std::string tempPort = ports.substr( 0 , pos );
		ports.erase( 0 , pos );

		//read upLimit
		pos = upLimits.find_first_of (';' );
		std::string tempUpLimits = upLimits.substr( 0 , pos  );
		upLimits.erase( 0 , pos );

		//read DownLimit
		pos = downLimits.find_first_of(';');
		std::string tempDownLimits = downLimits.substr ( 0 , pos  );
		downLimits.erase( 0 , pos );
#ifdef DEBUG
		std::cout<<"\n going to register--\n"
			<<"IP---["<<tempIp<<"]\n"
			<<"Port--["<<tempPort<<"]\n"
			<<"Uplimit--["<<tempUpLimits<<"]\n"
			<<"DownLimits--["<<tempDownLimits<<"]\n";
#endif

		//insert in var
		__limitsContainerIterator__ it = 
			limitStore.find ( atoi (tempPort.c_str() ) );
		if ( it == limitStore.end() ){
			try{
				__limits__ tempLimit;// = new __limits__;
				tempLimit.upLimit = atoi( tempUpLimits.c_str() );
				tempLimit.downLimit = atoi ( tempDownLimits.c_str() );
				tempLimit.bindPort = atoi ( tempPort.c_str() ); 
				tempLimit.ipAddress =tempIp;
				tempLimit.latency = atoi(latency.c_str());
				if ( tempLimit.upLimit > 0 ) tempLimit.upLimit *=( 1000  );
				if ( tempLimit.downLimit > 0 )tempLimit.downLimit *=( 1000  );
			       	

				tempLimit.remainingReadByteForCurrentWindow = 
					tempLimit.downLimit;// * 1000;// * 1024;
				tempLimit.remainingWriteByteForCurrentWindow = 
					tempLimit.upLimit ;//* 1000;// * 1024;
				tempLimit.startTimeRead.tv_sec = 0;
				tempLimit.startTimeWrite.tv_sec = 0;
				tempLimit.startTimeRead.tv_nsec = 0;
				tempLimit.startTimeWrite.tv_nsec = 0;
				tempLimit.readLock = PTHREAD_MUTEX_INITIALIZER;
				tempLimit.writeLock = PTHREAD_MUTEX_INITIALIZER;


				std::pair< __limitsContainerIterator__ , bool > ret; 
				ret = limitStore.insert( std::pair< uint32_t , __limits__ > 
						( tempLimit.bindPort , tempLimit ) );
				if ( !ret.second ){
					std::cerr<<PRINT<<"Error on registering port [ "
						<<tempLimit.bindPort<<" ] "<<std::endl;
					exit ( EXIT_FAILURE );
				}
#ifdef DEBUG
					std::cout<<PRINT<<"REgistered UpLimits[ "<<tempLimit.upLimit
						<<" ] DownLimit [ "<<tempLimit.downLimit
						<<" ] Bind Port [ "<<tempLimit.bindPort<<" ] \n";
#endif
					//--- to do-- read buff size from config file
					//create buffer cache
					returnVal = buffIns.init( );

			}
			catch ( const std::exception &e ){
				std::cerr<<"\n Exception--"<<e.what()<<std::endl;
				returnVal = false;
			}
		}else{
			std::cout<<"\n Ip["<<tempIp<<"] -- already registered\n";
		}
	}
	
	return returnVal;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  modifyLimit
 *  Description:  modify the registered data/port 
 *               @tempLimit: structure containg all data for update/regis.
 *               @return: true on success else false
 * =====================================================================================
 */
bool 
regEnv::RegisterEnv::modifyEntry( __updateLimits__ tempLimit	){
	bool    returnVal;
	__limitsContainerIterator__    it;
	returnVal = true;
	try{
		it =  limitStore.find ( tempLimit.bindPort );
		
		  
		if ( it != limitStore.end() ){
			if ( tempLimit.upLimit >= 0 ){
				it->second.upLimit = tempLimit.upLimit;
			}
			if ( tempLimit.downLimit >= 0 ){
				it->second.downLimit = tempLimit.downLimit;
			}
		}
		else{
			std::cerr<<"Port [ "<<tempLimit.bindPort <<" ] not registered\n";
			returnVal = false;
		}
	}catch ( const std::exception &e ){
		std::cerr<<"\n Exception--"<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInitUpLimit
 *  Description:  return the Upload bandwidth limit maped with the port
 * =====================================================================================
 */
ssize_t 
regEnv::RegisterEnv::getInitUpLimit( const uint32_t port ){
#ifdef DEBUG
	std::cout<<PRINT<<"Port [ "<<port<<" ] "<<std::endl;
#endif
	
	__limitsContainerIterator__    it;
	long unsigned int                       returnVal;

	it = limitStore.find( port );
	if ( it == limitStore.end() ){
#ifdef DEBUG
		std::cerr<<"\n ["<<port<<"]--Not registered\n";
#endif
		returnVal = -1;
	} else{
		returnVal = it->second.upLimit;
		
	}
	return returnVal;

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInitDownlimit
 *  Description:  return the Download Limit maped with the corresponding ipAddress
 * =====================================================================================
 */
ssize_t
regEnv::RegisterEnv::getInitDownLimit ( const uint32_t port ){
#ifdef DEBUG
	std::cout<<PRINT<<" Port [ "<<port<<" ] "<<std::endl;
#endif

	__limitsContainerIterator__    it;
	ssize_t                      returnVal;

	it = limitStore.find( port );
	if ( it == limitStore.end() ){
#ifdef DEBUG
		std::cerr<<"\n ["<<port<<"]--Not registered]n";
#endif
		returnVal = -1;
	} else{
		returnVal = it->second.downLimit;
		
	}
	return returnVal;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getTimeVal
 *  Description:  return the start time of the read/write window
 *                if flag is true : readWindow: false: writeWindow
 *                @port: port for filtering
 *                @flag: true->read window else send window
 *                @return: last data send/received time
 * =====================================================================================
 */
struct 
timespec regEnv::RegisterEnv::getTimeVal( const uint32_t port, bool flag  ){
#ifdef DEBUG
	std::cout<<PRINT<<"Get the last access time of POrt [ "
		<<port<<" ] Flag [ "<<flag<<"] \n";
#endif

	__limitsContainerIterator__     it;
	struct timespec                 timerClock;

	it =limitStore.find( port );
	
	if ( it == limitStore.end() ){
		std::cerr<<PRINT<<"Not able to get the last access time\
			for port ["<<port<<"]--Not registered \
			Situation should not appear-- EXITNG\n";
		exit ( EXIT_FAILURE );//have to remove
		timerClock.tv_sec = 9999;
		timerClock.tv_nsec = 9999;
	} else{
		if (flag ){
			timerClock = it->second.startTimeRead;
		}else{
			timerClock=it->second.startTimeWrite;
		}
		
	}
	return timerClock;

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  updateTimeVal
 *  Description:  update the time window start time mapped to the port
 *              @port: p[ort number
 *              @flag: true for read else write 
 * =====================================================================================
 */
bool 
RegisterEnv::updateTimeVal( const uint32_t port ,  bool flag  ){

#ifdef DEBUG
	std::cout<<PRINT<<" Update access time of Port [ "
		<<port<< " ] Flag [ "<<flag<<" ] "<<std::endl;
#endif
	__limitsContainerIterator__    it;
	bool                           returnVal;

	returnVal = true;

	it =limitStore.find( port );
	
	if ( it == limitStore.end() ){
		std::cerr<<PRINT<<"Failed on Update access time for port ["
			<<port<<"]--Not registered Situation\
		       	Situation Should not appear -- EXITING\n";
		exit(EXIT_FAILURE);
		returnVal = false;
	} else{
		struct timespec ts;
		if ( clock_gettime(CLOCK_REALTIME, &ts) != -1 ){
			flag == true ?it->second.startTimeRead = ts:
				it->second.startTimeWrite = ts;
		}else{
			std::cerr<<PRINT<<"Not able to get the current sys time--EXITING";
			exit(EXIT_FAILURE);
		}
		
	}
	return returnVal;

}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getRemainingByteCountForCurrentWindow
 *  Description:  get the available data can be downloaded/sendback for the current
 *                window frame.
 *                @port: Port number
 *                @flag :-> true : forRead ,   false : for write
 * =====================================================================================
 */

ssize_t
regEnv::RegisterEnv::getRemainingByteCountForCurrentWindow ( int port , bool flag ){

#ifdef DEBUG
	std::cout<<PRINT<<" Remaining byte count @current window for Port [ "
		<<port<<" ] Flag [ "<<flag<<" ] "<<std::endl;
#endif

	__limitsContainerIterator__     it;
	ssize_t                         remaingCount;

	it = limitStore.find( port );
	if ( it == limitStore.end() ){
		std::cerr<<PRINT" Port  ["<<port<<"] Not registered--hCondition should not\
		       	happen--EXIT";
		exit( EXIT_FAILURE); // have to remove
//		remaingCount = 9999999;
	} else{
		flag == true?remaingCount = it->second.remainingReadByteForCurrentWindow:
			remaingCount=it->second.remainingWriteByteForCurrentWindow;
		
	}
	return remaingCount;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  updateByteCountCurrentWindow
 *  Description:  update the remaing buyte transfer available for the current window time
 *               @port: port number
 *               @usedByte: byte just trunsfared 
 *               @flag: true for read else write
 * =====================================================================================
 */
bool 
regEnv::RegisterEnv::updateByteCountCurrentWindow( int port , ssize_t usedBytes , bool flag ){
#ifdef DEBUG
	std::cout<<PRINT<<" Update byte count for Port [ "<<port<<" ] Used BYtes [ "
		<<usedBytes<<" ] Flag [ "<<flag<<" ] "<<std::endl;
#endif
	__limitsContainerIterator__     it;
	bool                            returnVal;

	returnVal = true;

	it = limitStore.find( port );
	if ( it == limitStore.end() ){
		std::cerr<<PRINT<<" ["<<port<<"]--Not registered--EXITING";
		exit( EXIT_FAILURE);
		returnVal = false;
	} else{
		flag == true? it->second.remainingReadByteForCurrentWindow-=usedBytes :
			it->second.remainingWriteByteForCurrentWindow -=usedBytes;
		if ( it->second.remainingReadByteForCurrentWindow < 0 ) 
			it->second.remainingReadByteForCurrentWindow = 0;
		if ( it->second.remainingWriteByteForCurrentWindow < 0 )
			it->second.remainingWriteByteForCurrentWindow = 0;
		
	}
	return returnVal;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setByteCountWindow
 *  Description:  set the byte count for the current window mapped by port
 *              @port: port number.
 *              @size: no of Bytes this window will be able to handle.
 *              @flag: true for read else send
 * =====================================================================================
 */
bool 
regEnv::RegisterEnv::setByteCountWindow( int port  , ssize_t  size , bool flag ){

#ifdef DEBUG
	std::cout<<PRINT<<" Set byte count for Port [ "<<port<<" ]  BYtes [ "
		<<size<<" ] Flag [ "<<flag<<" ] "<<std::endl;
#endif
	__limitsContainerIterator__     it;
	bool                            returnVal;

	returnVal = true;

	it = limitStore.find( port );
	if ( it == limitStore.end() ){
		std::cerr<<PRINT<<" ["<<port<<"]--Not registered --\
		       	This Condition should not happen\n";
		exit ( EXIT_FAILURE );
		returnVal = false;
	} else{
		flag == true? it->second.remainingReadByteForCurrentWindow= size:
			it->second.remainingWriteByteForCurrentWindow = size;
		
	}
	return returnVal;
}
bool
regEnv::RegisterEnv::isPortRegistered( const uint32_t  port) {
#ifdef DEBUG
	std::cout<<PRINT<<" Check  Port [ "<<port<<" ]  registered ? "<<std::endl;
#endif
	__limitsContainerIterator__     it;
	bool                            returnVal;

	try{
		it = limitStore.find( port );
		if ( it == limitStore.end() ){
#ifdef DEBUG
			std::cout<<PRINT<<"Port [ "<<port <<" ] Not registered"<<std::endl;
#endif
			returnVal = false;
		}else{
#ifdef DEBUG
			std::cout<<PRINT<<"Port [ "<<port <<" Registered for filtering"<<std::endl;
#endif
			returnVal = true;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}


bool
regEnv::RegisterEnv::getIpAddress ( int port, std::string &ip ){


	__limitsContainerIterator__     it;
	bool                            returnVal;

	try {
		it = limitStore.find( port );
		if ( it == limitStore.end() ){
			std::cerr<<PRINT<<" ["<<port<<"] ThCondition should not\
				happen--Not registered-- EXIT";
			exit ( EXIT_FAILURE );
			returnVal = false;
		} else{
			ip = it->second.ipAddress;
			returnVal = true;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}

void
 regEnv::RegisterEnv::acquireLock( const uint32_t port , const bool readWriteFlag ){
#ifdef DEBUG
	 std::cout<<PRINT<<"Acquire lock for port [ "<<port<<" ] "<<std::endl;
#endif
	 //readWriteFlag == true == for read else for write		
	__limitsContainerIterator__     it;
	try {
		it = limitStore.find( port );
		if ( it == limitStore.end() ){
			std::cerr<<PRINT<<"["<<port<<"] This Condition should not\
				happen--Not registered--EXIT";
			exit( EXIT_FAILURE );
		} else{
#ifdef LOCK_ERROR
			readWriteFlag == true ? pthread_mutex_lock( &readLock1) : 
				pthread_mutex_lock( &writeLock1 );
#else
			readWriteFlag == true ? pthread_mutex_lock( &it->second.readLock) : 
				pthread_mutex_lock( &it->second.writeLock );
#endif

#ifdef TEST
			std::cout<<PRINT<<"Port [ "<<port<<" ReadWriteFlag [ "<
				<readWriteFlag<<" ] "<<std::endl;
#endif
		}
	}
	catch( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
	}

 }
void
regEnv::RegisterEnv::releaseLock ( const uint32_t port ,const bool readWriteFlag ){


	 //readWriteFlag == true == for read else for write		
	__limitsContainerIterator__     it;
	try {
		it = limitStore.find( port );
		if ( it == limitStore.end() ){
			std::cerr<<"\n ["<<port<<"] This Condition should not\
				happen--Not registered-- EXIT";
			exit( EXIT_FAILURE );
		} else{
#ifdef LOCK_ERROR
			readWriteFlag == true ? pthread_mutex_unlock( &readLock1) : 
				pthread_mutex_unlock( &writeLock1 );
#else

			readWriteFlag == true ? pthread_mutex_unlock( &it->second.readLock) :
			       	pthread_mutex_unlock( &it->second.writeLock );
#endif

#ifdef TEST
			std::cout<<PRINT<<"Port [ "<<port<<" ReadWriteFlag [ "
				<<readWriteFlag<<" ] "<<std::endl;
#endif
		}
	}
	catch( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
	}
}


bool
regEnv::RegisterEnv::getLatencyOfPort( const uint32_t  port , uint32_t &latency ){
	__limitsContainerIterator__     it;
	bool                            returnVal;
	
	try {
		it = limitStore.find( port );
		if ( it == limitStore.end() ){
			std::cerr<<"\n ["<<port<<"] This Condition should not\
				happen--Not registered-- EXIT";
			returnVal = false;
			exit( EXIT_FAILURE );
		} else{
		       	latency =it->second.latency ;
			returnVal = true;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception " <<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}

