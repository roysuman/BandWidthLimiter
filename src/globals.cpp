/*
 * =====================================================================================
 *
 *       Filename:  globals.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Thursday 25 June 2015 11:02:08  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */

#include "globals.hpp"
using namespace globals;
std::fstream           JsonData::fileHandler ;
jsonPortData           JsonData::portBindData[SIZE];
jsonIpcServerConfig    JsonData::ipcServerConfig;
std::string            JsonData::glibcPath;
//print the error
bool
JsonData::notParsed( std::string errorString ){
	std::cerr<<"Not able to parsed from config file "<<errorString<<std::endl;
	return false;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readFromConfigFile
 *  Description:  read configuration data
 *   Return @true on success
 *          @false on error
 * =====================================================================================
 */
bool
JsonData::readFromConfigFile( void ){
	int              pos;
	Json::Value      root;
	std::string      ports;
	Json::Reader     reader;
	std::string      latency;
	std::string      upLimits;
	bool             returnVal;
	Json::Value      nodeValue;
	std::string      downLimits;

#ifdef DEBUG
	std::cout<<PRINT<<"Parse json config file"<<std::endl;
#endif
	try{
		fileHandler.open( "CONFIG/systemConfig.json" );
		if ( !fileHandler.is_open() ){
			std::cerr<<PRINT<<"Not able to open config fail"<<std::endl;
			returnVal = false;
		}
		else{
			if ( !reader.parse( fileHandler , root )  ){
				std::cerr<<PRINT<<"Failed to parse Json config value"<<std::endl;
				returnVal = false;
			}
			else{
				nodeValue = root [ "NoPort" ] ;
				if ( not nodeValue.isNull() ){
					portCount = nodeValue.asInt();
				} else return ( notParsed( "No of port") );
				nodeValue = root [ "Port" ] ;
				if ( not nodeValue.isNull() ){
					ports = nodeValue.asString();
				}else return ( notParsed( "Port Numbers") );
				nodeValue = root[ "DownLimits" ];
				if ( not nodeValue.isNull() ){
					downLimits= nodeValue.asString();
				}else return (notParsed("DownLimits") );
				nodeValue = root [ "UpLoadLimits"];
				if ( not nodeValue.isNull() ){
					upLimits = nodeValue.asString();
				} else return ( notParsed("UploadLimits") );
				nodeValue = root [ "Latency" ];
				if ( not nodeValue.isNull() ){
					latency = nodeValue.asString();
				}else return ( notParsed("Latency") );
				nodeValue = root ["IpcServerIp"];
			       if ( not nodeValue.isNull() ){
				       ipcServerConfig.ipcIpAddress = nodeValue.asString();
			       }else return ( notParsed("IpcIpAddr") );
			       nodeValue = root [ "IpcServerPort"];
			       if ( not nodeValue.isNull() ){
				       ipcServerConfig.port = nodeValue.asInt();
			       }else return ( notParsed("IpcPOrt") );
/* 			       nodeValue= root [ "GLIBC"];
			       if ( not nodeValue.isNull() ){
				       glibcPath= nodeValue.asString();
				       setenv("RESILIENCE_GLIBC_PATH",glibcPath.c_str(),1);
			       } else return (notParsed("Glibc path") );
			       */
			       //insert into DS
			       int loop ;
			       for ( loop = 0 ; loop < portCount ; ++ loop ){
				       pos = ports.find_first_of ( ":" );
				       if ( pos == -1 ) break;
				       portBindData[ loop  ].port = atoi( ports.substr( 0 , pos ).c_str() );
				       ports.erase(0 , pos + 1 );

				       pos = downLimits.find_first_of(":");
				       if ( pos == -1 ) break;
				       portBindData[ loop ].downLimit  = atoi( downLimits.substr( 0 , pos ).c_str() );
				       downLimits.erase( 0 , pos + 1 );

				       pos = upLimits.find_first_of( ":");
				       if ( pos == -1 ) break;
				       portBindData[loop].upLimit = atoi( upLimits.substr( 0 , pos ).c_str() );
				       upLimits.erase( 0 , pos + 1);

				       pos = latency.find_first_of( ":");
				       if ( pos == -1 ) break;
				       portBindData[loop].latency = atoi( latency.substr(0, pos ).c_str() );
				       latency.erase(0 , pos + 1);
			       }
			       if ( pos == -1 ) returnVal = false;
			       else {returnVal = true;
#ifdef TEST
			       for ( int i = 0 ; i < portCount ; ++i ){
				       std::cout<<"Index [ "<<i <<" ]  Port [ "<<portBindData [i].port
					       <<" ] DownLimit [ "<< portBindData[ i ].downLimit
					       <<" ] UpLimit [ "<< portBindData[i].upLimit
					       <<" ] Latency [ "<<portBindData[i].latency<<" ] "<<std::endl;
			       }
			       
#endif
			       }

			}
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception "<<e.what()<<std::endl;
		returnVal = false;
	}
	return returnVal;
}
