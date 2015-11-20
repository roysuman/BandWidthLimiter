/*
 * =====================================================================================
 *
 *       Filename:  file_write.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Tuesday 29 September 2015 08:15:28  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include<string>
using namespace std;

int main () {

  ofstream myfile ("example.txt");
  if (myfile.is_open())
  {
	  std::string temp ="suman roy is a bad boy";
	  for ( size_t loop =0 ; loop < 10 ; ++ loop ){
		  temp = temp + temp;

	  }
    myfile << temp;
    myfile.close();
  }
  else cout << "Unable to open file";
return 0;
}
 
