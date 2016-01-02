#define _GNU_SOURCE 1
#include<stdbool.h>
#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<ctype.h>
#define VERSION "1.0"
#define MAX_PATH_LENGTH 256
#define LIB_NAME "BandwidthLimiter/build-32/libc_band.so"
#define DEBUG 1
void usage(void  ){

	fprintf( stdout , "\n\t -f/-F Enabled config file\n");
	fprintf( stdout ,"\n\t-u: Uplimits\n");
	fprintf(stdout , "\t-d: Downlimits\n");
	fprintf(stdout , "\t-h: Help\n");
	fprintf(stdout, "\t-l: Licence\n");
	fprintf(stdout , "\t-i: Ipaddress\n");
	fprintf(stdout , "\t-p: Ports\n");
	fprintf( stdout , "\t-P: Ipc Server Port");
	fprintf( stdout , "\t-v: Version\n");
	fprintf( stdout , "\t-I:system IP\n");
	fprintf(stdout , "\t-g: Kernel GlibC path at local machine\n");
	fprintf( stdout , "\t At the end programm to execute\n");
	fprintf( stdout , "Option -g and executable prog name is mandatory\nResilience Version[ %s ]  Author: Suman roy ( suman.roy@cognizant.com)\n",VERSION);
	exit(EXIT_FAILURE);
}

void 
parse(char *line, char **argv){
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv = NULL;//'\0';                 /* mark the end of argument list  */
}
void  
execute(char **argv){
#ifdef DEBUG
	fprintf(stdout , "argv [ 0 ] = %s\n " , argv[0]);
	fprintf(stdout , "argv [ 1 ] = %s\n ",argv [1 ] );
//	fprintf( stdout , "argv [ 2 ] = %s \n ", argv [ 2] );
/* 	fprintf( stdout , "argv [ 3 ] = %s \n ", argv [ 3] );
	fprintf( stdout , "argv [ 4 ] = %s \n ", argv [ 4] );
	fprintf( stdout , "argv [ 5 ] = %s \n ", argv [ 5] );*/

#endif
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
}
int main( int argc , char** argv){
	
	int      opt;
	bool     flag_config = false;
	char     _port[] ="-1";
	char     _ip_addr[]="127.0.0.1";
	char     _sys_ip[]="127.0.0.1";
	char     _up_limit[]="-1";
	char     _down_limit[]="-1";
	char     _glibc[]="\0";
	char     _latency[]="\0";
	char     comman[]="\0";
	
	char*    port= _port;//"5000";
	char*    ip_addr=_ip_addr;// "127.0.0.1";
	char*    sys_ip=_sys_ip;//	"127.0.0.1";
	char*    up_limit= _up_limit;//"-1";
	char*    down_limit=_down_limit;// "-1";
	char*    glibc=_glibc;// "";
	char*    latency=_latency;// "0";
	char     buff [ MAX_PATH_LENGTH];
	bool     if_glib;
	bool     cmd_flag;
	char     *command = comman;//="\0";//[1024];             /* the input line                 */

	char     *argv2[64];              /* the command line argument      */
	const char     *ipcPort = "8888";

	if_glib = false;
	cmd_flag = false;
	while (  ( opt = getopt ( argc , argv ,"fFhHvVi:I:u:U:d:D:l:L:p:P:g:G:c:C:") ) != -1 && !if_glib )
		switch ( opt ){
			case 'f':
			case 'F':
				flag_config = true;
				break;

			case 'u':
			case 'U':
				up_limit =  optarg;
				break;
			case 'd':
			case 'D':
				down_limit = optarg ;
				break;
			case 'l':
			case 'L':
				latency = optarg;
				break;
			case 'i':
				ip_addr = optarg;
				break;
			case 'I':
				sys_ip = optarg;
			case 'p':
				port = optarg;
				break;
			case 'P':
				ipcPort = optarg;
			case 'v':
			case 'V':
				fprintf(stdout , "%s VERSION -- UNDER TEST", VERSION);
				break;
			case 'g':
			case 'G':
				glibc = optarg;
				if_glib = true;

				break;
			case 'C':
			case 'c':
				command = optarg;
				cmd_flag = true;
			case '?':
				if (optopt == 'c')
					fprintf( stderr , "%d requires an argument", optopt );
				else if (isprint (optopt))
					fprintf( stderr , "%d Unknown Option", optopt);
				break;
			case 'h':
			case 'H':
			default:
				usage();
		}
		argc -= optind  ;
		argv +=optind  ;
#ifdef DEBUG
		fprintf(stdout , "IpAddress[ %s ] \n",ip_addr);
		fprintf(stdout , "Port[ %s]\n",port);
		fprintf(stdout,"UpLimit [ %s ]\n",up_limit);
		fprintf(stdout,"DownLimit [ %s ]\n",down_limit);
		fprintf(stdout ,"Latency [ %s ]\n",latency);
		fprintf(stdout ,"Command [ %s ] \n",command);
#endif
		if ( flag_config ){
			if ( !cmd_flag )
				usage();
			else{
				setenv("RESILIENCE_CONFIGFLAG","1", 1 );
				goto CALL;
			}
		}

		if(  !if_glib || !cmd_flag  ) usage();
		setenv("RESILIENCE_UP_LIMITS",up_limit, 1 );
		setenv("RESILIENCE_IPS", ip_addr, 1);
		setenv("RESILIENCE_PORTS", port, 1 );
		setenv("RESILIENCE_DOWN_LIMITS",down_limit, 1 );
		setenv("RESILIENCE_LATENCY",latency ,1); 
		setenv("RESILIENCE_SYS_IP",sys_ip,1);
		setenv("RESILIENCE_IPC_PORT" , ipcPort , 1 );
		setenv("RESILIENCE_CONFIGFLAG","-1", 1 );
CALL:
		setenv("RESILIENCE_GLIBC_PATH",glibc,1);
		parse(command,argv2);
		if ( getcwd( buff , sizeof(buff)) == NULL ) {
			fprintf(stderr , "getcwd Error\n");
			return(0);
		}
		strcat( buff , "/");
		const char *lib = LIB_NAME;
		strcat( buff , lib);// , sizeof( lib));
		setenv( "LD_PRELOAD",buff,1);
		fprintf(stdout,"\nArgv2  [ %s ] \n",*argv2);
		execute(argv2);
		//control will never return
		return 0;
}

