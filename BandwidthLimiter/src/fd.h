
#ifndef FD_H_
#define FD_H_
#define DEFAULT_UP 2
#define DEFAULT_DOWN 2
#ifdef UBUNTU
#include<uistd.h>
#endif
#include<stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "red_black_tree.h"
#include "bandwidth_globals.h"


/*-----------------------------------------------------------------------------
 *  the libc socket method creates a socket_fd and then using that socket_fd 
 *  other connection_fd geting created. Struct socket_fd_struct is used to look-
 *  up all sockets created by method socket(),
 *  @@port: port of the sopcket
 *  @@ref_count: how many connected socket created using this sock_fd
 *  
 *  ref_count = 0 indicates no connection established using this socket_fd
 *-----------------------------------------------------------------------------*/
typedef struct _socket_fd_struct socket_fd_struct;
struct _socket_fd_struct{
	uint32_t       port;
	ssize_t        ref_count;
};

/*-----------------------------------------------------------------------------
 *  structure to keep lookup information of all active sockets which has been 
 *  created by the sock_fd (returned by sock() system call.
 *  
 *  @@root_fd_number: the socket_fd created by sock(),using this fd connection 
 *                    has been created.
 *  @@start_time_active_read: start time of  current read request.//TODO for latency
 *  @@start_time_active_write: start time of current write request//TODO for latency 
 *-----------------------------------------------------------------------------*/
typedef struct _acp_socket_fd_struct acp_socket_fd_struct;
struct _acp_socket_fd_struct{
	int               root_fd_number;
	struct timespec   start_time_active_read;
	struct timespec   start_time_active_write;
};
typedef struct _sock_struct  sock_struct;


/*-----------------------------------------------------------------------------
 *  all supporting methods to access and operate socket_struct
 *-----------------------------------------------------------------------------*/
typedef struct _sock_struct_operations sock_struct_operations;
struct _sock_struct_operations{
	bool (*is_acp_sock_fd_present)( const sock_struct*, const int);
	bool (*get_dest_port)(  sock_struct* , const int , uint32_t * );
	bool (*init_sock)( sock_struct* , int, const uint32_t  );
	bool (*register_sock)(sock_struct*, const int ,const int );
	bool  (*unregister_socket_descriptor)(sock_struct* , const int );
	bool  (*insert_start_time_of_packet)(sock_struct*, const int , const bool , const struct timespec  );
	bool (*get_start_time_of_packet)(sock_struct*, const int , const bool , struct timespec *);
	void (*clean_sock)( sock_struct*);
	bool (*copy_fd) ( sock_struct *,const int , const int);
};

/*-----------------------------------------------------------------------------
 *  structure that helps to maintain object oriented abstraction.
 *  this structure maintain all datastruct and methods for bookkeeping 
 *  socket_fd stat.
 *-----------------------------------------------------------------------------*/
struct _sock_struct{

	red_black_tree_struct     *sock_rb;
	red_black_tree_struct     *acp_socket_rb;
	sock_struct_operations    *sock_ops ;
};


/*-----------------------------------------------------------------------------
 *  declaration of all methods
 *-----------------------------------------------------------------------------*/
extern bool __is_acp_sock_fd_present__( const sock_struct*, const int);
extern	bool __get_dest_port__ ( sock_struct* , const int , uint32_t* );
extern bool __init_sock__( sock_struct* ,const int, const uint32_t  );
extern bool __register_sock__(sock_struct*, const int ,const int );
extern bool  __unregister_socket_descriptor__ (sock_struct* , const int );
extern bool  __insert_start_time_of_packet__ (sock_struct*, const int , const bool , const struct timespec  );
extern bool __get_start_time_of_packet__ (sock_struct*, const int , const bool , struct timespec *);
extern bool __copy_fd__ ( sock_struct *,const int , const int);
extern bool __setup_sock_env__( sock_struct *);
extern void __clean_sock__ ( sock_struct *);


/*-----------------------------------------------------------------------------
 *  structure variable of sock_struct_operations which holds all methods
 *  to access and maintain sock_struct data
 *-----------------------------------------------------------------------------*/
extern sock_struct_operations sock_ops;

#endif

