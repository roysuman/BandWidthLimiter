/*
 * =====================================================================================
 *
 *       Filename:  registerEnv.h
 *
 *    Description:  Get the port number, bandwodth limit for filtering. And 
 *                  store them into appro DS for better lookup time
 *        Version:  1.0
 *        Created:  Saturday 25 April 2015 12:17:13  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), suman.roy@cognizant.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#ifndef REGISTERENV_H__
#define REGISTERENV_H__  
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<stdlib.h>
#include <unistd.h>
#include<time.h>
#include<stdint.h>
#include <stdbool.h>

#include "bandwidth_globals.h"
#include "red_black_tree.h"
#include "parse_config.h"
/*-----------------------------------------------------------------------------
 *  structure for maintaing the upload limit and downloadlimit of a Port
 *  -----------------------------------------------------------------------------*/
typedef struct _port_struct port_struct;
struct _port_struct{
	ssize_t                  up_limit;
	uint32_t                 bind_port;
	char*                    ip_address;
	ssize_t                  down_limit;
	struct timespec          start_time_read;
	struct timespec          start_time_write;
	ssize_t                  remaining_read_byte_for_current_window;
	ssize_t                  remaining_write_byte_for_current_window;
	uint32_t                 latency;
};
typedef struct _register_env_struct register_env_struct;


/*-----------------------------------------------------------------------------
 *  structure to hold all methods needed to operate on struct register_env_struct
 *-----------------------------------------------------------------------------*/
typedef struct _register_env_struct_operations register_env_struct_operations;
struct _register_env_struct_operations{
	bool (*do_register)( register_env_struct* );
	ssize_t (*get_init_up_limit) ( const register_env_struct*, const uint32_t ); // todo--ip
	ssize_t (*get_init_down_limit)( const register_env_struct* ,  const uint32_t  ); // todo--ip
	struct timespec(*get_time_val)( const register_env_struct* , const uint32_t , const bool );
	bool (*update_time_val)( const register_env_struct* , const uint32_t , const bool);
	bool (*set_byte_count_window )( const register_env_struct* ,const uint32_t ,const ssize_t ,const bool );
	ssize_t(*get_remaining_byte_count_for_current_window)( const register_env_struct* , const uint32_t , const bool);
	bool(*update_byte_count_current_window)( const register_env_struct* ,const uint32_t , const ssize_t  ,const bool);
	bool (*is_port_registered)( const register_env_struct* , const uint32_t ); 
	void (*reg_env_clean_up)( register_env_struct *);
	bool (*do_register_from_config)( register_env_struct *);
};

/*-----------------------------------------------------------------------------
 *  struct that acts like a class. All DS for init the env
 *-----------------------------------------------------------------------------*/
struct _register_env_struct{
	red_black_tree_struct             *port_rb;
	register_env_struct_operations    *env_ops;


};


/*-----------------------------------------------------------------------------
 *  declaration of all method
 *-----------------------------------------------------------------------------*/


extern bool __do_register__( register_env_struct* );


extern ssize_t __get_init_up_limit__(const register_env_struct* , const uint32_t );
extern ssize_t __get_init_down_limit__ (const register_env_struct* ,  const uint32_t );
extern struct timespec  __get_time_val__( const register_env_struct*  , const uint32_t , const bool );
extern bool __update_time_val__(const register_env_struct*  ,const uint32_t  , const bool );
extern ssize_t __get_remaining_byte_count_for_current_window__ (const register_env_struct* , const uint32_t , const bool );
extern bool __update_byte_count_current_window__( const register_env_struct* , const uint32_t  , const ssize_t , const bool );
extern bool __set_byte_count_window__( const register_env_struct* ,const uint32_t , const ssize_t  , const bool );
extern bool __is_port_registered__(const register_env_struct* , const uint32_t  );
extern void __reg_clean_up__( register_env_struct *);

extern bool __setup_env_( register_env_struct*);
extern bool __do_register_from_config__( register_env_struct *);


/*-----------------------------------------------------------------------------
 *  variable to hole all methods
 *-----------------------------------------------------------------------------*/
extern register_env_struct_operations env_ops;
#endif
