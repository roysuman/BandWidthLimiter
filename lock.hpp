/*
 * =====================================================================================
 *
 *       Filename:  lock.hpp
 *
 *    Description:  lock for thread safe
 *
 *        Version:  1.0
 *        Created:  Wednesday 13 May 2015 01:15:36  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#ifndef LOCK_HPP__
#define LOCK_HPP__
#include<iostream>
#include <stdlib.h>
#include <mutex>
#include <thread>
//#define DEBUG 1
class Lock{
	private:
		static pthread_mutex_t   __myLock__;
		static pthread_mutex_t   __objPool__;
		static pthread_mutex_t   __readLock__;
		static pthread_mutex_t   __portStat__;
		static pthread_mutex_t   __writeLock__;
		static pthread_mutex_t   __createCloseSock__;
	public:
		Lock(){}
		~Lock(){}
		void acquirePoolLock( void );
		void releasePoolLock(void  );
		void acquirePortStatLock ( void );
		void releasePortStatLock( void );
		void acquireWriteLock ( void );
		void releseWriteLock ( void );
		void acquireReadLock( void );
		void releaseReadLock( void );
		void releaseCreateCloseSock( void );
		void acquireCreateCloseSock ( void );
};
#endif
