/*
 **=====================================================================================
 **
 **       Filename:  buffer.cpp
 **
 **    Description:  buffer cache management. kinda similar to LINUX buffer management. 
                     During the start up a list of free buffer is beaing created.
		     To write data into a buffer:
		      ->takle  a free buffer from the free buffer list and write the data
		      --> and map the fd with the buffer queue
		     To Read data from the buffer
		     --> read the data from the front of the buffer queue.
		     --> retrun the buffer to the free buffer list
 **
 **        Version:  1.0
 **        Created:  Tuesday 12 May 2015 12:32:53  IST
 **       Revision:  none
 **       Compiler:  gcc
 **
 **         Author:  ROY (), suman.roy@cognizant.com
 **   Organization:  NA
 **
 ** =====================================================================================
 **/

#include "buffer.hpp"

__message__       *Message::freeHead = NULL;
pthread_mutex_t   Message::bufferPoolUpdate= PTHREAD_MUTEX_INITIALIZER;
/* 
 *=== FUNCTION  ======================================================================
 *         Name:  getNode
 *  Description:  create an empty node , both left and right pointer will
 *                point the node itself.
 * =====================================================================================
 */
__message__ * 
Message::getNode( void ){

	__message__    *temp;

	try {
		temp = new __message__;
		temp->next = temp;
		temp->prev = temp;
		temp->length = 0;
	}
	catch ( const std::bad_alloc &ba ){
		temp = NULL;
		std::cerr<<PRINT<<"Exception :"<<ba.what()<<std::endl;
	}
	return temp;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init
 *  Description:  create the buffer cache-- initially all the buffer wi 
 *                 ll be attached with the freeBuffer Head. 
 *                 @count: no of buffer will be created
 *                 @Return true on success else false. *   
 * =====================================================================================
 */
bool 
Message::init( void ){
	__message__    *temp;
	unsigned int   loop;
	bool           returnValue;

	pthread_mutex_lock( &bufferPoolUpdate);
#ifdef DEBUG
	std::cout<<"CREATING FREE BUFFER POOL\n";
#endif
	try{
		for ( loop = 1 ; loop <= BUFFER_POOL_INIT_SIZE ; ++loop ){
			temp = getNode();
			if ( temp == NULL ) { 
				std::cerr<<"Can;t init buffere cache\n";
				returnValue = false; 
				break;
			}
			temp->length = loop;
			if ( freeHead == NULL ){
				freeHead = temp;
			}else{
				temp->prev = freeHead->prev;
				freeHead->prev->next = temp;
				temp->next = freeHead;
				freeHead->prev = temp;
			}
			returnValue = true;
		}
#ifdef DEBUG
		std::cout<<PRINT<<" Buffer cache init done for [ " << loop 
			<<" ] no of message struct list And return Val [ "
			<<returnValue<<" ]\n";
#endif
#ifdef TEST
		temp = freeHead;
		while ( temp->next != freeHead ){
			std::cout<<temp->length<<std::endl;
			temp = temp->next;
		}
		exit(0);
#endif
		//update buffer pool size
		bufferPoolSize += loop;
		pthread_mutex_unlock( &bufferPoolUpdate);
#ifdef DEBUG
		std::cout<<PRINT<<"Buffer pool Size : "<<bufferPoolSize<<" ] \n";
#endif
	}catch ( const std::exception &e){

		std::cerr<<PRINT<<"Exception"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getNode
 *  Description:  take a node from the free buffer list
 *                if free buffer list is empty then return NULL
 *                @Parameter: void
 *                @Return: pointer of the node taken from the buffer ( on success)
 * =====================================================================================
 */

__message__ * 
Message::getFreeNode( void ){

#ifdef DEBUG
	std::cout<<PRINT<<"Get a free node from the free buffer list\n";
#endif
	__message__    *node;
	pthread_mutex_lock( &bufferPoolUpdate);
	try{
		if ( freeHead == NULL ) node = NULL;
		else{
			node = freeHead;
			if ( freeHead->next == freeHead )freeHead = NULL;
			else{
				freeHead->next->prev =  freeHead->prev;
				freeHead->prev->next = freeHead->next;
				freeHead = freeHead->next;
			}
			node->prev = node;
			node->next = node;
		}
		if ( node == NULL ){ 
#ifdef DEBUG
			std::cout<<"Increasing Free Buffer Pool"<<std::cout;
#endif
			pthread_mutex_unlock( &bufferPoolUpdate);
			init();
			node = getFreeNode();
			pthread_mutex_lock( &bufferPoolUpdate);
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception"<<e.what()<<std::endl;
		node = NULL;
	}
	pthread_mutex_unlock( &bufferPoolUpdate);
	return node;

}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  returnNode
 *  Description:  return a node to free buffer node list
 *                @node : reference of the node, have to return  
 * =====================================================================================
 */
void 
Message::returnNode(__message__ *&node){
#ifdef DEBUG
	std::cout<<PRINT<<"Returning a node\n";
#endif
	pthread_mutex_lock( &bufferPoolUpdate);
	try{
		memset ( node->data ,0 , node->length );
		if ( freeHead == NULL ){
			freeHead = node;
		}else{
			node->prev = freeHead->prev;
			freeHead->prev->next = node;
			node->next = freeHead;
			freeHead->prev = node;
		}
	}catch ( const std::exception &e){
		std::cerr<<PRINT<<"Exception"<<e.what()<<std::endl;
	}
	pthread_mutex_unlock( &bufferPoolUpdate);
	//to do
	//maintain current buffer size, if current buffer size>=
	//Max buff size.. hen no need to return the free buff at free buffer list
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  write
 *  Description:  write data to buffer list
 *                @head: the head pointer of the buffer list associated with the fd
 *                @buff: buffer contains data
 *                @length: length od the data in buff
 *                @return: >0 no of byte wrote to the buffer cache
 *                         -1 : if the buff pointing NULL || length <= 0
 * =====================================================================================
 */
ssize_t 
Message::write( __message__ *& head , void *buff , size_t length  ){
	ssize_t    wroteByte;
	ssize_t    writeByte;
	__message__    *node;

	wroteByte = 0;

#ifdef DEBUG
	std::cout<<PRINT<<"Write data into buffer cache of Length [ "
		<<length<<" ] \n";
#endif
	try{
		if ( buff == NULL || length <= 0 ){
			wroteByte = -1;
		}
		if ( wroteByte == 0 ){
			do{
				node = getFreeNode();
				if ( node == NULL ) break;
				length - wroteByte < MESSAGE_SIZE ? writeByte = 
					length - wroteByte : writeByte = MESSAGE_SIZE;
				memcpy ( node->data ,(char*) buff + wroteByte , writeByte );
				node->length = writeByte;
				wroteByte += writeByte;
				if ( head == NULL ){
					head = node;
				}
				else{
					node->prev = head->prev;
					head->prev->next = node;
					node->next = head;
					head->prev = node;
					//	if ( isBegin ) head = head->prev;
				}
			}while( wroteByte < (ssize_t )length );
		}
#ifdef TEST
		__message__ *head2 = head;
		int cc= 0;
		while ( head2->next != head ){
			++cc;
			std::cout<<head2->length<<std::endl;
			head2= head2->next;
		} std::cout<<head2->length<<"Total no : [ "<<cc<<" ] \n";exit(0);
#endif
	}
	catch ( const std::exception &e){
		std::cerr<<PRINT<<"Exception"<<e.what()<<std::endl;
	}
		return wroteByte;
}	

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  read
 *  Description:  read data from the buffer list 
 *              @head: pointer pointing the head node of the buffer list, asso to the fd
 *              @buff: container tom read the data,
 *              @length: no of Bytes need tp read.
 *              @return: >0 : no of bytes read
 *                       -1: buffer cahce has no data
 *                       0: no of byte request to read is 0.                
 * =====================================================================================
 */
ssize_t 
Message::read ( __message__ *& head, void *buff , size_t length ){
	ssize_t        readByte;
	ssize_t        read;
	__message__    *temp;

	readByte = 1;
#ifdef DEBUG
	std::cout<<PRINT<<" Read from buffer cache of Length [ "
		<<length<<" ] \n";

#endif
	try{
		if ( length <= 0 ) readByte =  0;
		if ( head == NULL ){
#ifdef DEBUG
			std::cout<<"Buffer cache NULL \n";
#endif
			readByte = -1;
		}
		//if length == -1 read all data till buffer size
		if ( readByte == 1 ){
			readByte = 0;
			do{
				length - readByte < head->length? read = 
					length - readByte:read = head->length;
				memcpy( (char*)buff + readByte , head->data , read );
				readByte += read;
				if ( read < (ssize_t )head->length  ){
					memcpy( head->data ,head->data +read , 
							(ssize_t )head->length - read );
					head->length-= (size_t )read; 
				}else{
					temp = head;
					if ( head->next == head ){
						head = NULL;
					}else{
						head->prev->next = head->next;
						head->next->prev = head->prev;
						head = head->next;
					}
					returnNode( temp );
				}
#ifdef DEBUG
				std::cout<<"Read Byte : [ "<<readByte
					<<" ] Length [ "<<length<<" ] \n";
#endif
			}while( readByte <(ssize_t ) length && head != NULL );
		}
	}
	catch ( const std::exception &e){
		std::cerr<<PRINT<<"Exception"<<e.what()<<std::endl;
	}
	return readByte;
}
