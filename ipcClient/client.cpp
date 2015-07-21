
/****************** CLIENT CODE ****************/
#include "client.hpp"
static char *serverIp="127.0.0.1";   //"10.226.195.210";
//std::thread    IpcClient::threadClient;
static pthread_mutex_t    lockKey = PTHREAD_MUTEX_INITIALIZER;
bool
IpcClient::createClient(){
	bool         returnVal;
	socklen_t    addr_size;
	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
	fd = socket(PF_INET, SOCK_STREAM, 0);
	if ( fd == -1 ){
		std::cerr<<"Not able to create socket"<<std::endl;
		exit(0);
	}
	/*---- Configure settings of the server address struct ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;
	/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_port = htons(8888);
	/* Set IP address to localhost */
	serverAddr.sin_addr.s_addr = inet_addr(serverIp);
	/* Set all bits of the padding field to 0 */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
	/*---- Connect the socket to the server using the address struct ----*/
	addr_size = sizeof serverAddr;
	connect( fd , (struct sockaddr *) &serverAddr, addr_size) == 0 ? returnVal = true:returnVal = false;
	if ( !returnVal){
		std::cerr<<"Not abble to c=establish connection with server:"<<std::endl;
		exit( EXIT_FAILURE);
	}
	pthread_mutex_unlock( &lockKey);
	return returnVal;

}

bool
IpcClient::sendData(__updateLimits__  limit ){
	bool      returnVal;
	char      buffer[ 1000 ];
	ssize_t    count;
	pthread_mutex_lock( &lockKey);
	std::cout<<"SEND\n";

//	count = send( fd , &limit,sizeof(__updateLimits__ ) , 0 );
        count = write ( fd , &limit , sizeof ( __updateLimits__ ) );
	if ( count >= 0 ){
		/*---- Read the message from the server into the buffer ----*/
		std::cout<<"RECV\n";
		count =recv(fd, buffer, 1024, 0);
		if ( count > 0 ){
			std::cout<<buffer<<std::endl;
			memset( buffer , '\0' , count );
		}else if ( count <0 ){ std::cerr<<"Not able to read data from IPC server"<<std::endl; returnVal = false;}
	}else { 
		std::cerr<<"Error on send Data to IPC client"<<std::endl;
		returnVal = false;
	}
	std::this_thread::yield();
	close(fd);
	pthread_mutex_unlock( &lockKey);
	return returnVal;

}
void
IpcClient::initClient( void ){
	pthread_mutex_lock( &lockKey);
	std::cout<<__FUNCTION__<<__LINE__<<std::endl;
	threadClient = std::thread ( &IpcClient::createClient , this );
}
void 
IpcClient::usage(void  ){
	std::cout<<"\n\t-u Uplimits\n"
		<<"\t-d Downlimits\n"
		<<"\t-h Help\n"
		<<"\t-l Licence\n"
		<<"\t-i serverIp\n"
		<<"\t-p Ports\n"
		<<"\t-v Version"<<std::endl;
	exit(1);
}

int main( int argc , char** argv){
	
	int opt;
	int port;
	int upLimit;
	int downLimit;
	__updateLimits__    limits;
	IpcClient    ipc;
	/*
	if ( !ipc.createClient( ) ){
		std::cerr<<"Can't connect with the server\n";
		exit(1);
	}
	*/
	upLimit = 0;
	downLimit = 0;
	port = 0;
	bool u,d,p;
	u = false;
	d = false;
	p = false;
	std::cout<<"CHECK\n";
	while (  ( opt = getopt ( argc , argv ,"lhvU:u:d:D:p:P:I:i") ) != -1 )
		switch ( opt ){
			case 'u':
			case 'U':
				upLimit = atoi( optarg );
				u = true;
				break;
			case 'd':
			case 'D':
				downLimit = atoi( optarg );
				d = true;
				break;
			case 'l':
			case 'L':
				std::cout<<"\n Licensed under CTS open-source\n";
				break;
			case 'p':
			case 'P':
				port = atoi(optarg );
				p = true;
				break;
			case 'v':
			case 'V':
				std::cout<<"Version alpha 0.1-- under test\n";
				break;
			case 'i':
			case 'I':
				serverIp = optarg;


			case '?':
				if (optopt == 'c')
					std::cerr<<"Option -"<<optopt<<" requires an argument.\n";
				else if (isprint (optopt))
					std::cerr<<"Unknown option "<< optopt<<std::endl;
				ipc.usage();
				exit(0);
				break;
			case 'h':
			case 'H':
			default:
				ipc.usage();
		}
	if ( !p || !d || !u ){ ipc.usage(); ipc.threadClient.join(); exit(0);}
	ipc.initClient();
		argc -= optind  ;
		argv +=optind  ;
		limits.bindPort = port;
		limits.upLimit = upLimit;
		limits.downLimit = downLimit;
		ipc.sendData( limits );
		return 0;
}
