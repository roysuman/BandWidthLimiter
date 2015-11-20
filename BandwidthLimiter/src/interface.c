/*
 * =====================================================================================
 *
 *       Filename:  interface.c
 *
 *    Description:  all method's defination of interface.h file
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


#include"interface.h"


/*-----------------------------------------------------------------------------
 *  declaration of static methods 
 *-----------------------------------------------------------------------------*/
static struct timespec get_current_time( void );
static int do_complete_nanosleep (struct timespec sleeptime);
static long int time_difference__( struct timespec old_time , 
		struct timespec current_time );


/*-----------------------------------------------------------------------------
 *  assign the methods to struct
 *-----------------------------------------------------------------------------*/
controller_struct_operations controller_ops={
	.pre_op =__pre_op__,
	.do_update =__do_update__,
	.update_time = __update_time__,
	.func_dup_manager = __func_dup_manager__,
	.get_port_remaining_byte = __get_port_remainf_byte__,
	.regis_new_created_socket = __regis_new_created_socket__,
	.init_start_time_for_latency = __init_start_time_for_latency__
};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_env
 *  Description:  create the memory and corresponding datastruct before the operation 
 *               starts.
 *               @@cont_str: instance of global variable of type  controller_struct.
 * =====================================================================================
 */
bool 
__init_env__( controller_struct *cont_str){
	bool    return_val;
#ifdef INFO
	fprintf(stdout,"initialize the environment forresilience_bandwidth\n");
#endif
	sock_struct            *s_s;
        register_env_struct    *r_s;

	cont_str->this_flag = 0;
	s_s =(sock_struct*) safe_malloc( sizeof( sock_struct));
	if ( s_s == NULL ) {
		return_val = false;
		goto RET;
	}
	cont_str->this_flag |= MEM_SOCK_STRUCT;
	s_s->sock_ops = &sock_ops;
	cont_str->soc_str = s_s;
	r_s = (register_env_struct*)safe_malloc( sizeof ( register_env_struct ));
	if ( r_s == NULL ){
		return_val = false;
		goto RET;
	}
	cont_str->this_flag |= MEM_ENV_STR;
	r_s->env_ops = &env_ops;
	cont_str->reg_env_str = r_s;
	cont_str->controller_ops = &controller_ops;
	//setup sock environment
	int flag = atoi(getenv("RESILIENCE_CONFIGFLAG"));
	if ( flag == 1 )
		return_val = r_s->env_ops->do_register_from_config(cont_str->reg_env_str);
	else
		return_val = r_s->env_ops->do_register( cont_str->reg_env_str );
	if (return_val ) return_val =  __setup_sock_env__( cont_str->soc_str ) ;
RET:
	return return_val;
	
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  __clean_up__
 *  Description:  release all kernel resources
 * =====================================================================================
 */
void
__clean_up__( controller_struct *cont_str ){

	fprintf(stdout,"INFO: Cleaning the struct memory\n");

	//free mem
	if ( cont_str->this_flag && MEM_ENV_STR ){
	       cont_str->reg_env_str->env_ops->reg_env_clean_up(  cont_str->reg_env_str );
	       free ( cont_str->reg_env_str );
	}
	if ( cont_str->this_flag && MEM_SOCK_STRUCT ){
		cont_str->soc_str->sock_ops->clean_sock(cont_str->soc_str);
		free ( cont_str->soc_str );
	}
	return;
	

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  regis_new_created_socket
 *  Description:  call this when to register a new created spcket by sock()
 *                @@sopck_fd: socket des fd.
 *                @@port: port number
 * =====================================================================================
 */
bool 
 __regis_new_created_socket__ ( controller_struct *cont_str, 
		                 const int sock_fd , 
				 const uint32_t port ){
#ifdef DEBUG
	fprintf(stdout,"Register a created socket [ %d] with\
		       	correspond port [ %u ]\n", sock_fd,port);
#endif
	bool                  return_val;
	sock_struct           *temp_soc_str;
	register_env_struct   *temp_reg_env_str;

	temp_soc_str = cont_str->soc_str;
	temp_reg_env_str = cont_str->reg_env_str;
	//check port registered or not
	 return_val =  temp_reg_env_str->env_ops->is_port_registered
		 ( temp_reg_env_str,  port );
	 //TODO start ipc server__beter start at init
	 return_val == true ? (return_val= temp_soc_str->sock_ops->init_sock
			 ( temp_soc_str ,sock_fd , port )) : (return_val = false);
	 return return_val;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_current_time
 *  Description:  return the current system time
 *                @parameter: void
 *                @return_val CurrentTime.
 * =====================================================================================
 */
static struct timespec 
get_current_time( void ){
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  time_difference__
 *  Description:  get the time difference in nanoseconds between two time.
 *                 @Parameter: two different time variable.
 * =====================================================================================
 */
static long int
time_difference__( struct timespec old_time , 
		   struct timespec current_time ){
	long int time_diff;
	time_diff = ( ((current_time.tv_sec * NANO_MULT ) + current_time.tv_nsec) -
			( ( old_time.tv_sec  * NANO_MULT  ) + old_time.tv_nsec ) );
	return time_diff;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_port_remainf_byte
 *  Description:  get the number of remaining byte avail for the port(current window)
 *                if the remaing buyte is zero, then go to sleep for this time window
 *                and set the byte limit to allowed byte limit.
 *                @@cont_str: the struct itself.
 *                @@acp_sock_fd: the socket descriptor
 *                @@flag: read/write
 * =====================================================================================
 */
ssize_t 
__get_port_remainf_byte__(  controller_struct *cont_str,
		            int acp_sock_fd  , 
			    bool flag){
#ifdef INFO
	fprintf(stdout, "Get remaining byte count for acp_socket[%d] 	n",acp_sock_fd);
#endif
	uint32_t                port;
	long int               time_diff;
	struct timespec        prev_time;
	bool                   return_val;
	struct timespec        sleep_until;
	long int               delay_sleep;
	struct timespec        current_time;
	sock_struct            *temp_soc_str;
	register_env_struct    *temp_reg_env_str;
	ssize_t                byte_can_handle_current_window;
	ssize_t                byte_can_handle_current_window2;

	temp_soc_str = cont_str->soc_str;
	temp_reg_env_str = cont_str->reg_env_str;

	return_val = temp_soc_str->sock_ops->get_dest_port(temp_soc_str,acp_sock_fd , &port ); 
	if ( !return_val ){
		byte_can_handle_current_window = -9;
		exit(0);
		goto END;
	}
	
	if ( !flag )
		byte_can_handle_current_window = 
			temp_reg_env_str->env_ops->get_init_up_limit( temp_reg_env_str, port ) ;
	else byte_can_handle_current_window =
		temp_reg_env_str->env_ops->get_init_down_limit( temp_reg_env_str, port );
	if ( byte_can_handle_current_window != -1 ) {
		current_time = get_current_time();
		prev_time = 
			temp_reg_env_str->env_ops->get_time_val( temp_reg_env_str ,port , flag );
		time_diff = time_difference__( prev_time , current_time );
#ifdef TEST
		if ( time_diff == -1 ){
			fprintf(stdout ,"Changed took effect\n");
			exit( 1 );
		}
#endif
#ifdef DEBUG
		fprintf( stdout,"Time Diff[%ld]\n ",time_diff);
#endif
		if ( time_diff > NANO_MULT ){ // 1000000000 ){// time widow slot not present
#ifdef DEBUG
			fprintf(stdout ,"Last packet was 1sec before\n");
#endif
			temp_reg_env_str->env_ops->set_byte_count_window(
				      	temp_reg_env_str , port , byte_can_handle_current_window , flag );
		}else{
#ifdef DEBUG
			fprintf(stdout,"Get remaining byte counts for current window\n");
#endif
			byte_can_handle_current_window2 =
				temp_reg_env_str->env_ops->get_remaining_byte_count_for_current_window
				(temp_reg_env_str, port , flag );
#ifdef DEBUG
			fprintf( stdout ," Remaining Byte Count For Current Window[ %zd ]\n",
					byte_can_handle_current_window2);
#endif
			//if bytecan read=0..then sleep untill this window gets over
			if ( byte_can_handle_current_window2 <= 0 ){
				delay_sleep = NANO_MULT - time_diff;
				sleep_until.tv_sec = delay_sleep / NANO_MULT;
				sleep_until.tv_nsec = delay_sleep % NANO_MULT;
				do_complete_nanosleep( sleep_until);
				temp_reg_env_str->env_ops->set_byte_count_window
					(temp_reg_env_str, port , byte_can_handle_current_window , flag );
			}else byte_can_handle_current_window = byte_can_handle_current_window2;
		}
		}else temp_reg_env_str->env_ops->set_byte_count_window
			( temp_reg_env_str, port , 0 , flag );
END:
	return byte_can_handle_current_window;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  update_time
 *  Description:  update the time...of last access 
 *                @@cont_str: the struct itself
 *                @@acp_sock_fd: the socket fd
 *                @@flag: read/write
 * =====================================================================================
 */
bool 
__update_time__( controller_struct *cont_str, 
		const int acp_sock_fd , 
		const bool flag ){
	uint32_t                    port;
	bool                   return_val;
	sock_struct            *temp_soc_str;
	register_env_struct    *temp_reg_env_str;

	temp_soc_str = cont_str->soc_str;
	temp_reg_env_str = cont_str->reg_env_str;


	return_val = false;

	return_val = temp_soc_str->sock_ops->get_dest_port
		( temp_soc_str ,acp_sock_fd , &port ); 
	if ( return_val  ) 
		return_val = temp_reg_env_str->env_ops->update_time_val
			( temp_reg_env_str , port , flag );


	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  do_update
 *  Description: deduct the used byte for current window. Maintain the available 
 *               byte for current window 
 *               @@cont_str: this struct
 *               @@access_byte_count: byte to deduct
 *               @@acp_soc_fd: spcket fd
 *               @@flaf: read/write
 * =====================================================================================
 */
void
__do_update__( controller_struct *cont_str , 
		const ssize_t access_byte_count , 
		const int acp_soc_fd ,
		const  bool flag ){
#ifdef INFO
	fprintf(stdout,"Deduct the byte used for read/write\
		       	for current transaction\n");
#endif

	uint32_t               port;
	ssize_t                byte_count;
	sock_struct            *temp_soc_str;
	register_env_struct    *temp_reg_env_str;

	temp_soc_str = cont_str->soc_str;
	temp_reg_env_str = cont_str->reg_env_str;
	byte_count = access_byte_count;
	if ( byte_count >0 && 
			temp_soc_str->sock_ops->get_dest_port(temp_soc_str , 
				acp_soc_fd , &port ) ){

		temp_reg_env_str->env_ops->update_byte_count_current_window
			( temp_reg_env_str,  port , byte_count  , flag ) ;

	}else fprintf(stdout,"error update byte count current window[ %zd]\n",byte_count);
	return;
}

//FIXME USE ME DURING LATENCY IMPLEMENTATION
bool
__init_start_time_for_latency__( controller_struct *cont_str,  
		                 const int acp_soc_fd , 
				 const bool flag ){

	
	sock_struct           *temp_soc_str;
	register_env_struct   *temp_reg_env_str;

	temp_soc_str = cont_str->soc_str;
	temp_reg_env_str = cont_str->reg_env_str;

	return ( temp_soc_str->sock_ops->insert_start_time_of_packet 
			(temp_soc_str , acp_soc_fd , flag ,get_current_time() ) ); 

}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pre_op
 *  Description:  check whether the socket_fd that is performing read/write request
 *                needs to cap the bandwidth or not.If needed then provide the 
 *                appropriate byte to maintain the candwidth cap
 * =====================================================================================
 */
bool    
__pre_op__( controller_struct *cont_str, 
		const int acp_sock_fd  , 
		ssize_t *how_many_byte_demand , 
		const bool flag){

	ssize_t               access_byte_count;
	bool                  return_val;

	sock_struct           *temp_soc_str;
	register_env_struct   *temp_reg_env_str;

	temp_soc_str = cont_str->soc_str;
	temp_reg_env_str = cont_str->reg_env_str;

	//Check that fd registered or not
	if ( temp_soc_str->sock_ops->is_acp_sock_fd_present( temp_soc_str , acp_sock_fd )){
		//TODO update packet start time for latency calc
		access_byte_count = cont_str->controller_ops->get_port_remaining_byte( cont_str , acp_sock_fd , flag );
		if ( *how_many_byte_demand >= access_byte_count )
			*how_many_byte_demand = access_byte_count;
		return_val = true;

	} else return_val = false; //IF ACP SOCK NOT REGISTERED
	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  do_compleate_nanosleep : Source: ARJUN
 *  Description:  perform nanosleep
 * =====================================================================================
 */
 
static int 
do_complete_nanosleep (struct timespec sleeptime){

	struct timespec treq, tret;
	int retval = 0;
	treq = sleeptime;
	do{
		retval = nanosleep (&treq, &tret);
		if (retval == -1){
			switch (errno){
				case EINTR:
#ifdef DEBUG
					error (0, errno, "Request: [%d %ld], Remain: [%d %ld]\n", 
							(int) treq.tv_sec, (long int) treq.tv_nsec, 
							(int) tret.tv_sec, (long int) tret.tv_nsec);
#endif
					break;
				case EFAULT:
#ifdef DEBUG
					error (0, errno, "Request: [%d %ld], Remain: [%d %ld]\n", 
							(int) treq.tv_sec, (long int) treq.tv_nsec, 
							(int) tret.tv_sec, (long int) tret.tv_nsec);
#endif
					goto DO_COMPLETE_NANOSLEEP_OUT;
					break;
				case EINVAL:
#ifdef DEBUG
					error (0, errno, "Request: [%d %ld], Remain: [%d %ld]\n", 
							(int) treq.tv_sec, (long int) treq.tv_nsec, 
							(int) tret.tv_sec, (long int) tret.tv_nsec);
#endif
					goto DO_COMPLETE_NANOSLEEP_OUT;
					break;
			}
		}
		treq = tret;
	} while (retval == -1);
	DO_COMPLETE_NANOSLEEP_OUT:
	return retval;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  func_dup_manager
 *  Description:  dup helper. coppy the old fd info into new fd.
 *                
 * =====================================================================================
 */
void
__func_dup_manager__( controller_struct *cont_str, 
		      const int old_fd , 
		      const int new_fd ){

	sock_struct           *temp_soc_str;
	temp_soc_str = cont_str->soc_str;

	if ( !temp_soc_str->sock_ops->copy_fd( temp_soc_str ,  old_fd , new_fd ) ){
		fprintf( stderr, "Dup error on coppy old_fd[%d] structto new fd\
			       	[ %d] struct\n",old_fd,new_fd);
	}
	
	return ;
}


