#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include<stdint.h>
#define MAXLEN 1024
#define CONFIG_FILE "BandwidthLimiter/limiter.config"
#include "list.h"
#include"safe_malloc.h"
#define SIZE 4
//#define DEBUG 1
typedef struct port_str_ port_str;
struct port_str_{
	uint32_t    port_no;
	ssize_t     up_limit;
	ssize_t     down_limit;
	uint32_t    latency;
	list_head   port_link;
};
typedef struct config_struct_ config_struct;
typedef struct config_struct_operations_ config_struct_operations;
struct config_struct_{
	size_t                      no_port;
	char                        *libc_path;
	char                        *server_ip;
	uint32_t                    server_port;  
	list_head                   port_list_head;
	config_struct_operations    *config_ops;
};

struct config_struct_operations_{
	int(*parse_config)(config_struct*);
	void(*do_clean)(list_head*);
};
extern int __parse_config__(config_struct *);
extern config_struct_operations config_operations;
extern void __do_cleanup_list__(list_head *);
#endif
