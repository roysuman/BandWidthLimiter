/*
 * =====================================================================================
 *
 *       Filename:  objectpool.cpp
 *
 *    Description:  Pool handler for Interface class objects
 *
 *        Version:  1.0
 *        Created:  Sunday 10 May 2015 08:01:54  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include"objectpool.hpp"

Lock    ObjectPool::lock;
/*-----------------------------------------------------------------------------
 * static method for accessing class poolInstance
 * returns objectpool instance
 * -----------------------------------------------------------------------------*/
ObjectPool* 
ObjectPool::getInstance(){

	if (poolInstance == 0){
		poolInstance = new ObjectPool;
	}
	return poolInstance;
}
/*-----------------------------------------------------------------------------
 * returns instance of class Interface
 * new instance will be creted if all the objects of Interface
 * class were used at the same time
 * -----------------------------------------------------------------------------*/
        
__resilience__::Interface * 
ObjectPool::getInterfaceObj(){
	//acquire lock
	lock.acquirePoolLock( );

	__resilience__::Interface    *instance;
	try{
		if (objectInstance.empty()){


#ifdef DEBUG
		std::cout << PRINT
			<<"Creating new INstance of Interface." << std::endl;
#endif
		
			instance =  new __resilience__::Interface;
		
	}else{
#ifdef DEBUG
		std::cout <<PRINT
			<< "Reusing existing." << std::endl;
#endif
		instance = objectInstance.front();
		objectInstance.pop_front();
		}
	}
	catch ( const std::exception &ba ){
	//	lock.releasePoolLock(  );
		std::cerr<<PRINT<<"EXCEPTION:"<<ba.what()<<std::endl;
		exit(EXIT_FAILURE);
	}
	lock.releasePoolLock(  );
	return instance;

}
/*-----------------------------------------------------------------------------
 * return the Interface instance back to the pool
 * the resource must be initialized back to the default setting
 * before someone else attempts to use it
 * -----------------------------------------------------------------------------*/

void 
ObjectPool::returnInterfaceObj( __resilience__::Interface *object ){
	//	object->reset();//some method at Interface class
	lock.acquirePoolLock(  );
	try{
		objectInstance.push_back(object);
	}catch ( const std::bad_alloc &ba ){
		std::cerr<<PRINT<<"Error on returning object to object pool\n";
	}
	lock.releasePoolLock(  );
}

ObjectPool* ObjectPool::poolInstance = 0;


