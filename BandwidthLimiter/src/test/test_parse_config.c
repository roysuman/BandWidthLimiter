#include <stdlib.h>
#include<stdio.h>
#include "parse_config.h"
#include"list.h"
int main(){
	config_struct c_str;
	
	c_str.config_ops =&config_str ;
	c_str.config_ops->parse_config(&c_str);
	list_head *config_port_itr;
	port_str *p_str;
	list_for_each(config_port_itr,&c_str.port_list_head){
		p_str = get_member_struct( config_port_itr,port_str,port_link);
		fprintf(stdout,"PORT NO[ %u ] \n",p_str->port_no);

	}

	return 0;

}
