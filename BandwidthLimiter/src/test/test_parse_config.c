/*
 * =====================================================================================
 *
 *       Filename:  test_parse_config.c
 *
 *    Description:  test the parse config
 *
 *        Version:  1.0
 *        Created:  Monday 05 October 2015 02:15:43  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
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
