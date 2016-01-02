#include "fd.h"

/*-----------------------------------------------------------------------------
 *  assignment --- all methods to struct 
 *-----------------------------------------------------------------------------*/
sock_struct_operations sock_ops ={
	.copy_fd = __copy_fd__,
	.init_sock = __init_sock__,
	.clean_sock = __clean_sock__,
	.get_dest_port = __get_dest_port__,
	.register_sock = __register_sock__,
	.is_acp_sock_fd_present = __is_acp_sock_fd_present__,
	.get_start_time_of_packet = __get_start_time_of_packet__,
	.insert_start_time_of_packet = __insert_start_time_of_packet__,
	.unregister_socket_descriptor = __unregister_socket_descriptor__
};



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_sock
 *  Description:  register the socket created by glibc socket() call
 *                a red black tree has been used (O(log n ) ) to store all socket_fd
 *                information for better lookup time.
 *                @@so_s: the structure variable itself
 *                @@sock_des: the socket_fd created by glibc->sock()
 *                @@port: port number on which the socket is bind
 * =====================================================================================
 */
bool
__init_sock__ ( sock_struct *so_s, 
		const int sock_des , 
		const uint32_t port  ){

	int                 *sock_;
	socket_fd_struct    *sock_str;
	bool                return_val;
	int                 temp_sock_des;
#ifdef INFO
	fprintf(stdout,"register root socket [ %d] on  Port [ %u ] \n",sock_des, port);

#endif
	//check whether the sock_des is already registered or not
	//it should not be registered else there is a functional
	//prob at code
	return_val = true;

	temp_sock_des = sock_des;
	red_black_node_struct* rb_node;
	rb_node = rb_exact_query( so_s->sock_rb , &temp_sock_des );

	if ( rb_node == NULL ){// socket not registered
#ifdef DEBUG
		fprintf(stdout,"Socket [ %d ] not registered\n",sock_des);
#endif

		sock_str = ( socket_fd_struct*) safe_malloc( sizeof(socket_fd_struct ) ); 
		sock_ =(int*) safe_malloc( sizeof ( int ) );
		*sock_ =(int)sock_des;
//		*sock_ = 3;
		if ( sock_str != NULL ){
			sock_str->ref_count = ZERO;
			sock_str->port = port;
			rb_tree_insert( so_s->sock_rb , sock_, sock_str ); 
#ifdef DEBUG
			rb_tree_print(so_s->sock_rb);
#endif

		}else{
			fprintf(stderr,"malloc returns NULL\n");
			return_val = false;
		}
	}else {
		fprintf( stderr , "Socket already registered\n");
		return_val = false;
	}
	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  register_sock
 *  Description:  register the socket_fd which maintain the communication.
 *                socket_fd that is returned during bind and connect glibc
 *                method is called listen_sock_fd. 
 *                @@so_s: the structure variable itself/
 *                @@org_socket_fd: socket that created by socket call.
 *                @@listen_sock_fd_: socket_fd returned by bind || connect glibc method
 * =====================================================================================
 */
bool
__register_sock__( sock_struct *so_s, 
		const int org_socket_fd ,
		const int listen_sock_fd_  ){

	socket_fd_struct        *soc_str;
	int                     temp_sock;
	bool                    return_val;
	acp_socket_fd_struct    *acp_soc_str;
	int                     listen_sock_fd;


#ifdef INFO
	fprintf( stdout , "Register accepted_socket_fd[ %d] with respect \
			to org_fd [%d]\n",listen_sock_fd_, org_socket_fd ) ;
#endif
	return_val = true;
	listen_sock_fd = listen_sock_fd_;
	
	//search both red black trees for the corresponding fd
	red_black_node_struct* rb_node_org , *rb_node_acp;
	temp_sock = listen_sock_fd;
	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
	temp_sock = org_socket_fd;
	rb_node_org = rb_exact_query( so_s->sock_rb , &temp_sock );
#ifdef DEBUG
	rb_tree_print(so_s->sock_rb);
#endif

	if ( (rb_node_org != NULL ) && (rb_node_acp ==  NULL )){
		//insert the listen_socket_fd into the cprresponding red black tree
		//ie. acp_socket_rb
		acp_soc_str = ( acp_socket_fd_struct *)
			safe_malloc ( sizeof ( acp_socket_fd_struct ) );

		if ( acp_soc_str != NULL ){ //memory allocated
			acp_soc_str->root_fd_number = temp_sock;
			//insert
			int *sock =(int*) safe_malloc( sizeof ( int ));
			*sock = (int)listen_sock_fd;
			if ( rb_tree_insert( so_s->acp_socket_rb , 
						sock, acp_soc_str ) != NULL ){//insert success
#ifdef TEST
				rb_tree_print(so_s->acp_socket_rb);
#endif
				soc_str = ( socket_fd_struct *)rb_node_org->info;
				soc_str->ref_count ++;
			}else{
				fprintf(stderr,"Error on inserting in red \
						black tree of accepted socket_fd\n");
				return_val = false;
			}
		}else return_val = false;
	}else {
#ifdef DEBUG
		fprintf(stdout,"Error: either org_socket not registered or\
			       	acp_socket descriptor already registered\n");
#endif
		return_val = false;
	}
	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name: isPresent
 *  Description:  check the socket descriptor is already registered or not
 *                @@so_s: the structure var itself
 *                @@acp_sock_fd:the accepted socket fd
 * =====================================================================================
 */

bool 
__is_acp_sock_fd_present__( const sock_struct *so_s, 
		            const int acp_sock_fd ){

#ifdef INFO
	fprintf(stdout , "Check whether acp_socket_fd [ %d] present into\
		       	the redblack tree or not\n",acp_sock_fd);
#endif
	red_black_node_struct    *rb_node_acp;
	bool                     return_val;
	int                      temp_sock;

	temp_sock = acp_sock_fd;

	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
	if ( rb_node_acp == NULL ){
		return_val = false;
	}else{
		return_val = true;
	}
	return return_val;
}
bool
__copy_fd__(  sock_struct *so_s , 
		const int old_fd , 
		const int new_fd ){
#ifdef DEBUG
	fprintf(stdout,"CP sock fd old_fd [ %d ] ->new_fd\
		       	[ %d ] \n", old_fd , new_fd);
#endif
	red_black_node_struct    *rb_node_acp;
	bool                     return_val;
	int                      temp_sock;
	acp_socket_fd_struct     *acp_soc_str, *old_soc_str;
	temp_sock = old_fd;

	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
	if ( rb_node_acp == NULL ){

		return_val = false;
	}else{
		old_soc_str = (  acp_socket_fd_struct*)rb_node_acp->info;
		acp_soc_str = ( acp_socket_fd_struct *)
			safe_malloc ( sizeof ( acp_socket_fd_struct ) );
		if ( acp_soc_str != NULL ){ //memory allocated
			memcpy( acp_soc_str , old_soc_str , 
					sizeof ( acp_socket_fd_struct ) );
			//TODO increase the root fd ref count
			
			//insert
			int *sock =(int*) safe_malloc( sizeof ( int ));
			*sock = (int)new_fd;
			if ( rb_tree_insert( so_s->acp_socket_rb , sock, 
						acp_soc_str ) != NULL ){//insert success
				return_val = true;
			}else{
				return_val = false;
				free ( acp_soc_str);
				free ((int*)sock);

			}
		}else{
			return_val = false;
		}
	}
	return return_val;

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
__get_dest_port__ ( sock_struct *so_s, 
		    const int acp_sock_fd , 
		    uint32_t *port ){
#ifdef DEBUG
	fprintf(stdout ,"Get the port bind with the accepted_socket_fd");
#endif
	acp_socket_fd_struct   *acp_soc_str;
	socket_fd_struct       *soc_str;
	red_black_node_struct  *rb_node_org , *rb_node_acp;
	int                    temp_sock;
	bool                   return_val;


	temp_sock = acp_sock_fd;
	return_val = true;

	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
#ifdef DEBUG
	rb_tree_print(so_s->acp_socket_rb);	
#endif
	if ( rb_node_acp == NULL ){
		fprintf(stderr, "acp_socket_fd not present in\
			       	the red black tree\n");

		return_val = false;
	}else{
		acp_soc_str = (acp_socket_fd_struct*)rb_node_acp->info;
		temp_sock = acp_soc_str->root_fd_number;
		rb_node_org = rb_exact_query( so_s->sock_rb , &temp_sock );
		if ( rb_node_acp == NULL ){
			fprintf(stderr, "org_socket_fd not present in\
				       	the red black tree\n");
			return_val = false;
		}else{
			soc_str = (socket_fd_struct *) rb_node_org->info;
			*port = soc_str->port;
		}
	}
	return return_val;
}



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  unregisterFileDescriptor
 *  Description:  unregister a registered FD when fd is geting closed
 *                @@so_s : the struct itself
 *                @@acp_sock_fd:: the socket fd.
 *                remove the acp_sock_fd from the red black tree and increment the 
 *                ref count of parent sock_fd by -1( sock_fd returned by sock() );
 * =====================================================================================
 */
bool
__unregister_socket_descriptor__( sock_struct *so_s,
		                  const int acp_sock_fd ){

#ifdef INFO
	fprintf(stdout,"close the socket_fd %d\n",acp_sock_fd);
#endif

	socket_fd_struct         *soc_str;
	int                      temp_sock;
	bool                     return_val;
	acp_socket_fd_struct     *acp_soc_str;
	red_black_node_struct    *rb_node_org;
	red_black_node_struct    *rb_node_acp;


	temp_sock = acp_sock_fd;
	return_val = true;

	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
	if ( rb_node_acp == NULL ){
		fprintf(stderr, "acp_socket_fd not present in\
			       	the red black tree\n");
		return_val = true;//there is now error, this sock is not registered that's why operation not success.
	}else{
		acp_soc_str = (acp_socket_fd_struct*)rb_node_acp->info;
		temp_sock = acp_soc_str->root_fd_number;
		rb_node_org = rb_exact_query(so_s->sock_rb , &temp_sock); 
		if ( rb_node_acp == NULL ){
			fprintf(stderr, "org_socket_fd not present in\
				       	the red black tree\n");
			return_val = false;
		}else{
			soc_str = (socket_fd_struct *) rb_node_org->info;
			soc_str->ref_count --;
			if ( soc_str->ref_count <= 0){
				rb_delete( so_s->sock_rb ,rb_node_org );  
			}
			rb_delete(so_s->acp_socket_rb , rb_node_acp );

		}
	}
	return return_val;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  insert_start_timee_of_packet
 *  Description:  insert the start time of current read/write request
 *               @@so_s : this sructure
 *               @@acp_socket_fd: the accepted socket fd
 *               @@flag: flag for read/write
 *               @@start_time: time itself
 * =====================================================================================
 */
bool
__insert_start_time_of_packet__( sock_struct *so_s, 
		                 const int acp_socket_fd , 
				 const bool flag , 
				 const struct timespec start_time ){
#ifdef INFO
	fprintf(stdout ,"Insert start time of packet for\
		       	acp_socket_fd [ %d ]",acp_socket_fd );
#endif
	int                      temp_sock;
	bool                     return_val;
	acp_socket_fd_struct     *acp_soc_str;
	red_black_node_struct    *rb_node_acp;

	temp_sock = acp_socket_fd;
	return_val = true;

	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
	if ( rb_node_acp == NULL ){
		fprintf(stderr, "acp_socket_fd not present in\
			       	the red black tree\n");
		return_val = false;
	}else{
		acp_soc_str = (acp_socket_fd_struct*)rb_node_acp->info;
		if ( flag ){
			acp_soc_str->start_time_active_read = start_time;
		}else{
			acp_soc_str->start_time_active_write = start_time;
		}
	}
	return return_val;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_start_timee_of_packet
 *  Description:  return the start time of the current packet.(ended transaction)
 *                @@so_s: the struct itself
 *                @@acp_socket_fd:: the socket fd
 *                @@flag: read/write flag
 *                @@*start_time: pointer reference to hold the start time.
 * =====================================================================================
 */
bool
__get_start_time_of_packet__( sock_struct *so_s,
		              const int acp_socket_fd ,
			      const bool flag , 
			      struct timespec *start_time){

	int                      temp_sock;
	bool                     return_val;
	acp_socket_fd_struct     *acp_soc_str;
	red_black_node_struct    *rb_node_acp;

	temp_sock = acp_socket_fd;
	return_val = true;

	rb_node_acp = rb_exact_query( so_s->acp_socket_rb , &temp_sock );
	if ( rb_node_acp == NULL ){
		fprintf(stderr, "acp_socket_fd not present in the red black tree\n");
		return_val = false;
	}else{
		acp_soc_str = (acp_socket_fd_struct*)rb_node_acp->info;
		if ( flag ){
			*start_time = acp_soc_str->start_time_active_read;
		}else{
			*start_time =acp_soc_str->start_time_active_write;
		}
	}
	return return_val;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setup_sock_env
 *  Description:  create the rb tree for socket bookkeeping
 *               @@so_s: the struct itself
 * =====================================================================================
 */
bool 
__setup_sock_env__( sock_struct *so_s){
	
	bool    return_val;
	
	if (( (so_s->sock_rb =rb_tree_create(int_comp, int_dest,info_dest,int_print,info_print) 
					)!= NULL ) && 
			((so_s->acp_socket_rb=rb_tree_create(int_comp, int_dest,
							     info_dest,int_print,info_print)
			  )  != NULL )){
		return_val = true;
		;
	}else return_val = false;
	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  clean_sock
 *  Description:  destroy the redblack tree(FREE_MEM)
 *                @@so_s: the socket itself
 * =====================================================================================
 */
void
__clean_sock__( sock_struct *so_s ){
	fprintf(stdout,"INFO: cleaning buffer and red black tree\n");
	if ( so_s->acp_socket_rb != NULL ){
		rb_tree_destroy(so_s->acp_socket_rb);
	}
	if ( so_s->sock_rb != NULL )
		rb_tree_destroy(so_s->sock_rb);
	return;
}
