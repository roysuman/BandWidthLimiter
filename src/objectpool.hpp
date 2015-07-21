/*
 * =====================================================================================
 *
 *       Filename:  objectpool.hpp
 *
 *    Description:  object pool management for Interface class objects
 *
 *        Version:  1.0
 *        Created:  Sunday 10 May 2015 08:11:37  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), suman.roy@cognizant.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#ifndef __OBJECTPOOL_HPP__
#define __OBJECTPOOL_HPP__
#include <string>
#include<list>
#include "interface.hpp"
//#include "lock.hpp"
class ObjectPool{

    private:
        static ObjectPool*                        poolInstance;
        std::list<__resilience__::Interface *>    objectInstance;
	static Lock                               lock;
 
    public:
	ObjectPool() {poolInstance = NULL; }
	~ObjectPool(){}

	/* -----------------------------------------------------------------------------
 * 	 *  static method for accessing class poolInstance
 * 	 	 *  returns objectpool instance
 * 	 	 	 *-----------------------------------------------------------------------------*/
        static ObjectPool* getInstance();

	/* -----------------------------------------------------------------------------
 * 	 *  returns instance of class Interface
 * 	 	 *  new instance will be creted if all the objects of Interface
 * 	 	 	 *  class were used at the same time
 * 	 	 	 	 *  
 * 	 	 	 	 	 *-----------------------------------------------------------------------------*/
        __resilience__::Interface* getInterfaceObj();

	/* -----------------------------------------------------------------------------
 * 	 *  return the Interface instance back to the pool
 * 	 	 *  the resource must be initialized back to the default setting
 * 	 	 	 *  before someone else attempts to use it
 * 	 	 	 	 *-----------------------------------------------------------------------------*/
        void returnInterfaceObj(__resilience__::Interface* object);
	void freeObjectPool( void );

};
#endif
