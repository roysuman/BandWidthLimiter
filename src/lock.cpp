/*
 * =====================================================================================
 *
 *       Filename:  lock.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Wednesday 13 May 2015 01:20:42  IST
 *       Revision:  none
 *       Compiler:  g++4.7
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include "lock.hpp"
pthread_mutex_t   Lock::__objPool__;
pthread_mutex_t   Lock::__readLock__;
pthread_mutex_t   Lock::__portStat__;
pthread_mutex_t   Lock::__writeLock__;
pthread_mutex_t   Lock::__createCloseSock__;

void 
Lock::acquirePoolLock(void ){
	pthread_mutex_lock ( &Lock::__objPool__);
}

void 
Lock::releasePoolLock ( void ){
	pthread_mutex_unlock( &Lock::__objPool__ );
}

void 
Lock::acquirePortStatLock( void ){
	pthread_mutex_lock( &Lock::__portStat__ );
}
void
Lock::releasePortStatLock( void ){
	pthread_mutex_unlock( &Lock::__portStat__ );
}
void
Lock::acquireReadLock( void ){
	pthread_mutex_lock( &Lock::__readLock__ );
}
void
Lock::releaseReadLock( void ){
	pthread_mutex_unlock( &Lock::__readLock__ );
}

void 
Lock::acquireWriteLock( void ){
	pthread_mutex_lock( &Lock::__writeLock__ );
}
void
Lock::releseWriteLock( void ){
	pthread_mutex_unlock( &Lock::__writeLock__ );
}
void
Lock::acquireCreateCloseSock( void ){
	pthread_mutex_lock( &Lock::__createCloseSock__ );
}
void
Lock::releaseCreateCloseSock( void ){
	pthread_mutex_unlock( &Lock::__createCloseSock__ );
}
