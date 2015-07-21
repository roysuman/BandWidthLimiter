/*
 * =====================================================================================
 *
 *       Filename:  dump.cpp
 *
 *    Description:  create dump file
 *
 *        Version:  1.0
 *        Created:  Monday 11 May 2015 02:54:39  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include<iostream>
#include<fstream>
int main(){
	std::fstream fileHandler;
	std::string data= "suman roy is a bad boy:";
	fileHandler.open ( "testData.txt" , std::ios::out );
	if ( fileHandler.is_open() ){
			std::string data= "suman roy is a bad boy:";
		for ( unsigned int outerLoop = 1 ; outerLoop <= 100 ; ++outerLoop ){
			for ( unsigned int loop = 1 ; loop <= 20 ; ++ loop ){
				data = data + data;
				if ( data.length() > 5000 ){
					fileHandler<<data<<std::endl;
					data = "";
				}

			}
		}
	}else std::cerr<<"Not abel to open\n";
	fileHandler.close();
	return 0;
}

