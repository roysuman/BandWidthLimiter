/*
 * =====================================================================================
 *
 *       Filename:  buffer_test.cpp
 *
 *    Description:  tset the buffer class
 *
 *        Version:  1.0
 *        Created:  Monday 25 May 2015 04:09:30  IST
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
#include"buffer.hpp"
int main(){
	std::fstream fileHandler;
	Message buf;
	buf.init(1000);
	__message *readBuff, *writeBuff;
	writeBuff = NULL;
	readBuff = NULL;
	std::string getline;
	char data[1025];
	fileHandler.open("test/testData.txt", std::ios::in );
	while( std::getline( fileHandler , getline )){
		std::cout<<"\nData wrote: [ "<<buf.write( writeBuff , (void*)getline.c_str() , getline.length())<<" ] \n";
		size_t count = buf.read( writeBuff , data , 1024  );
		std::cout<<"Read count [ "<<count<<" ] "<<std::endl;
	}
	fileHandler.close();

}

