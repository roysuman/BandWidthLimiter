/*
 * =====================================================================================
 *
 *       Filename:  registerEnv.c
 *
 *    Description:  do refister
 *
 *        Version:  1.0
 *        Created:  Saturday 25 April 2015 12:19:57  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), suman.roy@cognizant.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include"register_env.h"
#include "bandwidth_globals.h"


/*-----------------------------------------------------------------------------
 *  assign all methods
 *-----------------------------------------------------------------------------*/
register_env_struct_operations env_ops = {
	.do_register = __do_register__,
	.get_time_val = __get_time_val__,
	.reg_env_clean_up = __reg_clean_up__,
	.update_time_val = __update_time_val__,
	.get_init_up_limit = __get_init_up_limit__,
	.is_port_registered = __is_port_registered__,
	.get_init_down_limit = __get_init_down_limit__,
	.set_byte_count_window = __set_byte_count_window__,
	.do_register_from_config = __do_register_from_config__,
	.update_byte_count_current_window = __update_byte_count_current_window__,
	.get_remaining_byte_count_for_current_window = __get_remaining_byte_count_for_current_window__

};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  do_register_from_config
 *  Description:  read the config file and init the environment
 * =====================================================================================
 */
bool
__do_register_from_config__( register_env_struct *env){
	bool    return_val;
	config_struct c_str;
	list_head *config_port_itr;
	port_str      *p_str;//config file port struct
	port_struct   *port_data;//redblack tree lookupp ort struct
	
	c_str.config_ops =&config_operations ;
	return_val = ( bool )c_str.config_ops->parse_config(&c_str);
	if ( return_val ){
		env->port_rb =  rb_tree_create(uint32_comp, uint32_dest,
				info_dest,uint32_print,info_print);
		if ( (env->port_rb != NULL) ){
			list_for_each(config_port_itr,&c_str.port_list_head){
				p_str = get_member_struct( config_port_itr,port_str,port_link);
				fprintf(stdout,"PORT NO[ %u ] \n",p_str->port_no);
				port_data =(port_struct*) 
					safe_malloc ( sizeof ( port_struct) );
				if ( port_data == NULL ) return_val = false;
				else{
					port_data->up_limit = p_str->up_limit;
					port_data->down_limit = p_str->down_limit;
					port_data->bind_port = p_str->port_no;
					port_data->latency = p_str->latency;
					if ( port_data->up_limit > 0 ) 
						port_data->up_limit *=( 1000  );
					if ( port_data->down_limit > 0 )
						port_data->down_limit *=( 1000  );
					port_data->remaining_read_byte_for_current_window =port_data->down_limit;
					port_data->remaining_write_byte_for_current_window =port_data->up_limit ;
					port_data->start_time_read.tv_sec = 0;
					port_data->start_time_write.tv_sec = 0;
					port_data->start_time_read.tv_nsec = 0;
					port_data->start_time_write.tv_nsec = 0;
					uint32_t *temp_port =
						(uint32_t*) safe_malloc( sizeof ( uint32_t ) );
					temp_port = &(port_data->bind_port);
					if ( rb_tree_insert( env->port_rb , temp_port , port_data ) == NULL ) {
						return_val = false;
						free (temp_port);
						free(port_data);
					}else return_val = true;
				}
			}
			fprintf(stdout,"Glibc Pth %s\n",c_str.libc_path);
			setenv("RESILIENCE_GLIBC_PATH",c_str.libc_path,1);
			//delete all memory 
			c_str.config_ops->do_clean(&c_str.port_list_head);


		}else{
			fprintf(stderr,"error on rb tree creation\n");
			return_val = false;
		}
	}else{
		fprintf(stderr,"error on parsing the config file]n");
	}
	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  do_register
 *  Description:  register the port and limits for filtering
 * =====================================================================================
 */
bool 
__do_register__( register_env_struct *env ){

#ifdef DEBUG
	fprintf(stdout , "Register filter port into RB_TREE");
#endif

	char*    port;
	char*    latency;
	char*    up_limit;
	char*    down_limit;
	bool           return_val;
	port_struct   *port_data;
	char ip_[]="TODO";
	const char*  ip = ip_;
	up_limit   = getenv( "RESILIENCE_UP_LIMITS" ) ;
	down_limit = getenv( "RESILIENCE_DOWN_LIMITS");
	ip        = getenv( "RESILIENCE_IPS");
	port      = getenv ("RESILIENCE_PORTS" );
	latency    = getenv ("RESILIENCE_LATENCY" );
#ifdef DEBUG
	fprintf( stdout , "parameter going to register\n");
	fprintf(stdout , "Ip Address [ %s ] ",ip);
	fprintf(stdout , "Port [ %s ] ",port);
	fprintf(stdout , "UploadLimit [ %s ] ",up_limit);
	fprintf(stdout,"DownloadLimits [ %s ] ",down_limit);
#endif
	//FIXME if no port is registered then port contains -1, then return some -ve value to indicate.i
	int p_temp = atoi(port);
	if ( p_temp == -1 ){
#ifdef INFO
		fprintf(stdout,"No port for filtering\n");
#endif
		return_val = true;
		return return_val;

	}

	//check whether the port already registered or not
	//check the red black tree
	env->port_rb =  rb_tree_create(uint32_comp, uint32_dest,
			info_dest,uint32_print,info_print);

	if ( (env->port_rb != NULL ) && 
			(port_data =(port_struct*) 
			 safe_malloc ( sizeof ( port_struct) ) )!=0){

		port_data->up_limit = atoi( up_limit );
		port_data->down_limit = atoi ( down_limit );
		port_data->bind_port = atoi ( port ); 
		port_data->latency = atoi(latency);
		if ( port_data->up_limit > 0 ) 
			port_data->up_limit *=( 1000  );
		if ( port_data->down_limit > 0 )
			port_data->down_limit *=( 1000  );
		port_data->remaining_read_byte_for_current_window =
			port_data->down_limit;
		port_data->remaining_write_byte_for_current_window =
			port_data->up_limit ;//* 1000;// * 1024;
		port_data->start_time_read.tv_sec = 0;
		port_data->start_time_write.tv_sec = 0;
		port_data->start_time_read.tv_nsec = 0;
		port_data->start_time_write.tv_nsec = 0;
		uint32_t *temp_port =
			(uint32_t*) safe_malloc( sizeof ( uint32_t ) );
		temp_port = &(port_data->bind_port);
		if ( rb_tree_insert( env->port_rb , temp_port , port_data ) == NULL ) {
			return_val = false;
			free (temp_port);
			free(port_data);
		}else return_val = true;
	} else{
	       	return_val = false;

	}
	return return_val;
	
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInitUpLimit
 *  Description:  return the Upload bandwidth limit maped with the port
 * =====================================================================================
 */
ssize_t 
__get_init_up_limit__( const register_env_struct *env, 
		        const uint32_t port ){
#ifdef DEBUG
	fprintf(stdout," get up limit of Port [ %u ] \n",port);
#endif
	long unsigned int   return_val;
	port_struct         *p_data;
	uint32_t            pp;

	pp = port;
 

	red_black_node_struct* rb_node;

	rb_node = rb_exact_query( env->port_rb , &pp ); 

	if ( rb_node == 0 ){
#ifdef DEBUG
		fprintf( stderr , "Port [ %u ] Not registered\n",port);
#endif
		return_val = -1;
	}else{
		p_data = ( port_struct*)rb_node->info;
		return_val = p_data->up_limit;
	}
	
	return return_val;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInitDownlimit
 *  Description:  return the Download Limit maped with the corresponding ip_address
 * =====================================================================================
 */
ssize_t
__get_init_down_limit__ ( const register_env_struct *env,  
		          const uint32_t port ){
#ifdef DEBUG
	fprintf(stdout," get down limit of Port [ %u ] \n",port);
#endif
	long unsigned int         return_val;
	red_black_node_struct*    rb_node;
	uint32_t                  pp;// = port;
	port_struct               *p_data;

	pp = port;

	rb_node = rb_exact_query( env->port_rb , &pp ); 

	if ( rb_node == 0 ){
#ifdef DEBUG
		fprintf( stderr , "Port [ %u ] Not registered\n",port);
#endif
		return_val = -1;
	}else{
		p_data = ( port_struct*)rb_node->info;
		return_val = p_data->down_limit;
	}
	return return_val;
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
struct timespec 
__get_time_val__( const register_env_struct  *env, 
		  const uint32_t port, 
		  const bool flag  ){
#ifdef DEBUG
	fprintf(stdout," get last acccess time of Port [ %u ] \n",port);
#endif

	struct timespec          timer_clock;
	red_black_node_struct*   rb_node;
	port_struct*             p_data;
	uint32_t                 pp;// = port;

	pp = port;

	rb_node = rb_exact_query( env->port_rb , &pp ); 
	if ( rb_node == 0 ){
#ifdef DEBUG
		fprintf( stderr , "Port [ %u ] Not registered\n",port);
#endif
		timer_clock.tv_sec = 9999;
		timer_clock.tv_nsec = 9999;
	}else{
		p_data = ( port_struct*)rb_node->info;
		if (flag ){
			timer_clock = p_data->start_time_read;
		}else{
			timer_clock= p_data->start_time_write;
		}
	}
	return timer_clock;
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
__update_time_val__(const register_env_struct *env ,
		    const uint32_t port ,  
		    const bool flag  ){

#ifdef DEBUG
	fprintf(stdout," update last access time of port [ %u ] \n",port);
#endif
	struct timespec          ts;
	bool                     return_val;
	red_black_node_struct*   rb_node;
	port_struct*             p_data;
	uint32_t                 pp;// = port;

	pp = port;

	return_val= true;

	rb_node = rb_exact_query( env->port_rb , &pp ); 
	if ( rb_node == 0 ){
		return_val = false;
	} else if ( clock_gettime(CLOCK_REALTIME, &ts) != -1 ){
		p_data = ( port_struct*)rb_node->info;
		flag == true ? (p_data->start_time_read = ts):
			(p_data->start_time_write = ts);
	}else{
		fprintf(stderr ,"Not able to get the current sys time--EXITING");
		return_val = false;
	}
	return return_val;
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
__get_remaining_byte_count_for_current_window__ (const register_env_struct *env,
	       	                                 const uint32_t port , 
						 const bool flag ){

#ifdef DEBUG
	fprintf(stdout," get remaining byte count for current window\
		       	port [ %u ] \n",port);
#endif
	ssize_t                  remaing_count;
	red_black_node_struct*   rb_node;
	port_struct*             p_data;
	uint32_t                 pp;// = port;

	pp = port;
	rb_node = rb_exact_query( env->port_rb , &pp ); 
	if ( rb_node == 0 ){
		fprintf(stderr ," Port  [ %u ] Not registered--hCondition \
				should not happen--EXIT",port);
		remaing_count = 0;
	} else{
		p_data = ( port_struct*)rb_node->info;
		flag == true?remaing_count = 
			p_data->remaining_read_byte_for_current_window:
			(remaing_count=p_data->remaining_write_byte_for_current_window);
	}
	return remaing_count;
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
__update_byte_count_current_window__( const register_env_struct *env, 
		                      const uint32_t port , 
				      const ssize_t used_bytes , 
				      const bool flag ){
#ifdef DEBUG
	fprintf(stdout," update byte count of port [ %u ]  and used byte\
		       	is [ %zu ]\n",port , used_bytes);
#endif
	bool                     return_val;
	red_black_node_struct*   rb_node;
	port_struct*             p_data;
	uint32_t                 pp;

	pp = port;
	return_val = true;


	rb_node = rb_exact_query( env->port_rb , &pp ); 
	if ( rb_node == 0 ){
		return_val = false;
		fprintf( stderr , "port [ %u ] not registered\n", port );
	} else{
		p_data = ( port_struct*)rb_node->info;

		flag == true? p_data->remaining_read_byte_for_current_window -= used_bytes :
			(p_data->remaining_write_byte_for_current_window-=used_bytes);
		if ( p_data->remaining_read_byte_for_current_window < 0 ) 
			p_data->remaining_read_byte_for_current_window = 0;
		if ( p_data->remaining_write_byte_for_current_window < 0 )
			p_data->remaining_write_byte_for_current_window = 0;
	}
	return return_val;
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
__set_byte_count_window__( const register_env_struct *env , 
		           const uint32_t port  , 
			   const ssize_t  size , const bool flag ){

#ifdef DEBUG
	fprintf( stdout , "set byte count port [ %u ] Bytes\
		       	[ %zu] \n",port , size );
#endif
	bool                      return_val;
	red_black_node_struct*    rb_node;
	port_struct*              p_data;
	uint32_t                  pp;

	pp = port;

	return_val= true;

	rb_node = rb_exact_query( env->port_rb , &pp ); 

	if ( rb_node == 0 ){
		fprintf(stdout ,"port not registered\n");

		return_val = false;
	} else{
		p_data = ( port_struct*)rb_node->info;
		flag == true? (p_data->remaining_read_byte_for_current_window= size):
			(p_data->remaining_write_byte_for_current_window = size);
	}
	return return_val;
}

bool
__is_port_registered__( const register_env_struct *env , 
		        const uint32_t  port) {
#ifdef DEBUG
	fprintf( stdout , "is port [ %u ] registered ? ",port );
#endif
	bool                     return_val;
	red_black_node_struct*   rb_node;
	uint32_t                 pp;// = port;

	pp = port;

	return_val= true;

	rb_node = rb_exact_query( env->port_rb , &pp ); 
	if ( rb_node == NULL ){
		return_val = false;
#ifdef DEBUG
		fprintf( stderr , "Port [ %u ] not registered ",port);
#endif
	}else{
		return_val = true;
		fprintf(stdout,"Port [ %u ] registerd \n",pp);

	}
	return return_val;
}
void
__reg_clean_up__( register_env_struct *env){
	fprintf(stdout,"INFO: cleaning reg_env TODO: RESOLVE MEM LK\n");

	if ( env->port_rb != NULL ){
		rb_tree_destroy(env->port_rb);
	}
	return;
}



