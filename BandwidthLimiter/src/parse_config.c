#include"parse_config.h"

config_struct_operations config_operations = {
	.parse_config = __parse_config__,
	.do_clean     =__do_cleanup_list__
};

void
__do_cleanup_list__(list_head *port_attr_list_head){

	list_head *port_attr_list_temp, *temp_list_store;
	port_str  *port_attr_temp;

	list_for_each_safe (port_attr_list_temp, temp_list_store, port_attr_list_head){
		port_attr_temp = get_member_struct (port_attr_list_temp, port_str, port_link);
		delete_list (port_attr_list_temp);
		free(port_attr_temp);
	}
	return;
}
/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
static inline char *
trim (char * s)
{
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];

  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';

  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;

  /* Copy finished string */
  strcpy (s, s1);
  return s;
}

int
__parse_config__ ( config_struct *params){
#ifdef DEBUG
	fprintf(stdout,"__parse_config__\n");
#endif
	int    return_val;
	char *s, buff[MAXLEN];
	char *temp1,*temp2;

	return_val = 1;
	INIT_LIST_HEAD(&params->port_list_head);
	FILE *fp = fopen (CONFIG_FILE, "r");
	if (fp == NULL){
		fprintf(stderr,"Not able to open file\n");

		return_val = -1;
		return return_val;
	}
	 /* Read next line */
	 while ( ( (s = fgets (buff, sizeof buff, fp)) != NULL) &&  ( return_val == 1 ) ){
		  /* Skip blank lines and comments */
		 if (buff[0] == '\n' || buff[0] == '#')
			 continue;
		  /* Parse name/value pair from line */
		 char name[MAXLEN], value[MAXLEN];
		 s = strtok (buff, "=");
		 if (s==NULL)
			 continue;
		 else
			 strncpy (name, s, MAXLEN);
		 s = strtok (NULL, "=");
		 if (s==NULL)
			 continue;
		 else
			 strncpy (value, s, MAXLEN);
		 trim (value);
		 /* Copy into correct entry in parameters struct */
		 if (strcmp(name, "NO_PORT")==0){
			 params->no_port = (size_t)atoi( value );
#ifdef DEBUG
			 fprintf(stdout,"TOTAL PORT TO FILTER %zu\n",params->no_port);
#endif
			 size_t loop = 1;
			 for ( ; loop <= params->no_port;++loop ){
#ifdef DEBUG
				 fprintf(stdout,"Reading config data for port_loop %zu\n",loop);
#endif
				 if ( fgets(buff,sizeof buff , fp ) != NULL ){
					 if (buff[0] == '\n' || buff[0] == '#'){
						 --loop;
						  continue;
					 }
					 s = strtok (buff, "=");
					 if (s==NULL)
						 continue;
					 else
						 strncpy (name, s, MAXLEN);
					 s = strtok (NULL, "=");
					 if (s==NULL)
						 continue;
					 else
						 strncpy (value, s, MAXLEN);
					 trim(value);
					 port_str *p_data = (port_str*)safe_malloc(sizeof ( port_str));
					 if ( p_data == NULL )
						 return -1;
					 temp1 = strtok(value,"[");
					 size_t loop2 = 1;
					 for ( ;loop2<=SIZE;++loop2){
						 if ( temp1){
#ifdef DEBUG
							 fprintf(stdout,"%s\n",temp1);
#endif
							 if ( loop2 != SIZE )
								 temp2=strtok(temp1,",");
							 else
								 temp2 =strtok(temp1,"]");
							 if (temp2)
								 switch(loop2){
									 case 1:
#ifdef DEBUG
										 fprintf(stdout,"ASSIGN PORT NO FOR LOOP %zu\n",loop2);
#endif
										 p_data->port_no = (uint32_t)atoi(temp2);
#ifdef DEBUG
										 fprintf(stdout,"Port No [ %u]\n",p_data->port_no);
#endif
										 break;
									 case 2:
#ifdef DEBUG
										 fprintf(stdout,"ASSIGN UP_LIMIT FOR LOOP %zu\n",loop2);

#endif
										 p_data->up_limit = (ssize_t)atoi(temp2);
#ifdef DEBUG
										 fprintf(stdout,"UP_LIMIT [ %zd]\n",p_data->up_limit);
#endif
										 break;
									 case 3:
#ifdef DEBUG
										 fprintf(stdout,"ASSIGN DOWN_LIMIT FOR LOOP %zu\n",loop2);
#endif
										 p_data->down_limit = (ssize_t)atoi(temp2);
#ifdef DEBUG
										 fprintf(stdout,"DownLimit [ %zd n",p_data->down_limit);
#endif
										 break;
									 case 4:
#ifdef DEBUG
										 fprintf(stdout,"ASSIGN LATENCY FOR LOOP %zun",loop2);
#endif
										 p_data->latency =(uint32_t) atoi(temp2);
#ifdef DEBUG
										 fprintf(stdout,"Latency [ %u]\n",p_data->latency);
#endif
										 break;
									 default:
										 fprintf(stderr,"Switch input un-expected\n");
								 }
							 temp1 = strtok(NULL,"]");
						 }
					 }
#ifdef DEBUG
					 fprintf(stdout,"INFO: INSERT into list\n");
#endif
					 //insert into list
					 list_add_tail(&p_data->port_link,&params->port_list_head);
				 }
			 }
		 }
		  else if (strcmp(name, "LIBC_PATH")==0){
#ifdef DEBUG
			 fprintf(stdout,"ASSIGN LIBC PATH%s\n",value);
#endif
			 temp1 = strtok(value,"[");
			 temp2=strtok(temp1,"]");
			 params->libc_path = temp2;
#ifdef DEBUG
			 fprintf(stdout,"Libc_Path [ %s ]\n ",params->libc_path);
#endif
		 }
		 else if (strcmp(name, "SERVER_IP")==0){
#ifdef DEBUG
			 fprintf(stdout,"ASSIGN SERVER_IP");
#endif
			 temp1 = strtok(value,"[");
			 temp2=strtok(temp1,"]");
			 fprintf(stdout,"%s\n",temp2);
			 params->server_ip = temp2;
#ifdef DEBUG
			 fprintf(stdout,"SERVER_IP [ %s ] \n",params->server_ip);
#endif
		 }
		 else if (strcmp(name, "SERVER_PORT")==0){
#ifdef DEBUG
			 fprintf(stdout,"ASSIGN SERVER PORT%s\n",value);
#endif
			 temp1 = strtok(value,"[");
			 fprintf(stdout,"%s\n",temp1);
			 temp2=strtok(temp1,"]");
			 fprintf(stdout,"%s\n",temp2);
//			 params->server_port = atoi(temp2);
#ifdef DEBUG
			 fprintf(stdout,"SERVER_PORT [ %u ] \n",params->server_port);
#endif
		 }
	 }
	 fclose(fp);
	 return return_val;

}

