#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <err.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdarg.h>
#include <string.h>
#include<stdbool.h>
#include <signal.h>

#include "interface.h"
char*                       __error__;
static void*                __handle__;
controller_struct          ins_cont_str;
static int                 init0,init1;
static void close_dl( void  );
static void init_overload( void );
static void init_clean_up( void );
#define INIT_ENV do {                               \
	        if (!init0 && !init1)      \
	                init_overload();                 \
} while (0)


#ifdef PTHREAD

static pthread_mutex_t bandwidth_global_lock = PTHREAD_MUTEX_INITIALIZER;
static void bandwidth_lock( ){
	pthread_mutex_lock( &bandwidth_global_lock);
	return;
}
static void bandwidth_unlock(){
	pthread_mutex_unlock( &bandwidth_global_lock);
	return;
}
#else
static void bandwidth_lock(){}
static void bandwidth_unlock(){}
#endif

#define __FUNCTION_DECLARE_(funcName, returnType, arguments)\
       	static returnType (*libc_##funcName) arguments


#define __LOAD__ADDR__( var ) do {\
	if ( ( libc_##var = dlsym ( __handle__ , #var) ) == NULL ){ \
		fprintf(stderr,"not able to load libc method #var%s",#var);\
		exit ( -1 );	}	\
} while (0)
#define CONTROLLER_OPS ins_cont_str.controller_ops
#define SOCK_STR ins_cont_str.soc_str
#define SOCK_STR_OPS ins_cont_str.soc_str->sock_ops

#define ENV_STR ins_cont_str.reg_env_str
#define ENV_STR_OPS ins_cont_str.reg_env_str->env_ops

/* all socket layer method 
 */

__FUNCTION_DECLARE_(dup, int, (int));
__FUNCTION_DECLARE_(close, int, (int));
__FUNCTION_DECLARE_(dup2, int, (int, int));
__FUNCTION_DECLARE_(connect, int ,( int , const struct sockaddr* , socklen_t) );
__FUNCTION_DECLARE_(socket, int, (int, int, int));
//__FUNCTION_DECLARE_(dup3, int, (int, int , int ));
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
static void 
close_dl( void ){
	char*    err_str;

	if ( __handle__ != NULL ){
		if ( dlclose( __handle__ ) != ZERO ){
			init0 = init1 = ZERO;
			err_str = dlerror();
			if (err_str != NULL)
				fprintf( stderr ,"A dynamic linking error\
					       	occurred: (%s)\n", err_str);
		}
	}
	return;
}
static void
init_clean_up( void ){
#ifdef INFO
	fprintf(stdout,"INFO: Init clean up kernel resources\n");
#endif
	close_dl();
	__clean_up__( &ins_cont_str );
	fprintf(stdout,"BYE\n");
}
	
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  INIT_ENV
 *  Description:  make the environemnt ready........
 * =====================================================================================
 */
static void 
init_overload( void ){


	char    *glibc_path;
	char    *err_str;
	//TODO register function for at exit
	atexit(init_clean_up);
	init0 = 1;
	if (!__init_env__ ( &ins_cont_str )  ){
		fprintf(stderr ,"Resilience_Bandwidth_Limitor init terminated\n");
		bandwidth_unlock();
		exit(EXIT_FAILURE);
	}
	glibc_path = getenv("RESILIENCE_GLIBC_PATH") ;
	if ( ( __handle__ = dlopen(glibc_path ,RTLD_LAZY ) ) == NULL ){
		err_str = dlerror();
		if (err_str != NULL)
			fprintf( stderr ,"A dynamic linking error\
				       	occurred: (%s)\n", err_str);
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
//	__LOAD__ADDR__( dup3 );
	__LOAD__ADDR__ ( bind );
	__LOAD__ADDR__ ( connect );
	init1 = 1;
#ifdef DEBUG
	fprintf(stdout,"Environment initialization for \
			Resilience_Bandwidth_Limitor Done\n");
#endif
	return;
}

/*-----------------------------------------------------------------------------
 *  all the below methodes are a wrapper for libc sock methodes..
 *  to understand the method description plese do "man methodName"
 *-----------------------------------------------------------------------------*/
int 
socket( int domain , int type , int protocol ){
#ifdef DEBUG
	fprintf(stdout,"socket\n");
#endif	
	int    fd;
	bandwidth_lock();
	
	INIT_ENV;
	bandwidth_unlock();

	fd = ( *libc_socket ) ( domain, type , protocol );
#ifdef DEBUG
	fprintf(stdout,"Socket created sock_id [ %d ] :\n",fd);
#endif
	return ( fd );
}

int 
close ( int fd ){ 

	int    ret_val;

	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	ret_val = (*libc_close)(fd);
	bandwidth_lock();
       	//loop untill sock not closed... if it loop continious..it's a bug
	(void) SOCK_STR_OPS->unregister_socket_descriptor(SOCK_STR, fd ) ;
	bandwidth_unlock();
	return ret_val;
}
 
 
ssize_t 
read( int fd , void *buff , size_t count ){
	
	ssize_t                      read_count;
	ssize_t                      byte_read_demand;
	
#ifdef DEBUG
	fprintf(stdout, "Read request for acp_socket_fd [ %d ]\n",fd );
#endif
	bandwidth_lock();
	INIT_ENV;
	byte_read_demand = count;
	if ( CONTROLLER_OPS->pre_op(&ins_cont_str, fd ,&byte_read_demand , READ_REQUSET) == ACTIVE_DELAY ){
		bandwidth_unlock();
		read_count = (*libc_read)(fd , buff , byte_read_demand );
		bandwidth_lock();
		if ( read_count > ZERO ){
			CONTROLLER_OPS->do_update( &ins_cont_str ,read_count , fd , READ_REQUSET );
			CONTROLLER_OPS->update_time( &ins_cont_str , fd , READ_REQUSET );
		}
		bandwidth_unlock();
	} else {
		bandwidth_unlock();
		read_count = (*libc_read)(fd , buff , count );
	}
	return read_count;
}


ssize_t 
recv( int fd , void* buff , size_t len, int flags ){
	ssize_t                      read_count;
	ssize_t                      byte_read_demand;
#ifdef DEBUG
	fprintf(stdout, "recv request for acp_socket_fd [ %d ]\n",fd );
#endif
	bandwidth_lock();
	INIT_ENV;

	byte_read_demand = len;
	if (  CONTROLLER_OPS->pre_op(&ins_cont_str, fd ,&byte_read_demand , READ_REQUSET) == ACTIVE_DELAY ){
		bandwidth_unlock();
		read_count = (*libc_recv)(fd , buff , byte_read_demand , flags);
		bandwidth_lock();
		if ( read_count > ZERO ){
			CONTROLLER_OPS->do_update( &ins_cont_str ,read_count , fd, READ_REQUSET );
			CONTROLLER_OPS->update_time( &ins_cont_str , fd , READ_REQUSET);
		}
		bandwidth_unlock();
	}else{
		bandwidth_unlock();
		read_count = (*libc_recv)(fd , buff , len , flags);
	}
	return read_count;

}

//no need any delay logic.. becayse it's calling read method only.. see kernel source
ssize_t 
readv( int fd , const struct iovec* iovec , int iov_count){
	ssize_t    read_count;
#ifdef DEBUG
	fprintf(stdout, "readv request for acp_socket_fd [ %d ]\n",fd );
#endif
	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	read_count = (*libc_readv )(fd , iovec , iov_count );
	return read_count;
}

ssize_t 
recvfrom ( int fd, void* buff , size_t len , int flags , struct sockaddr * from , 
		socklen_t *fromlen ){

	ssize_t                      read_count;
	ssize_t                      byte_read_demand;
#ifdef DEBUG
	fprintf(stdout, "recvfrom request for acp_socket_fd [ %d ]\n",fd );
#endif
	bandwidth_lock();
	INIT_ENV;
	byte_read_demand = len;
	if ( CONTROLLER_OPS->pre_op(&ins_cont_str, fd ,&byte_read_demand , READ_REQUSET ) == ACTIVE_DELAY ){
		bandwidth_unlock();
		read_count = (*libc_recvfrom )( fd , buff , byte_read_demand , 
				flags , from , fromlen );
		bandwidth_lock();
		if ( read_count > ZERO ){
			CONTROLLER_OPS->do_update( &ins_cont_str ,read_count , fd, READ_REQUSET);
		CONTROLLER_OPS->update_time( &ins_cont_str , fd , READ_REQUSET );
		}
		bandwidth_unlock();
	}else{
		bandwidth_unlock();
			read_count = (*libc_recvfrom )( fd , buff , len , 
					flags , from , fromlen );
	}
	return read_count;
}

ssize_t 
write(int fd ,const void * buff, size_t len ){

	ssize_t    byte_send_demand;
	ssize_t    send_count;
#ifdef DEBUG
	fprintf(stdout,"write socket call for acp_fd [ %d ] of size [ %zu ]\n",fd ,len );
#endif
	byte_send_demand = len;
	bandwidth_lock();
	INIT_ENV;
	if ( CONTROLLER_OPS->pre_op(&ins_cont_str,fd,&byte_send_demand, WRITE_REQUEST ) == ACTIVE_DELAY ){
		bandwidth_unlock();
		send_count = (*libc_write)(fd  , buff , byte_send_demand );
		bandwidth_lock();
		
		if ( send_count > ZERO ){
			CONTROLLER_OPS->do_update( &ins_cont_str, send_count , fd, WRITE_REQUEST );
			CONTROLLER_OPS->update_time( &ins_cont_str , fd , WRITE_REQUEST );
		}
		bandwidth_unlock();
	}else{
		bandwidth_unlock();
		send_count = (*libc_write)(fd  , buff , len );
	}
#ifdef DEBUG
			fprintf(stdout,"[%zd] bytes send to kernel buffer for write\n",send_count);
#endif
	return send_count ;
}


ssize_t 
send(int fd , const void * buff , size_t len , int  flags ){
	ssize_t    byte_send_demand;
	ssize_t    send_count;
	
#ifdef DEBUG
	fprintf(stdout,"write socket call for acp_fd [ %d ] of size [ %zu ]\n",fd ,len );
#endif
	byte_send_demand = len;
	bandwidth_lock();
	INIT_ENV;
	if ( CONTROLLER_OPS->pre_op (&ins_cont_str, fd ,&byte_send_demand , WRITE_REQUEST) == ACTIVE_DELAY){
		bandwidth_unlock();
		send_count = (*libc_send)(fd  , buff , byte_send_demand, flags );
		bandwidth_lock();
		if ( send_count > ZERO ){
			CONTROLLER_OPS->do_update( &ins_cont_str, send_count , fd, WRITE_REQUEST );
			CONTROLLER_OPS->update_time( &ins_cont_str , fd , WRITE_REQUEST );
		}
		bandwidth_unlock();
	}else{
		bandwidth_unlock();
		send_count = (*libc_send)(fd  , buff ,len, flags );
	}
#ifdef DEBUG
		fprintf(stdout,"[%zd] bytes send to kernel buffer for write\n",send_count);
#endif
	return send_count;
}
//no need , because it's calling write method only... see kernel source
ssize_t 
writev(int fd , const struct iovec * iovec , int iov_count ){
	ssize_t    no_of_bytes;
	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();
	no_of_bytes = (*libc_writev )( fd , iovec , iov_count );
	return no_of_bytes;
}

ssize_t 
sendto(int fd ,  const  void *buff, size_t len, int flags,
		const struct sockaddr *dest_addr, socklen_t addr_len ){

	ssize_t    byte_send_demand;
	ssize_t    send_count;

	byte_send_demand = len;
	bandwidth_lock();
	INIT_ENV;
	if ( CONTROLLER_OPS->pre_op (&ins_cont_str, fd , &byte_send_demand , WRITE_REQUEST ) == ACTIVE_DELAY ){
		bandwidth_unlock();
		send_count = (*libc_sendto)(fd  , buff , byte_send_demand, flags  , dest_addr , addr_len );
		bandwidth_lock();
		
		if ( send_count > ZERO ){
			CONTROLLER_OPS->do_update( &ins_cont_str, send_count , fd, WRITE_REQUEST );
			CONTROLLER_OPS->update_time( &ins_cont_str , fd , WRITE_REQUEST );
		}
		bandwidth_unlock();
	}else{
		bandwidth_unlock();
		send_count = (*libc_sendto)(fd  , buff , len, flags  , dest_addr , addr_len );
	}
#ifdef DEBUG
		fprintf(stdout,"[%zd] bytes send to kernel buffer for write\n",send_count);
#endif
	return send_count ;
}

int 
select(int nfds , fd_set * read_fds, fd_set * write_fds, fd_set *exceptfds, 
		struct timeval * timeout){

	int   noFd;
	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	noFd = (*libc_select)( nfds , read_fds , write_fds , exceptfds , timeout );

	return noFd;
}

int 
poll(struct pollfd *fds, nfds_t nfds, int timeout){
	int    return_val;

	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	return_val = (*libc_poll )( fds , nfds , timeout );
	
	return ( return_val );
}
int 
accept(int fd, struct sockaddr *addr, socklen_t *addr_len){
	int   acp_fd;
	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	acp_fd = (*libc_accept )( fd , addr , addr_len );
#ifdef DEBUG
	fprintf( stdout ,"Org_fd [ %d ] ..acceptedFd [ %d ] \n",fd,acp_fd);
#endif
	if ( acp_fd != -1 ){
		bandwidth_lock();
		(void)SOCK_STR_OPS->register_sock( SOCK_STR, fd , acp_fd );
		bandwidth_unlock();
	}
	return acp_fd;
}
 
int 
dup(int old_fd){
	int    new_fd;
	
	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	new_fd = (*libc_dup )( old_fd );
	if ( new_fd != NEG_ONE ){
		bandwidth_lock();
		(void)CONTROLLER_OPS->func_dup_manager(&ins_cont_str, old_fd ,new_fd );
		bandwidth_unlock();
	}
	return new_fd;
 }

int 
dup2(int old_fd, int new_fd){
	int    return_val;
	
	bandwidth_lock();
	INIT_ENV;
	bandwidth_unlock();

	return_val = (*libc_dup2 )( old_fd , new_fd );
	if ( return_val != NEG_ONE ){
		bandwidth_lock();
		(void)CONTROLLER_OPS->func_dup_manager(&ins_cont_str, old_fd ,new_fd );
		bandwidth_unlock();
	}
	return return_val;
}
int 
bind(int fd, const struct sockaddr *addr, socklen_t addr_len){
#ifdef DEBUG
	fprintf(stdout , "File Descriptor [ %d ] bind\n",fd);
#endif
	uint32_t                     port;
	int                          ip_addr;
	int                          return_val;
	char                         str[INET_ADDRSTRLEN];

	bandwidth_lock();
	INIT_ENV;
	if (addr->sa_family == AF_INET){
		port = ntohs (((struct sockaddr_in*)addr)->sin_port);
		ip_addr = ((struct sockaddr_in*)&addr)->sin_addr.s_addr;
		inet_ntop( AF_INET, &ip_addr, str, INET_ADDRSTRLEN );
	}
	else
		port = ntohs(((struct sockaddr_in6*)addr)->sin6_port);
#ifdef DEBUG
	fprintf(stdout,"socket_fd [ %d ] of port [ %u ]\n",fd,port);
#endif
	(void)CONTROLLER_OPS->regis_new_created_socket( &ins_cont_str,fd ,port );
	bandwidth_unlock();
	return_val = ( *libc_bind ) ( fd , addr , addr_len );

	return return_val;

}
int 
connect(int fd, const struct sockaddr *addr, socklen_t addr_len){
#ifdef DEBUG
	fprintf(stdout ,"\nFile Descriptor[ %d ] connect\n ",fd);
#endif
	uint32_t                          port;
	int                          ip_addr;
	int                          return_val;
	char                         str[INET_ADDRSTRLEN];

	bandwidth_lock();
	INIT_ENV;
	if (addr->sa_family == AF_INET){
		port = ntohs (((struct sockaddr_in*)addr)->sin_port);
		ip_addr = ((struct sockaddr_in*)&addr)->sin_addr.s_addr;
		inet_ntop( AF_INET, &ip_addr, str, INET_ADDRSTRLEN );
	}
	else
		port = ntohs(((struct sockaddr_in6*)addr)->sin6_port);
#ifdef DEBUG
	fprintf(stdout,"socket_fd [ %d ] of port [ %u ]\n",fd,port);
#endif
//TODO remove the following line and add at method socket--if error happens in future
	if ( CONTROLLER_OPS->regis_new_created_socket( &ins_cont_str,fd ,port ) ){
		(void)SOCK_STR_OPS->register_sock( SOCK_STR, fd , fd );
	}else 
		fprintf(stderr ," acp_sock FD [%d ] not able to register \n ",fd);

	bandwidth_unlock();
	return_val = ( *libc_connect ) ( fd , addr , addr_len );
	return return_val;
}
