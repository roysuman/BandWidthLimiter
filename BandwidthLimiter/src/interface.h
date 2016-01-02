
#ifndef INTERFACE_H_
#define INTERFACE_H_

#include<stdio.h>
#include <time.h>
#include <sys/time.h>
#include "fd.h"
#include "register_env.h"
#include "fd.h"
#define NANO_MULT  1000000000L

typedef struct _controller_struct controller_struct;


/*-----------------------------------------------------------------------------
 *  structure to hold all methods that work;s on controller_struct
 *-----------------------------------------------------------------------------*/
typedef struct _controller_struct_operations controller_struct_operations;
struct _controller_struct_operations{
	bool (*regis_new_created_socket)(controller_struct* ,const  int ,const  uint32_t  );
	ssize_t (*get_port_remaining_byte)(controller_struct* , int  ,  bool);
	bool (*update_time)(controller_struct* ,  int , bool );
	void (*do_update)(controller_struct* , const  ssize_t  ,const  int , const bool );
	bool (*init_start_time_for_latency)(controller_struct* , const int  , const bool  );
	bool (*pre_op)(controller_struct* ,const int  , ssize_t* , const bool );
	void (*func_dup_manager)( controller_struct* , const int , const int );

};


/*-----------------------------------------------------------------------------
 *  declaration of all method. see .c file for sefination
 *-----------------------------------------------------------------------------*/
extern bool __init_env__( controller_struct* );
extern void __clean_up__( controller_struct *);

extern bool __regis_new_created_socket__ ( controller_struct* ,  const int , const uint32_t  );
extern ssize_t __get_port_remainf_byte__(controller_struct* , int  ,  bool);
extern bool __update_time__(controller_struct* ,  int , bool );
extern void __do_update__(controller_struct* , const  ssize_t  ,const  int, const bool );
extern bool __init_start_time_for_latency__(controller_struct* , const int  , const bool  );
extern bool __pre_op__(controller_struct* ,const int ,  ssize_t* ,const bool  );
extern void __func_dup_manager__( controller_struct* , const int , const int );

/*-----------------------------------------------------------------------------
 *  striucture for holding the instance of othe rinternal styructure.
 *  It provides OOP conceps in ANSI C
 *-----------------------------------------------------------------------------*/
struct _controller_struct{
	unsigned int                   this_flag;//flag 
	sock_struct                    *soc_str;//socket bookeeoing instance
	register_env_struct            *reg_env_str;//environment init instance
	controller_struct_operations   *controller_ops;//method struct for this struct
};

/*-----------------------------------------------------------------------------
 *  operation struct variable
 *-----------------------------------------------------------------------------*/
extern  controller_struct_operations controler_ops;
#endif

